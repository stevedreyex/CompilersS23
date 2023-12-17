# hw4 report

|||
|-:|:-|
|Name|Tzu-Chun Liao|
|ID|311554032|

## How much time did you spend on this project

> e.g. 2 hours.

## Project overview

> Please describe the structure of your code and the ideas behind your implementation in an organized way.
> The point is to show us how you deal with the problems. It is not necessary to write a lot of words or paste all of your code here.

1. Create the `AstNodeInclude.hpp` then all the node can refrence by the Sema visitor
2. Add the control of `//&D+/-` for the option to open/close the symbol table dump
3. A `SemanticAnalyzer` will visit all nodes through the AST using visitor pattern, then do the same thing as we did like `AstDumper`, we let `root->accept(semaAnalyzer)` to start our traverse.
4. Add the classes like `SymbolTable`, `SymbolManager` in the `SemanticAnalyzer`...

## What is the hardest you think in this project

> Not required, but bonus point may be given.

## Feedback to T.A.s

> Not required, but bonus point may be given.
