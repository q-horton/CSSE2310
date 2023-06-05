// crackserver.c
//
// By Quinn Horton
//
// The server program for a brute-force password cracking system, the fourth
// assignment task for CSSE2310 in semester 1, 2023.

#include "crackserver.h"

int main(int argc, char** argv) {
    // Checks commandline arguments
    if (!are_args_valid(argc, argv)) {
	fprintf(stderr, ERR_USAGE);
	exit(1);
    }
    ArgVals* args = extract_cla(argc, argv);

    // Reads in dictionary file
    FILE* dict = fopen(args->dict, "r");
    if (!dict) {
	fprintf(stderr, ERR_DICT, args->dict);
	exit(2);
    }
    WordArray* dictWords = parse_dict(dict);
    fclose(dict);
    if (!dictWords->length) {
	fprintf(stderr, ERR_DICT_NO_WORDS);
	exit(3);
    }

    // Opens socket for listening
    Socket* socket = open_port(args->port);
    fprintf(stderr, "%d\n", socket->port);
    fflush(stderr);
    connection_handler(socket->sock, args->maxConn, dictWords);
    return 0;
}

/* is_pos_int()
 * ---------------
 *  Determines whether the provided string is a non-negative integer.
 *
 *  string: An arbitrary string to evaluate.
 *
 *  Returns: a bool representing whether the string is a non-negative integer. 
 */
bool is_pos_int(char* string) {
    for (int i = 0; i < strlen(string); i++) {
	if (!isdigit(string[i])) {
	    return false;
	}
    }
    return true;
}

/* are_args_valid()
 * ---------------
 *  Determines whether the commandline arguments provided are valid arguments.
 *
 *  argc: The number of commandline arguments provided.
 *  argv: The values of the commandline arguments.
 *
 *  Returns: a boolean representation of whether the args are valid.
 */
bool are_args_valid(int argc, char** argv) {
    if (argc > MAX_ARGS) {
	return false;
    } else if (argc % 2 == 0) {
	return false;
    }
    int conn = -1, port = -1, dict = -1;
    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], CONN)) {
	    if (dict != i - 1 && conn > 0 && i % 2 == 0) {
		return false;
	    }
	    conn = i;
	} else if (!strcmp(argv[i], PORT)) {
	    if (dict != i - 1 && port > 0 && i % 2 == 0) {
		return false;
	    }
	    port = i;
	} else if (!strcmp(argv[i], DICT)) {
	    if (dict != i - 1 && dict > 0 && i % 2 == 0) {
		return false;
	    }
	    dict = i;
	}
    }
    if (argc != 2 * ((conn > 0) + (port > 0) + (dict > 0)) + 1) {
	return false;
    } else if (conn > 0 && !is_pos_int(argv[conn + 1])) {
	return false;
    } else if (port > 0 && !is_pos_int(argv[port + 1])) {
	return false;
    } else if (port > 0) {
	int portNum = atoi(argv[port + 1]);
	if (!(portNum == 0 || (portNum >= MIN_PORT && portNum <= MAX_PORT))) {
	    return false;
	}
    }
    return true;
}

/* extract_cla()
 * ---------------
 *  Pulls out the commandline arguments provided into an easily accessible
 *  format. Where a value is not specified, a default is provided in its place.
 *
 *  argc: The number of commandline arguments provided.
 *  argv: The values of the commandline rguments provided.
 *
 *  Returns: An ArgVals pointer containing the arguments in most usable type.
 */
ArgVals* extract_cla(int argc, char** argv) {
    ArgVals* args = (ArgVals*)malloc(sizeof(ArgVals));
    args->maxConn = 0;
    args->port = NULL;
    args->dict = NULL;
    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], CONN)) {
	    args->maxConn = atoi(argv[i + 1]);
	} else if (!strcmp(argv[i], PORT)) {
	    args->port = strdup(argv[i + 1]);
	} else if (!strcmp(argv[i], DICT)) {
	    args->dict = strdup(argv[i + 1]);
	}
    }
    if (!args->port) {
	args->port = strdup(EPHEMERAL);
    }
    if (!args->dict) {
	args->dict = strdup(DEF_DICT);
    }
    return args;
}

/* parse_dict()
 * ---------------
 *  Parses the dictionary file, reading in the words to be more easily
 *  accessed. Omits words that exceed the character limit.
 *
 *  dict: The file pointer to the dictionary.
 *
 *  Returns: a WordArray pointer containing all words from the dictionary that
 *	meet the criteria.
 */
WordArray* parse_dict(FILE* dict) {
    int length = 0;
    char** words = (char**)malloc(0);
    char* word = read_line(dict);
    while (word) {
	if (strlen(word) <= MAX_WORD_LEN) {
	    words = realloc(words, (++length) * sizeof(char*));
	    words[length - 1] = word;
	} else {
	    free(word);
	}
	word = read_line(dict);
    }
    WordArray* dictionary = (WordArray*)malloc(sizeof(WordArray));
    dictionary->length = length;
    dictionary->words = words;
    return dictionary;
}

/* open_port()
 * ---------------
 *  Opens a server for listening on the port provided. Listens for connections.
 *
 *  port: The port to open on.
 *
 *  Returns: The Socket used to interact with the network socket.
 *  Errors: Should the the port be unable to be opened / listened on, return
 *	an error message and exit with code 4.
 *  REF: CSSE2310 2023 Semester 1, week 10 lecture code samples.
 */
Socket* open_port(char* port) {
    // Sets up address information
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &ai)) {
        freeaddrinfo(ai);
        fprintf(stderr, ERR_PORT);
        exit(4);
    }

    // Opens socket
    int serv = socket(AF_INET, SOCK_STREAM, 0);
    //int v = 1;
    //setsockopt(serv, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
    if (bind(serv, ai->ai_addr, sizeof(struct sockaddr))) {
        fprintf(stderr, ERR_PORT);
        exit(4);
    }

    // Gets port number
    struct sockaddr_in ad;
    memset(&ad, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    if (getsockname(serv, (struct sockaddr*)&ad, &len)) {
	fprintf(stderr, ERR_PORT);
        exit(4);
    }

    // Opens for listening
    if (listen(serv, 1)) {
        fprintf(stderr, ERR_PORT);
        exit(4);
    }
    Socket* socket = (Socket*)malloc(sizeof(Socket));
    socket->sock = serv;
    socket->port = ntohs(ad.sin_port);
    return socket;
}

/* connection_handler()
 * ---------------
 *  Accepts incoming client connection requests, sets up the read and write
 *  file pointers, and parses messages.
 *
 *  listenFd: The file descriptor to listen to for incoming connection
 *	requests.
 *  maxConn: The maximum amount of connections that may exist at any one time.
 *  dict: The dictionary of words to test against when decrypting a string.
 *
 *  REF: CSSE2310 2023 Semester 1, week 8 lecture code samples.
 */
void connection_handler(int listenFd, int maxConn, WordArray* dict) {
    sem_t writeProtect;
    sem_init(&writeProtect, 0, 1);
    int liveConnections = 0;
    while(true) {
	if (!maxConn || liveConnections < maxConn) {
	    int fd = accept(listenFd, 0, 0);
	    sem_wait(&writeProtect);
	    liveConnections++;
	    sem_post(&writeProtect);
	    if (fd >= 0) {
		Client* clientPtr = malloc(sizeof(Client));
		clientPtr->fd = fd;
		clientPtr->dict = dict;
		clientPtr->writeProtect = &writeProtect;
		clientPtr->liveConn = &liveConnections;
		pthread_t threadId;
		pthread_create(&threadId, NULL, new_client_thread, clientPtr);
		pthread_detach(threadId);
	    }
	}
    }
}

/* is_valid_salt()
 * ---------------
 *  Checks the provided salt string to verify whether it meets the required
 *  length and character constraints.
 *
 *  salt: The string to check.
 *
 *  Returns: whether the salt is valid.
 */
bool is_valid_salt(char* salt) {
    if (strlen(salt) != 2) {
	// If invalid length
	return false;
    } else if (!isalpha(salt[0]) && !isdigit(salt[0]) && salt[0] != '.' &&
	    salt[0] != '/') {
	// If first character is invalid
	return false;
    } else if (!isalpha(salt[1]) && !isdigit(salt[1]) && salt[1] != '.' &&
	    salt[1] != '/') {
	// If second character is invalid
	return false;
    } else {
	return true;
    }
}

/* is_valid_command()
 * ---------------
 *  Determines whether the provided command is a valid crack or crypt request.
 *
 *  command: An array of the command arguments
 *
 *  Returns: whether the command is valid.
 */
bool is_valid_command(char** command) {
    // Checks whether is command and all args provided
    if (strcmp(command[0], "crack") && strcmp(command[0], "crypt")) {
	return false;
    } else if (!command[1] || !command[2]) {
	return false;
    } else if (!strlen(command[1]) || !strlen(command[2])) {
	return false;
    } else if (!strcmp(command[0], "crack")) {
	// Checks that crack arguments are valid
	if (strlen(command[1]) != ENCRYPT_LEN) {
	    return false;
	} else if (!is_pos_int(command[2])
		|| strlen(command[2]) > MAX_INT_LEN) {
	    return false;
	} else if (atoi(command[2]) < MIN_THREADS
		|| atoi(command[2]) > MAX_THREADS) {
	    return false;
	}
	char* salt = strndup(command[1], 2);
	if (!is_valid_salt(salt)) {
	    free(salt);
	    return false;
	}
	free(salt);
    } else if (!strcmp(command[0], "crypt")) {
	// Checks that crypt args are valid
	if (!is_valid_salt(command[2])) {
	    return false;
	}
    }
    return true;
}

/* talk_to_client()
 * ---------------
 *  Handles the interactions with a connected client.
 *
 *  read: The file pointer to read in client messages.
 *  write: The file pointer to write messages back to the client.
 *  dict: The dictionary of words used in decrypting a string.
 */
void talk_to_client(FILE* read, FILE* write, WordArray* dict) {
    char* client;
    while ((client = read_line(read))) {
	char** command = split_by_char(client, ' ', COMM_ARGC);
	if (is_valid_command(command)) {
	    if (!strcmp(command[0], "crypt")) {
		char* encrypted = crypt(command[1], command[2]);
		fprintf(write, "%s\n", encrypted);
	    } else if (!strcmp(command[0], "crack")) {
		int numThreads = atoi(command[2]);
		char* plaintext = crypt_crack(command[1], numThreads, dict);
		if (plaintext) {
		    fprintf(write, "%s\n", plaintext);
		} else {
		    fprintf(write, ":failed\n");
		}
	    }
	} else {
	    fprintf(write, ":invalid\n");
	}
	fflush(write);
	free(client);
    }
}

/* new_client_thread()
 * ---------------
 *  Creates a new thread for each client that connects to the server.
 *
 *  clientPtr: The file descriptor of the new connection.
 *
 *  Returns: NULL
 */
void* new_client_thread(void* clientPtr) {
    Client client = *(Client*)clientPtr;
    free(clientPtr);
    int fd2 = dup(client.fd);
    FILE* read = fdopen(client.fd, "r");
    FILE* write = fdopen(fd2, "w");
    talk_to_client(read, write, client.dict);
    fclose(read);
    fclose(write);
    sem_wait(client.writeProtect);
    (*client.liveConn)--;
    sem_post(client.writeProtect);
    return NULL;
}

/* cracking_thread()
 * ---------------
 *  Runs a thread for cracking processes. Runs crypt_r() over a subset of the
 *  dictionary with the salt provided, and checks for a match with the provided
 *  ciphertext. Updates the main client thread if successfully found.
 *
 *  passed: The data passed into the thread, is a void* wrapped Cracking*
 *	object.
 *
 *  Returns: NULL
 *  REF: CSSE2310 2023 Semester 1, week 8 lecture code samples.
 */
void* cracking_thread(void* passed) {
    Cracking* tools = (Cracking*)passed;
    for (int i = 0; i < tools->numWords; i++) {
	// If the word is found, stop looking
	if (*(tools->word)) {
	    break;
	}

	// Encrypt the current word to see if it matches
	struct crypt_data data;
	memset(&data, 0, sizeof(struct crypt_data));
	char* attempt = crypt_r(tools->dict[i], tools->salt, &data);
	
	// If a match is found, send the result to the main thread
	if (!strcmp(attempt, tools->encryption)) {
	    sem_wait(tools->writeAccess);
	    *(tools->word) = tools->dict[i];
	}	
    }
    return NULL;
}

/* crypt_crack()
 * ---------------
 *  Cracks the provided encryption from the dictionary provided. Splits this
 *  load across the number of threads provided.
 *
 *  encrypted: The string to decrypt.
 *  numThreads: The number of threads to run the encryption on.
 *  dict: The dictionary of words to test encryptions against.
 *
 *  Returns: the decrypted word.
 *  Errors: In the event that the decryption should fail, the function returns
 *	a NULL pointer.
 */
char* crypt_crack(char* encrypted, int numThreads, WordArray* dict) {
    // Establish variables for simpler cracking
    sem_t writeAccess;
    sem_init(&writeAccess, 0, 1);
    char** plaintext = malloc(sizeof(char*));
    *plaintext = NULL;
    char* salt = strndup(encrypted, 2);
    int wordsPerThread = dict->length / numThreads;
    pthread_t threads[numThreads];

    // Establish the necessary information and create each thread
    for (int i = 0; i < numThreads; i++) {
	Cracking* tools = malloc(sizeof(Cracking));
	tools->writeAccess = &writeAccess;
	tools->word = plaintext;
	if (i == numThreads - 1) {
	    tools->numWords = dict->length - i * wordsPerThread;
	} else {
	    tools->numWords = wordsPerThread;
	}
	tools->salt = salt;
	tools->encryption = encrypted;
	tools->dict = dict->words + i * wordsPerThread;
	pthread_create(threads + i, NULL, cracking_thread, tools);
    }

    // Ensure that all threads have stopped running
    for (int i = 0; i < numThreads; i++) {
	void* v;
	pthread_join(threads[i], &v);
    }
    return *plaintext;
}
