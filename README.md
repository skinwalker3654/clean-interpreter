# Custom Script Language (CSL)

A minimal, readable scripting language designed for simplicity and educational use.

## Overview

This language is built to feel like readable pseudocode while still being structured and deterministic.  
It supports variables, output and input

## Features

- Variables with `put`
- showText function that prints stuff to the screen
- input with `readVar`

## Syntax

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
---

## Build And Run
```bash
make  # compile the program
./main <filename> # execute scripts
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
