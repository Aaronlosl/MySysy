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
    void Dump() const override {
      cout << INDENT() << "CompUnitAST {\n";
      INDENTATION++;
      func_def->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
};

class FuncDefAST : public BaseAST {
  public:
    unique_ptr<BaseAST> func_type;
    string ident;
    unique_ptr<BaseAST> block;
    void Dump() const override {
      cout << INDENT() << "FuncDefAST {\n";
      INDENTATION++;
      func_type->Dump();
      cout << INDENT() << "INDENT: " << ident << ",\n";
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

class BlockAST : public BaseAST {
  public:
    unique_ptr<BaseAST> stmt;
    void Dump() const override {
      cout << INDENT() << "BlockAST {\n";
      INDENTATION++;
      stmt->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }
};

class StmtAST : public BaseAST {
  public:
    string keyword;
    unique_ptr<BaseAST> value;
    void Dump() const override {
      cout << INDENT() << "StmtAST {\n";
      INDENTATION++;
      cout << INDENT() << "keyword: " << keyword << ",\n";
      value->Dump();
      INDENTATION--;
      cout << INDENT() <<  "}\n";
    }
};

class ExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> unary_exp;
    void Dump() const override {
      cout << INDENT() << "ExpAST {\n";
      INDENTATION++;
      unary_exp->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }  
};

class PrimaryExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> value;
    void Dump() const override {
      cout << INDENT() << "PrimaryExpAST {\n";
      INDENTATION++;
      value->Dump();
      INDENTATION--;
      cout << INDENT() << "}\n";
    }  
};

class UnaryExpAST : public BaseAST {
  public:
    unique_ptr<BaseAST> exp_or_op_1;
    unique_ptr<BaseAST> exp_or_op_2;
    void Dump() const override {
      cout << INDENT() << "UnaryExpAST {\n";
      INDENTATION++;
      exp_or_op_1->Dump();
      exp_or_op_2->Dump();
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

class NullAST : public BaseAST {
  public:
    void Dump() const override {}
};



