// Header file for the crack client
#ifndef CRACK_CLIENT
#define CRACK_CLIENT

// Inclusions
#include <errno.h>
#include "cracka4.h"

// Struct definitions

/* Contains the file descriptors needed to read and write to a given server.
 */
typedef struct {
    FILE* read;
    FILE* write;
} Server;

// Function declarations
Server* connect_server(const char* port);
void comm_loop(Server* server);

// Constants
#define MAND_ARG 1
#define OPT_ARG 2
#define IP_ADDR "localhost"
#define NULL_TERM '\0'
#define COMMENT '#'

// Error lines
#define ARG_NUM_ERR "Usage: crackclient portnum [jobfile]\n"
#define JOB_FILE_ERR "crackclient: unable to open job file \"%s\"\n"
#define PORT_NUM_ERR "crackclient: unable to connect to port %s\n"
#define SOCK_CLOSE_ERR "crackclient: server connection terminated\n"
#define REC_INVALID "Error in command\n"
#define REC_FAILED "Unable to decrypt\n"

#endif
