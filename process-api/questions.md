Questions:
1. Write a program that calls fork(). Before calling fork(), have the
main process access a variable (e.g., x) and set its value to something (e.g., 100). What value is the variable in the child process?
What happens to the variable when both the child and parent change
the value of x?
- The value in the child process is the same as the one in the parent process because calling fork() clones the parent process' initial state but with different address
- Since the initialized state of the child process is the same as the parent process but with different address, changing the value in one process is isolated in that process only 

2. Write a program that opens a file (with the open() system call)
and then calls fork() to create a new process. Can both the child
and parent access the file descriptor returned by open()? What
happens when they are writing to the file concurrently, i.e., at the
same time?
- Both the parent and the child can access the fd returned by calling open() and both of them also share the file offset.
- Both processes can write to the same fd concurrently but the order of execution is unpredictable

3. Write another program using fork(). The child process should
print “hello”; the parent process should print “goodbye”. You should
try to ensure that the child process always prints first; can you do
this without calling wait() in the parent?
- Yes, by using the pipe() system call. A pipe has two file descriptors: one for read and one for write. In the parent process, before printing the message, we call the read()
function and pass the read fd so that it will wait for the write() to the pipe, turning it to a blocking call. Then in the child process we print the message and call the 
write() function and pass the write fd. Doing this will ensure that the message in the child process will always print first.

4. Write a program that calls fork() and then calls some form of
exec() to run the program /bin/ls. See if you can try all of the
variants of exec(), including execl(), execle(), execlp(),
execv(), execvp(), and execvP(). Why do you think there
are so many variants of the same basic call?
- The reason for so many variants of the exec() system call is that overtime, they needed new functionality to go along with it. So instead of depracating the older function
and create a new one that would cater all the functions, they decided to separate them because in doing so, it wouldn't break the codebase of so many people that already use 
the basic function.

5. Now write a program that uses wait() to wait for the child process
to finish in the parent. What does wait() return? What happens if
you use wait() in the child?
- The wait returns the PID of the child process on success and -1 on failure. Calling wait in the child process will result in wait() returning -1 because there is no child process 
created there

6. Write a slight modification of the previous program, this time us-
ing waitpid() instead of wait(). When would waitpid() be
useful?
- waitpid() is useful for when waiting a specific child process to die

7. Write a program that creates a child process, and then in the child
closes standard output (STDOUT FILENO). What happens if the child
calls printf() to print some output after closing the descriptor?
- If we execute printf() after closing the fd of the STDOUT, printf() will have nowhere to put its output.

8. Write a program that creates two children, and connects the stan-
dard output of one to the standard input of the other, using the
pipe() system call.
- Done :)
