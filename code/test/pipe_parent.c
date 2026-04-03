/* pipe_parent.c
 * Parent process: creates a pipe, forks a child via ExecP,
 * writes a message into the pipe, then joins the child.
 */
#include "syscall.h"
#define stdout 1

int main() {
    int parentFd, childFd;
    int pid;
    char msg[] = "Hello from parent!";

    /* Create pipe: parentFd = write end, childFd = read end */
    Pipe(&parentFd, &childFd);

    /* Exec child, passing it the read-end descriptor */
    pid = ExecP("pipe_child", childFd);
    if (pid < 0) {
        PrintString("ExecP failed\n");
        Exit(-1);
    }

    /* Parent writes into the pipe */
    pipeWrite(parentFd, msg, 18);

    /* Wait for child to finish */
    Join(pid);
    Exit(0);
}
