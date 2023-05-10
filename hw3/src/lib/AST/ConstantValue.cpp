#include "AST/ConstantValue.hpp"
#include <string>

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                                     Variable *const_value)
    : ExpressionNode{line, col}, const_value(const_value) {}

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {}

const char *ConstantValueNode::getNameCString()
{
    if(const_value->type == Datatype::INT){
        return std::to_string(const_value->int_t).c_str();
    }
    else if(const_value->type == Datatype::REAL_){
        return std::to_string(const_value->real_t).c_str();
    }
    else if(const_value->type == Datatype::STR){
        return const_value->str_t;
    }
    else if(const_value->type == Datatype::BOOL){
        std::string bool_val;
        if(const_value->bool_t == Boolean_type::booltype_True){
            return "true";
        }
        else if(const_value->bool_t == Boolean_type::booltype_False){
            return "false";
        }
    }
    return "";
}
