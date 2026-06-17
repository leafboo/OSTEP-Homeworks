#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 100

int question_one();
int question_two();
int question_three();
char *safe_malloc(size_t);

int 
main(int argc, char *argv[]) {
    question_three();
    return 0;
}

int 
question_one() {
    int x = 100;
    printf("Value of x: %d\n", x);

    int pid = fork();

    if (pid < 0) {
	fprintf(stderr, "Process creation failed\n");
	exit(EXIT_FAILURE);
    } else if (pid == 0) { // child process
	x = 105;
	printf("child process x value: %d\n", x);
	exit(EXIT_SUCCESS);
    } else { // parent process
	x = 110;
	printf("parent process x value: %d\n", x);
    }
    printf("this should only print once\n");
    exit(EXIT_SUCCESS);

}

int 
question_two() {
    // we need a separate fd for reading so that it will start at the beginning because when using the same fd as the write, its offset is increased based on the bytes written
    int write_open_fd = open("./test.md", O_CREAT|O_WRONLY, S_IRWXU);
    int fd = fork();
    char *file_content = safe_malloc(BUFFER_SIZE);
    assert(file_content != NULL);
    char parent_message[] = "frm parent";
    char child_message[] = "frm child";
    int stat_loc;

    if (write_open_fd < 0) {
	perror("open()");
	exit(EXIT_FAILURE);
    }
    
    // NOTE: parent and child process run concurrently without the wait() in the parent
    if (fd < 0) {
	fprintf(stderr, "failed fork() call");
	exit(EXIT_FAILURE);
    } else if (fd == 0) {
	if (write(write_open_fd, child_message, 9) < 0) {
	    perror("child write()");
	    exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
    } else {
	pid_t child_pid;
	if ((child_pid = wait(NULL)) < 0) {
	    perror("wait()");
	    exit(EXIT_FAILURE);
	}
	printf("child pid(%d) exited", child_pid);
	if (write(write_open_fd, parent_message, 10) < 0) {
	    perror("parent write()");
	    exit(EXIT_FAILURE);
	}
    }
    return 0;
}

int 
question_three() {
    int pipe_fd[2];
    int fd;

    if (pipe(pipe_fd) < 0) {
	perror("pipe()");
	exit(EXIT_FAILURE);
    }
    if ((fd = fork()) < 0) {
	perror("fork()");
	exit(EXIT_FAILURE);
    } else if (fd == 0) {
	printf("hello\n");
	write(pipe_fd[1], "a", 1);
	exit(EXIT_SUCCESS);
    } else {
	char c;
	read(pipe_fd[0], &c, 1);
	printf("goodbye\n");
    }
    return 0;
}

char *
safe_malloc(size_t size) {
    char *buffer = malloc(size);
    if (buffer == NULL) {
	perror("malloc()");
	exit(EXIT_FAILURE);
    }
    return buffer;
}
