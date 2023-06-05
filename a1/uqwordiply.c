// uqwordiply.c
//
// By Quinn Horton
//
// The main file for UQ Wordiply, the first assignment task for CSSE2310 in
// semester 1, 2023.

// Header files required for the program
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <csse2310a1.h>
#include "uqwordiply.h"

// Stores an array of strings, its length, the total non-terminating char count
// 	and the length of the longest word.
typedef struct {
    char** values;
    int length;
    int charCount;
    int maxWordLength;
} WordArray;

// Stores the values needed after each guess is parsed.
typedef struct {
    char* guess;
    int guessLen;
    bool reachedEOF;
} GuessReturn;

/* free_word_array()
 * ---------------
 *  Frees the memory in the provided WordArray.
 *
 *  wordArray: The WordArray pointer that needs to be freed.
 */
void free_word_array(WordArray* wordArray) {
    for (int i = 0; i < wordArray->length; i++) {
	free(wordArray->values[i]);
    }
    free(wordArray->values);
    free(wordArray);
}

/* str_to_upper()
 * ---------------
 *  Convert a string from lowercase to uppercase.
 *
 *  str: The string that is to get converted to uppercase.
 *
 *  Returns: str with all characters capitalised.
 */
char* str_to_upper(char* str) {
    int len = strlen(str);
    char* upper = (char*)calloc(len + 1, sizeof(char));
    for (int i = 0; i < len; i++) {
	upper[i] = (char)toupper(str[i]);
    }
    upper[len] = '\0';
    return upper;
}

/* is_all_alpha()
 * ---------------
 *  Determines whether the full string provided is alphabetical.
 *
 *  word: The string to check is alphabetical.
 *
 *  Returns: true if every character in the string is alphabetical, false
 *  	otherwise.
 */
bool is_all_alpha(char* str) {
    for (int c = 0; c < strlen(str); c++) {
	if (!isalpha(str[c])) {
	    return false;
	}
    }
    return true;
}	

/* is_valid_len()
 * ---------------
 *  Check whether the provided len value is valid.
 *
 *  len: The string passed in as the '--len' value.
 *
 *  Returns: true if len is the number 3 or 4, false otherwise.
 */
bool is_valid_len(char* len) {
    if (strlen(len) > 1) {
	return false;
    } else if (!isdigit(len[0])) {
	return false;
    } else if (atoi(len) < MIN_START_LEN || atoi(len) > MAX_START_LEN) {
	return false;
    }
    return true;
}

/* are_args_valid()
 * ---------------
 *  Check that the provided command line arguments are valid. Returns an error
 *         code if they are not.
 *
 *  argc: The number of commandline arguments provided to the program.
 *  argv: The array of commandline arguments provided to the program.
 * 
 *  Returns: 0 if arguments are valid, otherwise an error code per the
 *  	following scheme:
 *             1 - Both "--len" and "--start" arguments supplied
 *             2 - Option argument provided without value
 *             3 - Invalid "--len" argument provided
 *             4 - Unexpected argument provided
 *             5 - Argument provided multiple times
 */
int are_args_valid(int argc, char** argv) {
    bool lenProv = false, startProv = false, dictProv = false;
    int lenIndex = -1, startIndex = -1, dictIndex = -1;
    
    // Iterates over the provided commandline arguments to extract info and
    //     test for some errors.
    for (int i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "--len")) {
	    if (lenProv) {
		return 5;
	    } else if (startIndex == i - 1 || dictIndex == i - 1) {
		return 2;
	    }
	    lenProv = true;
	    lenIndex = i;
	} else if (!strcmp(argv[i], "--start")) {
	    if (startProv) {
		return 5;
	    } else if (lenIndex == i - 1 || dictIndex == i - 1) {
		return 2;
	    }
	    startProv = true;
	    startIndex = i;
	} else if (!strcmp(argv[i], "--dictionary")) {
	    if (dictProv) {
		return 5;
	    } else if (lenIndex == i - 1 || startIndex == i - 1) {
		return 2;
	    }
	    dictProv = true;
	    dictIndex = i;
	}
    }

    // Using info found during iteration, tests for further errors.
    if (lenProv && startProv) {
	return 1;
    } else if (argc < 1 + 2 * (lenProv + startProv + dictProv)) {
        return 2;
    } else if (lenProv && !is_valid_len(argv[lenIndex + 1])) {
	return 3;
    } else if (argc > 1 + 2 * (lenProv + startProv + dictProv) ||
	    (argc > 1 && lenIndex != 1 && startIndex != 1 && dictIndex != 1)) {
	return 4;
    }
    return 0;
}

/* is_start_valid()
 * ---------------
 *  Determines whether the provided '--start' argument is valid. Returns an
 * 	error code if not.
 *
 *  argc: The number of commandline arguments provided.
 *  argv: An array of the commandline arguments provided.
 *
 *  Returns: 0 if the '--start' argument is valid, otherwise the following
 *  	error code:
 * 	    1 - Contains non-letter characters
 * 	    2 - Invalid length (neither 3 or 4)
 */
int is_start_valid(int argc, char** argv) {
    int startIndex = 0;
    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], "--start")) {
	    startIndex = i + 1;
	}
    }

    // If no '--start' is found, the argument is valid.
    if (!startIndex) {
	return 0;
    }

    // Checks the length of provided start argument is valid.
    if (strlen(argv[startIndex]) < MIN_START_LEN || 
	    strlen(argv[startIndex]) > MAX_START_LEN) {
	return 2;
    } else {
	if (!is_all_alpha(argv[startIndex])) {
	    return 1;
	}

	return 0;
    }
}

/* get_start_word()
 * ---------------
 *  Determines the starting word for the game. This will be the '--start'
 * 	argument value from the commandline arguments if provided, otherwise
 * 	the value from the provided library.
 * 
 *  argc: The number of commandline arguments provided.
 *  argv: An array of the values of the commandline arguments passed.
 *
 *  Returns: the starting word for the game.
 */
char* get_start_word(int argc, char** argv) {
    int len = 0;
    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], "--start")) {
	    return str_to_upper(argv[i + 1]); 
	} else if (!strcmp(argv[i], "--len")) {
	    len = atoi(argv[i + 1]);
	    break;
	}
    }
    const char* starter = get_wordiply_starter_word(len);
    return strdup(starter);
}

/* get_dict_arg()
 * ---------------
 *  Looks through the commandline arguments provided and returns the
 *  	'--dictionary' argument if provided.
 *
 *  argc: The number of commandline arguments provided.
 *  argv: An array of the commandline arguments passed.
 *
 *  Returns: a string containing the dictionary filepath if provided,
 *  	otherwise the default dictionary path.
 */
char* get_dict_arg(int argc, char** argv) {
    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], "--dictionary")) {
	    return argv[i + 1];
	}
    }
    return DEF_DICT;
}

/* open_dict()
 * ---------------
 *  Attempts to open the dictionary file parsed, exiting with error code 3 if
 *  	unsuccessful. Returns the open dictionary pointer if successful.
 *
 *  dict: The filepath of the dictionary file to open.
 *
 *  Returns: the open file pointer to the dictionary.
 *  Errors: if the provided dictionary file is unable to be opened, an error
 *  	message will be printed to stderr, and it will exit with code 3.
 */
FILE* open_dict(char* dict) {
    FILE* openDict = fopen(dict, "r");
    if (!openDict) {
	fprintf(stderr, "%s%s%s", DICT_ERR_L, dict, DICT_ERR_R);
	exit(3);
    }
    return openDict;
}

/* dict_extract()
 * ---------------
 *  Takes the dictionary file passed in and creates a WordArray of words that
 *  	are both completely alphabetical and contain the provided starter word.
 *
 *  dict: The pointer to the provided dictionary file.
 *  start: The starter word for the game.
 *
 *  Returns: an array of the valid words in the dictionary that contain the
 *  	starter word.
 */
WordArray* dict_extract(FILE* dict, char* start) {
    WordArray* words = (WordArray*)malloc(sizeof(WordArray));
    words->values = (char**)malloc(0);
    words->length = 0;
    words->maxWordLength = 0;
    char buffer[BUFFER_SIZE];
    char* word = fgets(buffer, BUFFER_SIZE, dict);
    char* wordFiltered;
    int wordLen;

    // Iterates over lines in dictionary file, ignores empty lines and removes
    //     lines breaks.
    while (word) {
	if (!strcmp(word, "\n")) {
	    break;
	}
	if (word[strlen(word) - 1] == '\n') {
	    wordLen = strlen(word) - 1;
	} else {
	    wordLen = strlen(word);
	}
	char* noNewline = strndup(word, wordLen);
	wordFiltered = str_to_upper(noNewline);
	free(noNewline);
	
	// Adds only words that contain the starter (to optimise memory).
	if (is_all_alpha(wordFiltered) &&
		strstr(wordFiltered, start) != NULL) {
	    words->values = realloc(words->values,
		    (words->length + 1) * sizeof(char*));
	    words->values[words->length] = strdup(wordFiltered);
	    words->length++;
	    if (strlen(wordFiltered) > words->maxWordLength) {
		words->maxWordLength = strlen(wordFiltered);
	    }
	}
	free(wordFiltered);
	word = fgets(buffer, BUFFER_SIZE, dict);
    }
    fclose(dict);
    return words;
}

/* is_guess_valid()
 * ---------------
 *  Checks whether the guess entered is valid.
 *
 *  guess: The guess to be checked.
 *  dict: The dictionary of possible words.
 *  start: The starting word that needs to be included.
 *
 *  Returns: a bool representing whether the provided guess is valid.
 */
bool is_guess_valid(char* guess, WordArray* dict, char* start) {
    // Checks that the guess doesn't contain non-letter characters.
    if (!is_all_alpha(guess)) {
	printf(GUESS_ERR_ALPHA);
	return false;
    }

    // Checks that the guess contains the starter word.
    if (strstr(guess, start) == NULL) {
	printf(GUESS_ERR_START);
	return false;
    }

    // Checks that the word IS NOT the starter word.
    if (!strcmp(guess, start)) {
	printf(GUESS_ERR_IS_START);
	return false;
    }

    // Checks that the word is in the dictionary.
    bool inDict = false;
    for (int i = 0; i < dict->length; i++) {
	if (!strcmp(guess, dict->values[i])) {
	    inDict = true;
	    break;
	}
    }
    if (!inDict) {
	printf(GUESS_ERR_DICT);
	return false;
    }

    return true;
}

/* add_guess()
 * ---------------
 *  Adds the guess to the provided word array and updates counters.
 *
 *  guesses: The array of current guesses that the guess is to be added to.
 *  guess: The guess to append.
 *  guessLen: The length of the guess being appended.
 */
void add_guess(WordArray* guesses, char* guess, int guessLen) {
    guesses->values = (char**)realloc(guesses->values,
	    ++(guesses->length) * sizeof(char*));
    guesses->values[guesses->length - 1] = strdup(guess); 
    guesses->charCount += guessLen;
    if (guessLen > guesses->maxWordLength) {
	guesses->maxWordLength = guessLen;
    }
}

/* read_in_guess()
 * ---------------
 *  Reads in the next guess provided to stdin by the user.
 *
 *  guessNum: The numberical index of the current guess.
 *
 *  Returns: whether the reading terminated with an EOF
 */
GuessReturn* read_in_guess(int guessNum) {
    int guessBufferLen = BUFFER_SIZE;
    char* guess = (char*)calloc(guessBufferLen, sizeof(char));
    guess[0] = '\0';
    int charCount = 1;
    printf("Enter guess %d:\n", guessNum);
    int buffer = fgetc(stdin);

    // Iterates through provided characters, adding them to the guess buffer.
    // Stops at newline or when EOF is reached.
    while (buffer != EOF && buffer != (int)'\n') {
	if (charCount >= guessBufferLen) {
	    guessBufferLen *= 2;
	    guess = (char*)realloc(guess, guessBufferLen * sizeof(char));
	}
	guess[charCount - 1] = (char)toupper(buffer);
	guess[++charCount] = '\0';
	buffer = fgetc(stdin);
    }
    GuessReturn* result = (GuessReturn*)malloc(sizeof(GuessReturn));
    result->guess = guess;
    result->guessLen = charCount - 1;
    result->reachedEOF = buffer == EOF;
    return result;
}

/* collect_guesses()
 * ---------------
 *  Collects user guesses, verifies that they are correct, and returns them at
 *  	the end.
 *
 *  dict: The words from the dictionary.
 *  start: The starter word.
 *
 *  Returns: the successful guesses.
 *  Errors: if EOF is reached at the beginning of the first guess, allocated
 *  	memory is freed and program exits with code 4.
 */
WordArray* collect_guesses(WordArray* dict, char* start) {
    WordArray* guesses = (WordArray*)malloc(sizeof(WordArray));
    guesses->values = (char**)malloc(0);
    guesses->length = 0, guesses->charCount = 0, guesses->maxWordLength = 0;
    
    // Collects up to 5 guesses
    while (guesses->length < 5) {
	GuessReturn* result = read_in_guess(guesses->length + 1);
	char* guess = result->guess;
	int guessLen = result->guessLen;
	bool reachedEOF = result->reachedEOF;
	free(result);
	
	// Exits when no guess has been entered (first char is EOF)
	if (guessLen == 0 && reachedEOF) {
	    free(guess);
	    if (guesses->length == 0) {
		free(start);
		free_word_array(dict);
		free_word_array(guesses);
		exit(4);
	    } else {
		return guesses;
	    }
	}
	
	// Adds valid guess to array or prints error message for invalid guess
	if (is_guess_valid(guess, dict, start)) {
	    bool alreadyGuessed = false;
	    for (int i = 0; i < guesses->length; i++) {
		if (!strcmp(guess, guesses->values[i])) {
		    alreadyGuessed = true;
		}
	    }
	    if (!alreadyGuessed) {
		add_guess(guesses, guess, guessLen);	
	    } else {
		printf(GUESS_ERR_ALREADY);
	    }
	}
	free(guess);
    }
    return guesses;
}

/* get_longest_words()
 * ---------------
 *  Reads through the words supplied and returns the longest ones.
 *
 *  words: The array of words.
 *
 *  Returns: the longest words in the array.
 */
WordArray* get_longest_words(WordArray* words) {
    WordArray* longest = (WordArray*)malloc(sizeof(WordArray));
    longest->values = (char**)malloc(0);
    longest->length = 0;
    for (int i = 0; i < words->length; i++) {
	if (strlen(words->values[i]) == words->maxWordLength) {
	    longest->values = realloc(longest->values,
		    (longest->length + 1) * sizeof(char*));
	    longest->values[longest->length] = strdup(words->values[i]);
	    (longest->length)++;
	}
    }
    return longest;
}

int main(int argc, char** argv) {
    // Validates arguments, starter word and dictionary
    if (are_args_valid(argc, argv)) {
	fprintf(stderr, "%s", CLA_ERR);
	exit(1);
    }
    if (is_start_valid(argc, argv)) {
	fprintf(stderr, "%s", START_ERR);
	exit(2);
    }
    char* start = get_start_word(argc, argv);
    char* dictPath = get_dict_arg(argc, argv);
    FILE* dict = open_dict(dictPath);
    WordArray* words = dict_extract(dict, start);

    // Prints starter message
    printf("%s\n%s%s\n%s\n", START_LINE_1, START_LINE_2, start, START_LINE_3);
    
    // Collects and processes guesses
    WordArray* guesses = collect_guesses(words, start);

    //Prints final message
    printf("\n%s%d\n", SUM_TOTAL, guesses->charCount);
    WordArray* longestGuesses = get_longest_words(guesses);
    printf("%s\n", SUM_LONGEST);
    for (int i = 0; i < longestGuesses->length; i++) {
	printf("%s (%d)\n", longestGuesses->values[i], guesses->maxWordLength);
    }
    WordArray* longestPossible = get_longest_words(words);
    printf("%s\n", SUM_POSSIBLE);
    for (int i = 0; i < longestPossible->length; i++) {
	printf("%s (%d)\n", longestPossible->values[i], words->maxWordLength);
    }

    // Frees memory
    free(start);
    free_word_array(words);
    free_word_array(guesses);
    free_word_array(longestGuesses);
    free_word_array(longestPossible);
    
    return 0;
}

