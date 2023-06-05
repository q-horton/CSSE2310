// crackclient.c
//
// By Quinn Horton
//
// The client program for a brute-force password cracking system, the fourth
// assignment task for CSSE2310 in semester 1, 2023.

#include "crackclient.h"

int main(int argc, char** argv) {
    // Validates the provided commandline arguments
    if (argc < MAND_ARG + 1 || argc > OPT_ARG + 1) {
	fprintf(stderr, ARG_NUM_ERR);
	exit(1);
    } else if (argc == OPT_ARG + 1) {
	// If a job file is provided, replaces stdin with it
	int fd = open(argv[OPT_ARG], O_RDONLY);
	if (errno) {
	    fprintf(stderr, JOB_FILE_ERR, argv[OPT_ARG]);
	    exit(2);
	} else {
	    dup2(fd, 0);
	    close(fd);
	}
    }

    // Connects to server and exchanges user's messages
    Server* server = connect_server(argv[MAND_ARG]);
    comm_loop(server);
    fclose(server->read);
    fclose(server->write);
    free(server);
    return 0;
}

/* connect_server()
 * ---------------
 *  Opens the connection to the server on the specified port.
 *
 *  port: The port number or service name to connect to.
 *
 *  Returns: the Server object used to communicate with the server.
 *  Errors: Should the port number not contain a server, or is unable to be
 *	connected to, return an error message and exit with code 3.
 *  REF: CSSE2310 2023 Semester 1, week 10 lecture code samples.
 */
Server* connect_server(const char* port) {
    // Sets up the address information for server connections
    struct addrinfo* ai = NULL;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(IP_ADDR, port, &hints, &ai)) {
        freeaddrinfo(ai);
        fprintf(stderr, PORT_NUM_ERR, port);
        exit(3);
    }

    // Opens the network socket to the server
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(fd, ai->ai_addr, sizeof(struct sockaddr))) {
        fprintf(stderr, PORT_NUM_ERR, port);
        exit(3);
    }

    // Sets up file pointers for easy reading and writing to server
    int fd2 = dup(fd);
    Server* server = malloc(sizeof(server));
    server->read = fdopen(fd, "r");
    server->write = fdopen(fd2, "w");
    return server;
}

/* comm_loop()
 * ---------------
 *  The communication loop. Processes user inputs, sends them to the server,
 *  then parses the response.
 *
 *  server: The Server object used to communicate with the given server.
 */
void comm_loop(Server* server) {
    char* input = read_line(stdin);
    while (input != NULL) {
	// Skips comments and empty lines
	if (input[0] == NULL_TERM || input[0] == COMMENT) {
	    free(input);
	    input = read_line(stdin);
	    continue;
	}

	// Sends line directly to server
	fprintf(server->write, "%s\n", input);
	fflush(server->write);
	free(input);

	// Reads in server's response and parses
	char* reply = read_line(server->read);
	fflush(server->read);
	if (reply == NULL) {
	    fprintf(stderr, SOCK_CLOSE_ERR);
	    fflush(stderr);
	    exit(4);
	} else if (!strcmp(reply, ":invalid")) {
	    printf(REC_INVALID);
	} else if (!strcmp(reply, ":failed")) {
	    printf(REC_FAILED);
	} else {
	    printf("%s\n", reply);
	}
	fflush(stdout);
	free(reply);
	input = read_line(stdin);
    }
}
