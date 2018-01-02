C++ implementation of NachOS (Not Another Completely Heuristic Operating System)
================================================================================
The repository contains an implementation of [NachOS](https://homes.cs.washington.edu/~tom/nachos/). It was developed to approve the subject 'Operative Systems', of the career [Computer Science](https://dcc.fceia.unr.edu.ar/es/lcc) of the National University of Rosario.

# Overview

Not Another Completely Heuristic Operating System, or Nachos, is instructional software for teaching undergraduate, and potentially graduate level operating systems courses. It was developed at the University of California, Berkeley, designed by Thomas Anderson, and is used by numerous schools around the world.

Originally written in C++ for MIPS, Nachos runs as a user-process on a host operating system. A MIPS simulator executes the code for any user programs running on top of the Nachos operating system. Ports of the Nachos code exist for a variety of architectures.

In addition to the Nachos code, a number of assignments are provided with the Nachos system. The goal of Nachos is to introduce students to concepts in operating system design and implementation by requiring them to implement significant pieces of functionality within the Nachos system.

# Building and running

The Nachos kernel only looks for program executables in the current directory, so its best to run Nachos from the user directory. The Nachos program can be invoked as follows:

```bash
    ./nachos [-d <flags>] [<initprog>]
```

where <initprog> is the initial program to execute (and can be chosen to be any of the user programs in the directory). If it is not specified, then the program init is executed. (cf. kernel/main.cc).

The flags are useful for debugging purposes. Some of the important flags are:

- 'p': process management
- 't': thread management
- 's': system calls
- 'a': address space
- 'm': machine emulation 

You should initially try to run with -d pts to turn on the debugging flags related to threads, process, and system calls. The output should be useful in tracing the execution. The flags for machine and address space generate a lot of output, so use them as needed. When you make modifications, you may want to add your own DEBUG statements in your modified code for debugging purposes.

# Report

If you want to read the report, you can [get the PDF](https://github.com/rgaluppo/nachos/blob/master/informe/Informe%20final.pdf) directly.

## Authors

* **Raúl I. Galuppo** - [mail](mailto:raul.i.galuppo@gmail.com)
* **Andrea Graf** - [mail](mailto:andreagraf.lcc@gmail.com)
