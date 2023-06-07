#pragma once
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

using Int = std::int32_t;
using namespace std;

static int INDENTATION = 0;
inline string INDENT() {
  string ret = "";
  for (int i=0; i<INDENTATION; i++) {
    ret += "  ";
  }
  return ret;
}
static int IR_INDENTATION = 0;
inline string IR_INDENT() {
  string ret = "";
  for (int i=0; i<IR_INDENTATION; i++) {
    ret += "  ";
  }
  return ret;
}

static string AST_TARGET = "";
static const char* AST_OUTPUT = "./debug/test.koopa";

class BaseAST;
BaseAST* current();

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    virtual void toIr() const = 0;
};


class SAST : public BaseAST {
  public:
    unique_ptr<BaseAST> comp_unit;
    void Dump() const override {
      comp_unit->Dump();
    }
    void toIr() const override {
      comp_unit->toIr();
    }
};

class CompUnitAST : public BaseAST {
  public:
    unique_ptr<BaseAST> decl_or_func_def;
    unique_ptr<BaseAST> decl_or_func_defs;
    void Dump() const override {
      cout << INDENT() << "CompUnitAST {\n";
      INDENTATION++;
      decl_or_func_def->Dump();
      decl_or_func_defs->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
      decl_or_func_def->toIr();
      decl_or_func_defs->toIr();
    }
};

class FuncDefAST : public BaseAST {
  public:
    unique_ptr<BaseAST> func_type;
    string ident;
    unique_ptr<BaseAST> func_f_params;
    unique_ptr<BaseAST> block;
    void Dump() const override {
      cout << INDENT() << "FuncDefAST {\n";
      INDENTATION++;
      func_type->Dump();
      cout << INDENT() << "IDENT: " << ident << ",\n";
      func_f_params->Dump();
      block->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
      ofstream ofs;
      ofs.open(AST_OUTPUT, ios::out|ios::app);
      ofs.write(data(IR_INDENT()), sizeof(char)*IR_INDENTATION);
      ofs.write("func @", sizeof(char)*6);
      ofs.write(data(ident), sizeof(char)*ident.length());
      ofs.write("(): ", sizeof(char)*4);
      ofs.flush();
      ofs.close();

      func_type->toIr();

      ofs.open(AST_OUTPUT, ios::out|ios::app);
      ofs.write(" {\n%entry:\n", sizeof(char)*11);
      ofs.flush();
      ofs.close();

      IR_INDENTATION++;
      block->toIr();
      IR_INDENTATION--;

      ofs.open(AST_OUTPUT, ios::out|ios::app);
      ofs.write(data(IR_INDENT()), sizeof(char)*IR_INDENTATION);
      ofs.write("}\n", sizeof(char)*2);
      ofs.flush();
      ofs.close();
    }
};

class FuncTypeAST : public BaseAST {
  public:
    string type;
    void Dump() const override {
      cout << INDENT() << "FuncTypeAST: " << type << ",\n";
    }
    void toIr() const override {
      ofstream ofs;
      ofs.open(AST_OUTPUT, ios::out|ios::app);
      if (type == "int")
        ofs.write("i32", sizeof(char)*3);
      ofs.flush();
      ofs.close();
    }
};

class StmtAST : public BaseAST {
  public:
    string keyword;
    string optional_keyword = "";
    unique_ptr<BaseAST> l_value_or_single;
    unique_ptr<BaseAST> r_value_1;
    unique_ptr<BaseAST> r_value_2;
    void Dump() const override {
      cout << INDENT() << "StmtAST {\n";
      INDENTATION++;
      cout << INDENT() << "keyword: " << keyword << ",\n";
      l_value_or_single->Dump();
      r_value_1->Dump();
      if (optional_keyword.length()) cout << INDENT() << "optional_keyword: " << optional_keyword << ",\n";
      r_value_2->Dump();
      INDENTATION--;
      cout << INDENT() <<  "}\n";
    }
    void toIr() const override {
      if (keyword == "return") {
        l_value_or_single->toIr();

        ofstream ofs;
        ofs.open(AST_OUTPUT, ios::out|ios::app);
        ofs.write(data(IR_INDENT()), sizeof(char)*IR_INDENTATION);
        ofs.write("ret ", sizeof(char)*4);
        ofs.write(data(AST_TARGET), sizeof(char)*AST_TARGET.length());
        ofs.write("\n", sizeof(char));
        ofs.flush();
        ofs.close();
      }
    }
};

class ExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    void Dump() const override {
      // cout << INDENT() << "ExpAST {\n";
      // INDENTATION++;
      exp->Dump();
      // INDENTATION--;
      // cout << INDENT() << "}\n";
    }  
    void toIr() const override {
      exp->toIr();
    }
};

class PrimaryExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> value;
    void Dump() const override {
      // cout << INDENT() << "PrimaryExpAST {\n";
      // INDENTATION++;
      value->Dump();
      // INDENTATION--;
      // cout << INDENT() << "}\n";
    }  
    void toIr() const override {
      value->toIr();
    }
};

class UnaryExpAST : public BaseAST {
  public:
    string ident = "";
    unique_ptr<BaseAST> exp_or_op_or_params_1;
    unique_ptr<BaseAST> exp_or_op_2;
    void Dump() const override {
      cout << INDENT() << "UnaryExpAST {\n";
      INDENTATION++;
      cout << INDENT() << "IDENT: " << ident << "\n";
      exp_or_op_or_params_1->Dump();
      exp_or_op_2->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }  
    void toIr() const override {
      exp_or_op_or_params_1->toIr();
    }
};

class AddExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp_1;
    unique_ptr<BaseAST> op_2;
    unique_ptr<BaseAST> exp_3;
    void Dump() const override {
      cout << INDENT() << "AddExpAST {\n";
      INDENTATION++;
      exp_1->Dump();
      op_2->Dump();
      exp_3->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }   
    void toIr() const override {

    }
};

class MulExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp_1;
    unique_ptr<BaseAST> op_2;
    unique_ptr<BaseAST> exp_3;
    void Dump() const override {
      cout << INDENT() << "MulExpAST {\n";
      INDENTATION++;
      exp_1->Dump();
      op_2->Dump();
      exp_3->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class LOrExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp_1;
    unique_ptr<BaseAST> op_2;
    unique_ptr<BaseAST> exp_3;
    void Dump() const override {
      cout << INDENT() << "LOrExpAST {\n";
      INDENTATION++;
      exp_1->Dump();
      op_2->Dump();
      exp_3->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class RelExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp_1;
    unique_ptr<BaseAST> op_2;
    unique_ptr<BaseAST> exp_3;
    void Dump() const override {
      cout << INDENT() << "RelExpAST {\n";
      INDENTATION++;
      exp_1->Dump();
      op_2->Dump();
      exp_3->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class EqExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp_1;
    unique_ptr<BaseAST> op_2;
    unique_ptr<BaseAST> exp_3;
    void Dump() const override {
      cout << INDENT() << "EqExpAST {\n";
      INDENTATION++;
      exp_1->Dump();
      op_2->Dump();
      exp_3->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class LAndExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp_1;
    unique_ptr<BaseAST> op_2;
    unique_ptr<BaseAST> exp_3;
    void Dump() const override {
      cout << INDENT() << "LAndExpAST {\n";
      INDENTATION++;
      exp_1->Dump();
      op_2->Dump();
      exp_3->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class NumberAST : public BaseAST {
  public:
    Int int_const;
    void Dump() const override {
      cout << INDENT() << "NumberAST: " << to_string(int_const) << "\n";
    }  
    void toIr() const override {
      ofstream ofs;
      ofs.open(AST_OUTPUT, ios::out|ios::app);
      ofs.write((const char*)&int_const, sizeof(Int));
      ofs.flush();
      ofs.close();
      AST_TARGET = to_string(int_const);
    }
};

class UnaryOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "UnaryOpAST: " << op << "\n";
    }  
    void toIr() const override {

    }
};

class AddOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "AddOpAST: " << op << "\n";
    }  
    void toIr() const override {
    }
};

class MulOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "MulOpAST: " << op << "\n";
    } 
    void toIr() const override {
    } 
};

class RelOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "RelOpAST: " << op << "\n";
    }  
    void toIr() const override {
    }
};

class EqOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "EqOpAST: " << op << "\n";
    } 
    void toIr() const override {
    } 
};

class LAndOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "LAndOpAST: " << op << "\n";
    } 
    void toIr() const override {
    } 
};

class LOrOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "LOrOpAST: " << op << "\n";
    }  
    void toIr() const override {
    }
};

class NullAST : public BaseAST {
  public:
    void Dump() const override {}
    void toIr() const override {
    }
};

// 4

class DeclAST : public BaseAST {
  public:
    unique_ptr<BaseAST> const_decl_or_var_decl;
    void Dump() const override {
      cout << INDENT() << "DeclAST {\n";
      INDENTATION++;
      const_decl_or_var_decl->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
      const_decl_or_var_decl->toIr();
    }
};

class ConstDeclAST : public BaseAST {
  public:
    string keyword;
    unique_ptr<BaseAST> b_type;
    unique_ptr<BaseAST> const_def;
    unique_ptr<BaseAST> comma_const_defs;
    void Dump() const override {
      cout << INDENT() << "ConstDeclAST " << keyword << " {\n";
      INDENTATION++;
      b_type->Dump();
      const_def->Dump();
      comma_const_defs->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class CommaConstDefsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> const_def;
    unique_ptr<BaseAST> comma_const_defs;
    void Dump() const override {
      const_def->Dump();
      comma_const_defs->Dump();
    }
    void toIr() const override {
    }
};

class BTypeAST : public BaseAST {
  public:
    string type;
    void Dump() const override {
      cout << INDENT() << "BTypeAST: " << type << "\n";
    }
    void toIr() const override {
    }
};

class ConstDefAST : public BaseAST {
  public:
    string ident;
    unique_ptr<BaseAST> bracket_const_exps;
    unique_ptr<BaseAST> const_init_val;
    void Dump() const override {
      cout << INDENT() << "ConstDefAST { IDENT: " << ident << ",\n";
      INDENTATION++;
      bracket_const_exps->Dump();
      const_init_val->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class ConstInitValAST : public BaseAST {
  public:
    unique_ptr<BaseAST> const_exp;
    unique_ptr<BaseAST> comma_const_exps;
    void Dump() const override {
      cout << INDENT() << "ConstInitValAST {\n";
      INDENTATION++;
      const_exp->Dump();
      comma_const_exps->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class BlockAST : public BaseAST {
  public:
    unique_ptr<BaseAST> stmt_or_block_items;
    void Dump() const override {
      // cout << INDENT() << "BlockAST {\n";
      // INDENTATION++;
      stmt_or_block_items->Dump();
      // INDENTATION--;
      // cout << INDENT() << "}\n";
    }
    void toIr() const override {
      stmt_or_block_items->toIr();
    }
};

class BlockItemsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> block_item;
    unique_ptr<BaseAST> block_items;
    void Dump() const override {
      block_item->Dump();
      block_items->Dump();
    }
    void toIr() const override {
    }
};

class BlockItemAST : public BaseAST {
  public:
    unique_ptr<BaseAST> decl_or_stmt;
    void Dump() const override {
      cout << INDENT() << "BlockItemAST {\n";
      INDENTATION++;
      decl_or_stmt->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class LValAST : public BaseAST {
  public:
    string ident;
    unique_ptr<BaseAST> bracket_exps;
    void Dump() const override {
      cout << INDENT() << "LValAST: IDENT: " << ident << "\n";
      bracket_exps->Dump();
    }
    void toIr() const override {
    }
};

class ConstExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    void Dump() const override {
      // cout << INDENT() << "ConstExpAST {\n";
      // INDENTATION++;
      exp->Dump();
      // INDENTATION--;
      // cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class VarDeclAST : public BaseAST {
  public:
    unique_ptr<BaseAST> b_type;
    unique_ptr<BaseAST> var_def;
    unique_ptr<BaseAST> comma_var_defs;
    void Dump() const override {
      cout << INDENT() << "VarDeclAST {\n";
      INDENTATION++;
      b_type->Dump();
      var_def->Dump();
      comma_var_defs->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class CommaVarDefsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> var_def;
    unique_ptr<BaseAST> comma_var_defs;
    void Dump() const override {
      var_def->Dump();
      comma_var_defs->Dump();
    }
    void toIr() const override {
    }
};

class VarDefAST : public BaseAST {
  public:
    string ident;
    unique_ptr<BaseAST> bracket_const_exps;
    unique_ptr<BaseAST> init_val;
    void Dump() const override {
      cout << INDENT() << "VarDefAST { IDENT: " << ident << ",\n";
      INDENTATION++;
      bracket_const_exps->Dump();
      init_val->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class InitValAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    unique_ptr<BaseAST> comma_exps;
    void Dump() const override {
      cout << INDENT() << "InitValAST {\n";
      INDENTATION++;
      exp->Dump();
      comma_exps->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

// 8

class FuncFParamsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> func_f_param;
    unique_ptr<BaseAST> comma_func_f_params;
    void Dump() const override {
      cout << INDENT() << "FuncFParamsAST {\n";
      INDENTATION++;
      func_f_param->Dump();
      comma_func_f_params->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class CommaFuncFParamsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> func_f_param;
    unique_ptr<BaseAST> comma_func_f_params;
    void Dump() const override {
      func_f_param->Dump();
      comma_func_f_params->Dump();
    }
    void toIr() const override {
    }
};

class FuncFParamAST : public BaseAST {
  public:
    unique_ptr<BaseAST> b_type;
    string ident;
    void Dump() const override {
      cout << INDENT() << "FuncFParamAST {\n";
      INDENTATION++;
      b_type->Dump();
      cout << INDENT() << "IDENT: " << ident << "\n";
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class FuncRParamsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    unique_ptr<BaseAST> comma_exps;
    void Dump() const override {
      cout << INDENT() << "FuncRParamsAST {\n";
      INDENTATION++;
      exp->Dump();
      comma_exps->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
    }
};

class CommaExpsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    unique_ptr<BaseAST> comma_exps;
    void Dump() const override {
      exp->Dump();
      comma_exps->Dump();
    }
    void toIr() const override {
    }
};

class DeclOrFuncDefsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> decl_or_func_def;
    unique_ptr<BaseAST> decl_or_func_defs;
    void Dump() const override {
      cout << INDENT() << "DeclOrFuncDefsAST {\n";
      INDENTATION++;
      decl_or_func_def->Dump();
      decl_or_func_defs->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
      decl_or_func_def->toIr();
      decl_or_func_defs->toIr();
    }
};

class DeclOrFuncDefAST : public BaseAST {
  public:
    unique_ptr<BaseAST> decl_or_func_def;
    void Dump() const override {
      cout << INDENT() << "DeclOrFuncDefAST {\n";
      INDENTATION++;
      decl_or_func_def->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
    void toIr() const override {
      decl_or_func_def->toIr();
    }
};
// 8.3 still in progress
class CommaConstExpsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> const_exp;
    unique_ptr<BaseAST> comma_const_exps;
    void Dump() const override {
      const_exp->Dump();
      comma_const_exps->Dump();
    }
    void toIr() const override {
      const_exp->toIr();
      comma_const_exps->toIr();
    }
};

class BracketConstExpsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> const_exp;
    unique_ptr<BaseAST> bracket_const_exps;
    void Dump() const override {
      const_exp->Dump();
      bracket_const_exps->Dump();
    }
    void toIr() const override {
    }
};

class BracketExpsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    unique_ptr<BaseAST> bracket_exps;
    void Dump() const override {
      exp->Dump();
      bracket_exps->Dump();
    }
    void toIr() const override {
    }
};
