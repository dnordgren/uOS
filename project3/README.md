Project 3 - Semaphores
=====
Operating System Kernels CSCE 351 Fall 2015

**Authors:** Rees Klintworth, Derek Nordgren

## Code structure

- `project3.c`: contains the `prototype_os` function that serves as the entry point
  for the prototype operating system. Here, the bear and bee threads are initialized
  and assigned work.
- `alarm_handler.c/.h`: these files were provided with the sample project 2 code. It
   manages the alarm used to regularly schedule interrupts on the NIOS II board
- `queue.c/.h`: this file was provided with the sample project 2 code. It manages
  the linked list run thread run queue. We modified the `dequeue(..)` function
  to only pop unblocked threads from the run queue.
- `semaphore.c/.h`: These files were written by the authors. It provides the `sem`
  structure as well as functions related to initializing and managing semaphores
  in addition to `up`ing and `down`ing a semaphore.
- `thread_handler.c/.h`: these files were provided with the sample project 2 code.
  It provides the `tcb` structure as well as functions related to managing `tcb`s
  and scheduling threads.
- `exception_hanlder.S`: these files were provided with the sample project 2 code.
  It contains assembly that is "injected" into `alt_exception_entry.S` at
  interrupt handling-time. Using the `global_flag` defined in `main.c`, this code
  causes `alt_exception_entry.S` to resume a new thread on interrupt rather than
  the interrupted thread.

## How to compile/run

1. Create a new project using the Altera/Eclipse IDE.
1. Using the `File` menu, add the provided source files. Eclipse will automatically
   update the project makefile to include the new files.
1. Use the `Run` button in Eclipse to run the project.
