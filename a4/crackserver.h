// Header for the crack server program
#ifndef CRACK_SERVER
#define CRACK_SERVER

// Server only imports
#include <ctype.h>
#include <crypt.h>
#include <pthread.h>
#include <semaphore.h>
#include <csse2310a4.h>
#include "cracka4.h"

// Struct definitions
/* Stores the values extracted from the commandline arguments.
 */
typedef struct {
    int maxConn;
    char* port;
    char* dict;
} ArgVals;

/* Stores an array of strings with an associated length, useful for the
 * dictionary.
 */
typedef struct {
    char** words;
    int length;
} WordArray;

/* Contains the useful information after a socket is set up to be listened on.
 */
typedef struct {
    int sock;
    uint16_t port;
} Socket;

/* Stores information that a client thread needs to know.
 */
typedef struct {
    int fd;
    WordArray* dict;
    sem_t* writeProtect;
    int* liveConn;
} Client;

/* Contains the information needed for cracking threads.
 */
typedef struct {
    sem_t* writeAccess;
    char** word;
    int numWords;
    char* salt;
    char* encryption;
    char** dict;
} Cracking;

/* Used to store and protect the global statistics output in the event of a
 * SIGHUP
 */
typedef struct {
    sem_t* writeProtect;
    int clientsConnected;
    int totalClients;
    int totalCrackRequests;
    int succCrackRequests;
    int unCrackRequests;
    int cryptRequests;
    int cryptFuncRequests;
} Statistics;

// Function declarations
bool is_pos_int(char* string); 
bool are_args_valid(int argc, char** argv);
ArgVals* extract_cla(int argc, char** argv);
WordArray* parse_dict(FILE* dict);
Socket* open_port(char* port);
void connection_handler(int listenFd, int maxConn, WordArray* dict);
void talk_to_client(FILE* read, FILE* write, WordArray* dict);
void* new_client_thread(void* clientPtr);
char* crypt_crack(char* encrypted, int numThreads, WordArray* dict);

// Constants
#define MAX_ARGS 7
#define MIN_PORT 1024
#define MAX_PORT 65535
#define MAX_INT_LEN 9
#define MAX_WORD_LEN 8
#define ENCRYPT_LEN 13
#define MIN_THREADS 1
#define MAX_THREADS 50
#define COMM_ARGC 3

#define CONN "--maxconn"
#define PORT "--port"
#define DICT "--dictionary"
#define EPHEMERAL "0"
#define DEF_DICT "/usr/share/dict/words"

// Error messages
#define ERR_USAGE "Usage: crackserver [--maxconn connections] [--port portnum]\
 [--dictionary filename]\n"
#define ERR_DICT "crackserver: unable to open dictionary file \"%s\"\n"
#define ERR_DICT_NO_WORDS "crackserver: no plain text words to test\n"
#define ERR_PORT "crackserver: unable to open socket for listening\n"

#endif
