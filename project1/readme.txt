Project 1 -- CSCE 351 OS Kernels
===========

Rees Klintworth, Derek Nordgren
10/5/2015

HANDIN CONTENTS
	- readme.txt: this file
	- main.c: the C source file for the project

SOURCE ORGANIZATION
main.c consists of three functions:
	- main(..)
	- uOS(..)
	- interrupt_handler(..)
main(..) simply serves as the entry-point for the program and only calls uOS(..).

uOS(..) is the entry-point for our prototype operating system. This function initializes the alarm interrupt and sets
up a loop that repeats endlessly.

interrupt_handler(..) is the callback function for the alarm. When the alarm goes off, uOS(..)'s normal execution is
interrupted and program execution switches to interrupt_handler(..). This function simply prints that it has executed
and returns the value to reset the alarm to.

HOW TO RUN THIS PROGRAM
	1. Replace the .c source file created in the NIOS-II base development project (hello world) with main.c.
	2. Use NIOS-II > Run to run the project. After building, output statements should begin to fill the console.