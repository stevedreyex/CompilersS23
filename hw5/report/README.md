# hw5 report

|||
|-:|:-|
|Name|Tzu-Chun Liao|
|ID|31154032|

## How much time did you spend on this project

> e.g. 2 hours.

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.

This is the project to generate the RISC-V code based on the parser which the semantic check is done. In codegen phase, we also traverse the AST and dump the given Machine code. I first processed with testcase `specExample`. This are the steps I handle all the parts.

1. Scan over the first `SymbolTable` and generate the `.bss` and `.rodata`
2. Generate the `function`. Adjust stack pointer like `addi sp, sp, -128 ...`.  
3. Place the arguments to the activation record of `function`
4. Handle different codegen of `lval` and `rval`
5. Handle `binary operations` and `assignment`
6. Handle return value

After the correct function part is generated, I start processed with `main`

1. Handle syscall `read`
2. Handle syscall `print`
3. Handle `if`
4. Handle `while`
5. Handle `for`

## What is the hardest you think in this project

1. The codegen of `lval` and `rval`. These cases are not only appear in the `assign` node, but also like `read`, `print` and `for`. You should be very notice that is this a `lval` or `rval` then you can generate the correct code.

2. "For Loop". this is the hardest and the most complicate part, as it was in HW4. But it's reasonable, cause for most cases (I mean the other compiler codegen), `For` is always the hardest. Do-Loop (`While`) might be easier. The part to generate control flow and index control is complicated. 
## Feedback to T.A.s

I will give this HW the difficulty `middle`. I think this homework is easier that `HW3` and `HW4`. Nevertheless, this is a good hw.  
If there might have tutorial to debug in spike that will be great.  
Thank you TAs.  
