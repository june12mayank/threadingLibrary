# MyThreadingLibrary

## Introduction
This repo is an implementation of kernel-level (one-one) threads and user-level (many-one) threads
in C language like pthreads on Linux System (Ubuntu).

## Implemented Functions

The functions implemented are 

```
mythread_create()
mythread_join()
mythread_kill()
mythread_exit()
mythread_spin_init()
mythread_spin_lock()
mythread_spin_unlock()
mythread_spin_trylock()

// additional function 
mythread_self()
```

working of function `mythread_xyz` is same as `pthread_xyz` function.
in addition to these functions, there are many functions which library needs to call internally 
and a `set_active_thread_signal()` function which will be needed to set custom signal handlers 
for various threads in user level threads model.

## Compilation

Any of many-one and one-one thread implementation consists of 2 files - a mythread.c and mythread.h.
A single C file is made instead of dividing code into multiple files to make it a library.
Use this command to create an object file from the source

`gcc -c -Wall mythread.c`
 
This will create an object file mythread.o
To link it with your program, say `main_program.c`, use

```
gcc -c -Wall main_program.c
gcc main_program.o mythread.o
```

This will create the executable file a.out which you can run.

Alternately, one may use `ar` command to create an archive from library and then link it with main 
program `main_program.c`.

### Testing Code

To test the functions in library, there are already some test files provided in directory 
`testing_code/`. Each file tests a basic functionality which is well explained in comments in that 
file.


