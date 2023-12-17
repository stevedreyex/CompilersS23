   .file "./bonus_cases/test-cases/arraytest2.p"
   .option nopic

.section    .text
    .align 2
    .globl sum
    .type sum, @function
sum:
    addi sp, sp, -128
    sw ra, 124(sp)
    sw s0, 120(sp)
    addi s0, sp, 128

    sw a0, -12(s0)
    # assign value to local variable/constant
    addi t0, s0, -16
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop two values from the stack
    lw t0, 0(sp)
    addi sp, sp, 4
    lw t1, 0(sp)
    addi sp, sp, 4
    # do the operation
    add t0, t1, t0
    # push the value to the stack
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # push the local variable/constant value to the stack
    lw t0, -16(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack
    lw t0, 0(sp)
    addi sp, sp, 4
    # load the value of variable to the return value register 'a0'
    mv a0, t0
    lw ra, 124(sp)
    lw s0, 120(sp)
    addi sp, sp, 128
    jr ra
    .size sum, .-sum

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

    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # assign value to local variable/constant
    addi t0, s0, -12
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 5
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # assign value to local variable/constant
    addi t0, s0, -12
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 10
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
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
    jal ra, sum
    mv t0, a0
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    # push the local constant to the stack
    li t0, 0
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 0
    addi sp, sp, -4
    sw t0, 0(sp)
    # load the address of the local variable to t0
    la t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # call function 'readInt'
    jal ra, readInt
    # pop the address from the stack, and store the return value to 'a'
    lw t0, 0(sp)
    addi sp, sp, 4
    sw a0, 0(t0)

    # push the local constant to the stack
    li t0, 0
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 0
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop two values from the stack
    lw t0, 0(sp)
    addi sp, sp, 4
    lw t1, 0(sp)
    addi sp, sp, 4
    # do the operation
    ble t1, t0, L2
L1:
    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    j L3
L2:
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

L3:
    # in the function epilogue
    lw ra, 124(sp)
    lw s0, 120(sp)
    addi sp, sp, 128
    jr ra
    .size main, .-main

