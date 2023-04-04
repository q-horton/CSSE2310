// Define numerical constants
#define BUFFER_SIZE 52
#define MIN_START_LEN 3
#define MAX_START_LEN 4

// Default dictionary directory
#define DEF_DICT "/usr/share/dict/words"

// Initial error values
#define CLA_ERR "Usage: uqwordiply [--start starter-word | --len length] \
[--dictionary filename]\n" 
#define START_ERR "uqwordiply: invalid starter word\n"
#define DICT_ERR_L "uqwordiply: dictionary file \""
#define DICT_ERR_R "\" cannot be opened\n"

// Starter lines
#define START_LINE_1 "Welcome to UQWordiply!"
#define START_LINE_2 "The starter word is: "
#define START_LINE_3 "Enter words containing this word."

// Guess parsing errors
#define GUESS_ERR_ALPHA "Guesses must contain only letters - try again.\n"
#define GUESS_ERR_START "Guesses must contain the starter word - try again.\n"
#define GUESS_ERR_IS_START "Guesses can't be the starter word - try again.\n"
#define GUESS_ERR_DICT "Guess not found in dictionary - try again.\n"
#define GUESS_ERR_ALREADY "You've already guessed that word - try again.\n"

// Summary / ending lines
#define SUM_TOTAL "Total length of words found: "
#define SUM_LONGEST "Longest word(s) found:"
#define SUM_POSSIBLE "Longest word(s) possible:"
