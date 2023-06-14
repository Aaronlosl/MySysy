#pragma once
#include <functional>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include "koopa.h"

using namespace std;
namespace ir {
    class OpName {
        protected:
            enum Type {
                Var,
                Imm,
                Null,
            };
        public:
            Type type;
            string name;
            int value;
            OpName();
            OpName(std::string name_);
            OpName(int value_);
            bool is_var() const;
            bool is_local_var() const;
            bool is_global_var() const;
            bool is_imm() const;
            bool is_null() const;
            bool operator==(const OpName& other) const;
            string toString() const;

    };
    enum class OpCode {
        // MALLOC_IN_STACK,  // dest = offset(new StackArray(size op1))
        // MOV,              // dest = op1
        FUNCTION_BEGIN,   // FUNCTION_BEGIN
        FUNCTION_END,     // FUNCTION_END
        INFO,             // info for compiler
        RET,              // return / return op1
        EQ,               // dest = op1 == op2
        NE,               // dest = op1 != op2 
        ADD,              // dest = op1 + op2
        SUB,              // dest = op1 - op2
        MUL,              // dest = op1 * op2
        DIV,              // dest = op1 / op2
        MOD,              // dest = op1 % op2
        OR,               // dest = op1 || op2
        AND,              // dest = op1 && op2
        LT,               // dest = op1 < op2
        GT,               // dest = op1 > op2
        LE,               // dest = op1 <= op2
        GE,               // dest = op1 >= op2

        // SET_ARG,          // if dest < 4: R(dest)) = op1 else: push_stack(op1)
        call,             // call label
        cmp,              // cmp op1, op2
        jm,               // jmp label
        JEQ,              // if EQ: jmp label
        JNE,              // if NE: jmp label
        JLE,              // if LE: jmp label
        JLT,              // if LT: jmp label
        JGE,              // if GE: jmp label
        JGT,              // if GT: jmp label
        MOVEQ,            // if EQ: dest = op1 else: dest = op2
        MOVNE,            // if NE: dest = op1 else: dest = op2
        MOVLE,            // if LE: dest = op1 else: dest = op2
        MOVLT,            // if LT: dest = op1 else: dest = op2
        MOVGE,            // if GE: dest = op1 else: dest = op2
        MOVGT,            // if GT: dest = op1 else: dest = op2
        SAL,              // dest = op1 << op2 算数左移
        SAR,              // dest = op1 >> op2 算数右移
        STORE,            // op1[op2] = op3
        LOAD,             // dest = op1[op2]
        LABEL,            // label:
        DATA_BEGIN,       //.data
        DATA_WORD,        //.word
        DATA_SPACE,       //.space
        DATA_END,         // nothing
        PHI_MOV,          // PHI
        NOOP,             // no operation
    };
    class IR {
        public:
            int line, column;
            OpCode op_code;
            OpName dest, op1, op2, op3;
            string label;
            list<IR>::iterator phi_block;
            IR(OpCode op_code, OpName dest, OpName op1, OpName op2, OpName op3,
                string label = "");
            IR(OpCode op_code, OpName dest, OpName op1, OpName op2,
                string label = "");
            IR(OpCode op_code, OpName dest, OpName op1, string label = "");
            IR(OpCode op_code, OpName dest, string label = "");
            IR(OpCode op_code, string label = "");

            // bool some(decltype(&ir::OpName::is_var) callback,
            //             bool include_dest = true) const;
            // bool some(std::function<bool(const ir::OpName&)> callback,
            //             bool include_dest = true) const;
            // void forEachOp(std::function<void(const ir::OpName&)> callback,
            //                 bool include_dest = true) const;
            void print(string filename, bool verbose = false) const;

    };
        
    class IR_DUMP {
        public:
        string filename;
        IR_DUMP(string filename);

        void writeALL(vector<IR*>IRList);

        void writeLibFuncs();
        void writeOpIr(vector<IR*>IRList);
        
    };
    
}  // namespace syc::ir

