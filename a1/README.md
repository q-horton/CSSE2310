# Assignment 1

## Performance

The result that I obtained from assignment 1 was 98% (73.50 / 75).

## Premise

From the assignment specification, the the premise of this assignment was:

> The goal of this assignment is to give you practice at C programming. You will be building on this ability in the remainder of the course (and subsequent programming assignments will be more difficult than this one). You are to create a program (called uqwordiply) which allows users to play the Wordiply game (wordiply.com). More details are provided below but you may wish to play the game to gain a practical understanding of how it operates. The assignment will also test your ability to code to a particular programming style guide, and to use a revision control system appropriately.

## Library

For this assignment, there was a library provided that was used within the implementation. This describes the functions that were added, which were:

```c
const char* get_wordiply_starter_word(unsigned int wordLen);
```

This function provides a three or four character alphabetic string to serve as the starter word for the game. It takes in a length parameter of either 3 or 4, or a wildcard of 0 which will result in a random choice of 3 or 4 characters.
