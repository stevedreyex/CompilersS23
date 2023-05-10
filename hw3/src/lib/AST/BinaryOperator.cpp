#include "AST/BinaryOperator.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                                        Op_infos *op, 
                                        AstNode *left_operand, 
                                        AstNode *right_operand)
    : ExpressionNode{line, col}, op(op), left_operand(left_operand), right_operand(right_operand) {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

// void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
//     // TODO
// }

void BinaryOperatorNode::accept(AstNodeVisitor &visitor) {
     visitor.visit(*this); 
}

void BinaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(left_operand != NULL){
        left_operand->accept(p_visitor);
    }

    if(right_operand != NULL){
        right_operand->accept(p_visitor);
    }
}

const char *BinaryOperatorNode::getNameCType(){

    std::string op_type;

    if(op->operator_type == Operator::OP_OR){
        op_type = "or";
    }
    else if(op->operator_type == Operator::OP_AND){
        op_type = "and";
    }
    else if(op->operator_type == Operator::OP_LESS){
        op_type = "<";
    }
    else if(op->operator_type == Operator::OP_LESS_OR_EQUAL){
        op_type = "<=";
    }
    else if(op->operator_type == Operator::OP_EQUAL){
        op_type = "=";
    }
    else if(op->operator_type == Operator::OP_GREATER){
        op_type = ">";
    }
    else if(op->operator_type == Operator::OP_GREATER_OR_EQUAL){
        op_type = ">=";
    }
    else if(op->operator_type == Operator::OP_NOT_EQUAL){
        op_type = "<>";
    }
    else if(op->operator_type == Operator::OP_PLUS){
        op_type = "+";
    }
    else if(op->operator_type == Operator::OP_MINUS){
        op_type = "-";
    }
    else if(op->operator_type == Operator::OP_MULTIPLY){
        op_type = "*";
    }
    else if(op->operator_type == Operator::OP_DEVIDE){
        op_type = "/";
    }
    else if(op->operator_type == Operator::OP_MOD){
        op_type = "mod";
    }

    return op_type.c_str();
}