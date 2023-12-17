#include "AST/function.hpp"

// TODO
FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                           const char *name, 
                           std::vector <AstNode*> *declaration_list, 
                           std::vector <Variable*> *parameter_list,
                           Variable *return_type, 
                           AstNode *compuound_statement)
    : AstNode{line, col}, name(name), declaration_list(declaration_list), parameter_list(parameter_list),
      return_type(return_type), compuound_statement(compuound_statement) {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

// void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }

void FunctionNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(declaration_list != NULL){
        for (auto &declaration_list : *declaration_list) {
            declaration_list->accept(p_visitor);
        }
    }

    if(compuound_statement != NULL){
        compuound_statement->accept(p_visitor);
    }
}

const char *FunctionNode::getNameCString(){
    return name;
}

std::string FunctionNode::getNameCParam(){
    std::string func_parameter;
    func_parameter = "(";
    if(parameter_list->size() > 0){
        for(int i = 0; i < parameter_list->size(); i++){
            // printf("DEBUG: type is %d\n", parameter_list->at(i)->type);
            if(parameter_list->at(i)->type == Datatype::INT){
                func_parameter += "integer";
            }
            else if(parameter_list->at(i)->type == Datatype::REAL_){
                func_parameter += "real";
            }
            else if(parameter_list->at(i)->type == Datatype::STR){
                func_parameter += "string";
            }
            else if(parameter_list->at(i)->type == Datatype::BOOL){
                func_parameter += "boolean";
            }

            if(parameter_list->at(i)->arr_num.size() > 0){
            func_parameter += " ";
            for(int j = 0; j < parameter_list->at(i)->arr_num.size(); j++){
                //std::cout << "num:" << parameter_list->at(i)->arr_num[i] << std::endl;
                func_parameter += "[";
                func_parameter += std::to_string(parameter_list->at(i)->arr_num[j]);
                func_parameter += "]";
            }        
            }

            if(i != parameter_list->size() - 1){
                func_parameter += ", ";
            }
            
        }
        
    }
    func_parameter += ")";
    // printf("DEBUG: Func Params: %s\n", func_parameter.c_str());
    return func_parameter;
}

const char *FunctionNode::getNameCType(){
    std::string func_parameter = "";
    // printf("DEBUG: the return type is %d\n", return_type->type);
    if(return_type->type == Datatype::INT){
        func_parameter += "integer";
    }
    else if(return_type->type == Datatype::REAL_){
        func_parameter += "real";
    }
    else if(return_type->type == Datatype::STR){
        func_parameter += "string";
    }
    else if(return_type->type == Datatype::BOOL){
        func_parameter += "boolean";
    }
    else if(return_type->type == Datatype::VOID){
        func_parameter += "void";
    }

    return func_parameter.c_str();
}