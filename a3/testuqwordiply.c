// testuqwordiply.c
//
// By Quinn Horton [46975919]
//
// The main file for testuqwordiply, the third assignment task for CSSE2310 in
// semester 1, 2023.

#include "testuqwordiply.h"

bool sigIntCaught = false; // Used to catch a SIGINT

int main(int argc, char** argv) {
    // Sets up signal handling
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = catch_sig_int;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, 0);

    // Checks commanline arguments
    if (!are_cla_valid(argc, argv)) {
	fprintf(stderr, "%s\n", INV_ARGS);
	exit(2);
    }
    bool quiet = args_contain(argc, argv, QUIET);
    bool parallel = args_contain(argc, argv, PARALLEL);
    char* testProgram = argv[argc - 2]; // Second last argument
    char* jobFile = argv[argc - 1]; // Last argument

    // Reads in job specification file
    FILE* jobSpec = open_job_spec_file(jobFile);
    JobSpecs* jobSpecs = read_job_specs_file(jobSpec, jobFile);
    fclose(jobSpec);
    if (jobSpecs->count == 0) {
	fprintf(stderr, "%s%s%s\n", NO_JOBS_1, jobFile, NO_JOBS_2);
	free_job_specs(jobSpecs);
	exit(6);
    }

    // Executes the programs / testing
    int exitStatus;
    if (parallel) {
	exitStatus = !run_tests_parallel(quiet, testProgram, jobSpecs);
    } else {
	exitStatus = !run_tests_series(quiet, testProgram, jobSpecs);
    }
    free_job_specs(jobSpecs);
    exit(exitStatus);
    return 0;
}

/* are_cla_valid()
 * ---------------
 *  Determines whether the commandline arguments provided are valid.
 *
 *  argc: The number of commandline arguments provided.
 *  argv: The value of the commandline arguments provided.
 *
 *  Returns: true if the arguments are valid, otherwise false.
 */
bool are_cla_valid(int argc, char** argv) {
    int optLen = strlen(OPT_ARG);
    if (argc < ARGS_W_NO_OPT || argc > ARGS_W_BOTH_OPT) {
	return false;
    } else if (argc == ARGS_W_BOTH_OPT) {
	// Checks if the first two arguments aren't --quiet and --parallel
	if ((strcmp(argv[1], QUIET) || strcmp(argv[2], PARALLEL))
		&& (strcmp(argv[1], PARALLEL)
		|| strcmp(argv[2], QUIET))) {
	    return false;

	// Checks if the last two options are optional
	} else if (!strncmp(argv[ARGS_W_BOTH_OPT - 2], OPT_ARG, optLen)
		|| !strncmp(argv[ARGS_W_BOTH_OPT - 1], OPT_ARG, optLen)) {
	    return false;
	} else {
	    return true;
	}
    } else if (argc == ARGS_W_ONE_OPT) {
	// Checks that one of the first arguments is --quiet or --parallel
	if (strcmp(argv[1], QUIET) && strcmp(argv[1], PARALLEL)) {
	    return false;
	} else if (!strncmp(argv[ARGS_W_ONE_OPT - 2], OPT_ARG, optLen)
		|| !strncmp(argv[ARGS_W_ONE_OPT - 1], OPT_ARG, optLen)) {
	    return false;
	} else {
	    return true;
	}
    } else {
	if (!strncmp(argv[ARGS_W_NO_OPT - 2], OPT_ARG, optLen)
		|| !strncmp(argv[ARGS_W_NO_OPT - 1], OPT_ARG, optLen)) {
	    return false;
	} else {
	    return true;
	}
    }
}

/* args_contain()
 * ---------------
 *  Checks whether one of the arguments is the provided string.
 *
 *  argc: The number of commandline arguments provided.
 *  argv: The values of the commandline arguments.
 *  str: The string to check for.
 *
 *  Returns: true if the one of the arguments is str, false otherwise.
 */
bool args_contain(int argc, char** argv, char* str) {
    for (int i = 0; i < argc; i++) {
	if (!strcmp(argv[i], str)) {
	    return true;
	}
    }
    return false;
}

/* open_job_spec_file()
 * ---------------
 *  Checks the provided job specification file to make sure that it can be
 *  	opened for reading, returns an error message and exits if not.
 *  	If successfully opened, returns the opened job spec file.
 *
 *  jobSpec: The job specification file to check.
 *
 *  Returns: the opened job specification file.
 *  Errors: if the file is unable to be opened, print an error message to
 *  	stderr and exit with code 3.
 */
FILE* open_job_spec_file(char* jobSpec) {
    FILE* opened = fopen(jobSpec, "r");
    if (!opened) {
	fprintf(stderr, "%s%s%s\n", INV_JOB_1, jobSpec, INV_JOB_2);
	exit(3);
    } else {
	return opened;
    }
}

/* init_job_specs()
 * ---------------
 *  Initialises a JobSpecs struct pointer and its attributes.
 *
 *  Returns: initialised JobSpecs pointer.
 */
JobSpecs* init_job_specs() {
    JobSpecs* jobSpecs = (JobSpecs*)malloc(sizeof(JobSpecs));
    jobSpecs->count = 0;
    jobSpecs->jobs = (Job**)malloc(0);
    return jobSpecs;
}

/* free_job_specs()
 * ---------------
 *  Frees the memory associated with a JobSpecs object.
 *
 *  jobSpecs: The object to free.
 */
void free_job_specs(JobSpecs* jobSpecs) {
    for (int i = 0; i < jobSpecs->count; i++) {	
	free(jobSpecs->jobs[i]->file);
	for (int j = 0; j < jobSpecs->jobs[i]->argCount; j++) {
	    free(jobSpecs->jobs[i]->args[j]);
	}
	free(jobSpecs->jobs[i]->args);	
    	free(jobSpecs->jobs[i]);
    }
    free(jobSpecs->jobs);
    free(jobSpecs);
}

/* job_from_line()
 * ---------------
 *  Creates a Job object from the string provided.
 *
 *  line: The line of text to create a Job from.
 *
 *  Returns: the Job representation of 'line'.
 */
Job* job_from_line(char* line) {
    // Splits the file from the arguments
    char** commaSplit = split_line(line, COMMA);
    if (commaSplit[2] != NULL) {
	free(commaSplit);
	return NULL; // Returns error if too many args passed
    } else if (commaSplit[0][0] == NULL_TERM) {
	free(commaSplit);
	return NULL; // Returns error if file name is blank
    }
    char* fileName = strdup(commaSplit[0]);
    char* rawArgs = strdup(commaSplit[1]);
    free(commaSplit);

    // Extracts arguments from string
    int* numArgs = malloc(sizeof(int));
    char** args = split_space_not_quote(rawArgs, numArgs);
    char** jobArgs = (char**)calloc(*numArgs, sizeof(char*));
    for (int i = 0; i < *numArgs; i++) {
	jobArgs[i] = strdup(args[i]);
    }
    free(rawArgs);
    free(args);

    // Creates and populates the Job from the values received
    Job* job = (Job*)malloc(sizeof(Job));
    job->file = fileName;
    job->args = jobArgs;
    job->argCount = *numArgs;
    free(numArgs);
    return job;
}

/* read_job_specs_file()
 * ---------------
 *  Reads through the provided job specification file and extracts the
 *  	information it provides. If any of the lines are syntactically invalid,
 *  	then return an error message and exit.
 *
 *  jobSpec: The job specification file to read.
 *
 *  Returns: the job lines provided in an easily processible standard.
 *  Errors: if any job line is syntactically invalid, print an error message
 *  	and exit with code 4.
 */
JobSpecs* read_job_specs_file(FILE* jobSpec, char* fileName) {
    JobSpecs* jobSpecs = init_job_specs();
    int lineNo = 0;
    char* line = read_line(jobSpec);

    // Iterates over each line in the provided job file
    while (line != NULL) {
	lineNo++;
	bool valid = true;
	if (line[0] == COMMENT || line[0] == NULL_TERM) {
	    // If line is a comment or empty, move on
	    free(line);
	    line = read_line(jobSpec);
	    continue;
	} else if (strstr(line, COMMA_STR)) {
	    Job* job = job_from_line(line);
	    if (job) {
		jobSpecs->jobs = realloc(jobSpecs->jobs, ++(jobSpecs->count)
			* sizeof(Job));
		jobSpecs->jobs[jobSpecs->count - 1] = job;
	    } else {
		valid = false;
	    }
	} else {
	    valid = false;
	}

	// Checks for and handles errors with the jobs
	if (!valid) {
	    fprintf(stderr, "%s%d%s%s%s\n", INV_SYN_1, lineNo, INV_SYN_2,
		    fileName, INV_SYN_3); 
	    free(line);
	    free_job_specs(jobSpecs);
	    exit(4);
	}
	char* jobFile = jobSpecs->jobs[jobSpecs->count - 1]->file;
	FILE* inputFile = fopen(jobFile, "r");
	if (inputFile) {
	    fclose(inputFile);
	} else {
	    fprintf(stderr, "%s%s%s%d%s%s%s\n", INV_FILE_1, jobFile,
		    INV_FILE_2, lineNo, INV_FILE_3, fileName, INV_FILE_4);
	    free(line);
	    free_job_specs(jobSpecs);
	    exit(5);
	}
	free(line);
	line = read_line(jobSpec);
    }
    return jobSpecs;
}

/* create_test_pipes()
 * ---------------
 *  Sets up the pipes that are needed when executing programs for testing.
 *
 *  Returns: a struct with the required pipes.
 */
TestPipes* create_test_pipes() {
    TestPipes* testPipes = (TestPipes*)malloc(sizeof(TestPipes));
    pipe(testPipes->testOut);
    pipe(testPipes->testErr);
    pipe(testPipes->demoOut);
    pipe(testPipes->demoErr);
    return testPipes;
}

/* close_test_pipes()
 * ---------------
 *  Close all of the pipes within a TestPipes struct.
 *
 *  pipes: The struct containing all of the test's pipes.
 *  safe: Signifies whether the program should leave the TEST_TO_CMP and
 *  	DEMO_TO_CMP file descriptors open.
 */
void close_test_pipes(TestPipes* pipes, bool safe) {
    // Ensures that required fds aren't possibly closed
    if (!safe || !(pipes->testOut[0] == TEST_TO_CMP
	    || pipes->testOut[0] == DEMO_TO_CMP)) {
    	close(pipes->testOut[0]);
    }
    if (!safe || !(pipes->testOut[1] == TEST_TO_CMP
	    || pipes->testOut[1] == DEMO_TO_CMP)) {
	close(pipes->testOut[1]);
    }
    if (!safe || !(pipes->testErr[0] == TEST_TO_CMP
	    || pipes->testErr[0] == DEMO_TO_CMP)) {
	close(pipes->testErr[0]);
    }
    if (!safe || !(pipes->testErr[1] == TEST_TO_CMP
	    || pipes->testErr[1] == DEMO_TO_CMP)) {
	close(pipes->testErr[1]);
    }
    if (!safe || !(pipes->demoOut[0] == TEST_TO_CMP
	    || pipes->demoOut[0] == DEMO_TO_CMP)) {
	close(pipes->demoOut[0]);
    }
    if (!safe || !(pipes->demoOut[1] == TEST_TO_CMP
	    || pipes->demoOut[1] == DEMO_TO_CMP)) {
	close(pipes->demoOut[1]);
    }
    if (!safe || !(pipes->demoErr[0] == TEST_TO_CMP
	    || pipes->demoErr[0] == DEMO_TO_CMP)) {
	close(pipes->demoErr[0]);
    }
    if (!safe || !(pipes->demoErr[1] == TEST_TO_CMP
	    || pipes->demoErr[1] == DEMO_TO_CMP)) {
	close(pipes->demoErr[1]);
    }
    free(pipes);
}

/* run_prog()
 * ---------------
 *  Sets up piping for, and runs a program used for testing, such that it reads
 *  	its input from a specified source and writes to a comparison pipe.
 *
 *  prgm: The filepath of the program to run.
 *  input: The input file to pass into 'prgm'.
 *  argc: The number of arguments to pass into the program.
 *  argv: The argument values to pass into the program.
 *  pipes: All the pipes needed for a test, to close before execution.
 *  output: The fd to route stdout to.
 *  error: The fd to route stderr to.
 *  jobSpecs: The job specifications to free before execution.
 */
void run_prog(char* prgm, char* input, int argc, char** argv, TestPipes* pipes,
	int output, int error, Freeable* memory) {
    // Organises all file descriptors for the programs
    int in = open(input, O_RDONLY);
    char** comArgs = prepare_com_args(argv, argc, prgm);
    dup2(in, STD_IN);
    close(in);
    dup2(output, STD_OUT);
    dup2(error, STD_ERR);
    close_test_pipes(pipes, false);
    free_job_specs(memory->jobSpecs);
    for (int i = 0; i < memory->numProcs; i++) {
	free(memory->procs[i]);
    }
    free(memory->procs);
    free(memory);
    execvp(prgm, comArgs);
    int c = 0;
    while (comArgs[c]) {
	free(comArgs[c]);
    }
    free(comArgs);
    exit(EXEC_FAIL_ERR);
}

/* create_cmp_pref()
 * ---------------
 *  Creates the prefix to pass in as an argument to uqcmp.
 *
 *  jobNum: The numerical index of the job being run.
 *  cmpType: What is being compared by the program (ie 'stderr', 'stdout').
 *
 *  Returns: The string to use as the prefix for uqcmp.
 */
char* create_cmp_pref(int jobNum, char* cmpType) {
    int bufferLen = 0;
    bufferLen += strlen("Job ") + 1; // Includes the space afterwards
    bufferLen += (int)(floor(log10((double)jobNum)) + 1) + 1; // Incl. space
    bufferLen += strlen(cmpType) + 1; // Incl. null terminator
    char* prefix = (char*)calloc(bufferLen, sizeof(char));
    sprintf(prefix, "Job %d %s", jobNum, cmpType);
    return prefix;
}

/* run_cmp()
 * ---------------
 *  Sets up piping for, and runs the 'uqcmp' program to compare the results of
 *  	the program being tested to that of the demo.
 *
 *  quiet: Whether the output should be silenced (ie sent to null).
 *  test: The fd for the output from the program being tested.
 *  demo: The fd for the output from the demo being tested against.
 *  pipes: All the pipes needed for a test, to close before execution.
 *  jobSpecs: The job specifications to free before execution.
 */
void run_cmp(bool quiet, int test, int demo, TestPipes* pipes,
	Freeable* memory, char* arg, int jobNum) {
    // Prepares the prefix argument
    char** argv = malloc(sizeof(char*));
    argv[0] = create_cmp_pref(jobNum, arg);
    char** comArgs = prepare_com_args(argv, 1, CMP_PROG);
    
    // Organises the required pipes for the program
    dup2(test, TEST_TO_CMP);
    dup2(demo, DEMO_TO_CMP);
    close_test_pipes(pipes, true);
    if (quiet) {
	int out = open(NULL_FILE, O_WRONLY);
    	dup2(out, STD_OUT);
    	dup2(out, STD_ERR);
    	close(out);
    }
    free_job_specs(memory->jobSpecs);
    for (int i = 0; i < memory->numProcs; i++) {
	free(memory->procs[i]);
    }
    free(memory->procs);
    free(memory);
    execvp(CMP_PROG, comArgs);
    int c = 0;
    while (comArgs[c]) {
	free(comArgs[c]);
    }
    free(comArgs);
    exit(EXEC_FAIL_ERR);
}

/* run_test_job()
 * ---------------
 *  Sets up the necessary inputs, outputs and error streams before executing
 *  	the four processes required for each test job.
 *
 *  prgm: The filepath of the program to test against the demo.
 *  input: The input file to pass into both 'prgm' and the demo.
 *  quiet: Whether the outputs of uqcmp are to be silenced.
 *  argc: The number of arguments to pass into the programs.
 *  argv: The argument values to pass into the programs. 
 *  jobSpecs: The job specifications that need freeing before children execute.
 *
 *  Returns: the process ID of all processes created by the function.
 */
pid_t* run_test_job(char* prgm, char* input, bool quiet, int argc,
	char** argv, Freeable* memory, int jobNum) {
    pid_t* pids = (pid_t*)calloc(NUM_FORKS, sizeof(pid_t));
    TestPipes* pipes = create_test_pipes();
    
    // Run test program
    pids[TEST_PROG_FORK] = fork();
    if (!pids[TEST_PROG_FORK]) {
	free(pids);
        run_prog(prgm, input, argc, argv, pipes, pipes->testOut[1],
		pipes->testErr[1], memory);
    }

    // Run demo program
    pids[DEMO_PROG_FORK] = fork();
    if (!pids[DEMO_PROG_FORK]) {
	free(pids);
	run_prog(DEMO, input, argc, argv, pipes, pipes->demoOut[1],
		pipes->demoErr[1], memory); 
    }

    // Run output compare
    pids[OUT_CMP_FORK] = fork();
    if (!pids[OUT_CMP_FORK]) {
	free(pids);
	run_cmp(quiet, pipes->testOut[0], pipes->demoOut[0], pipes, memory,
		OUT_NAME, jobNum);
    }
   
    // Run error compare
    pids[ERR_CMP_FORK] = fork();
    if (!pids[ERR_CMP_FORK]) {
	free(pids);
	run_cmp(quiet, pipes->testErr[0], pipes->demoErr[0], pipes, memory,
		ERR_NAME, jobNum);
    }
    close_test_pipes(pipes, false);
    return pids;
}

/* report_job_results()
 * ---------------
 *  Reports the results of running a single test job.
 *
 *  exitStatuses: The exit status of all programs run by a single job.
 *  jobNum: The index of the job being evaluated.
 *
 *  Returns: whether the test has passed
 */
bool report_job_results(int* exitStatuses, int jobNum) {
    bool passed = true;
    
    // Checks that all programs exited
    if (!(WIFEXITED(exitStatuses[TEST_PROG_FORK])
	    && WIFEXITED(exitStatuses[DEMO_PROG_FORK])
	    && WIFEXITED(exitStatuses[OUT_CMP_FORK])
	    && WIFEXITED(exitStatuses[ERR_CMP_FORK]))) {
	return false;
    }
    
    // Checks whether programs executed
    if (WEXITSTATUS(exitStatuses[TEST_PROG_FORK]) == EXEC_FAIL_ERR
	    || WEXITSTATUS(exitStatuses[DEMO_PROG_FORK]) == EXEC_FAIL_ERR
	    || WEXITSTATUS(exitStatuses[OUT_CMP_FORK]) == EXEC_FAIL_ERR
	    || WEXITSTATUS(exitStatuses[ERR_CMP_FORK]) == EXEC_FAIL_ERR) {
	printf("Job %d: Unable to execute test\n", jobNum);
	return false;
    }

    // Checks standard outputs match
    if (!WEXITSTATUS(exitStatuses[OUT_CMP_FORK])) {
        printf("Job %d: Stdout matches\n", jobNum);
    } else {
        printf("Job %d: Stdout differs\n", jobNum);
        passed = false;
    }
    fflush(stdout);

    // Checks standard errors match
    if (!WEXITSTATUS(exitStatuses[ERR_CMP_FORK])) {
	printf("Job %d: Stderr matches\n", jobNum);
    } else {
        printf("Job %d: Stderr differs\n", jobNum);
        passed = false;
    }
    fflush(stdout);

    // Compares exit statuses
    if (WEXITSTATUS(exitStatuses[TEST_PROG_FORK])
	    == WEXITSTATUS(exitStatuses[DEMO_PROG_FORK])) {
        printf("Job %d: Exit status matches\n", jobNum);
    } else {
        printf("Job %d: Exit status differs\n", jobNum);
        passed = false;
    }
    fflush(stdout);
    return passed;
}

/* sleep_two_secs()
 * ---------------
 *  Sleeps for two seconds, accounting for an interupt.
 */
void sleep_two_secs() {
    struct timespec twoSec = {2, 0};
    struct timespec rem = {2, 0};
    int failure = nanosleep(&twoSec, &rem);
    if (failure) {
	nanosleep(&rem, &twoSec);
    }
}

/* run_tests_series()
 * ---------------
 *  Runs the tests provided in series.
 *
 *  quiet: Whether the output of uqcmp should be silenced.
 *  testProgram: The filepath of the program to test against the demo program.
 *  jobSpecs: The inputs to pass for each of the test jobs to run.
 *
 *  Returns: whether all tests passed.
 */
bool run_tests_series(bool quiet, char* testProgram, JobSpecs* jobSpecs) {
    int passedTests = 0;
    int testsRun = 0;
    for (int i = 0; i < jobSpecs->count; i++) {
	testsRun++;
	Freeable* memory = (Freeable*)malloc(sizeof(Freeable));
	memory->jobSpecs = jobSpecs;
	memory->procs = (pid_t**)calloc(jobSpecs->count, sizeof(pid_t*));
	memory->numProcs = 0;
	printf("Starting job %d\n", i + 1);
	fflush(stdout);
	Job* job = jobSpecs->jobs[i];
	memory->procs[i] = run_test_job(testProgram, job->file, quiet,
		job->argCount, job->args, memory, i + 1);
	memory->numProcs++;

	// Sleeps for 2 seconds and kills the job's children
	sleep_two_secs();
	for (int j = 0; j < NUM_FORKS; j++) {
	    kill(memory->procs[i][j], SIGKILL);
	}

	// Get test results
	int exitStatuses[NUM_FORKS];
	for (int j = 0; j < NUM_FORKS; j++) {
	    waitpid(memory->procs[i][j], &(exitStatuses[j]), 0);
	}
	passedTests += (int)report_job_results(exitStatuses, i + 1);
	free(memory->procs[i]);
	free(memory->procs);
	free(memory);
	if (sigIntCaught) {
	    break;
	}
    }
    printf("testuqwordiply: %d out of %d tests passed\n",
	    passedTests, testsRun);
    return passedTests == testsRun;
}

/* run_tests_parallel()
 * ---------------
 *  Runs the tests provided in parallel.
 *
 *  quiet: Whether the output of uqcmp should be silenced.
 *  testProgram: The filepath of the program to test against the demo program.
 *  jobSpecs: The inputs to pass for each of the test jobs to run.
 *
 *  Returns: whether all tests passed.
 */
bool run_tests_parallel(bool quiet, char* testProgram, JobSpecs* jobSpecs) {
    Freeable* memory = (Freeable*)malloc(sizeof(Freeable));
    memory->jobSpecs = jobSpecs;
    memory->procs = (pid_t**)calloc(jobSpecs->count, sizeof(pid_t*));
    memory->numProcs = 0;
    int testsRun = 0;
    for (int i = 0; i < jobSpecs->count; i++) {
	testsRun++;
	printf("Starting job %d\n", i + 1);
	fflush(stdout);
	Job* job = jobSpecs->jobs[i];
	memory->procs[i] = run_test_job(testProgram, job->file, quiet,
		job->argCount, job->args, memory, i + 1);
	memory->numProcs++;
	if (sigIntCaught) {
	    break;
	}
    }

    // Sleeps for 2 seconds and kills all children
    sleep_two_secs();
    for (int i = 0; i < testsRun; i++) {
	for (int j = 0; j < NUM_FORKS; j++) {
	    kill(memory->procs[i][j], SIGKILL);
	}
    }

    // Get test results
    int passedTests = 0;
    for (int i = 0; i < testsRun; i++) {
	int exitStatuses[NUM_FORKS];
	for (int j = 0; j < NUM_FORKS; j++) {
	    waitpid(memory->procs[i][j], &(exitStatuses[j]), 0);
	}
	passedTests += report_job_results(exitStatuses, i + 1);
	free(memory->procs[i]);
    }
    free(memory->procs);
    free(memory);
    printf("testuqwordiply: %d out of %d tests passed\n",
	    passedTests, testsRun);
    return passedTests == testsRun;
}

/* catch_sig_int()
 * ---------------
 *  Used to catch a signal interupt, sets the variable sigIntCaught to true.
 *
 *  s: Passed in by the sigaction, is the signal number received.
 */
void catch_sig_int(int s) {
    sigIntCaught = true; 
}

/* prepare_com_args()
 * ---------------
 *  Takes in an array of arguments and prepares them for an execv call by
 *	prepending them with the program name and appending a NULL pointer.
 *
 *  argv: The argument values.
 *  argc: The number of arguments.
 *  prgm: The program the arguments are being passed to.
 */
char** prepare_com_args(char** argv, int argc, char* prgm) {
    char** newArgs = (char**)calloc(argc + 2, sizeof(char*));
    newArgs[0] = strdup(prgm);
    for (int i = 0; i < argc; i++) {
	newArgs[i + 1] = strdup(argv[i]);
    }
    newArgs[argc + 1] = NULL;
    return newArgs;
}
