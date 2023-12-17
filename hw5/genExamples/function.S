   .file "./basic_cases/test-cases/function.p"
   .option nopic

# declare golbal variable
.comm gv, 4, 4

# declare global constant
.section    .rodata
    .align 2
    .globl gc
    .type gc, @object
gc: 
    .word 2

.section    .text
    .align 2
    .globl product
    .type product, @function
product:
    addi sp, sp, -128
    sw ra, 124(sp)
    sw s0, 120(sp)
    addi s0, sp, 128

    sw a0, -12(s0)
    sw a1, -16(s0)
    # assign value to local variable/constant
    addi t0, s0, -20
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -16(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop two values from the stack
    lw t0, 0(sp)
    addi sp, sp, 4
    lw t1, 0(sp)
    addi sp, sp, 4
    # do the operation
    mul t0, t1, t0
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
    lw t0, -20(s0)
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
    .size product, .-product

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
    sw a1, -16(s0)
    # assign value to local variable/constant
    addi t0, s0, -20
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -16(s0)
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
    lw t0, -20(s0)
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
    .globl dot
    .type dot, @function
dot:
    addi sp, sp, -128
    sw ra, 124(sp)
    sw s0, 120(sp)
    addi s0, sp, 128

    sw a0, -12(s0)
    sw a1, -16(s0)
    sw a2, -20(s0)
    sw a3, -24(s0)
    # assign value to local variable/constant
    addi t0, s0, -28
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -16(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack to argument register
    lw a1, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a0, 0(sp)
    addi sp, sp, 4
    # call the function, and push the return value to the stack
    jal ra, product
    mv t0, a0
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -20(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -24(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack to argument register
    lw a1, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a0, 0(sp)
    addi sp, sp, 4
    # call the function, and push the return value to the stack
    jal ra, product
    mv t0, a0
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack to argument register
    lw a1, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a0, 0(sp)
    addi sp, sp, 4
    # call the function, and push the return value to the stack
    jal ra, sum
    mv t0, a0
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # push the local variable/constant value to the stack
    lw t0, -28(s0)
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
    .size dot, .-dot

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

    addi t0, s0, -16
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    lw t0, 0(sp)
    addi sp, sp, 4
    lw t1, 0(sp)
    addi sp, sp, 4
    sw t0, 0(t1)

    # assign value to gloabal variable
    la t0, gv
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the global constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # assign value to local variable/constant
    addi t0, s0, -12
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local constant to the stack
    li t0, 2
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # assign value to gloabal variable
    la t0, gv
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the global variable/constant value to the stack
    la t0, gv
    lw t1, 0(t0)
    mv t0, t1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the global variable/constant value to the stack
    la t0, gc
    lw t1, 0(t0)
    mv t0, t1
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack to argument register
    lw a1, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a0, 0(sp)
    addi sp, sp, 4
    # call the function, and push the return value to the stack
    jal ra, product
    mv t0, a0
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # assign value to local variable/constant
    addi t0, s0, -12
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the global variable/constant value to the stack
    la t0, gv
    lw t1, 0(t0)
    mv t0, t1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -16(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack to argument register
    lw a1, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a0, 0(sp)
    addi sp, sp, 4
    # call the function, and push the return value to the stack
    jal ra, product
    mv t0, a0
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

    # push the global variable/constant value to the stack
    la t0, gv
    lw t1, 0(t0)
    mv t0, t1
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
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    # assign value to gloabal variable
    la t0, gv
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the global variable/constant value to the stack
    la t0, gv
    lw t1, 0(t0)
    mv t0, t1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the global variable/constant value to the stack
    la t0, gc
    lw t1, 0(t0)
    mv t0, t1
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -12(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # push the local variable/constant value to the stack
    lw t0, -16(s0)
    addi sp, sp, -4
    sw t0, 0(sp)
    # pop the value from the stack to argument register
    lw a3, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a2, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a1, 0(sp)
    addi sp, sp, 4
    # pop the value from the stack to argument register
    lw a0, 0(sp)
    addi sp, sp, 4
    # call the function, and push the return value to the stack
    jal ra, dot
    mv t0, a0
    addi sp, sp, -4
    sw t0, 0(sp)
    # save the value into the variable
    lw t0, 0(sp)       # pop the value from the stack
    addi sp, sp, 4
    lw t1, 0(sp)       # pop the address from the stack
    addi sp, sp, 4
    sw t0, 0(t1)

    # push the global variable/constant value to the stack
    la t0, gv
    lw t1, 0(t0)
    mv t0, t1
    addi sp, sp, -4
    sw t0, 0(sp)
    # print
    lw a0, 0(sp)
    addi sp, sp, 4
    jal ra, printInt

    # in the function epilogue
    lw ra, 124(sp)
    lw s0, 120(sp)
    addi sp, sp, 128
    jr ra
    .size main, .-main

