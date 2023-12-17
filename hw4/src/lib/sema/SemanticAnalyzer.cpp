#ifndef MAX_LINE_LENG
#define MAX_LINE_LENG 512
#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#include <bits/stdc++.h>

using namespace std;
using PTypeSharedPtr = shared_ptr<PType>;
typedef PType::PrimitiveTypeEnum PTEnum;
int cur_level = 0;
int dim_not_match = 0;
int nextLevelUncheck = 0;
int lval_check_error = 0;
int para_mismatch_cast = 0;
int para_type_unmatch = 0;
int arr_ref_dim_mismatch = 0;
int under_array_subscript = 0;
int comp_not_bool_expr = 0;
const char * constOfVar = "";
extern int has_error;
extern uint32_t opt_dmp;
extern char *code[MAX_LINE_LENG];
string *function_return_type;
string *unary_operand;
pair<int, int> for_iter_range;
queue<string*> check_para_q;
vector<string> forLoopEntriesName;
stack<string> upperLevelParent;
stack<string*> assignTypeCompare;
stack<SymbolTable *> tempSymbolTableStack;

SymbolEntry::SymbolEntry(string * names, Kind kind, int level, string * type, string * attribute):
    names(names), kind(kind), level(level), type(type), attribute(attribute) {}

/*
 * Return top()-n layer of element
 * if Same, return 1, else return 0
 */
int compPrevNLayerElem(stack<string> st, int i, const char *src){
    std::stack<string> temp;
    while (!st.empty() && i >0){
        temp.push(st.top());
        st.pop();
        i--;
    }
    int ret = !strcmp( st.top().c_str(), src);
    while (!temp.empty()){
        st.push(temp.top());
        temp.pop();
    }
    return ret;
}

void SymbolTable::addSymbol(SymbolEntry *symbolEntry){
    entries.push_back(symbolEntry);
}

void dumpDemarcation(const char chr) {
  for (size_t i = 0; i < 110; ++i) {
    printf("%c", chr);
  }
  printf("\n");
}

string getKindString(Kind kind){
    if (kind == Kind::CONST) return "constant";
    else if (kind == Kind::FUNC) return "function";
    else if (kind == Kind::LOOPVAR) return "loop_var";
    else if (kind == Kind::PARA) return "parameter";
    else if (kind == Kind::PROG) return "program";
    else if (kind == Kind::VARS) return "variable";  
    else return "";
}

string getReigonString(int level){
    if (level == 0) return "(global)";
    else return "(local)";
}

void dumpSymbol(SymbolTable *table) {
    if (opt_dmp == 1){
        dumpDemarcation('=');
        //   for (int i = 0; i<110; i++) printf("=");
        printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type", "Attribute");
        dumpDemarcation('-');
        //   for (int i = 0; i<110; i++) printf("-");
        for(int i = 0; i < table->entries.size(); i++)
        {
            if(table != NULL && table->entries[i] != NULL){
                string kind = getKindString(table->entries[i]->kind);
                string reigon = getReigonString(table->entries[i]->level);

                printf("%-33s", table->entries[i]->names->c_str());
                printf("%-11s", kind.c_str());
                printf("%d%-10s", table->entries[i]->level, reigon.c_str());
                printf("%-17s", table->entries[i]->type->c_str());
                printf("%-11s\n", table->entries[i]->attribute->c_str());
                
            }
        }
        dumpDemarcation('-');
    }
}

void print_arrow(uint32_t col){
    for(int i = 1; i < col + 4; ++i) {
        std::cerr << " ";
    }
    std::cerr << "^\n";
}

SymbolTable * SemanticAnalyzer::SymbolManager::top(){
    return tables.top();
}

void SemanticAnalyzer::SymbolManager::pushScope(SymbolTable *new_scope){
    // TODO
    tables.push(new_scope);
}

void SemanticAnalyzer::SymbolManager::popScope(){
    // TODO
    tables.pop();
}

   /*
   Just some rules: 
    * 1. A scope corresponds to a symbol table. ProgramNode, FunctionNode, ForNode and CompoundStatementNode are nodes that form a scope. 
    *
    * 
    * 
    * 
    * 
    */

void SemanticAnalyzer::visit(ProgramNode &p_program) {

    //TODO:
    string name = p_program.getNameCString();
    //1. Push a new symbol table if this node forms a scope.
    SymbolTable *table = new SymbolTable();
    this->symbolManager.pushScope(table);
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    SymbolEntry *symbolEntry = new SymbolEntry((new string(p_program.getNameCString())), Kind::VARS, cur_level, (new string("void")), (new string("")));
    upperLevelParent.push("prog");
    this->current_entry = symbolEntry;
    this->current_table = table;
    this->current_table->addSymbol(symbolEntry);
    //3. Travere child nodes of this node.
    this->current_table->entries[0]->kind = Kind::PROG;
    p_program.visitChildNodes(*this);
    upperLevelParent.pop();
    //4. Perform semantic analyses of this node.
    // TODO
    //5. Pop the symbol table pushed at the 1st step.
    // this->current_table = this->symbolManager.top();
    symbolManager.popScope();
    dumpSymbol(table);
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    upperLevelParent.push("decl");
    p_decl.visitChildNodes(*this);
    upperLevelParent.pop();
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    int redeclared = 0;
    int index_not_greater_than_zero = 0;
    int result;

    // Check Current Table
    for (auto &entry : current_table->entries){
        result = strcmp(entry->names->c_str(),p_variable.getNameCString());
        // printf("DEBUG, entry->names->c_str() = %s, p_function.getNameCString = %s\n", entry->names->c_str(), p_variable.getNameCString());
        if(result == 0){
            current_error = 1;
            redeclared = 1;
            // has_error = 1;
            cerr << "<Error> Found in line " << p_variable.getLocation().line << ", column " << p_variable.getLocation().col 
            << ": symbol \'"  << p_variable.getNameCString() << "\' is redeclared\n";
            cerr << "    " <<  code[p_variable.getLocation().line] << endl;
            print_arrow(p_variable.getLocation().col);
            break;
        }
    }

    // Check tables from upper scope (It's safe )
    tempSymbolTableStack.push(symbolManager.top());
    symbolManager.popScope();
    // printf("DEBUG getPrevL: %s\n", *getPrevNLayerElem(upperLevelParent, 1));
    // [default] / [in inner scope] / [inner scope but i is loop_var]
    // while(!symbolManager.tables.empty() && ((!compPrevNLayerElem(upperLevelParent, 1, "compStmt") || compPrevNLayerElem(upperLevelParent, 2, "for")))){
    while(!symbolManager.tables.empty() ){
        tempSymbolTableStack.push(symbolManager.top());
        for (auto &entry : symbolManager.top()->entries){
            // printf("DEBUG, entry->names->c_str() = %s, p_function.getNameCString = %s\n", entry->names->c_str(), p_variable.getNameCString());
            if(!strcmp(entry->names->c_str(),p_variable.getNameCString()) && entry->kind == Kind::LOOPVAR){
                current_error = 1;
                redeclared = 1;
                // has_error = 1;
                cerr << "<Error> Found in line " << p_variable.getLocation().line << ", column " << p_variable.getLocation().col 
                << ": symbol \'"  << p_variable.getNameCString() << "\' is redeclared\n";
                cerr << "    " <<  code[p_variable.getLocation().line] << endl;
                print_arrow(p_variable.getLocation().col);
                break;
            }
        }
        symbolManager.popScope();
        // For access safety
    }
    while (!tempSymbolTableStack.empty())
    {
        symbolManager.pushScope(tempSymbolTableStack.top());
        tempSymbolTableStack.pop();
    }
    
    vector<uint64_t> dim = p_variable.getPTypeDimension();
    for (auto i : dim){
        // printf("DEBUG: has check, i is %d\n", i);
        if(i <= 0){
            has_error = 1;
            index_not_greater_than_zero = 1;
        }
    }

    // TODO:
    // 2. Insert the symbol into current symbol table if this node is related to
    //    declaration (ProgramNode, VariableNode, FunctionNode).
    // printf("%s\n", p_variable.getNameCType());
    if (!redeclared){
        Kind var_or_para = Kind::VARS;
        string upperTemp = upperLevelParent.top();
        upperLevelParent.pop();
        if (upperLevelParent.size() && upperLevelParent.top() == "function"){
            var_or_para = Kind::PARA;
        }
        if (upperLevelParent.size() && upperLevelParent.top() == "for"){
            var_or_para = Kind::LOOPVAR;
        }
        upperLevelParent.push(upperTemp);
        string *varName = new string(p_variable.getNameCString());

        // Do the dimension check 

        SymbolEntry *symbolEntry = new SymbolEntry(varName, var_or_para, cur_level, (new string(p_variable.getTypeCString())), (new string("")));
        if (index_not_greater_than_zero){
            symbolEntry->error =  1;
        }
        symbolEntry->dim = p_variable.getPTypeDimension().size();
        symbolEntry->ptype = p_variable.m_type;
        this->current_entry = symbolEntry;
        p_variable.visitChildNodes(*this);
        this->current_table->addSymbol(symbolEntry);
        // // 3. Travere child nodes of this node. 
        upperLevelParent.push("variable");
        p_variable.visitChildNodes(*this);
        upperLevelParent.pop();
        // // 4. Perform semantic analyses of this node.
        // // TODO
    }

    if (index_not_greater_than_zero){
        cerr << "<Error> Found in line " << p_variable.getLocation().line << ", column " << p_variable.getLocation().col 
                << ": \'"  << p_variable.getNameCString() << "\' declared as an array with an index that is not greater than 0\n";
        cerr << "    " <<  code[p_variable.getLocation().line] << endl;
        print_arrow(p_variable.getLocation().col);
    }

}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    // TODO:
    int arr_ref_is_int = 1;
    int func_para_mismatch = 0;
    string *correct_type;
    // 2. Insert the symbol into current symbol table if this node is related to
    //    declaration (ProgramNode, VariableNode, FunctionNode)
    // printf("DEBUG_INFO: %d\n", this->current_entry->kind);

    if (upperLevelParent.top() == "variable" && current_entry->kind == Kind::VARS){
        this->current_entry->kind = Kind::CONST;
        *(this->current_entry->attribute) = p_constant_value.getConstantValueCString();
    }

    if (upperLevelParent.top() == "varRef"){
        // printf("DEBUG: sharedptr %s\n", p_constant_value.getTypeSharedPtr()->getPTypeCString());
        if (!strcmp(p_constant_value.getTypeSharedPtr()->getPTypeCString(), "real")) {
            // printf("HIT");
            arr_ref_is_int = 0;
        }
    }

    if (!strcmp(upperLevelParent.top().c_str(), "for")){
        for_iter_range.second = atoi(p_constant_value.getConstantValueCString());
    }

    // Test case for loop (12)
    if (!strcmp(upperLevelParent.top().c_str(), "assign") && compPrevNLayerElem(upperLevelParent, 1, "for")) {
        for_iter_range.first = atoi(p_constant_value.getConstantValueCString());
    }


    p_constant_value.type = p_constant_value.getTypeSharedPtr();
    // 3. Travere child nodes of this node.
    // 4. Perform semantic analyses of this node.

    // if (upperLevelParent.top() == "assign"){
    //     assignTypeCompare.push(new string(p_constant_value.getTypeSharedPtr()->getPTypeCStringNoDim()));
    // }

    if (upperLevelParent.top() == "funcInvoke"){
        correct_type = check_para_q.front();
        if(strcmp(correct_type->c_str(), p_constant_value.getType())){
            func_para_mismatch = 1;
            para_mismatch_cast = 1;
        }
        check_para_q.pop();
    }

     // Error if propogate then don't need to error twice (case err)
    if (nextLevelUncheck == 0 && !arr_ref_is_int){
        has_error = 1;
        cerr << "<Error> Found in line " << p_constant_value.getLocation().line << ", column " << p_constant_value.getLocation().col 
        << ": index of array reference must be an integer\n";
        cerr << "    " <<  code[p_constant_value.getLocation().line] << endl;
        print_arrow(p_constant_value.getLocation().col);
    }

    if (func_para_mismatch){
        has_error = 1;
        cerr << "<Error> Found in line " << p_constant_value.getLocation().line << ", column " << p_constant_value.getLocation().col 
            << ": incompatible type passing \'" << p_constant_value.getType() << "\' to parameter of type \'" << *correct_type << "\'\n";
        cerr << "    " <<  code[p_constant_value.getLocation().line] << endl;
        print_arrow(p_constant_value.getLocation().col);
        func_para_mismatch = 0;
    }

    nextLevelUncheck = 0;
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    
    // TODO:
    int redeclared = 0;
    int result;
    string *retType = new string(p_function.getFunctionReturnType());
    function_return_type = retType;
    for (auto &entry : current_table->entries){
        if(!strcmp(entry->names->c_str(),p_function.getNameCString())){
            redeclared = 1;
        }
    }

    if (redeclared) {
        has_error = 1;
        cerr << "<Error> Found in line " << p_function.getLocation().line << ", column " << p_function.getLocation().col 
        << ": symbol \'"  << p_function.getNameCString() << "\' is redeclared\n";
        cerr << "    " <<  code[p_function.getLocation().line] << endl;
        print_arrow(p_function.getLocation().col);
    } 

    SymbolTable *table = new SymbolTable();
    // 1. Push a new symbol table if this node forms a scope.
    this->symbolManager.pushScope(table);
    // 2. Insert the symbol into current symbol table if this node is related to
    //    declaration (ProgramNode, VariableNode, FunctionNode).
    SymbolEntry *symbolEntry = new SymbolEntry((new string(p_function.getNameCString())), Kind::FUNC, cur_level, retType , (new string(p_function.getParametersTypeStringNoBrace())));

    if (redeclared == 0){
        current_entry = symbolEntry;
        current_entry->ptype = move(p_function.m_ret_type);
        current_table->addSymbol(symbolEntry); // add to the upper level
    } 
    
    // 3. Travere child nodes of this node.
    current_table = table;  // Child should add to the table this layer
    cur_level++;
    upperLevelParent.push("function");
    p_function.visitChildNodes(*this);
    upperLevelParent.pop();
    cur_level--;

    current_entry = symbolEntry;
    current_entry->dim = p_function.m_parameters.size();
    // 4. Perform semantic analyses of this node.
    // 5. Pop the symbol table pushed at the 1st step.
    this->symbolManager.popScope();
    dumpSymbol(table);
    current_table = this->symbolManager.top();

    // printf("DEBUG: ret_typ %s\n", p_function.getFunctionReturnType());
    
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    // TODO:
    if (upperLevelParent.top() == "function") {
        // The function case don't need new table
        upperLevelParent.push("compStmt");
        p_compound_statement.visitChildNodes(*this);
        upperLevelParent.pop();
    }
    else {
        // For Loop case need new table
        // 1. Push a new symbol table if this node forms a scop
        SymbolTable *table = new SymbolTable();
        this->symbolManager.pushScope(table);
        // 2. Insert the symbol into current symbol table if this node is related to
        //    declaration (ProgramNode, VariableNode, FunctionNode).
        // 3. Travere child nodes of this node.
        current_table = table;
        cur_level++;
        upperLevelParent.push("compStmt");
        p_compound_statement.visitChildNodes(*this);
        upperLevelParent.pop();
        cur_level--;
        // 4. Perform semantic analyses of this node.
        // 5. Pop the symbol table pushed at the 1st step.
        this->symbolManager.popScope();
        // if (current_error == 0)
            dumpSymbol(table);
        current_error = 0;
        current_table = this->symbolManager.top();
    }
}

void SemanticAnalyzer::visit(PrintNode &p_print) {

    // TODO:
    // 2. Insert the symbol into current symbol table if this node is related to
    //    declaration (ProgramNode, VariableNode, FunctionNode).

    // 3. Travere child nodes of this node.
    upperLevelParent.push("print");
    p_print.visitChildNodes(*this);
    upperLevelParent.pop();
    // 4. Perform semantic analyses of this node.
    if (under_array_subscript){
        has_error = 1;

    }
    // printf("DEBUG: %s\n", p_print)
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    int wrong_type = 0;
    const char *p;

    upperLevelParent.push("BinOP");
    p_bin_op.visitChildNodes(*this);
    upperLevelParent.pop();


    const char *op = p_bin_op.getOpCString();
    PTEnum left_operand_type = PTEnum::kErrorType;
    PTEnum right_operand_type = PTEnum::kErrorType;
    if(p_bin_op.m_left_operand->type)
        left_operand_type = p_bin_op.m_left_operand->type->getPrimitiveType();
    if (p_bin_op.m_right_operand->type)
        right_operand_type = p_bin_op.m_right_operand->type->getPrimitiveType();
    if (left_operand_type==PTEnum::kErrorType || right_operand_type==PTEnum::kErrorType)
    {
        shared_ptr<PType> temp(new PType(PTEnum::kErrorType));
        p_bin_op.type = temp;
        return;
    }
    
    // array<string, 4> arithmitic = {"+", "-", "*", "/"};
    // array<string, 3> boolop = {"and", "or", "not"};
    // array<string, 6> relop = {">=", "<=", "=", ">", "<", "<>"};
    if (!strcmp("+", op) || !strcmp("-",op) || !strcmp("*",op) || !strcmp("/", op)){
        // printf("DEBUG: %d %s %d\n", left_operand_type, op ,right_operand_type);
        if ((left_operand_type == PTEnum::kRealType && right_operand_type == PTEnum::kRealType)
        || (left_operand_type == PTEnum::kRealType && right_operand_type == PTEnum::kIntegerType)
        || (left_operand_type == PTEnum::kIntegerType && right_operand_type == PTEnum::kRealType)){
            shared_ptr<PType> temp(new PType(PTEnum::kRealType));
            p_bin_op.type = temp;
        }
        else if (left_operand_type == PTEnum::kIntegerType && right_operand_type == PTEnum::kIntegerType){
            shared_ptr<PType> temp(new PType(PTEnum::kIntegerType));
            p_bin_op.type = temp;
        }
        else if (left_operand_type == PTEnum::kStringType && right_operand_type == PTEnum::kStringType && !strcmp("+", op)){
            shared_ptr<PType> temp(new PType(PTEnum::kStringType));
            p_bin_op.type = temp;
        }
        else{
            shared_ptr<PType> temp(new PType(PTEnum::kErrorType));
            p_bin_op.type = temp;
            wrong_type = 1;
        }

        if (upperLevelParent.top() == "if" && !wrong_type){
            comp_not_bool_expr = 1;
        }
    }
    else if (!strcmp(op, "mod")){
        if (left_operand_type == PTEnum::kIntegerType && right_operand_type == PTEnum::kIntegerType){
            // shared_ptr<PType> temp(new PType(PTEnum::kIntegerType));
            p_bin_op.type = *(new shared_ptr<PType>(new PType(PTEnum::kIntegerType)));
        } 
        else{
            shared_ptr<PType> temp(new PType(PTEnum::kErrorType));
            p_bin_op.type = temp;
            wrong_type = 1;
        }
    }
    else if (!strcmp("and", op) ||  !strcmp("or", op) ||  !strcmp("not", op) ){
        // printf("DEBUG: %d %s %d\n", left_operand_type, op ,right_operand_type);
        if (left_operand_type == PTEnum::kBoolType && right_operand_type == PTEnum::kBoolType){
            shared_ptr<PType> temp(new PType(PTEnum::kBoolType));
            p_bin_op.type = temp;
        } 
        else{
            shared_ptr<PType> temp(new PType(PTEnum::kErrorType));
            p_bin_op.type = temp;
            wrong_type = 1;
        }
    }
    else if (!strcmp(">=", op) ||  !strcmp("<=", op) ||  !strcmp("=", op) || !strcmp(">", op) ||  !strcmp("<", op) ||  !strcmp("<>", op)){
        // printf("DEBUG: %d %s %d\n", left_operand_type, op ,right_operand_type);
        if ((left_operand_type == PTEnum::kIntegerType && right_operand_type == PTEnum::kIntegerType)
        || (left_operand_type == PTEnum::kRealType && right_operand_type == PTEnum::kIntegerType)
        || (left_operand_type == PTEnum::kIntegerType && right_operand_type == PTEnum::kRealType)
        || (left_operand_type == PTEnum::kIntegerType && right_operand_type == PTEnum::kIntegerType)){
            shared_ptr<PType> temp(new PType(PTEnum::kBoolType));
            p_bin_op.type = temp;
        }
        else{
            shared_ptr<PType> temp(new PType(PTEnum::kErrorType));
            p_bin_op.type = temp;
            wrong_type = 1;
        }
    }
    else{
        shared_ptr<PType> temp(new PType(PTEnum::kErrorType));
        p_bin_op.type = temp;
        wrong_type = 1;
    }

    int left_dimension = 0;
    int right_dimension = 0;
    if (p_bin_op.m_left_operand->type->getDimension().size())
        left_dimension += 1;
    if (p_bin_op.m_right_operand->type->getDimension().size())
        right_dimension += 1;
    // printf("DEBUG: %s, %s\n", p_bin_op.m_left_operand->type->getPTypeCString(), p_bin_op.m_left_operand->type->getPTypeCStringNoDim());
    string *left_type = new string(p_bin_op.m_left_operand->type->getPTypeCStringNoDim());
    string *right_type = new string(p_bin_op.m_right_operand->type->getPTypeCStringNoDim());

    // if (p_bin_op.m_left_operand->type->ref_dim_mismatch){}

    *left_type = "\'" + *left_type;
    if (p_bin_op.m_left_operand->ref_dim_mismatch){
        *left_type += " [1]";
    }
    *left_type += "\'";
    
    *right_type = "\'" + *right_type;
    if(p_bin_op.m_right_operand->ref_dim_mismatch){
        *right_type += " [1]";
    }
    *right_type += "\'";

    if(p_bin_op.m_left_operand->ref_dim_mismatch || p_bin_op.m_right_operand->ref_dim_mismatch){
        shared_ptr<PType> temp(new PType(PTEnum::kErrorType));
        p_bin_op.type = temp;
        wrong_type = 1;
    }
    
    //TODO:
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).

    //3. Travere child nodes of this node.
    //4. Perform semantic analyses of this node.
    if (wrong_type){
        has_error = 1;
        cerr << "<Error> Found in line " << p_bin_op.getLocation().line << ", column " << p_bin_op.getLocation().col 
                << ": invalid operands to binary operator \'" << op << "\' (" + *left_type + " and " + *right_type + ")\n";
        cerr << "    " <<  code[p_bin_op.getLocation().line] << endl;
        print_arrow(p_bin_op.getLocation().col);
        wrong_type = 0;
        arr_ref_dim_mismatch = 0;
    }
    
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {

    string *op = new string(p_un_op.getOpCString());
    //TODO:
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    //3. Travere child nodes of this node.
    upperLevelParent.push("unOP");
    p_un_op.visitChildNodes(*this);
    upperLevelParent.pop();

    shared_ptr<ExpressionNode> ptr = move(p_un_op.m_operand);
    p_un_op.type = ptr->type;
    
    //4. Perform semantic analyses of this node.
    if (under_array_subscript) {
        has_error = 1;
        cerr << "<Error> Found in line " << p_un_op.getLocation().line << ", column " << p_un_op.getLocation().col 
                << ": invalid operand to unary operator \'" << *op << "\' (\'" + *unary_operand + "\')\n";
        cerr << "    " <<  code[p_un_op.getLocation().line] << endl;
        print_arrow(p_un_op.getLocation().col);
    }
    unary_operand->clear();
    under_array_subscript = 0;
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    int use_of_undeclared = 1;
    int use_of_non_func = 0;

    for (auto &entry : current_table->entries){
        if (!strcmp(entry->names->c_str(), p_func_invocation.getNameCString())){
            current_entry = entry;
            use_of_undeclared = 0;
        }
    }

    // Find in Outer Scope, if we can't find entry in current table
    if (use_of_undeclared == 1){
        tempSymbolTableStack.push(symbolManager.top());
        symbolManager.popScope();
        while(!symbolManager.tables.empty()){
            tempSymbolTableStack.push(symbolManager.top());
            for (auto &entry : symbolManager.top()->entries){
                // printf("DEBUG, entry->names->c_str() = %s, p_function.getNameCString = %s\n", entry->names->c_str(), p_variable.getNameCString());
                if(!strcmp(entry->names->c_str(),p_func_invocation.getNameCString())){
                    use_of_undeclared = 0;
                    current_entry = entry;
                }
            }
        symbolManager.popScope();
        // For access safety
        }

        while (!tempSymbolTableStack.empty())
        {
            symbolManager.pushScope(tempSymbolTableStack.top());
            tempSymbolTableStack.pop();
        }
    }

    //TODO:
    // !!! git this expression a PType attribute!!!
    if (!use_of_undeclared){
        p_func_invocation.type = current_entry->ptype;

        if (current_entry->kind != Kind::FUNC)
            use_of_non_func = 1;
                        

        // printf("DEBUG: %s, %d, %d\n",current_entry->names->c_str(), current_entry->dim ,p_func_invocation.m_args.size());
        if (current_entry->dim != p_func_invocation.m_args.size())
            dim_not_match = 1;
    }

    if (!use_of_undeclared && !use_of_non_func && !dim_not_match){

        char *attr = strdup(current_entry->attribute->c_str());
        char *tok;
        tok = strtok(attr, ", ");
        while (tok != NULL)
        {
            check_para_q.push(new string(tok));
            tok = strtok(NULL, ", ");
        }
        free(attr);

        //2. Insert the symbol into current symbol table if this node is related to
        //   declaration (ProgramNode, VariableNode, FunctionNode).
        //3. Travere child nodes of this node.
        upperLevelParent.push("funcInvoke");
        p_func_invocation.visitChildNodes(*this);
        upperLevelParent.pop();
        //4. Perform semantic analyses of this node.
    }

    if (upperLevelParent.top() == "print" && !use_of_undeclared && !use_of_non_func && !dim_not_match && !para_mismatch_cast){
        has_error = 1;
        cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
                << ": expression of print statement must be scalar type\n";
        cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
        print_arrow(p_func_invocation.getLocation().col);
        para_mismatch_cast = 0;
    }

    if (use_of_undeclared){
        has_error = 1;
        cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
            << ": use of undeclared symbol \'"  << p_func_invocation.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
        print_arrow(p_func_invocation.getLocation().col);
    } else {
        if (use_of_non_func){
            has_error = 1;
            cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
                << ": call of non-function symbol \'"  << p_func_invocation.getNameCString() << "\'\n";
            cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
            print_arrow(p_func_invocation.getLocation().col);
        }
        else if (dim_not_match){
            has_error = 1;
            current_entry->error = 1;
            cerr << "<Error> Found in line " << p_func_invocation.getLocation().line << ", column " << p_func_invocation.getLocation().col 
                << ": too few/much arguments provided for function \'"  << p_func_invocation.getNameCString() << "\'\n";
            cerr << "    " <<  code[p_func_invocation.getLocation().line] << endl;
            print_arrow(p_func_invocation.getLocation().col);
            dim_not_match = 0;
        }
    }
    while(!check_para_q.empty()){
        check_para_q.pop();
    }
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    // Test Case 3: array ref
    int use_of_undeclared = 1;
    int use_of_non_variable = 0;
    int index_of_array_reference = 0;
    int over_array_subscript = 0;
    int func_para_mismatch = 0;
    int arr_assign = 0;
    string *correct_type;
    

/*
 * Phase: Check if the entry exitst
 */
    // In current table
    // printf("%d ", cur_level);
    for (auto &entry : current_table->entries){
        if (!strcmp(entry->names->c_str(), p_variable_ref.getNameCString())){
            current_entry = entry;
            use_of_undeclared = 0;
            // printf("DEBUG: currentTable %s, %s, %d\n", current_entry->names->c_str(), current_entry->type->c_str(), current_entry->level);
            break;
        }
    }


    // Find in Outer Scope, if we can't find entry in current table
    // New way: use the candidate mode
    if (use_of_undeclared == 1){
        tempSymbolTableStack.push(symbolManager.top());
        symbolManager.popScope();
        while(!symbolManager.tables.empty()){
            tempSymbolTableStack.push(symbolManager.top());
            for (auto &entry : symbolManager.top()->entries){
                // printf("DEBUG, e->n->c() = %s, p_CString = %s\n", entry->names->c_str(), p_variable_ref.getNameCString());
                if(!strcmp(entry->names->c_str(),p_variable_ref.getNameCString())){
                    use_of_undeclared = 0;
                    current_entry = entry;
                    break;
                }
            }
            symbolManager.popScope();
            if (use_of_undeclared == 0) break;
        // For access safety
        }

        while (!tempSymbolTableStack.empty())
        {
            symbolManager.pushScope(tempSymbolTableStack.top());
            tempSymbolTableStack.pop();
        }
        // printf("DEBUG: outerTable %s, %s, %d\n", current_entry->names->c_str(), current_entry->type->c_str(), current_entry->level);

    }

// End of undeclared check

/*
 * Phase: Check
 * 1. nextLevelUncheck: Already error then no error again
 * 2. use_of_non_variable: arrref type of func/prog not allowed
 * 3. over_array_subscript: dimension not match
 */

    if (current_entry->error) {
        nextLevelUncheck = 1;
    }

    if (!use_of_undeclared){
        // Then we know the refrence type
        p_variable_ref.type = current_entry->ptype;

        // printf("Debug: %s\n", entry->type->c_str());
        if (current_entry->kind == Kind::FUNC || current_entry->kind == Kind::PROG){
            use_of_non_variable = 1;
        } else {
            if (current_entry->dim < p_variable_ref.getArrdim()){
                // printf("DEBUG: %d < %d, %s\n", current_entry->dim,p_variable_ref.getArrdim(),p_variable_ref.getNameCString() );
                p_variable_ref.ref_dim_mismatch = true;
                over_array_subscript = 1;
            }
            if (current_entry->dim > p_variable_ref.getArrdim()){
                // printf("DEBUG: %d > %d, %s\n", current_entry->dim,p_variable_ref.getArrdim(),p_variable_ref.getNameCString() );
                p_variable_ref.ref_dim_mismatch = true;
                under_array_subscript = 1;
            }
        }
    }

    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    //3. Travere child nodes of this node.
    upperLevelParent.push("varRef");
    p_variable_ref.visitChildNodes(*this);
    upperLevelParent.pop();
    // if(!strcmp(upperLevelParent.top().c_str(), "return"))
    //     printf("DEBUG: %s, %s\n", function_return_type->c_str(), current_entry->type->c_str());
    //4. Perform semantic analyses of this node.
    // if (upperLevelParent.top() == "assign"){
    //     assignTypeCompare.push(new string(p_variable_ref.type->getPTypeCStringNoDim()));
    // }

    if (upperLevelParent.top() == "unOP")
        unary_operand = new string(p_variable_ref.getType());
    // printf("DEBUG: at %s\n",upperLevelParent.top().c_str());
    if (upperLevelParent.top() == "funcInvoke"){
        correct_type = check_para_q.front();
        if(strcmp(correct_type->c_str(), p_variable_ref.getType())){
            func_para_mismatch = 1;
        }
        check_para_q.pop();
    }
    // Handle assignment cases
    // if (current_entry->kind == Kind::CONST && upperLevelParent.top() == "assign")

    if(use_of_undeclared){
        has_error = 1;
        cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
            << ": use of undeclared symbol \'"  << p_variable_ref.getNameCString() << "\'\n";
        cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
        print_arrow(p_variable_ref.getLocation().col);
    } else {
        if(use_of_non_variable){
            has_error = 1;
            cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                << ": use of non-variable symbol \'"  << p_variable_ref.getNameCString() << "\'\n";
            cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
            print_arrow(p_variable_ref.getLocation().col);
        }
        if(over_array_subscript){
            has_error = 1;
            cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
            << ": there is an over array subscript on \'"  << p_variable_ref.getNameCString() << "\'\n";
            cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
            print_arrow(p_variable_ref.getLocation().col);
        }
        if (under_array_subscript){
            if(!strcmp(upperLevelParent.top().c_str(), "print")){
                has_error = 1;
                cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                        << ": expression of print statement must be scalar type\n";
                cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
                print_arrow(p_variable_ref.getLocation().col);
                under_array_subscript = 0;
            }
            if(!strcmp(upperLevelParent.top().c_str(), "read")){
                has_error = 1;
                cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                        << ": variable reference of read statement must be scalar type\n";
                cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
                print_arrow(p_variable_ref.getLocation().col);
                under_array_subscript = 0;
            }
            if(!strcmp(upperLevelParent.top().c_str(), "return")){
                has_error = 1;
                // This is suck ==
                string retTypeString = string(p_variable_ref.getType());
                retTypeString.erase(retTypeString.find("[10]"), 4);
                cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                    << ": return \'" << retTypeString << "\' from a function with return type \'" << *function_return_type << "\'\n";
                cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
                print_arrow(p_variable_ref.getLocation().col);
                under_array_subscript = 0;
            } 
        }
        // TODO: Test Case 14 but line 23 error unsolved
        if (!over_array_subscript && !under_array_subscript){
            if (!p_variable_ref.getArrdim() && !strcmp(upperLevelParent.top().c_str(), "return") && (strcmp(function_return_type->c_str(), p_variable_ref.getType()))){
                has_error = 1;
                cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                    << ": return \'" << p_variable_ref.getType() << "\' from a function with return type \'" << *function_return_type << "\'\n";
                cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
                print_arrow(p_variable_ref.getLocation().col);
            }
        }
        if((current_entry->kind == Kind::LOOPVAR || current_entry->kind == Kind::CONST) && !strcmp(upperLevelParent.top().c_str(), "read")){
            has_error = 1;
            cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                    << ": variable reference of read statement cannot be a constant or loop variable\n";
            cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
            print_arrow(p_variable_ref.getLocation().col);
        }
        if (func_para_mismatch){
            has_error = 1;
            cerr << "<Error> Found in line " << p_variable_ref.getLocation().line << ", column " << p_variable_ref.getLocation().col 
                << ": incompatible type passing \'" << p_variable_ref.getType() << "\' to parameter of type \'" << *correct_type << " [4]\'\n";
            cerr << "    " <<  code[p_variable_ref.getLocation().line] << endl;
            print_arrow(p_variable_ref.getLocation().col);
            // TODO: hey! this is cheating!
        }
    }
}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    //TODO:
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    //3. Travere child nodes of this node.
    int has_ltype = 0;
    int lentry_is_const = 0;
    int modify_loop_var = 0;
    upperLevelParent.push("assign");
    p_assignment.visitChildNodes(*this);
    upperLevelParent.pop();

    SymbolEntry *lEntry;

    for (auto &entry : current_table->entries){
        if (!strcmp(p_assignment.m_lvalue->getNameCString(), entry->names->c_str())){
            has_ltype = 1;
            lEntry = entry;
            // printf("DEBUG: currentTable %s, %s, %d\n", current_entry->names->c_str(), current_entry->type->c_str(), current_entry->level);
            break;
        }
    }

    lentry_is_const = lEntry->kind == Kind::CONST;
    modify_loop_var = (upperLevelParent.top() == "compStmt" && compPrevNLayerElem(upperLevelParent, 1, "for"));

    if (lentry_is_const){
        has_error = 1;
        cerr << "<Error> Found in line " << p_assignment.m_lvalue->getLocation().line << ", column " << p_assignment.m_lvalue->getLocation().col 
            << ": cannot assign to variable \'" << p_assignment.m_lvalue->getNameCString() << "\' which is a constant\n";
        cerr << "    " <<  code[p_assignment.m_lvalue->getLocation().line] << endl;
        print_arrow(p_assignment.m_lvalue->getLocation().col);
    }
    else if (lEntry->dim && p_assignment.m_lvalue->ref_dim_mismatch || p_assignment.m_expr->ref_dim_mismatch){
        if (p_assignment.m_lvalue->ref_dim_mismatch) {
            has_error = 1;
            cerr << "<Error> Found in line " << p_assignment.getLocation().line << ", column " << p_assignment.m_lvalue->getLocation().col 
                << ": array assignment is not allowed\n";
            cerr << "    " <<  code[p_assignment.getLocation().line] << endl;
            print_arrow(p_assignment.m_lvalue->getLocation().col);
        } else {
            has_error = 1;
            cerr << "<Error> Found in line " << p_assignment.getLocation().line << ", column " << p_assignment.m_expr->getLocation().col 
                << ": array assignment is not allowed\n";
            cerr << "    " <<  code[p_assignment.getLocation().line] << endl;
            print_arrow(p_assignment.m_expr->getLocation().col);
        }
    }
    // assign loop variable
    else if (modify_loop_var){
        has_error = 1;
        cerr << "<Error> Found in line " << p_assignment.getLocation().line << ", column " << p_assignment.m_lvalue->getLocation().col 
            << ": the value of loop variable cannot be modified inside the loop body\n";
        cerr << "    " <<  code[p_assignment.getLocation().line] << endl;
        print_arrow(p_assignment.m_lvalue->getLocation().col);
    }
    else if (!modify_loop_var && has_ltype && p_assignment.m_expr->type && lEntry->ptype){
        if ((lEntry->ptype->getPrimitiveType() == PTEnum::kIntegerType && p_assignment.m_expr->type->getPrimitiveType() == PTEnum::kRealType)
        || (lEntry->ptype->getPrimitiveType() == PTEnum::kRealType && p_assignment.m_expr->type->getPrimitiveType() == PTEnum::kIntegerType)
        || (lEntry->ptype->getPrimitiveType() == PTEnum::kIntegerType && p_assignment.m_expr->type->getPrimitiveType() == PTEnum::kIntegerType)
        || (lEntry->ptype->getPrimitiveType() == PTEnum::kRealType && p_assignment.m_expr->type->getPrimitiveType() == PTEnum::kRealType)){
            // Valid casting
        } else {
            has_error = 1;
            cerr << "<Error> Found in line " << p_assignment.getLocation().line << ", column " << p_assignment.getLocation().col 
                << ": assigning to \'" << p_assignment.m_lvalue->type->getPTypeCStringNoDim() << "\' from incompatible type \'" << p_assignment.m_expr->type->getPTypeCStringNoDim() << "\'\n";
            cerr << "    " <<  code[p_assignment.getLocation().line] << endl;
            print_arrow(p_assignment.getLocation().col);
        }
        // printf("COMP: %s, %s\n", p_assignment.m_expr->type->getPTypeCStringNoDim(), lEntry->ptype->getPTypeCStringNoDim());
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    //TODO:
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    //3. Travere child nodes of this node.
    upperLevelParent.push("read");
    p_read.visitChildNodes(*this);
    upperLevelParent.pop();
    //4. Perform semantic analyses of this node.
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    //TODO:
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    //3. Travere child nodes of this node.
    upperLevelParent.push("if");
    p_if.visitChildNodes(*this);
    upperLevelParent.pop();

    if (comp_not_bool_expr){
        has_error = 1;
        cerr << "<Error> Found in line " << p_if.getLocation().line << ", column " << p_if.m_condition->getLocation().col 
            << ": the expression of condition must be boolean type\n";
        cerr << "    " <<  code[p_if.getLocation().line] << endl;
        print_arrow(p_if.m_condition->getLocation().col);
        comp_not_bool_expr = 0;
    }
    //4. Perform semantic analyses of this node.
}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    //TODO:
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    //3. Travere child nodes of this node.
    upperLevelParent.push("while");
    p_while.visitChildNodes(*this);
    upperLevelParent.pop();
    //4. Perform semantic analyses of this node.
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    int decremental_bound = 0;
    // TODO:
    // 1. Push a new symbol table if this node forms a scope.
    SymbolTable *table = new SymbolTable();
    this->symbolManager.pushScope(table);
    // 2. Insert the symbol into current symbol table if this node is related to
    //    declaration (ProgramNode, VariableNode, FunctionNode).
    // 3. Travere child nodes of this node.
    current_table = table;
    cur_level++;
    upperLevelParent.push("for");
    p_for.visitChildNodes(*this);
    upperLevelParent.pop();
    cur_level--;
    // 4. Perform semantic analyses of this node.
    if (for_iter_range.first > for_iter_range.second)
        decremental_bound = 1;
    // 5. Pop the symbol table pushed at the 1st step.
    dumpSymbol(table); // Not sure for loop has a symtable
    this->symbolManager.popScope();
    current_table = this->symbolManager.top();
    if (decremental_bound){
        has_error = 1;
        cerr << "<Error> Found in line " << p_for.getLocation().line << ", column " << p_for.getLocation().col 
            << ": the lower bound and upper bound of iteration count must be in the incremental order\n";
        cerr << "    " <<  code[p_for.getLocation().line] << endl;
        print_arrow(p_for.getLocation().col);
    }
    decremental_bound = 0;
    for_iter_range.first = 0;
    for_iter_range.second = 0;
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    int void_ret = 1;
    // TODO:
    //2. Insert the symbol into current symbol table if this node is related to
    //   declaration (ProgramNode, VariableNode, FunctionNode).
    //3. Travere child nodes of this node.
    upperLevelParent.push("return");
    p_return.visitChildNodes(*this);
    upperLevelParent.pop();

    // printf("DEBUUG: current_level %d\n", cur_level);

    // printf("DEBUG: ret typ %s\n", function_return_type->c_str());
    //4. Perform semantic analyses of this node.
    if (compPrevNLayerElem(upperLevelParent, 1, "prog") || !strcmp("void", function_return_type->c_str())) {
        has_error = 1;
        cerr << "<Error> Found in line " << p_return.getLocation().line << ", column " << p_return.getLocation().col 
            << ": program/procedure should not return a value\n";
        cerr << "    " <<  code[p_return.getLocation().line] << endl;
        print_arrow(p_return.getLocation().col);
    }
}

#endif