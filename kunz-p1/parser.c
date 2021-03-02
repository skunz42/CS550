#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

#include "jobs.h"

//limits
#define MAX_TOKENS 100
#define MAX_STRING_LEN 100


size_t MAX_LINE_LEN = 10000;


// builtin commands
#define EXIT_STR "exit"
#define LIST_STR "listjobs"
#define FG_STR "fg"
#define EXIT_CMD 0
#define UNKNOWN_CMD 99
#define VALID_CMD 1


FILE *fp; // file struct for stdin
char **tokens;
char *line;
int token_count; // number of tokens in active command
List bg_procs;


/*** LIST FUNCTIONALITY ***/

// See jobs.c

/*** INPUT MANAGEMENT ***/


void initialize()
{

	// allocate space for the whole line
	assert( (line = malloc(sizeof(char) * MAX_STRING_LEN)) != NULL);

	// allocate space for individual tokens
	assert( (tokens = malloc(sizeof(char*)*MAX_TOKENS)) != NULL);

	// open stdin as a file pointer 
	assert( (fp = fdopen(STDIN_FILENO, "r")) != NULL);

    bg_procs.head = NULL;

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
    unsigned char is_background = 0;

    if (token_count > 0 && tokens[token_count-1][0] == '&') {
        is_background = 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }


    if (pid == 0) {
        //printf("Child: %s\n", tokens[0]);
        
        if (is_background) {
            tokens[token_count-1] = NULL;
        }
        
        int exec_ret = execvp(tokens[0], tokens);
        if (exec_ret < 0) {
            printf("exec failed\n");
            exit(1);
        }
        
        exit(99);
    }

    if (pid > 0) {
        //printf("Parent: %s\n", tokens[0]);
        if (!is_background) {
            //printf("In fg\n");
            wait_ret = waitpid(pid, &status, 0);

            if (wait_ret < 0) {
                printf("waitpid failed\n");
                exit(2);
            }
        } else {
            Job * my_job = malloc(sizeof(Job));
 
            if (is_background) {
                my_job->status = RUNNING;
                my_job->pid = pid;
                my_job->next = NULL;
                strcpy(my_job->name, tokens[0]);
                insert(&bg_procs, my_job);
            }

            //printf("Starting wait\n");
            
        }
    }

    return VALID_CMD;

}

int run_command() {

	if (strcmp( tokens[0], EXIT_STR ) == 0) {
        free_list(&bg_procs);
        free(line);
        free(tokens);
        fclose(fp);
		return EXIT_CMD;
    } else if (strcmp(tokens[0], LIST_STR) == 0) {
        check_finished(&bg_procs);
        listall(&bg_procs);
        return VALID_CMD;
    } else if (strcmp(tokens[0], FG_STR) == 0) {
        if (token_count > 1) {
            int tpid = atoi(tokens[1]);
            foreground(&bg_procs, tpid);
        }
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
        if (token_count == 0) {
            tokens[0] = " ";
        }		
	} while( run_command() != EXIT_CMD );

	return 0;
}

