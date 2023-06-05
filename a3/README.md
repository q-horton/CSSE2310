# Assignment 3

## Performance

The result that I obtained from assignment 3 was 93% (69.70 / 75).

## Premise

From the assignment specification, the the premise of this assignment was:

> The goal of this assignment is to demonstrate your skills and ability in fundamental process management and communication concepts, and to further develop your C programming skills with a moderately complex program.
>
> You are to create a program called testuqwordiply that creates and manages communicating collections of processes that test a uqwordiply program (from assignment one) according to a job specification file that lists tests to be run. For various test cases, your program will run both a test version of uqwordiply and the assignment one solution (available as demo-uqwordiply) and compare their standard outputs, standard errors and exit statuses and report the results. You will be provided with (and must use) a program named uqcmp that will compare the input arriving on two file descriptors. The assignment will also test your ability to code to a programming style guide and to use a revision control system appropriately.

## Library

For this assignment, there was a library provided that was used within the implementation. This describes the functions that were added, which were:

```c
char* read_line(FILE* stream);
char** split_line(char* line, char delimiter);
char** split_space_not_quote(char* input, int* numTokens);
```

The *read_line()* function returns the next line (up until a newline or EOF) from the provided stream. If it reaches EOF without first reading any characters, it will return NULL, otherwise it will return a null-terminated string (stripped of terminating newline). The string returned will have been malloc'd and need freeing.

The *split_line()* function splits the string *str* into an array split wherever the *delimiter* character is found. It does so by replacing each instance of *delimiter* by the null-terminator '\0', and mallocs the pointer for the array, setting the final element of the array to NULL. The allocated pointer for the array will need freeing.

The *split_space_not_quote()* function acts similar to *split_line()*, but exclusively uses a space as the delimiter, but allows spaces to be preserved in strings surrounded by double quotations. If *numTokens* is not NULL, then the function will set the value at that location to be the number of tokens in the array.
