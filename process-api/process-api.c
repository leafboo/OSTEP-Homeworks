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
int question_four();
int question_five();
int question_six();
int question_seven();
int question_eight();
int safe_dup2(int old_fd, int new_fd);
char *safe_malloc(size_t);

int 
main(int argc, char *argv[]) {
    question_eight();
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

int 
question_four() {
    int fd = fork();

    if (fd < 0) {
	perror("fork()");
	exit(EXIT_FAILURE);
    } else if (fd == 0) {
	// NOTE: 
	// l = args in list ("", "", "", NULL)
	// v = args in vector/array {"", "", "", NULL}
	// e = environment variable can be passed
	// p = in the first argument, it allows you to pass the file name and it will look for the PATH variables if the command is in the specified directories, and executes it
	// P = also looks for the files but instead of looking in the PATH variable, you get to specify the paths that you want to search for the executable
	
	char *envp[] = {"LS_COLORS=di=33:ln=35:ex=31", NULL};
	char *argv[] = {"ls", "--color=always", "-la" , NULL};
	// execl("/bin/ls", "ls", "--color=always", "-la" , NULL);
	// execle("/bin/ls", "ls", "--color=auto", "-la", NULL, envp);
	// execlp("ls", "ls", "--color=always", "-la" , NULL);
	// execv("/bin/ls", argv);
	execvp("ls", argv);
	// execvP("ls", "/path:/separated/by:/column", argv);
	
	exit(EXIT_SUCCESS);
    } else {
	wait(NULL);
    }
    return 0;
}

int 
question_five() {
    int fd = fork();
    
    if (fd < 0) {
	perror("fork()");
	exit(EXIT_FAILURE);
    } else if (fd == 0) {
	printf("Hello\n");
	// pid_t child_pid = wait(NULL);
	// printf("%d\n", child_pid);
	exit(EXIT_SUCCESS);
    } else {
	pid_t child_pid = wait(NULL);
	printf("%d\n", child_pid);
    }
    return 0;
}

int 
question_six() {
    int fd = fork();
    
    if (fd < 0) {
	perror("fork()");
	exit(EXIT_FAILURE);
    } else if (fd == 0) {
	printf("Hello\n");
	exit(EXIT_SUCCESS);
    } else {
	int dead_child_pid;
	pid_t pid = waitpid(fd, &dead_child_pid, 0);
	printf("pid of the dead child process: %d\n", pid);
	printf("status of the dead child process: %d\n", dead_child_pid);
    }
    return 0;
}

int 
question_seven() {
    int pipefd[2];
    int fd;

    if (pipe(pipefd) < 0) {
	perror("pipe()");
	exit(EXIT_SUCCESS);
    }
    if ((fd = fork()) < 0) {
	perror("fork()");
	exit(EXIT_FAILURE);
    } else if (fd == 0) {
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	printf("Hello world");
	fflush(stdout); // probably not needed since the stdout buffer gets flushed when the process exits
	exit(EXIT_SUCCESS);
    } else {
	char *argv[] = {"wc", NULL};
	char *message = malloc(1024);

	close(pipefd[1]);
	ssize_t message_bytes = read(pipefd[0], message, 11);	
	message[message_bytes] = '\0';
	printf("message: %s", message);
	fflush(stdout); // prob not needed
    }
    return 0;
}

int 
question_eight() {
    int fd_1, fd_2;

    if ((fd_1 = fork()) < 0) {
	perror("fork()");
	exit(EXIT_FAILURE);
    } else if (fd_1 == 0) { // child 1
	int pipefd[2];

	if (pipe(pipefd) < 0) {
	    perror("pipe()");
	    exit(EXIT_FAILURE);
	}
	if ((fd_2 = fork()) < 0) {
	    perror("fork()");
	    exit(EXIT_FAILURE);
	} else if (fd_2 == 0) { // child 2 / child of child 1
	    close(pipefd[0]);
	    safe_dup2(pipefd[1], STDOUT_FILENO);
	    close(pipefd[1]);
	    printf("hello from child 2");
	    fflush(stdout);
	    exit(EXIT_SUCCESS);
	} else { // child 1 / parent of child 2
	    wait(NULL);
	    close(pipefd[1]);
	    safe_dup2(pipefd[0], STDIN_FILENO);
	    close(pipefd[0]);
	    if (execlp("wc", "wc", NULL) == -1) {
		perror("execlp()");
		close(EXIT_FAILURE);
	    }
	}
    } else {
	wait(NULL);
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

int
safe_dup2(int old_fd, int new_fd) {
    int fd;
    if ((fd = dup2(old_fd, new_fd)) < 0) {
	perror("dup2()");
	exit(EXIT_FAILURE);
    }
    return fd;
}
