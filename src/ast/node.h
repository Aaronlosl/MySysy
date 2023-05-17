#pragma once
#include <iostream>
#include <string>
#include <vector>

using Int = std::int32_t;
using namespace std;
namespace ast::node {

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
        cout << "CompUnitAST { ";
        func_def->Dump();
        cout << " }";
      }
  };

  class FuncDefAST : public BaseAST {
    public:
      unique_ptr<BaseAST> func_type;
      string ident;
      unique_ptr<BaseAST> block;
      void Dump() const override {
        cout << "FuncDefAST { ";
        func_type->Dump();
        cout << ", " << ident << ", ";
        block->Dump();
        cout << " }";
      }
  };

  class FuncType : public BaseAST {
    public:
      string type = "int";
      void Dump() const override {
        cout << "FuncType { " << type << " }";
      }
  };

  class Block : public BaseAST {
    public:
      unique_ptr<BaseAST> stmt;
      void Dump() const override {
        cout << "Block { ";
        stmt->Dump();
        cout << " }";
      }
  };

  class RetStmt : public BaseAST {
    public:
      string keyword = "return";
      unique_ptr<BaseAST> num;
      void Dump() const override {
        cout << "RetStmt { ";
        num->Dump();
        cout << " }";
      }
  };

  class Number : public BaseAST {
    public:
      Int number;
      void Dump() const override {
        cout << "Number {" << number << " }";
      }
  };
}


