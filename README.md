# Micro Extended Compiler

![C](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Status](https://img.shields.io/badge/Status-Completed-success?style=for-the-badge)
![Academic](https://img.shields.io/badge/Context-UTN_Engineering-blue?style=for-the-badge)

## Overview

**Micro Extended Compiler** is a custom-built compiler for the "Micro" programming language, extended to support complex data types and advanced control structures. 

Developed as a Final Integrative Project for the **Systems Engineering** degree at **UTN**, this project simulates the full lifecycle of language processing: from lexical analysis to intermediate code generation, emphasizing manual memory management and low-level optimization.

## Key Features

Based on the core principles of Compiler Design, this project implements:

* **Lexical Analysis (Scanner):** A robust scanner built using **Finite State Automata (DFA)** to tokenize input streams efficiently .
* **Syntactic & Semantic Analysis:**
    * Implementation of **recursive logic** for parsing nested structures.
    * Integration of **iterative loops** (`while`, `for`) and **conditional structures** (`if-else`) .
* **Intermediate Code Generation:** Transforms source code into an optimized intermediate representation (Polish Notation/Quadruples) ready for execution .
* **Advanced Data Types:** Extended the original grammar to support **Floating Point Arithmetic**, handling type checking and conversion .
* **Memory Management:** Manual allocation and deallocation strategies to ensure memory efficiency during the compilation process .

## Tech Stack

* **Language:** C / C++
* **Tools:** Flex (Lexer), Bison (Parser), GCC Compiler.
* **Version Control:** Git.

## How It Works

The compiler follows a standard pipeline architecture:

1.  **Source Code (.m):** The user provides a script written in Micro Extended.
2.  **Scanner:** Reads characters and groups them into Tokens (Identifiers, Constants, Operators).
3.  **Parser:** Validates the grammatical structure using a Syntax Tree.
4.  **Semantic Analyzer:** Checks for type compatibility (e.g., Float vs. Int) and scope.
5.  **Code Generator:** Outputs the executable intermediate code.

## Installation & Usage

### Prerequisites
* GCC Compiler
* Make (Optional)

### Build
Clone the repository and compile the source code:

```bash
git clone [https://github.com/tomelt/Compilador-Micro-Extendido.git](https://github.com/tomelt/Compilador-Micro-Extendido.git)
cd Trabajo-Practico-Integrador
gcc fischer.c -o compiler
