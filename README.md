# Custom Script Language (CSL)

A minimal, readable scripting language designed for simplicity and educational use.

## Overview

This language is built to feel like readable pseudocode while still being structured and deterministic.  
It supports variables, output and input

## Features

- Variables with `put VALUE on VAR;`
- Output with `showText(VAR);` OR `showText("TEXT");`
- Input with `put readVar(PROMPT) on VAR`
- Conditions with `if CONDITION { CODE }`
- Loops with `while CONDITION { CODE }`

## Syntax

### comments
```bash
# this is a comment
```

### Variables

```bash
put 10 on number;
put "michael" on name;
```

### Ouput

```bash
showText(number);
showText(name);

showText("Actuall text");
```

### input

```bash
put readVar("Prompt: ") on var;
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
