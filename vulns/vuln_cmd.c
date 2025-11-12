#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

static bool allowed(const char *s) {
    // Tight whitelist: add only commands you truly need
    return strcmp(s, "date") == 0 ||
           strcmp(s, "uptime") == 0 ||
           strcmp(s, "whoami") == 0;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        const char *cmd = argv[1];

        if (!allowed(cmd)) {
            fprintf(stderr, "error: command not allowed\n");
            return 1;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            // child: execute command without a shell
            execlp(cmd, cmd, (char*)NULL);
            perror("execlp");
            _exit(127);
        }
        // parent: wait for child
        int status = 0;
        if (waitpid(pid, &status, 0) < 0) {
            perror("waitpid");
            return 1;
        }
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    return 0;
}

