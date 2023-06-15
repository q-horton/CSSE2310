# Assignment 4

## Performance

The result that I obtained from assignment 4 was 88% (66.25 / 75).

## Premise

From the assignment specification, the the premise of this assignment was:

> The goal of this assignment is to further develop your C programming skills, and to demonstrate your understanding of networking and multithreaded programming. You are to create two programs which together implement a brute-force password cracking system. One program – crackserver – is a network server which accepts connections from clients (including crackclient which you will implement). Clients connect, and provide encrypted passphrases that the server will attempt to crack (recover the original unencrypted passphrase). Clients may also request the server to encrypt passwords for later analysis. Communication between the crackclient and crackserver is over TCP using a newline-terminated text command protocol. Advanced functionality such as connection limiting, signal handling and statistics reporting are also required for full marks.
>
> The assignment will also test your ability to code to a particular programming style guide and to use a revision control system appropriately.

## Library

For this assignment, there was a library provided that was used within the implementation. The library from assignment three was also used. This describes the relevant functions that were added by the assignment four library, which were:

```c
char** split_by_char(char* str, char split, unsigned int maxFields);
```

This function acts similarly to *split_line()* from the third assignment library, but with the addition that there is a limiter on the number of fields it can create, that being the *maxFields* variable passed in. If this is zero, then there is no restriction and the functions act the same (ie `split_line(str, split) == split_by_char(str, split, 0)`).

## Disclaimer

The most recent version in this repository is not what was submitted, it also includes some final functionality that did not get completed prior to submission. The added functionality is the SIGHUP handling and statistic tracking in the server. The original submission can be found in [this commit](https://github.com/q-horton/CSSE2310/commit/8fdc2414cfe897f7715f3cc60fe703480553776d#diff-505eafb11100b073572560ba4cbd6e6b12fb26feeaa0f7cf1c4f486a1e612598).
