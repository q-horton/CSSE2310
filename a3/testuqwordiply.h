// Header file
#ifndef TUW_HEADER
#define TUW_HEADER

// Inclusions
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <math.h>
#include <time.h>
#include <csse2310a3.h>

// Struct definitions
typedef struct {
    char* file;
    char** args;
    int argCount;
} Job;

typedef struct {
    Job** jobs;
    int count;
} JobSpecs;

typedef struct {
    int testOut[2];
    int testErr[2];
    int demoOut[2];
    int demoErr[2];
} TestPipes;

typedef struct {
    JobSpecs* jobSpecs;
    pid_t** procs;
    int numProcs;
} Freeable;

// Constants
#define QUIET "--quiet"
#define PARALLEL "--parallel"
#define OPT_ARG "--"
#define COMMA ','
#define COMMA_STR ","
#define NULL_TERM '\0'
#define COMMENT '#'
#define CMP_PROG "uqcmp"
#define NULL_FILE "/dev/null"
#define DEMO "demo-uqwordiply"
#define OUT_NAME "stdout"
#define ERR_NAME "stderr"

#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2
#define TEST_TO_CMP 3
#define DEMO_TO_CMP 4
#define ARGS_W_BOTH_OPT 5
#define ARGS_W_ONE_OPT 4
#define ARGS_W_NO_OPT 3
#define EXEC_FAIL_ERR 69
#define NUM_FORKS 4
#define TEST_PROG_FORK 0
#define DEMO_PROG_FORK 1
#define OUT_CMP_FORK 2
#define ERR_CMP_FORK 3

// Function declarations
bool are_cla_valid(int argc, char** argv);
bool args_contain(int argc, char** argv, char* str);
FILE* open_job_spec_file(char* jobSpec);
JobSpecs* init_job_specs();
void free_job_specs(JobSpecs* jobSpecs);
Job* job_from_line(char* line);
JobSpecs* read_job_specs_file(FILE* jobSpec, char* fileName);
TestPipes* create_test_pipes();
void close_test_pipes(TestPipes* pipes, bool safe);
void run_prog(char* prgm, char* input, int argc, char** argv, TestPipes* pipes,
	int output, int error, Freeable* memory);
char* create_cmp_pref(int jobNum, char* cmpType);
void run_cmp(bool quiet, int test, int demo, TestPipes* pipes,
	Freeable* memory, char* arg, int jobNum);
pid_t* run_test_job(char* prgm, char* input, bool quiet, int argc,
	char** argv, Freeable* memory, int jobNum);
bool report_job_results(int* exitStatuses, int jobNum);
void sleep_two_secs();
bool run_tests_series(bool quiet, char* testProgram, JobSpecs* jobSpecs);
bool run_tests_parallel(bool quiet, char* testProgram, JobSpecs* jobSpecs);
void catch_sig_int(int s);
char** prepare_com_args(char** argv, int argc, char* prgm);

// Error codes
// Used when the provided commandline arguments are invalid
#define INV_ARGS "Usage: testuqwordiply [--quiet] [--parallel] testprogram \
jobfile"

// Used when the provided job file is invalid
#define INV_JOB_1 "testuqwordiply: Unable to open job file \""
#define INV_JOB_2 "\""

// Used when the syntax of a job in the jobfile is invalid
#define INV_SYN_1 "testuqwordiply: syntax error on line "
#define INV_SYN_2 " of \""
#define INV_SYN_3 "\""

// Used when a file within a job is unable to be opened
#define INV_FILE_1 "testuqwordiply: unable to open file \""
#define INV_FILE_2 "\" specified on line "
#define INV_FILE_3 " of \""
#define INV_FILE_4 "\""

// Used when a job file is empty
#define NO_JOBS_1 "testuqwordiply: no jobs found in \""
#define NO_JOBS_2 "\""

#endif
