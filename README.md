## OS Assignment 2

**Shivansh Seth** \
**2018113010**

### Compiling
`make`

### Running the shell
`./run`

## Program Structure

- main.c - Main shell loop
- prompt.c - Function for printing the prompt
- exec.c - Functions for executing builtins and other commands using exec_vp. Also handles signals, piping, redirection and foreground and background processes.
- builtins.c - Functions for builtins like ls, pwd, cd, pinfo, fg, bg, overkill, kjob, quit, setenv and unsetenv.
- globals.h - Global variables (home, working directory, jobs, last working directory) and constants.
- header.h - All required headers from std c library.
- utils.c - Utility functions for job handling, printing, processing path strings and tokenization.
- nightswatch.c - Function for nightswatch command.
- history.c - Functions for storing history and history command.
- makefile - makefile for compiling all c files.

## Assumptions
- The implementation of nightswatch, interrupt requires the /proc/interrupts file to be in a specific format. More precisely, values for keyboard controller interrupts must be the third line in the file. 
- Piped commands will run only in foreground.
- Builtins will run only in foreground.

*Assumptions made for allocating memory to various arrays, the limits can be changed in globals.h* 

- Number of commands in a single line (; separated), wont be more than 100.
- Size for a file path wont be more than 1024
- No command will have more than 100 args.