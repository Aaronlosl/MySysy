%code requires {
  #pragma once
  #include <memory>
  #include <iostream>
  #include <string>
  #include "node.h"
}

%{
#pragma once
#include <iostream>
#include <memory>
#include <string>
#include "node.h"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, string s);
string structure = "";

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token VOID INT RETURN CONST IF ELSE WHILE BREAK CONTINUE
%token <str_val> IDENT 
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Exp UnaryExp PrimaryExp AddExp MulExp
LOrExp RelExp EqExp LAndExp Number Null 
Decl ConstDecl BType ConstDef CommaConstDefs ConstInitVal BlockItems BlockItem LVal ConstExp
VarDecl CommaVarDefs VarDef InitVal
FuncFParams FuncFParam CommaFuncFParams FuncRParams CommaExps
DeclOrFuncDefs CompUnit DeclOrFuncDef
CommaConstExps
BracketConstExps BracketExps
%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
S
  : CompUnit { auto s_ = make_unique<SAST>();  s_->comp_unit = unique_ptr<BaseAST>($1);  ast = move(s_); }
  ;

CompUnit
  : DeclOrFuncDef DeclOrFuncDefs { 
    auto ast = new CompUnitAST(); 
    ast->decl_or_func_def = unique_ptr<BaseAST>($1); 
    ast->decl_or_func_defs = unique_ptr<BaseAST>($2); 
    $$ = ast; structure +="\nCompUnit: DeclOrFuncDef DeclOrFuncDefs"; 
  }
  ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
FuncDef
  : FuncType IDENT '(' Null ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->func_f_params = unique_ptr<BaseAST>($4);
    ast->block = unique_ptr<BaseAST>($6);
    $$ = ast;
    structure +="\nFuncDef: FuncType IDENT '(' Null ')' Block";
  }
  | FuncType IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->func_f_params = unique_ptr<BaseAST>($4);
    ast->block = unique_ptr<BaseAST>($6);
    $$ = ast;
    structure +="\nFuncDef: FuncType IDENT '(' FuncFParams ')' Block";
  }
  ;

FuncType
  : INT {
    auto ast = new FuncTypeAST();
    ast->type = "int";
    $$ = ast;
    structure +="\nFuncType: INT";
  }
  | VOID {
    auto ast = new FuncTypeAST();
    ast->type = "void";
    $$ = ast;
    structure +="\nFuncType: VOID";
  }
  ;

Block
  : '{' Stmt '}' {
    auto ast = new BlockAST();
    ast->stmt_or_block_items = unique_ptr<BaseAST>($2);
    $$ = ast;
    structure +="\nBlock: '{' Stmt '}'";
  }
  ;

Stmt
  : RETURN Exp Null Null ';' {
    auto ast = new StmtAST();
    ast->keyword = "return";
    ast->optional_keyword = "";
    ast->l_value_or_single = unique_ptr<BaseAST>($2);
    ast->r_value_1 = unique_ptr<BaseAST>($3);
    ast->r_value_2 = unique_ptr<BaseAST>($4);
    $$ = ast;
    structure +="\nStmt: RETURN Exp Null Null ';'";
  }
  // | LVal '=' Exp Null ';' {
  //   auto ast = new StmtAST();
  //   ast->keyword = "";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($1);
  //   ast->r_value_1 = unique_ptr<BaseAST>($3);
  //   ast->r_value_2 = unique_ptr<BaseAST>($4);
  //   $$ = ast;
  //   structure +="\nStmt: LVal '=' Exp Null ';'";
  // }
  // | Null Null Null ';' {
  //   auto ast = new StmtAST();
  //   ast->keyword = "";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($1);
  //   ast->r_value_1 = unique_ptr<BaseAST>($2);
  //   ast->r_value_2 = unique_ptr<BaseAST>($3);
  //   $$ = ast;
  //   structure +="\nStmt: Null Null Null ';'";
  // }
  // | Exp Null Null ';' {
  //   auto ast = new StmtAST();
  //   ast->keyword = "";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($1);
  //   ast->r_value_1 = unique_ptr<BaseAST>($2);
  //   ast->r_value_2 = unique_ptr<BaseAST>($3);
  //   $$ = ast;
  //   structure +="\nStmt: Exp Null Null ';'";
  // }
  // | Block Null Null {
  //   auto ast = new StmtAST();
  //   ast->keyword = "";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($1);
  //   ast->r_value_1 = unique_ptr<BaseAST>($2);
  //   ast->r_value_2 = unique_ptr<BaseAST>($3);
  //   $$ = ast;
  //   structure +="\nStmt: Block Null Null";
  // }
  // | IF '(' Exp ')' Stmt Null {
  //   auto ast = new StmtAST();
  //   ast->keyword = "if";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($3);
  //   ast->r_value_1 = unique_ptr<BaseAST>($5);
  //   ast->r_value_2 = unique_ptr<BaseAST>($6);
  //   $$ = ast;
  //   structure +="\nStmt: IF '(' Exp ')' Stmt Null";
  // }
  // | IF '(' Exp ')' Stmt ELSE Stmt {
  //   auto ast = new StmtAST();
  //   ast->keyword = "if";
  //   ast->optional_keyword = "else";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($3);
  //   ast->r_value_1 = unique_ptr<BaseAST>($5);
  //   ast->r_value_2 = unique_ptr<BaseAST>($7);
  //   $$ = ast;
  //   structure +="\nStmt: IF '(' Exp ')' Stmt ELSE Stmt";
  // }
  // | WHILE '(' Exp ')' Stmt Null {
  //   auto ast = new StmtAST();
  //   ast->keyword = "while";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($3);
  //   ast->r_value_1 = unique_ptr<BaseAST>($5);
  //   ast->r_value_2 = unique_ptr<BaseAST>($6);
  //   $$ = ast;
  //   structure +="\nStmt: WHILE '(' Exp ')' Stmt Null";
  // }
  // | BREAK Null Null Null ';' {
  //   auto ast = new StmtAST();
  //   ast->keyword = "break";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($2);
  //   ast->r_value_1 = unique_ptr<BaseAST>($3);
  //   ast->r_value_2 = unique_ptr<BaseAST>($4);
  //   $$ = ast;
  //   structure +="\nStmt: BREAK Null Null Null ';'";
  // }
  // | CONTINUE Null Null Null ';' {
  //   auto ast = new StmtAST();
  //   ast->keyword = "continue";
  //   ast->optional_keyword = "";
  //   ast->l_value_or_single = unique_ptr<BaseAST>($2);
  //   ast->r_value_1 = unique_ptr<BaseAST>($3);
  //   ast->r_value_2 = unique_ptr<BaseAST>($4);
  //   $$ = ast;
  //   structure +="\nStmt: CONTINUE Null Null Null ';'";
  // }
  ;

Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
    ast->value = ast->exp->value;
    structure +="\nExp: LOrExp";
  }
  ;

PrimaryExp 
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    ast->value = unique_ptr<BaseAST>($2);
    $$ = ast;
    structure +="\nPrimaryExp: '(' Exp ')'";
  }
  | LVal {
    auto ast = new PrimaryExpAST();
    ast->value = unique_ptr<BaseAST>($1);
    $$ = ast;
    structure +="\nPrimaryExp: LVal";
  }
  | Number {
    auto ast = new PrimaryExpAST();
    ast->value = unique_ptr<BaseAST>($1);
    $$ = ast;
    structure +="\nPrimaryExp: Number";
  }
  ;

UnaryExp
  : Null PrimaryExp {
    auto ast = new UnaryExpAST();
    ast->ident = "";
    ast->op = "";
    ast->exp_or_op_or_params_1 = unique_ptr<BaseAST>($1);
    ast->exp_or_op_2 = unique_ptr<BaseAST>($2);
    $$ = ast;
    ast->value = ast->exp_or_op_2->value;
    structure +="\nUnaryExp: Null PrimaryExp";
  }
  | Null '+' UnaryExp {
    auto ast = new UnaryExpAST();
    ast->ident = "";
    ast->op = "+";
    ast->exp_or_op_or_params_1 = unique_ptr<BaseAST>($1);
    ast->exp_or_op_2 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_or_op_2->value;
    structure +="\nUnaryExp: UnaryOp UnaryExp";
  }
  | Null '-' UnaryExp {
    auto ast = new UnaryExpAST();
    ast->ident = "";
    ast->op = "-";
    ast->exp_or_op_or_params_1 = unique_ptr<BaseAST>($1);
    ast->exp_or_op_2 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = -ast->exp_or_op_2->value;
    structure +="\nUnaryExp: UnaryOp UnaryExp";
  }
  | Null '!' UnaryExp {
    auto ast = new UnaryExpAST();
    ast->ident = "";
    ast->op = "!";
    ast->exp_or_op_or_params_1 = unique_ptr<BaseAST>($1);
    ast->exp_or_op_2 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = !ast->exp_or_op_2->value;
    structure +="\nUnaryExp: UnaryOp UnaryExp";
  }
  // | IDENT '(' Null ')' Null {
  //   auto ast = new UnaryExpAST();
  //   ast->ident = *unique_ptr<string>($1);
  //   ast->exp_or_op_or_params_1 = unique_ptr<BaseAST>($3);
  //   ast->exp_or_op_2 = unique_ptr<BaseAST>($5);
  //   $$ = ast;
  //   structure +="\nUnaryExp: IDENT '(' Null ')' Null";
  // }
  // | IDENT '(' FuncRParams ')' Null {
  //   auto ast = new UnaryExpAST();
  //   ast->ident = *unique_ptr<string>($1);
  //   ast->exp_or_op_or_params_1 = unique_ptr<BaseAST>($3);
  //   ast->exp_or_op_2 = unique_ptr<BaseAST>($5);
  //   $$ = ast;
  //   structure +="\nUnaryExp: IDENT '(' FuncRParams ')' Null";
  // }
  ;

AddExp
  : Null MulExp {
    auto ast = new AddExpAST();
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($2);
    $$ = ast;
    ast->value = ast->exp_3->value;
    structure +="\nAddExp: Null Null MulExp";
  }
  | AddExp '-' MulExp {
    auto ast = new AddExpAST();
    ast->op = "-";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_1->value - ast->exp_3->value;
    structure +="\nAddExp: AddExp AddOp MulExp";
  }
  | AddExp '+' MulExp {
    auto ast = new AddExpAST();
    ast->op = "+";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_1->value + ast->exp_3->value;
    structure +="\nAddExp: AddExp AddOp MulExp";
  }
  ;

MulExp
  : Null UnaryExp {
    auto ast = new MulExpAST();
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($2);
    $$ = ast;
    ast->value = ast->exp_3->value;
    structure +="\nMulExp: Null Null UnaryExp";
  }
  | MulExp '*' UnaryExp {
    auto ast = new MulExpAST();
    ast->op = "*";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_1->value * ast->exp_3->value;
    structure +="\nMulExp: MulExp MulOp UnaryExp";
  }
  | MulExp '/' UnaryExp {
    auto ast = new MulExpAST();
    ast->op = "/";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_1->value / ast->exp_3->value;
    structure +="\nMulExp: MulExp MulOp UnaryExp";
  }
  | MulExp '%' UnaryExp {
    auto ast = new MulExpAST();
    ast->op = "%%";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_1->value % ast->exp_3->value;
    structure +="\nMulExp: MulExp MulOp UnaryExp";
  }
  ;

RelExp
  : Null AddExp {
    auto ast = new RelExpAST();
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($2);
    $$ = ast;
    ast->value = ast->exp_3->value;
    structure +="\nRelExp: Null Null AddExp";
  }
  | RelExp '<' AddExp {
    auto ast = new RelExpAST();
    ast->op = "<";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_1->value < ast->exp_3->value;
    structure +="\nRelExp: RelExp RelOp AddExp";
  }
  | RelExp '>' AddExp {
    auto ast = new RelExpAST();
    ast->op = ">";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($3);
    $$ = ast;
    ast->value = ast->exp_1->value > ast->exp_3->value;
    structure +="\nRelExp: RelExp RelOp AddExp";
  }
  | RelExp '<' '=' AddExp {
    auto ast = new RelExpAST();
    ast->op = "<=";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($4);
    $$ = ast;
    ast->value = ast->exp_1->value <= ast->exp_3->value;
    structure +="\nRelExp: RelExp RelOp AddExp";
  }
  | RelExp '>' '=' AddExp {
    auto ast = new RelExpAST();
    ast->op = ">=";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($4);
    $$ = ast;
    ast->value = ast->exp_1->value >= ast->exp_3->value;
    structure +="\nRelExp: RelExp RelOp AddExp";
  }
  ;

EqExp
  : Null RelExp {
    auto ast = new EqExpAST();
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($2);
    $$ = ast;
    ast->value = ast->exp_3->value;
    structure +="\nEqExp: Null Null RelExp";
  }
  | EqExp '=' '=' RelExp {
    auto ast = new EqExpAST();
    ast->op = "==";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($4);
    $$ = ast;
    ast->value = ast->exp_1->value == ast->exp_3->value;
    structure +="\nEqExp: EqExp EqOp RelExp";
  }
  | EqExp '!' '=' RelExp {
    auto ast = new EqExpAST();
    ast->op = "!=";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($4);
    $$ = ast;
    ast->value = ast->exp_1->value != ast->exp_3->value;
    structure +="\nEqExp: EqExp EqOp RelExp";
  }
  ;

LAndExp
  : Null EqExp {
    auto ast = new LAndExpAST();
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($2);
    $$ = ast;
    ast->value = ast->exp_3->value;
    structure +="\nLAndExp: Null Null EqExp";
  }
  | LAndExp '&' '&' EqExp {
    auto ast = new LAndExpAST();
    ast->op = "&&";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($4);
    $$ = ast;
    ast->value = ast->exp_1->value && ast->exp_3->value;
    structure +="\nLAndExp: LAndExp LAndOp EqExp";
  }
  ;

LOrExp
  : Null LAndExp {
    auto ast = new LOrExpAST();
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($2);
    $$ = ast;
    ast->value = ast->exp_3->value;
    structure +="\nLOrExp: Null Null LAndExp";
  }
  | LOrExp '|' '|' LAndExp {
    auto ast = new LOrExpAST();
    ast->op = "||";
    ast->exp_1 = unique_ptr<BaseAST>($1);
    // ast->op_2 = unique_ptr<BaseAST>($2);
    ast->exp_3 = unique_ptr<BaseAST>($4);
    $$ = ast;
    ast->value = ast->exp_1->value || ast->exp_3->value;
    structure +="\nLOrExp: LOrExp LOrOp LAndExp";
  }
  ;

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->int_const = ($1);
    $$ = ast;
    ast->value = ast->int_const;
    structure +="\nNumber";
  }
  ;

// UnaryOp
//   : '+' {
//     auto ast = new UnaryOpAST();
//     ast->op = '+';
//     $$ = ast;
//     structure +="\nUnaryOp: +";
//   }
//   | '-' {
//     auto ast = new UnaryOpAST();
//     ast->op = '-';
//     $$ = ast;
//     structure +="\nUnaryOp: -";
//   }
//   | '!' {
//     auto ast = new UnaryOpAST();
//     ast->op = '!';
//     $$ = ast;
//     structure +="\nUnaryOp: !";
//   }
//   ;

// AddOp
//   : '+' {
//     auto ast = new AddOpAST();
//     ast->op = '+';
//     $$ = ast;
//     structure +="\nAddOp: +";
//   }
//   | '-' {
//     auto ast = new AddOpAST();
//     ast->op = '-';
//     $$ = ast;
//     structure +="\nAddOp: -";
//   }
//   ;

// MulOp
//   : '*' {
//     auto ast = new MulOpAST();
//     ast->op = '*';
//     $$ = ast;
//     structure +="\nMulOp: *";
//   }
//   | '/' {
//     auto ast = new MulOpAST();
//     ast->op = '/';
//     $$ = ast;
//     structure +="\nMulOp: /";
//   }
//   | '%' {
//     auto ast = new MulOpAST();
//     ast->op = '%';
//     $$ = ast;
//     structure +="\nMulOp: %%";
//   }
//   ;

// RelOp
//   : '<' {
//     auto ast = new RelOpAST();
//     ast->op = '<';
//     $$ = ast;
//     structure +="\nRelOp: <";
//   }
//   | '>' {
//     auto ast = new RelOpAST();
//     ast->op = '>';
//     $$ = ast;
//     structure +="\nRelOp: >";
//   }
//   | '<' '=' {
//     auto ast = new RelOpAST();
//     ast->op = "<=";
//     $$ = ast;
//     structure +="\nRelOp: <=";
//   }
//   | '>' '=' {
//     auto ast = new RelOpAST();
//     ast->op = ">=";
//     $$ = ast;
//     structure +="\nRelOp: >=";
//   }
//   ;

// EqOp
//   : '=' '=' {
//     auto ast = new EqOpAST();
//     ast->op = "==";
//     $$ = ast;
//     structure +="\nEqOp: ==";
//   }
//   | '!' '=' {
//     auto ast = new EqOpAST();
//     ast->op = "!=";
//     $$ = ast;
//     structure +="\nEqOp: !=";
//   }
//   ;

// LAndOp
//   : '&' '&' {
//     auto ast = new LAndOpAST();
//     ast->op = "&&";
//     $$ = ast;
//     structure +="\nLAndOp: &&";
//   }
//   ;

// LOrOp
//   : '|' '|' {
//     auto ast = new LOrOpAST();
//     ast->op = "||";
//     $$ = ast;
//     structure +="\nLOrOp: ||";
//   }
//   ;

Null
  : {
    auto ast = new NullAST();
    $$ = ast;
  }
  ;
// 4.2
Decl
  : ConstDecl {
    auto ast = new DeclAST();
    ast->const_decl_or_var_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
    structure +="\nDecl: ConstDecl";
  }
  // | VarDecl {
  //   auto ast = new DeclAST();
  //   ast->const_decl_or_var_decl = unique_ptr<BaseAST>($1);
  //   $$ = ast;
  //   structure +="\nDecl: VarDecl";
  // }
  ;

VarDecl
  : BType VarDef CommaVarDefs ';' {
    auto ast = new VarDeclAST();
    ast->b_type = unique_ptr<BaseAST>($1);
    ast->var_def = unique_ptr<BaseAST>($2);
    ast->comma_var_defs = unique_ptr<BaseAST>($3);
    $$ = ast;
    structure +="\nVarDecl: BType VarDef CommaVarDefs ';'";
  }
  ;

CommaVarDefs
  : ',' VarDef CommaVarDefs {
    auto ast = new CommaVarDefsAST();
    ast->var_def = unique_ptr<BaseAST>($2);
    ast->comma_var_defs = unique_ptr<BaseAST>($3);
    $$ = ast;  
    structure +="\nCommaVarDefs: ',' VarDef CommaVarDefs";
  }
  | Null Null {
    auto ast = new CommaVarDefsAST();
    ast->var_def = unique_ptr<BaseAST>($1);
    ast->comma_var_defs = unique_ptr<BaseAST>($2);
    $$ = ast;  
    structure +="\nCommaVarDefs: Null Null";
  }
  ;

VarDef
  : IDENT BracketConstExps Null {
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->bracket_const_exps = unique_ptr<BaseAST>($2);
    ast->init_val = unique_ptr<BaseAST>($3);
    $$ = ast;  
  }
  | IDENT BracketConstExps '=' InitVal {
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->bracket_const_exps = unique_ptr<BaseAST>($2);
    ast->init_val = unique_ptr<BaseAST>($4);
    $$ = ast;  
  }
  ;

InitVal
  : Exp Null {
    auto ast = new InitValAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->comma_exps = unique_ptr<BaseAST>($2);
    $$ = ast;  
  }
  | '{' Null Null '}' {
    auto ast = new InitValAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->comma_exps = unique_ptr<BaseAST>($3);
    $$ = ast;  
  }
  | '{' Exp CommaExps '}' {
    auto ast = new InitValAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->comma_exps = unique_ptr<BaseAST>($3);
    $$ = ast;  
  }
  ;

ConstDecl
  : CONST BType ConstDef CommaConstDefs ';' {
    auto ast = new ConstDeclAST();
    ast->keyword = "const";
    ast->b_type = unique_ptr<BaseAST>($2);
    ast->const_def = unique_ptr<BaseAST>($3);
    ast->comma_const_defs = unique_ptr<BaseAST>($4);
    $$ = ast;
    structure +="\nConstDecl: CONST BType ConstDef CommaConstDefs ';'";
  }
  ;

CommaConstDefs
  : ',' ConstDef CommaConstDefs {
    auto ast = new CommaConstDefsAST();
    ast->const_def = unique_ptr<BaseAST>($2);
    ast->comma_const_defs = unique_ptr<BaseAST>($3);
    $$ = ast;
    structure +="\nCommaConstDefs: ',' ConstDef CommaConstDefs";
  }
  | Null Null {
    auto ast = new CommaConstDefsAST();
    ast->const_def = unique_ptr<BaseAST>($1);
    ast->comma_const_defs = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

BType
  : INT {
    auto ast = new BTypeAST();
    ast->type = "int";
    $$ = ast;
  }
  ;

ConstDef
  : IDENT BracketConstExps '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->bracket_const_exps = unique_ptr<BaseAST>($2);
    ast->const_init_val = unique_ptr<BaseAST>($4);
    $$ = ast;
  }
  ;

ConstInitVal
  : ConstExp Null {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    ast->comma_const_exps = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | '{' ConstExp CommaConstExps '}' {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($2);
    ast->comma_const_exps = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | '{' Null Null '}' {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($2);
    ast->comma_const_exps = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

Block
  : '{' BlockItems {
    auto ast = new BlockAST();
    ast->stmt_or_block_items = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

BlockItems
  : BlockItem BlockItems {
    auto ast = new BlockItemsAST();
    ast->block_item = unique_ptr<BaseAST>($1);
    ast->block_items = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | Null Null '}' {
    auto ast = new BlockItemsAST();
    ast->block_item = unique_ptr<BaseAST>($1);
    ast->block_items = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

BlockItem
  : Decl {
    auto ast = new BlockItemAST();
    ast->decl_or_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Stmt {
    auto ast = new BlockItemAST();
    ast->decl_or_stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

LVal
  : IDENT {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    // ast->bracket_exps = unique_ptr<BaseAST>($2);
    $$ = ast;
    structure +="\nLVal: IDENT Null";
  }
  ;
  
ConstExp
  : Exp {
    auto ast = new ConstExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
    ast->value = ast->exp->value;
    structure +="\nConstExp: Exp";
  }
  ;
// 8
FuncFParams
  : FuncFParam CommaFuncFParams {
    auto ast = new FuncFParamsAST();
    ast->func_f_param = unique_ptr<BaseAST>($1);
    ast->comma_func_f_params = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

CommaFuncFParams
  : ',' FuncFParam CommaFuncFParams {
    auto ast = new CommaFuncFParamsAST();
    ast->func_f_param = unique_ptr<BaseAST>($2);
    ast->comma_func_f_params = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | Null Null {
    auto ast = new CommaFuncFParamsAST();
    ast->func_f_param = unique_ptr<BaseAST>($1);
    ast->comma_func_f_params = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

FuncFParam
  : BType IDENT {
    auto ast = new FuncFParamAST();
    ast->b_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    $$ = ast;
  }
  ;
  
FuncRParams
  : Exp CommaExps {
    auto ast = new FuncRParamsAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->comma_exps = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;
  
CommaExps
  : ',' Exp CommaExps {
    auto ast = new CommaExpsAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->comma_exps = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | Null Null {
    auto ast = new CommaExpsAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->comma_exps = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;
  
DeclOrFuncDefs
  : DeclOrFuncDef DeclOrFuncDefs {
    auto ast = new DeclOrFuncDefsAST();
    ast->decl_or_func_def = unique_ptr<BaseAST>($1);
    ast->decl_or_func_defs = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | Null Null {
    auto ast = new DeclOrFuncDefsAST();
    ast->decl_or_func_def = unique_ptr<BaseAST>($1);
    ast->decl_or_func_defs = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

DeclOrFuncDef
  : Decl {
    auto ast = new DeclOrFuncDefAST();
    ast->decl_or_func_def = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | FuncDef {
    auto ast = new DeclOrFuncDefAST();
    ast->decl_or_func_def = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

// 8.3 still in progress
CommaConstExps
  : ',' ConstExp CommaConstExps {
    auto ast = new CommaConstExpsAST();
    ast->const_exp = unique_ptr<BaseAST>($2);
    ast->comma_const_exps = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | Null Null {
    auto ast = new CommaConstExpsAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    ast->comma_const_exps = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

BracketConstExps
  : '[' ConstExp ']' BracketConstExps {
    auto ast = new BracketConstExpsAST();
    ast->const_exp = unique_ptr<BaseAST>($2);
    ast->bracket_const_exps = unique_ptr<BaseAST>($4);
  }
  | Null Null {
    auto ast = new BracketConstExpsAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    ast->bracket_const_exps = unique_ptr<BaseAST>($2);
  }
  ;

BracketExps
  : '[' Exp ']' BracketExps {
    auto ast = new BracketExpsAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->bracket_exps = unique_ptr<BaseAST>($4);
  }
  | Null Null {
    auto ast = new BracketExpsAST();
    ast->exp = unique_ptr<BaseAST>($1);
    ast->bracket_exps = unique_ptr<BaseAST>($2);
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, string s) {
  cerr << "--> error: \n       " << structure << endl;
  cerr << endl;
}
