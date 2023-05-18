#pragma once
#include <iostream>
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
class BaseAST;
BaseAST* current();

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
};


class CompUnitAST : public BaseAST {
  public:
    unique_ptr<BaseAST> func_def;
    unique_ptr<BaseAST> func_defs;
    void Dump() const override {
      // cout << INDENT() << "CompUnitAST {\n";
      // INDENTATION++;
      func_def->Dump();
      func_defs->Dump();
      // INDENTATION--;
      // cout << INDENT() << "}\n";
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
};

class FuncTypeAST : public BaseAST {
  public:
    string type;
    void Dump() const override {
      cout << INDENT() << "FuncTypeAST: " << type << ",\n";
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
};

class NumberAST : public BaseAST {
  public:
    Int int_const;
    void Dump() const override {
      cout << INDENT() << "NumberAST: " << to_string(int_const) << "\n";
    }  
};

class UnaryOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "UnaryOpAST: " << op << "\n";
    }  
};

class AddOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "AddOpAST: " << op << "\n";
    }  
};

class MulOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "MulOpAST: " << op << "\n";
    }  
};

class RelOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "RelOpAST: " << op << "\n";
    }  
};

class EqOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "EqOpAST: " << op << "\n";
    }  
};

class LAndOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "LAndOpAST: " << op << "\n";
    }  
};

class LOrOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "LOrOpAST: " << op << "\n";
    }  
};

class NullAST : public BaseAST {
  public:
    void Dump() const override {}
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
      cout << INDENT() << "};\n";
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
};

class BTypeAST : public BaseAST {
  public:
    string type;
    void Dump() const override {
      cout << INDENT() << "BTypeAST: " << type << "\n";
    }
};

class ConstDefAST : public BaseAST {
  public:
    string ident;
    unique_ptr<BaseAST> const_init_val;
    void Dump() const override {
      cout << INDENT() << "ConstDefAST { IDENT: " << ident << ",\n";
      INDENTATION++;
      const_init_val->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
};

class ConstInitValAST : public BaseAST {
  public:
    unique_ptr<BaseAST> const_exp;
    void Dump() const override {
      cout << INDENT() << "ConstInitValAST {\n";
      INDENTATION++;
      const_exp->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
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
};

class BlockItemsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> block_item;
    unique_ptr<BaseAST> block_items;
    void Dump() const override {
      block_item->Dump();
      block_items->Dump();
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
};

class LValAST : public BaseAST {
  public:
    string ident;
    void Dump() const override {
      cout << INDENT() << "LValAST: IDENT: " << ident << "\n";
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
      cout << INDENT() << "};\n";
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
};

class VarDefAST : public BaseAST {
  public:
    string ident;
    unique_ptr<BaseAST> init_val;
    void Dump() const override {
      cout << INDENT() << "VarDefAST { IDENT: " << ident << ",\n";
      INDENTATION++;
      init_val->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
};

class InitValAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    void Dump() const override {
      cout << INDENT() << "InitValAST {\n";
      INDENTATION++;
      exp->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
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
};

class CommaFuncFParamsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> func_f_param;
    unique_ptr<BaseAST> comma_func_f_params;
    void Dump() const override {
      func_f_param->Dump();
      comma_func_f_params->Dump();
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
};

class CommaExpsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    unique_ptr<BaseAST> comma_exps;
    void Dump() const override {
      exp->Dump();
      comma_exps->Dump();
    }
};

class FuncDefsAST : public BaseAST {
  public:
    unique_ptr<BaseAST> func_def;
    unique_ptr<BaseAST> func_defs;
    void Dump() const override {
      func_def->Dump();
      func_defs->Dump();
    }
};


