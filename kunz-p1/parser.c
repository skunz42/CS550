#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

//limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100

size_t MAX_LINE_LEN = 10000;


// builtin commands
#define EXIT_STR "exit"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99
#define VALID_CMD 1


FILE *fp; // file struct for stdin
char **tokens;
char *line;
int token_count; // number of tokens in active command

void initialize()
{

	// allocate space for the whole line
	assert( (line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);

	// allocate space for individual tokens
	assert( (tokens = malloc(sizeof(char*)*MAX_TOKENS)) != NULL);

	// open stdin as a file pointer 
	assert( (fp = fdopen(STDIN_FILENO, "r")) != NULL);

}

void tokenize (char * string)
{
	token_count = 0;
	int size = MAX_TOKENS;
	char *this_token;

	while ( (this_token = strsep( &string, " \t\v\f\n\r")) != NULL) {

		if (*this_token == '\0') continue;

		tokens[token_count] = this_token;

		//printf("Token %d: %s\n", token_count, tokens[token_count]);

		token_count++;

		// if there are more tokens than space ,reallocate more space
		if(token_count >= size){
			size*=2;

			assert ( (tokens = realloc(tokens, sizeof(char*) * size)) != NULL);
		}
	}

    tokens[token_count] = NULL;
    //token_count++;
}

void read_command() 
{

	// getline will reallocate if input exceeds max length
	assert( getline(&line, &MAX_LINE_LEN, fp) > -1); 

	//printf("Shell read this line: %s\n", line);

	tokenize(line);
}

int handle_command() {

    pid_t pid;
    int status;
    int wait_ret;

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        //printf("Child: %s\n", tokens[0]);
        int exec_ret = execvp(tokens[0], tokens);
        if (exec_ret < 0) {
            printf("exec failed\n");
            exit(1);
        }
        exit(99);
    }

    if (pid > 0) {
        //printf("Parent: %s\n", tokens[0]);
        wait_ret = waitpid(pid, &status, 0);

        if (wait_ret < 0) {
            printf("waitpid failed\n");
            exit(2);
        }

    }

    return 1;

}

int run_command() {

	if (strcmp( tokens[0], EXIT_STR ) == 0) {
		return EXIT_CMD;
    } else {
        return handle_command();
    }

	return UNKNOWN_CMD;
}

int main()
{
	initialize();

	do {
		printf("sh550> ");
		read_command();
		
	} while( run_command() != EXIT_CMD );

	return 0;
}

