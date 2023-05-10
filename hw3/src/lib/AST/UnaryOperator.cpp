#include "AST/UnaryOperator.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                                     Op_infos *op, AstNode *operand)
    : ExpressionNode{line, col}, op(op), operand(operand) {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    // TODO
    if(operand != NULL){
        operand->accept(p_visitor);
    }
}

const char *UnaryOperatorNode::getNameCType(){
    std::string op_type;
    if(op->operator_type == Operator::OP_MINUS){
        op_type = "neg";
    }
    else if(op->operator_type == Operator::OP_NOT){
        op_type = "not";
    }
    return op_type.c_str();
}
