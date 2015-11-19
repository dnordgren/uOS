Project 2 - Thread scheduler
=====
Operating System Kernels CSCE 351 Fall 2015

**Authors:** Rees Klintworth, Derek Nordgren

## Code structure

- `main.c`: contains the `prototype_os` function that serves as the entry point
  for the prototype operating system. This file also provides all thread control functions, such as create, join, and destroy;    also defines the `tcb` structure.
- `my_scheduler.S`: contains assembly that is "injected" into `alt_exception_entry.S` at
  interrupt handling-time. Using the `global_flag` defined in `main.c`, this code
  causes `alt_exception_entry.S` to resume a new thread on interrupt rather than
  the interrupted thread.

## How to compile/run

1. Create a new project using the Altera/Eclipse IDE.
1. Using the `File` menu, add the provided source files. Eclipse will automatically
   update the project makefile to include the new files.
1. Use the `Run` button in Eclipse to run the project.
