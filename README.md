# Custom Script Language (CSL)

A minimal, readable scripting language designed for simplicity and educational use.

## Overview

This language is built to feel like readable pseudocode while still being structured and deterministic.  
It supports variables, output and input

## Features

- Variables with `put EXPR on IDENT;`
- Output with `showText(EXPR);` OR `showText("TEXT");`
- Input with `put readVar(PROMPT) on IDENT`
- Conditions with `if CONDITION { CODE }`
- Loops with `while CONDITION { CODE }`
- Variable Assignments `EXPR on IDENT;`

## Syntax

### comments
```bash
# this is a comment
```

### Variables

```bash
put 10 + (4 / 2) on number;
put "michael" on name;
```

### Ouput

```bash
showText(number + 2);
showText(name);

showText("Actuall text");
```

### input

```bash
put readVar("Prompt: ") on buffer;
```

### conditions
```bash
if number == 4 {
    # code
}

# you can use either variables or numbers for the conditions comparation and also you can use >,<,>=,<=,!=
```

### loops
```bash
while number == 4 {
    # code
}

# you can use either variables or numbers for the conditions comparation and also you can use >,<,>=,<=,!=
```

### var assignments
```bash
3 + 2 * (2 / 3) on number;

int_var on new_int_var;

# this is invalid because the language doesent allow to put the string value of a var into a new one
str_var on new_str_var;

#this is valid
"string" on buffer;
```
---

## Build And Run
```bash
git clone https://github.com/skinwalker3654/clean-interpreter
cd clean-interpreter
make  
```

## use
```bash
# to execute the scripts just simply run
./main <file_name>
```
---

## clean executables
```bash
make clean
```

Profecional and Clean for everyone to learn building programing languages.
I spend lot of time learning about languages so i can make this simple demo but fully done.

---
# clean-interpreter
