#pragma once
#include "ir.h"

#include <cassert>
#include <string>

namespace ir {
OpName::OpName() : type(OpName::Type::Null) {}
OpName::OpName(std::string name) : type(OpName::Type::Var), name(name) {}
OpName::OpName(int value) : type(OpName::Type::Imm), value(value) {}
bool OpName::is_var() const { return this->type == OpName::Type::Var; }
bool OpName::is_local_var() const {
  return (this->is_var()) && (this->name[0] == '%');
}
bool OpName::is_global_var() const {
  return (this->is_var()) && (this->name[0] == '@');
}
bool OpName::is_imm() const { return this->type == OpName::Type::Imm; }
bool OpName::is_null() const { return this->type == OpName::Type::Null; }

bool OpName::operator==(const OpName& other) const {
  if (this->type != other.type) return false;
  if (this->is_var()) {
    return this->name == other.name;
  } else if (this->is_imm()) {
    return this->value == other.value;
  } else {
    assert(this->is_null());
    return true;
  }
}
string OpName::toString() const {
    return this->name;
}

IR::IR(OpCode op_code, OpName dest, OpName op1, OpName op2, OpName op3,
       std::string label)
    : op_code(op_code), dest(dest), op1(op1), op2(op2), op3(op3), label(label) {}
IR::IR(OpCode op_code, OpName dest, OpName op1, OpName op2, std::string label)
    : op_code(op_code),
      dest(dest),
      op1(op1),
      op2(op2),
      op3(OpName()),
      label(label) {}
IR::IR(OpCode op_code, OpName dest, OpName op1, std::string label)
    : op_code(op_code),
      dest(dest),
      op1(op1),
      op2(OpName()),
      op3(OpName()),
      label(label) {}
IR::IR(OpCode op_code, OpName dest, std::string label)
    : op_code(op_code),
      dest(dest),
      op1(OpName()),
      op2(OpName()),
      op3(OpName()),
      label(label) {}
IR::IR(OpCode op_code, std::string label)
    : op_code(op_code),
      dest(OpName()),
      op1(OpName()),
      op2(OpName()),
      op3(OpName()),
      label(label) {}
// bool IR::some(decltype(&syc::ir::OpName::is_var) callback,
//               bool include_dest) const {
//   return this->some([callback](OpName op) { return (op.*callback)(); },
//                     include_dest);
// }
// bool IR::some(std::function<bool(const syc::ir::OpName&)> callback,
//               bool include_dest) const {
//   return (include_dest && callback(this->dest)) || callback(this->op1) ||
//          callback(this->op2) || callback(this->op3);
// }
// void IR::forEachOp(std::function<void(const syc::ir::OpName&)> callback,
//                    bool include_dest) const {
//   this->some(
//       [callback](const syc::ir::OpName& op) {
//         callback(op);
//         return false;
//       },
//       include_dest);
// }

void IR::print(string filename, bool verbose) const {
    ofstream ofs;
    ofs.open(filename, ios::out|ios::app);
    switch(this->op_code) {
        case OpCode::FUNCTION_BEGIN:
        case OpCode::FUNCTION_END:
            ofs << this->label<< endl;
            break;
        case OpCode::INFO:
            ofs << this->label<< endl;
            break;
        case OpCode::RET:
            ofs << "ret " << this->op1.toString() << endl;
            break;
        case OpCode::EQ:
            ofs << this->dest.toString() << " = eq " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::NE:
            ofs << this->dest.toString() << " = ne " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::ADD:
            ofs << this->dest.toString() << " = add " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::SUB:
            ofs << this->dest.toString() << " = sub " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::MUL:
            ofs << this->dest.toString() << " = mul " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::DIV:
            ofs << this->dest.toString() << " = div " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::MOD:
            ofs << this->dest.toString() << " = mod " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::OR:
            ofs << this->dest.toString() << " = or " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::AND:
            ofs << this->dest.toString() << " = and " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::LT:
            ofs << this->dest.toString() << " = lt " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::GT:
            ofs << this->dest.toString() << " = gt " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::LE:
            ofs << this->dest.toString() << " = le " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        case OpCode::GE:
            ofs << this->dest.toString() << " = ge " << this->op1.toString() << ", " << this->op2.toString() << endl;
            break;
        // case OpCode::
        default:break;
    }
    ofs.flush();
    ofs.close();
}

IR_DUMP::IR_DUMP(string filename): filename(filename) {}
void IR_DUMP::writeALL(vector<IR*>IRList) {
    this->writeLibFuncs();
    this->writeOpIr(IRList);
}
void IR_DUMP::writeLibFuncs() {
    ofstream ofs;
    ofs.open(this->filename, ios::out|ios::app);

    ofs.open(filename, ios::trunc);
    ofs << "decl @getint(): i32\n";
    ofs << "decl @getch(): i32\n";
    ofs << "decl @getarray(*i32): i32\n";
    ofs << "decl @putint(i32)\n";
    ofs << "decl @putch(i32)\n";
    ofs << "decl @putarray(i32, *i32)\n";
    ofs << "decl @starttime()\n";
    ofs << "decl @stoptime()\n\n";

    ofs.flush();
    ofs.close();
}
void IR_DUMP::writeOpIr(vector<IR*>IRList) {
    for (auto it = IRList.begin(); it != IRList.end(); it++) {
        (*it)->print(this->filename);
    }
}

}  // namespace syc::ir
