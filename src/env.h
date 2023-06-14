// #pragma once
// #include <unordered_map>
// #include <string>
// #include <iostream>
// #include "node.h"
// #include "ir.h"

// namespace env {
//     class Env {
//         public:
//         vector<ir::IR*> IRList;
//         vector<int> regControl;
//         vector<pair<int, std::string>> funcEntry;
//         Env(vector<ir::IR*> IRList, vector<int> regControl, vector<pair<int, std::string>> funcEntry){}
//     };
// }

// // class Env {
// //     public:
// //         unordered_map<string, BaseAST*> var_tb;         // id, ast
// //         unordered_map<string, vector<string>> type_tb;  // id, (<ret_type, par_1_type, par_2_type,...> or type)
// //         unordered_map<string, BaseAST*> func_tb;        // id, ast
// //         void putVar(string var_name, BaseAST* ast) {
// //             if (!var_tb.insert(pair<string, BaseAST*>(var_name, ast)).second) {
// //                 cout << "--> var: " << var_name << " re-defined" << endl;
// //             }
// //         }
// //         BaseAST* getVarAST(string var_name) {
// //             auto ret = var_tb.find(var_name);
// //             if (ret != var_tb.end()) {
// //                 return ret->second;
// //             } else {
// //                 return nullptr;
// //             }
// //         }
// //         void putType(string var_or_func_name, vector<string>type_list) {
// //             if (!type_tb.insert(pair<string, vector<string>>(var_or_func_name, type_list)).second) {
// //                 cout << "--> type: " << var_or_func_name << " re-defined" << endl;
// //             }
// //         }
// //         vector<string> getVarOrFuncTypeList(string var_or_func_name) {
// //             auto ret = type_tb.find(var_or_func_name);
// //             if (ret != type_tb.end()) {
// //                 return ret->second;
// //             } else {
// //                 return vector<string>();
// //             }
// //         }
// //         void putFunc(string func_name, BaseAST* ast) {
// //             if (!func_tb.insert(pair<string, BaseAST*>(func_name, ast)).second) {
// //                 cout << "--> func: " << func_name << " re-defined" << endl;
// //             }
// //         }
// //         BaseAST* getFuncAST(string func_name) {
// //             auto ret = func_tb.find(func_name);
// //             if (ret != func_tb.end()) {
// //                 return ret->second;
// //             } else {
// //                 return nullptr;
// //             }
// //         }
// // };