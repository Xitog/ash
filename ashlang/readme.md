# Ash

A simple language transpiling to Python or interpreted.
The default implementation of the transpiler and interpreter are written in Python 3.

## Summary

Introduction

1. First step with Ash
  1.A Hello World!
  1.B Comments
  1.C Keywords
2. Variables and constants
3. Types and operators
4. Functions and procedures
5. Classes and objects
6. Modules
7. Exceptions
8. Standard library
9. Interacting with Python
10. Links

Conclusion

## Introduction

Todo

## 1. First steps with Ash

### 1.A Hello, World!

This program writes on the standard output the string Hello, World!

``ash> writeln("Hello, World!")``

The function ``writeln`` writes on the standard output one or more arguments, goes to the next line and come back at its first position, on the left. It takes here only one argument, the string "Hello, World!". The entire line is a statement.

This second program below writes on the standard output the string *What is your name?* then reads a string stored in the variable name.

```
ash> name = read('What is your name?')
ash> writeln("Hello", name)
```

The function ``read`` reads a string on the standard input. This code is composed of two lines, each line being a statement. But you can write the same example this way also:

``ash> name = read('What is you name?') ; writeln("Hello", name)``

This time, we have only one line composed of two statements separated by a ``;``, called the *statement separator*. For both ways, the function ``writeln`` is called this time with two arguments, the string literal *"Hello"* and the variable name. This function accepts a variable number of arguments.

### 1.B Comments

A comment is a text ignored by the interpreter. It does nothing. The transpiler with recopy the text in the transpiled output.

``-- This is a comment``

### 1.C Keywords

Ash uses 26 keywords which mustn't be used as variable or constant names:
* Nil literal: ``nil``
* Boolean literals: ``true``, ``false``
* Operators: ``and``, ``or``, ``not``, ``xor``
* Choice: ``if``, ``then``, ``elif``, ``else``, ``end``
* Loop: ``while``, ``do``, ``end``, ``break``, ``next``
* Subprogram: ``fun``, ``pro``, ``return``
* Class: ``class``
* Module: ``import``
* Exceptions: ``try``, ``when``, ``finally``, ``raise``

## 2. Variables and constants

Declare and assign a variable:
``a = 5``

Declare, assign and restrict a variable to a type:
``a : int = 5``

A constant starts with a capital letter and cannot be changed through the execution of the script:
``A = 5``

## 3. Types and operators

* ``int`` for integer
* ``flt`` for float
* ``bool`` for boolean
* ``"..."`` or ``'...'`` for strings
* ``[a, b]`` for list
* ``[a = 5, b = 34]`` for hash

## 4. Functions and procedures

Todo

## 5. Classes and objects

Todo

## 6. Modules

Todo

## 7. Exceptions

Todo

## 8. Standard library

Todo

## 9. Interacting with Python

Todo

## 10. Links

Todo

## Conclusion

Damien Gouteux 2020