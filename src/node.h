#pragma once
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include "ir.h"
// #include "env.h"

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

static vector<string> AST_REG;
static int AST_REG_COUNT = 0;
static int AST_BLOCK_COUNT = 0;
inline string block2str() {
  return "%_b_"+to_string(AST_BLOCK_COUNT)+":";
}

struct IrRet {
  enum tag{
    Var,
    Imm,
    None
  } type;
  int value;
  IrRet(tag t, int v): type(t), value(v) {}
};
inline string reg2str(IrRet ret) {
  if (ret.type == IrRet::tag::Var){
    return "%"+to_string(ret.value);
  } else {
    return to_string(ret.value);
  }
}
inline string reg2str(int reg) {
  return "%"+to_string(reg);
}

class BaseAST;
BaseAST* current();

// 所有 AST 的基类
class BaseAST {
  public:
    virtual ~BaseAST() = default;
    virtual void Dump() const = 0;
    virtual IrRet toIr(string filename) const = 0;
    virtual string toString() { return "BaseAST"; };
};


class SAST : public BaseAST {
  public:
    unique_ptr<BaseAST> comp_unit;
    void Dump() const override {
      comp_unit->Dump();
    }
    IrRet toIr(string filename) const override {
      return comp_unit->toIr(filename);
    }
    string toString() override {
      // comp_unit->toString();
      return "SAST";
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
    IrRet toIr(string filename) const override {
      decl_or_func_def->toIr(filename);
      decl_or_func_defs->toIr(filename);
      return IrRet(IrRet::tag::None, -1);
    }
    string toString() override {
      return "CompUnitAST";
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
    IrRet toIr(string filename) const override {
      /* First line */
      ir::IR* func_def_ir1 = new  ir::IR(
        ir::OpCode::FUNCTION_BEGIN, 
        "fun @"+ident+"("+func_f_params->toString()+"): "+func_type->toString() + " {"
      );
      func_def_ir1->print(filename);

      block->toIr(filename);

      /* End line */
      ir::IR* func_def_ir2 = new  ir::IR(
        ir::OpCode::FUNCTION_END, 
        "}"
      );
      func_def_ir2->print(filename);
      return IrRet(IrRet::tag::None, -1);
    }
    string toString() override {
      return "FuncDefAST";
    }
};

class FuncTypeAST : public BaseAST {
  public:
    string type;
    void Dump() const override {
      cout << INDENT() << "FuncTypeAST: " << type << ",\n";
    }
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
    string toString () override {
      if (type == "int") return "i32";
      return type; 
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
    IrRet toIr(string filename) const override {
      if (keyword == "return") {
        string op1 = reg2str(l_value_or_single->toIr(filename));
        ir::IR* stmt_ir = new  ir::IR(
          ir::OpCode::RET, 
          ir::OpName(), 
          ir::OpName(op1)
        );
        stmt_ir->print(filename);
        return IrRet(IrRet::tag::None, -1);
      } else {
        return l_value_or_single->toIr(filename);// TODO
      }
    }
    string toString () override { return "StmtAST"; }
};

class ExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp;
    void Dump() const override {
      cout << INDENT() << "ExpAST {\n";
      INDENTATION++;
      exp->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }  
    IrRet toIr(string filename) const override {
      return exp->toIr(filename);
    }
    string toString() override { return "ExpAST"; }
};

class PrimaryExpAST : public BaseAST {
  public:
    const volatile int reg_idx = -1;
    unique_ptr<BaseAST> value;
    void Dump() const override {
      // cout << INDENT() << "PrimaryExpAST {\n";
      // INDENTATION++;
      value->Dump();
      // INDENTATION--;
      // cout << INDENT() << "}\n";
    }  
    IrRet toIr(string filename) const override {
      return value->toIr(filename);
    }

    string toString() override { return "PrimaryExpAST"; }
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
    IrRet toIr(string filename) const override {
      if (exp_or_op_or_params_1->toString() == "!") {
        string op1 = reg2str(exp_or_op_2->toIr(filename));
        ir::IR* unaryexp_ir = new  ir::IR(
          ir::OpCode::EQ, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(to_string(0))
        );
        unaryexp_ir->print(filename);
      } else if (exp_or_op_or_params_1->toString() == "-") {
        string op2 = reg2str(exp_or_op_2->toIr(filename));
        ir::IR* unaryexp_ir = new  ir::IR(
          ir::OpCode::SUB, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(to_string(0)),
          ir::OpName(op2)
        );
        unaryexp_ir->print(filename);
      } else {
        return exp_or_op_2->toIr(filename);
      }
      return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
    }
    string toString() override { return "UnaryExpAST"; }
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
    IrRet toIr(string filename) const override {
      if (op_2->toString() == "+") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* addexp_ir = new  ir::IR(
          ir::OpCode::ADD, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        addexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else if (op_2->toString() == "-") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* addexp_ir = new  ir::IR(
          ir::OpCode::SUB, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        addexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else {
        return exp_3->toIr(filename);
      }
    }
    string toString() override { return "AddExpAST"; }
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
    IrRet toIr(string filename) const override {
      if (op_2->toString() == "*") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* mulexp_ir = new  ir::IR(
          ir::OpCode::MUL, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        mulexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else if (op_2->toString() == "/") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* mulexp_ir = new  ir::IR(
          ir::OpCode::DIV, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        mulexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else if (op_2->toString() == "%") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* mulexp_ir = new  ir::IR(
          ir::OpCode::MOD, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        mulexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else {
        return exp_3->toIr(filename);
      }
    }
    string toString() override { return "MulExpAST"; }
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
    IrRet toIr(string filename) const override {
      if (op_2->toString() == "||") {
        // 拼
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));

        ir::IR* lorexp_ir1 = new  ir::IR(
          ir::OpCode::NE, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(to_string(0))
        );
        lorexp_ir1->print(filename);
        AST_REG_COUNT++;

        ir::IR* lorexp_ir2 = new  ir::IR(
          ir::OpCode::NE, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op2),
          ir::OpName(to_string(0))
        );
        lorexp_ir2->print(filename);
        AST_REG_COUNT++;

        ir::IR* lorexp_ir3 = new  ir::IR(
          ir::OpCode::OR, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(reg2str(AST_REG_COUNT-1)),
          ir::OpName(reg2str(AST_REG_COUNT-2))
        );
        lorexp_ir3->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else {
        return exp_3->toIr(filename);
      } 
    }
    string toString() override { return "LOrExpAST"; }
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
    IrRet toIr(string filename) const override {
      if (op_2->toString() == "<") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* relexp_ir = new  ir::IR(
          ir::OpCode::LT, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        relexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else if (op_2->toString() == ">") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* relexp_ir = new  ir::IR(
          ir::OpCode::GT, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        relexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else if (op_2->toString() == "<=") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* relexp_ir = new  ir::IR(
          ir::OpCode::LE, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        relexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else if (op_2->toString() == ">=") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* relexp_ir = new  ir::IR(
          ir::OpCode::GE, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        relexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else {
        return exp_3->toIr(filename);
      }
    }
    string toString() override { return "RelExpAST"; }
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
    IrRet toIr(string filename) const override {
      if (op_2->toString() == "==") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* eqexp_ir = new  ir::IR(
          ir::OpCode::EQ, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        eqexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else if (op_2->toString() == "!=") {
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));
        ir::IR* eqexp_ir = new  ir::IR(
          ir::OpCode::NE, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(op2)
        );
        eqexp_ir->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else {
        return exp_3->toIr(filename);
      }
    }
    string toString() override { return "EqExpAST"; }
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
    IrRet toIr(string filename) const override {
      if (op_2->toString() == "&&") {
        // 拼
        string op1 = reg2str(exp_1->toIr(filename)), op2 = reg2str(exp_3->toIr(filename));

        ir::IR* landexp_ir1 = new  ir::IR(
          ir::OpCode::NE, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op1),
          ir::OpName(to_string(0))
        );
        landexp_ir1->print(filename);
        AST_REG_COUNT++;

        ir::IR* landexp_ir2 = new  ir::IR(
          ir::OpCode::NE, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(op2),
          ir::OpName(to_string(0))
        );
        landexp_ir2->print(filename);
        AST_REG_COUNT++;

        ir::IR* landexp_ir3 = new  ir::IR(
          ir::OpCode::AND, 
          ir::OpName(reg2str(AST_REG_COUNT)), 
          ir::OpName(reg2str(AST_REG_COUNT-1)),
          ir::OpName(reg2str(AST_REG_COUNT-2))
        );
        landexp_ir3->print(filename);
        return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      } else {
        return exp_3->toIr(filename);
      }
    }
    string toString() override { return "LAndExpAST"; }
};

class NumberAST : public BaseAST {
  public:
    Int int_const;
    void Dump() const override {
      cout << INDENT() << "NumberAST: " << to_string(int_const) << "\n";
    }  
    IrRet toIr(string filename) const override {
      // ir::IR* number_ir = new  ir::IR(
      //   ir::OpCode::ADD, 
      //   ir::OpName(reg2str(AST_REG_COUNT)), 
      //   ir::OpName(to_string(int_const)), 
      //   ir::OpName(to_string(0))
      // );
      // number_ir->print(filename);
      // return IrRet(IrRet::tag::Var, AST_REG_COUNT++);;
      return IrRet(IrRet::tag::Imm, int_const);
    }
    string toString() override { return to_string(int_const); }
};

class UnaryOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "UnaryOpAST: " << op << "\n";
    }  
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
    string toString() override { return op; }
};

class AddOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "AddOpAST: " << op << "\n";
    }  
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
    string toString() override { return op; }
};

class MulOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "MulOpAST: " << op << "\n";
    } 
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    } 
    string toString() override { return op; }
};

class RelOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "RelOpAST: " << op << "\n";
    }  
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
    string toString() override { return op; }
};

class EqOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "EqOpAST: " << op << "\n";
    } 
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    } 
    string toString() override { return op; }
};

class LAndOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "LAndOpAST: " << op << "\n";
    } 
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    } 
    string toString() override { return op; }
};

class LOrOpAST : public BaseAST {
  public:
    string op;
    void Dump() const override {
      cout << INDENT() << "LOrOpAST: " << op << "\n";
    }  
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
    string toString() override { return op; }
};

class NullAST : public BaseAST {
  public:
    void Dump() const override {}
    IrRet toIr(string filename) const override { return IrRet(IrRet::tag::None, -1); }
    string toString() override { return ""; }
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
    IrRet toIr(string filename) const override {
      return const_decl_or_var_decl->toIr(filename);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
};

class BTypeAST : public BaseAST {
  public:
    string type;
    void Dump() const override {
      cout << INDENT() << "BTypeAST: " << type << "\n";
    }
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      ir::IR* block_ir = new  ir::IR(
        ir::OpCode::INFO, 
        block2str()
      );
      AST_BLOCK_COUNT++;
      block_ir->print(filename);
      return stmt_or_block_items->toIr(filename);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
};

class LValAST : public BaseAST {
  public:
    string ident;
    // unique_ptr<BaseAST> bracket_exps;
    void Dump() const override {
      cout << INDENT() << "LValAST: IDENT: " << ident << "\n";
      // bracket_exps->Dump();
    }
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      decl_or_func_def->toIr(filename);
      decl_or_func_defs->toIr(filename);
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return decl_or_func_def->toIr(filename);
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
    IrRet toIr(string filename) const override {
      const_exp->toIr(filename);
      comma_const_exps->toIr(filename);
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
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
    IrRet toIr(string filename) const override {
      return IrRet(IrRet::tag::None, -1);
    }
};
