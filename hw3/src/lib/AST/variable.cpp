#include "AST/variable.hpp"

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                           const char *name, Variable *type,
                           AstNode *constant_value)
    : AstNode{line, col}, name(name), type(type), constant_value(constant_value) {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {}

void VariableNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO

    if(constant_value != NULL){
        constant_value->accept(p_visitor);
    }
}

const char *VariableNode::getNameCString(){
    return name;
}

const char *VariableNode::getNameCType(){
    std::string var_type;
    // normal type and array
    if(type->type == Datatype::INT){
        var_type = "integer";
    }
    else if(type->type == Datatype::REAL_){
        var_type = "real";
    }
    else if(type->type == Datatype::STR){
        var_type = "string";
    }
    else if(type->type == Datatype::BOOL){
        var_type = "boolean";
    }

    if(type->arr_num.size() > 0){
        var_type += " ";
        for(int i = 0; i < type->arr_num.size(); i++){
            var_type += "[";
            var_type += std::to_string(type->arr_num[i]);
            var_type += "]";
        }        
    }

    return var_type.c_str();
}   