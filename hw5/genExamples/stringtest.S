   .file "./bonus_cases/test-cases/stringtest.p"
   .option nopic

.section    .text
    .align 2
    .globl test
    .type test, @function
test:
    addi sp, sp, -128
    sw ra, 124(sp)
    sw s0, 120(sp)
    addi s0, sp, 128

    sw a0, -12(s0)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    lw ra, 124(sp)
    lw s0, 120(sp)
    addi sp, sp, 128
    jr ra
    .size test, .-test

.section    .text
    .align 2
    .globl main
    .type main, @function
main:
    # in the function prologue
    addi sp, sp, -128
    sw ra, 124(sp)
    sw s0, 120(sp)
    addi s0, sp, 128

    # assign value to local variable/constant
    addi t0, s0, -12
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, hello
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack to argument register
    lw a0, 0(sp)
    addi sp, sp, 4
    # call the function, and push the return value to the stack
    jal ra, test
    mv t0, a0
    addi sp, sp, -4
    sw t0, 0(sp)
    # in the function epilogue
    lw ra, 124(sp)
    lw s0, 120(sp)
    addi sp, sp, 128
    jr ra
    .size main, .-main

