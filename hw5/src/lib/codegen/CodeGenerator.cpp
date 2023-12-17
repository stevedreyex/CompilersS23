#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"
#include "sema/SymbolTable.hpp"
#include<bits/stdc++.h>

using namespace std;
using Entries = vector<std::unique_ptr<SymbolEntry>>;

// stack<string> upperLevelParent;

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

CodeGenerator::CodeGenerator(const std::string source_file_name,
                             const std::string save_path,
                             const SymbolManager *const p_symbol_manager)
    : m_symbol_manager_ptr(p_symbol_manager),
      m_source_file_path(source_file_name) {
    // FIXME: assume that the source file is always xxxx.p
    const std::string &real_path =
        (save_path == "") ? std::string{"."} : save_path;
    auto slash_pos = source_file_name.rfind("/");
    auto dot_pos = source_file_name.rfind(".");

    if (slash_pos != std::string::npos) {
        ++slash_pos;
    } else {
        slash_pos = 0;
    }
    std::string output_file_path(
        real_path + "/" +
        source_file_name.substr(slash_pos, dot_pos - slash_pos) + ".S");
    m_output_file.reset(fopen(output_file_path.c_str(), "w"));
    assert(m_output_file.get() && "Failed to open output file");
}

static void dumpInstructions(FILE *p_out_file, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(p_out_file, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // Generate RISC-V instructions for program header
    // clang-format off
    constexpr const char*const riscv_assembly_file_prologue =
        "    .file \"%s\"\n"
        "    .option nopic\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_prologue,
                     m_source_file_path.c_str());

    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    this->realTablePtr.push(const_cast<SymbolTable *>(p_program.getSymbolTable()));
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_program.getSymbolTable());

    vector<const SymbolEntry *> bssEntries;
    vector<const SymbolEntry *> rodataEntries;


    /*
     * Find bss and rodata
     */
    for(auto &entry : p_program.getSymbolTable()->getEntries()){
        if (entry->getKind() == SymbolEntry::KindEnum::kConstantKind) {
            rodataEntries.push_back(m_symbol_manager_ptr->lookup(entry->getName()));
            this->gloablVariableTable.push_back(entry->getName());
        }
        else if (entry->getKind() == SymbolEntry::KindEnum::kVariableKind) {
            bssEntries.push_back(m_symbol_manager_ptr->lookup(entry->getName()));
            this->gloablVariableTable.push_back(entry->getName());
        }
    }

    string *outputGlobals = new string();
    
    /*
     * Output .bssEntries
     */
    if (bssEntries.size()){
        for (auto &entry : bssEntries) {
            // clang-format off
            outputGlobals->clear();
            *outputGlobals += ".comm %s, %d, 4\n"; // align 4 
            int size = 4;
            if (!entry->getTypePtr()->isScalar()){
                for(auto dim : entry->getTypePtr()->getDimensions()){
                    size *= dim;
                }
            }
            // clang-format on
            dumpInstructions(m_output_file.get(), outputGlobals->c_str(),
                        entry->getNameCString(), size);
        }
    }

    /*
     * Output .rodata
     */
    if (rodataEntries.size()){
        /*TODO*/
        outputGlobals->clear();
        // clang-format off
        *outputGlobals += ".section    .rodata       # emit rodata section\n"
        "    .align 2\n";
        // clang-format on
        dumpInstructions(m_output_file.get(), outputGlobals->c_str());
        for (auto &entry : rodataEntries) {
            outputGlobals->clear();
            // clang-format off
            *outputGlobals += "    .globl %s              # emit symbol '%s' to the global symbol table\n"
            "    .type %s, @object\n"
            "%s:\n"
            "    .word %s\n";
            // clang-format on
            dumpInstructions(m_output_file.get(), outputGlobals->c_str(),
                        entry->getNameCString(), entry->getNameCString(), entry->getNameCString(), entry->getNameCString(),
                        entry->getAttribute().constant()->getConstantValueCString());
        }
    }

/* Note: instead of constexpr const char*const can I just use char* ? */

    auto visit_ast_node = [&](auto &ast_node) { ast_node->accept(*this); };

    this->upperLevelParent.push("program");
    // I think the Traverse of Decl in program node is trivial since we have the SymbloTable
    // for_each(p_program.getDeclNodes().begin(), p_program.getDeclNodes().end(),
    //          visit_ast_node);
    
    // But the function part we still have to construct.
    // Start .text section #function
    if (p_program.getFuncNodes().size())
        dumpInstructions(m_output_file.get(), ".section    .text\n    .align 2\n");
    for_each(p_program.getFuncNodes().begin(), p_program.getFuncNodes().end(),
             visit_ast_node);

    
    // Start .text section #main
    constexpr const char*const riscv_assembly_main_prologue =
        ".section    .text\n"
        "    .align 2\n"
        "    .globl main          # emit symbol 'main' to the global symbol table\n"
        "    .type main, @function\n"
        "main:\n"
        "    # in the function prologue\n"
        "    addi sp, sp, -128    # move stack pointer to lower address to allocate a new stack\n"
        "    sw ra, 124(sp)       # save return address of the caller function in the current stack\n"
        "    sw s0, 120(sp)       # save frame pointer of the last stack in the current stack\n"
        "    addi s0, sp, 128     # move frame pointer to the bottom of the current stack\n\n";
    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_main_prologue,
                     m_source_file_path.c_str());

    const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);

    this->upperLevelParent.pop();

    // clang-format off
    constexpr const char*const riscv_assembly_file_epilogue =
    "    lw ra, 124(sp)       # load return address saved in the current stack\n"
    "    lw s0, 120(sp)       # move frame pointer back to the bottom of the last stack\n"
    "    addi sp, sp, 128     # move stack pointer back to the top of the last stack\n"
    "    jr ra                # jump back to the caller function\n"
    "    .size main, .-main\n";

    // clang-format on
    dumpInstructions(m_output_file.get(), riscv_assembly_file_epilogue,
                     m_source_file_path.c_str());
    // Remove the entries in the hash table
    this->realTablePtr.pop();
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_program.getSymbolTable());
}

void CodeGenerator::visit(DeclNode &p_decl) {
    // this->upperLevelParent.push("declNode");
    p_decl.visitChildNodes(*this);
    // this->upperLevelParent.pop();
}

void CodeGenerator::visit(VariableNode &p_variable) {
    if (this->upperLevelParent.top() == "compStmt" && p_variable.getConstantPtr()){
        int tableOffset = this->tableLookup(p_variable.getNameCString()) + 1;
        dumpInstructions(m_output_file.get(), 
        "    # load the local address of '%s' and push the address to the stack\n"
        "    addi t0, s0, %d\n"
        "    addi sp, sp, -4\n"
        "    sw t0, 0(sp)\n", p_variable.getNameCString(), (tableOffset + 2) * -4);
        p_variable.visitChildNodes(*this);
        dumpInstructions(m_output_file.get(), 
        "    # Assign the value to '%s'\n"
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    lw t1, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    sw t0, 0(t1)\n\n"
        , p_variable.getNameCString());
    }
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    dumpInstructions(m_output_file.get(), 
    "    # push constant value to the stack\n"
    "    li t0, %s%s\n"
    "    addi sp, sp, -4\n"
    "    sw t0, 0(sp)\n", this->negOpVal ? "-" : "" , p_constant_value.getConstantValueCString());
}

void CodeGenerator::visit(FunctionNode &p_function) {

    string *function_prologue = new string(
    // clang-format off
        "    .globl %s          # emit symbol '%s' to the global symbol table\n"
        "    .type %s, @function\n"
        "%s:\n"
        "    # in the function prologue\n"
        "    addi sp, sp, -128    # move stack pointer to lower address to allocate a new stack\n"
        "    sw ra, 124(sp)       # save return address of the caller function in the current stack\n"
        "    sw s0, 120(sp)       # save frame pointer of the last stack in the current stack\n"
        "    addi s0, sp, 128     # move frame pointer to the bottom of the current stack\n");
    // clang-format on
    dumpInstructions(m_output_file.get(), function_prologue->c_str(), p_function.getNameCString(), p_function.getNameCString(),
    p_function.getNameCString(), p_function.getNameCString());

    // Reconstruct the hash table for looking up the symbol entry
    this->realTablePtr.push(const_cast<SymbolTable *>(p_function.getSymbolTable()));
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_function.getSymbolTable());

    // Save parameter to the local stack
    int argsCnt = 0;
    for(auto &entry : p_function.getSymbolTable()->getEntries()){
        if (entry->getKind() == SymbolEntry::KindEnum::kParameterKind) {
            if (argsCnt <= 7) 
                dumpInstructions(m_output_file.get(), "    sw a%d, %d(s0)        # save parameter '%s' in the local stack\n", argsCnt, (argsCnt + 3)*-4, entry->getNameCString());
            else 
                dumpInstructions(m_output_file.get(), "    sw t%d, %d(s0)        # spilled parameter '%s' in the local stack\n", 11-argsCnt, (argsCnt + 3)*-4, entry->getNameCString());
            argsCnt++;
        }
    }

    this->upperLevelParent.push("function");
    this->inFunction = 1;
    p_function.visitChildNodes(*this);
    this->inFunction = 0;
    this->upperLevelParent.pop();

    // Remove the entries in the hash table
    this->realTablePtr.pop();
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_function.getSymbolTable());

    string *function_epilogue = new string(
    // clang-format off
    "    lw ra, 124(sp)       # load return address saved in the current stack\n"
    "    lw s0, 120(sp)       # move frame pointer back to the bottom of the last stack\n"
    "    addi sp, sp, 128     # move stack pointer back to the top of the last stack\n"
    "    jr ra                # jump back to the caller function\n"
    "    .size %s, .-%s\n");
    // clang-format on
    dumpInstructions(m_output_file.get(), function_epilogue->c_str(), p_function.getNameCString(), p_function.getNameCString());
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {
    // Reconstruct the hash table for looking up the symbol entry
    if (this->upperLevelParent.top() == "program"){
        this->realTablePtr.push(const_cast<SymbolTable *>(p_compound_statement.getSymbolTable()));
    }

    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_compound_statement.getSymbolTable());

    this->upperLevelParent.push("compStmt");
    p_compound_statement.visitChildNodes(*this);
    this->upperLevelParent.pop();

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(
        p_compound_statement.getSymbolTable());

    if (this->upperLevelParent.top() == "program"){
        this->realTablePtr.pop();
    }
}

void CodeGenerator::visit(PrintNode &p_print) {
    this->upperLevelParent.push("print");
    p_print.visitChildNodes(*this);
    this->upperLevelParent.pop();
    dumpInstructions(m_output_file.get(),
        "    # print, place a0\n"
        "    lw a0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    jal ra, printInt      # call function 'printInt'\n\n"
    );
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    this->upperLevelParent.push("binOp");
    p_bin_op.visitChildNodes(*this);
    this->upperLevelParent.pop();
    // TODO: only add operation is done
    dumpInstructions(m_output_file.get(),
    "\n    # Do Binary Operation: \n" 
    "    lw t0, 0(sp)      # pop the value from the stack\n"
    "    addi sp, sp, 4\n"
    "    lw t1, 0(sp)      # pop the value from the stack\n"
    "    addi sp, sp, 4\n");

    string *op = new string();
    int lab = 0;
    switch (p_bin_op.getOp()){
        case Operator::kPlusOp:
            dumpInstructions(m_output_file.get(), "    add t0, t1, t0\n");
            break;
        case Operator::kMinusOp:
            dumpInstructions(m_output_file.get(), "    sub t0, t1, t0\n");
            break;
        case Operator::kMultiplyOp:
            dumpInstructions(m_output_file.get(), "    mul t0, t1, t0\n");
            break;
        case Operator::kDivideOp:
            dumpInstructions(m_output_file.get(), "    div t0, t1, t0\n");
            break;
        case Operator::kModOp:
            dumpInstructions(m_output_file.get(), "    rem t0, t1, t0\n");
            break;
        case Operator::kEqualOp:
            lab = this->getCurrLabel();
            this->retLabel.push(lab);
            dumpInstructions(m_output_file.get(), "    bne t1, t0, L%d\n", lab);
            break;
        case Operator::kGreaterOrEqualOp:
            lab = this->getCurrLabel();
            this->retLabel.push(lab);
            dumpInstructions(m_output_file.get(), "    blt t1, t0, L%d\n", lab);
            break;
        case Operator::kGreaterOp:
            lab = this->getCurrLabel();
            this->retLabel.push(lab);
            dumpInstructions(m_output_file.get(), "    ble t1, t0, L%d\n", lab);
            break;
        case Operator::kLessOp:
            lab = this->getCurrLabel();
            this->retLabel.push(lab);
            dumpInstructions(m_output_file.get(), "    bge t1, t0, L%d\n", lab);
            break;
        case Operator::kLessOrEqualOp:
            lab = this->getCurrLabel();
            this->retLabel.push(lab);
            dumpInstructions(m_output_file.get(), "    bgt t1, t0, L%d\n", lab);
            break;
        case Operator::kNotEqualOp:
            lab = this->getCurrLabel();
            this->retLabel.push(lab);
            dumpInstructions(m_output_file.get(), "    beq t1, t0, L%d\n", lab);
            break;
        default:
            *op = "add";
            break;
    }

    // "    %s t0, t1, ", op->c_str());
    if (p_bin_op.getOp() == Operator::kPlusOp || p_bin_op.getOp() == Operator::kMinusOp || p_bin_op.getOp() == Operator::kMultiplyOp || p_bin_op.getOp() == Operator::kDivideOp || p_bin_op.getOp() == Operator::kModOp){
        dumpInstructions(m_output_file.get(),
        "    addi sp, sp, -4\n"
        "    sw t0, 0(sp)      # push the value to the stack\n\n");
    }
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    this->negOpVal = 1;
    this->upperLevelParent.push("unOp");
    p_un_op.visitChildNodes(*this);
    this->upperLevelParent.pop();
    this->negOpVal = 0;
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    // int argNo = 0;
    this->upperLevelParent.push("funcInvoke");
    p_func_invocation.visitChildNodes(*this);
    this->upperLevelParent.pop();
    int args = p_func_invocation.getArguments().size();
    for(int i = args-8; i > 0; i--){
        dumpInstructions(m_output_file.get(), 
        "\n    lw t%d, 0(sp)          # pop the value from the stack, save the spilled register on stack\n"
        "    addi sp, sp, 4\n", i+2);
        // handle spilled, from t7
    }
    for(int i = args > 8 ? 8 : args; i > 0; i--){
        dumpInstructions(m_output_file.get(), 
        "\n    lw a%d, 0(sp)          # pop the value from the stack to the argument register 'a%d'\n"
        "    addi sp, sp, 4\n", i-1, i-1);
    }
    dumpInstructions(m_output_file.get(), "\n    jal ra, %s           # call function '%s'\n", p_func_invocation.getNameCString(), p_func_invocation.getNameCString());
    if (p_func_invocation.getInferredType()->getPrimitiveType() != PType::PrimitiveTypeEnum::kVoidType ){
        dumpInstructions(m_output_file.get(), 
        "    mv t0, a0             # always move the return value to a certain register you choose\n"
        "    addi sp, sp, -4\n"
        "    sw t0, 0(sp)          # push the value to the stack\n\n");
    }
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {
    this->upperLevelParent.push("varRef");
    p_variable_ref.visitChildNodes(*this);
    this->upperLevelParent.pop();
    int tableOffset = this->tableLookup(p_variable_ref.getNameCString()) + 1;
    if (this->getIsLval()){
        if (tableOffset){
            dumpInstructions(m_output_file.get(), 
            "    # load the local address of '%s' and push it on to the stack\n"
            "    addi t0, s0, %d\n"
            "    addi sp, sp, -4\n"
            "    sw t0, 0(sp)\n", p_variable_ref.getNameCString(), (tableOffset + 2) * -4);
        }
        else if (this->isGlobal(p_variable_ref.getNameCString())) {
            dumpInstructions(m_output_file.get(), 
            "    # load the global address of '%s' and push it on to the stack\n"
            "    la t0, %s\n"
            "    addi sp, sp, -4\n"
            "    sw t0, 0(sp)\n", p_variable_ref.getNameCString(), p_variable_ref.getNameCString());
        }
    } 
    else {
        if (this->isGlobal(p_variable_ref.getNameCString()) && !this->inFunction){
            dumpInstructions(m_output_file.get(), 
            "    # load the global value of '%s' and push it on to the stack\n"
            "    la t0, %s\n"
            "    lw t1, 0(t0)\n"
            "    mv t0, t1\n"
            "    addi sp, sp, -4\n"
            "    sw t0, 0(sp)\n", p_variable_ref.getNameCString(), p_variable_ref.getNameCString());
        }
        else if (tableOffset){
            dumpInstructions(m_output_file.get(), 
            "    # load the local value of '%s' and push it on to the stack\n"
            "    lw t0, %d(s0)\n"
            "    addi sp, sp, -4\n"
            "    sw t0, 0(sp)\n", p_variable_ref.getNameCString(), (tableOffset + 2) * -4);
        }
    }
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {
    // p_assignment.visitChildNodes(*this);
    // ref: const_cast<CompoundStatementNode &>(p_program.getBody()).accept(*this);
    this->upperLevelParent.push("assign");
    this->setIsLval(1);
    const_cast<VariableReferenceNode &>(p_assignment.getLvalue()).accept(*this);
    this->setIsLval(0);
    const_cast<ExpressionNode &>(p_assignment.getExpr()).accept(*this);
    this->upperLevelParent.pop();
    dumpInstructions(m_output_file.get(), 
    "    # Assign the value to '%s'\n"
    "    lw t0, 0(sp)\n"
    "    addi sp, sp, 4\n"
    "    lw t1, 0(sp)\n"
    "    addi sp, sp, 4\n"
    "    sw t0, 0(t1)\n\n"
    , p_assignment.getLvalue().getNameCString());
}

void CodeGenerator::visit(ReadNode &p_read) {
    this->upperLevelParent.push("read");
    this->setIsLval(1);
    p_read.visitChildNodes(*this);
    this->setIsLval(0);
    this->upperLevelParent.pop();
    dumpInstructions(m_output_file.get(),
        "\n    jal ra, readInt       # call function 'readInt'\n"
        "    lw t0, 0(sp)\n"
        "    addi sp, sp, 4\n"
        "    sw a0, 0(t0)          # save the return value to 'a'\n\n"
    );
}

void CodeGenerator::visit(IfNode &p_if) {
    this->upperLevelParent.push("if");

    int firstLabel = this->getCurrLabel();
    const_cast<ExpressionNode &>(p_if.getCondition()).accept(*this);

    dumpInstructions(m_output_file.get(), "L%d:\n", firstLabel);
    const_cast<CompoundStatementNode &>(p_if.getBody()).accept(*this);
    int secondLabel = this->getCurrLabel();
    dumpInstructions(m_output_file.get(), "    j L%d\nL%d:\n", secondLabel, this->retLabel.top());
    this->retLabel.pop();
    const_cast<CompoundStatementNode &>(p_if.getElseBody()).accept(*this);
    dumpInstructions(m_output_file.get(), "L%d:\n", secondLabel);
    // p_if.visitChildNodes(*this);
    this->upperLevelParent.pop();
}

void CodeGenerator::visit(WhileNode &p_while) {
    this->upperLevelParent.push("while");
    // p_while.visitChildNodes(*this);
    int firstLabel = this->getCurrLabel();
    dumpInstructions(m_output_file.get(), "L%d:\n", firstLabel);
    const_cast<ExpressionNode &>(p_while.getCondition()).accept(*this);
    int secondLabel = this->getCurrLabel();
    dumpInstructions(m_output_file.get(), "L%d:\n", secondLabel);
    const_cast<CompoundStatementNode &>(p_while.getBody()).accept(*this);

    dumpInstructions(m_output_file.get(), "    j L%d\nL%d:\n", firstLabel, this->retLabel.top());
    this->retLabel.pop();
    this->upperLevelParent.pop();
}

void CodeGenerator::visit(ForNode &p_for) {
    this->forLoopDepth += 1;
    // Reconstruct the hash table for looking up the symbol entry
    this->table_offset += this->realTablePtr.top()->getEntries().size();
    this->realTablePtr.push(const_cast<SymbolTable *>(p_for.getSymbolTable()));
    m_symbol_manager_ptr->reconstructHashTableFromSymbolTable(
        p_for.getSymbolTable());
    this->upperLevelParent.push("for");
    // p_for.visitChildNodes(*this);
    // const_cast<DeclNode &>(p_for.getDecl()).accept(*this);

    const_cast<AssignmentNode &>(p_for.getAssign()).accept(*this);
    int firstLabel = this->getCurrLabel();
    dumpInstructions(m_output_file.get(), "L%d:\n", firstLabel);

    // The loop bound; need to compare
    const_cast<VariableReferenceNode &>(p_for.getAssign().getLvalue()).accept(*this);
    const_cast<ExpressionNode &>(p_for.getCondition()).accept(*this);
    int secondLabel = this->getCurrLabel();
    int thirdLabel = this->getCurrLabel();
    // this->retLabel.push(lab);
    dumpInstructions(m_output_file.get(), 
    "\n    # Do Loop Bound Check: \n" 
    "    lw t0, 0(sp)      # pop the value from the stack\n"
    "    addi sp, sp, 4\n"
    "    lw t1, 0(sp)      # pop the value from the stack\n"
    "    addi sp, sp, 4\n"
    "    bge t1, t0, L%d\nL%d:\n", thirdLabel, secondLabel);
    // dumpInstructions(m_output_file.get(), 

    const_cast<CompoundStatementNode &>(p_for.getBody()).accept(*this);

    // dumpInstructions(m_output_file.get(), "    j L%d\n", firstLabel);

    // loop index increment (i = i+1)
    this->setIsLval(1);
    const_cast<VariableReferenceNode &>(p_for.getAssign().getLvalue()).accept(*this);
    this->setIsLval(0);
    // As rval
    const_cast<VariableReferenceNode &>(p_for.getAssign().getLvalue()).accept(*this);
    // 1
    dumpInstructions(m_output_file.get(), 
    "    # push constant value to the stack\n"
    "    li t0, 1\n"
    "    addi sp, sp, -4\n"
    "    sw t0, 0(sp)\n");
    // Add'em up
    dumpInstructions(m_output_file.get(),
    "\n    # Do Index Increment: \n" 
    "    lw t0, 0(sp)      # pop the value from the stack\n"
    "    addi sp, sp, 4\n"
    "    lw t1, 0(sp)      # pop the value from the stack\n"
    "    addi sp, sp, 4\n" 
    "    add t0, t1, t0\n");
    dumpInstructions(m_output_file.get(),
    "    addi sp, sp, -4\n"
    "    sw t0, 0(sp)      # push the value to the stack\n\n");
    dumpInstructions(m_output_file.get(), 
    "    # Assign the value to '%s'\n"
    "    lw t0, 0(sp)\n"
    "    addi sp, sp, 4\n"
    "    lw t1, 0(sp)\n"
    "    addi sp, sp, 4\n"
    "    sw t0, 0(t1)\n\n"
    , p_for.getAssign().getLvalue().getNameCString());


    dumpInstructions(m_output_file.get(), "    j L%d\nL%d:\n", firstLabel, thirdLabel);

    this->upperLevelParent.pop();

    // Remove the entries in the hash table
    m_symbol_manager_ptr->removeSymbolsFromHashTable(p_for.getSymbolTable());
    this->realTablePtr.pop();
    this->table_offset -= this->realTablePtr.top()->getEntries().size();
    this->forLoopDepth -= 1;
}

void CodeGenerator::visit(ReturnNode &p_return) {
    this->upperLevelParent.push("return");
    p_return.visitChildNodes(*this);
    this->upperLevelParent.pop();
    dumpInstructions(m_output_file.get(), 
    "    lw t0, 0(sp)      # pop the value from the stack\n"
    "    addi sp, sp, 4\n"
    "    mv a0, t0         # load the value to the return value register 'a0'\n");
}

int CodeGenerator::tableLookup(const char *name) {
    int index = 0;
    for (auto &entry : this->realTablePtr.top()->getEntries()){
        if (!strcmp(name, entry->getNameCString())) return index+table_offset;
        index++;
    }
    index = -1;
    
    while (this->realTablePtr.size() > 1 && this->forLoopDepth){
        index = 0;
        this->table_offset -= this->realTablePtr.top()->getEntries().size();
        tempTable.push(this->realTablePtr.top());
        this->realTablePtr.pop();
        for (auto &entry : this->realTablePtr.top()->getEntries()){
            if (!strcmp(name, entry->getNameCString())) goto OUT;
            index++;
        }
    }
OUT:
    // restore
    while (tempTable.size() != 0 && this->forLoopDepth){
        this->realTablePtr.push(tempTable.top());
        tempTable.pop();
        this->table_offset += this->realTablePtr.top()->getEntries().size();
    }
    // But you'd still better look up whehter it's in the table
    return index + 1 ? index : -1;
}

int CodeGenerator::isGlobal(const char *name) {
    for(auto i : this->gloablVariableTable){
        if(!strcmp(name, i.c_str())) return 1;
    }
    return 0;
}