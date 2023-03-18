#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// not available on OS X, because why would it
// #include <seccomp.h>

using namespace std;

vector<int> exec_file(string filename, bool restricted) {
    if(restricted) {
        struct rlimit limits;

        // limit cpu time
        limits.rlim_cur = 1;
        limits.rlim_max = 1;
        setrlimit(RLIMIT_CPU, &limits);

        // limit mem usage
        limits.rlim_cur = 1024 * 1024;
        limits.rlim_max = 1024 * 1024;
        setrlimit(RLIMIT_AS, &limits);

        // sandbox process file access
        chdir("/");
        chroot("/path/to/sandbox/directory");

        //kill syscalls
        // scmp_filter_ctx ctx;
        // ctx = seccomp_init(SCMP_ACT_ALLOW);
        // seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 0);
        // seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(socket), 0);
        // seccomp_load(ctx);
    }
}

int main() {
    int judge_to_p1[2];
    int p1_to_judge[2];
    pid_t pid;

    if (pipe(judge_to_p1) == -1) {
        cerr << "Error: Failed to create pipe from judge to p1" << endl;
        return 1;
    }
    if (pipe(p1_to_judge) == -1) {
        cerr << "Error: Failed to create pipe from p1 to judge" << endl;
        return 1;
    }

    pid = fork();
    if (pid == -1) {
        cerr << "Error: Failed to create child process." << endl;
        return 1;
    } else if (pid == 0) {
        // Child process (player1)

        // pipe from judge
        // Close the write end of the pipe
        close(judge_to_p1[1]);
        // Redirect stdin to the read end of the pipe
        dup2(judge_to_p1[0], STDIN_FILENO);

        // pipe to judge
        // Close the write end of the pipe
        close(p1_to_judge[0]);
        // Redirect stdout to the write end of the pipe
        dup2(p1_to_judge[1], STDOUT_FILENO);

        // Execute player1
        execl("./player1", "player1", NULL);

        // The code below is only executed if execl() fails
        cerr << "Error: Failed to execute player1" << endl;
        return 1;
    } else {
        // Parent process (judge)

        // Close unneeded pipes:
        close(judge_to_p1[0]);
        close(p1_to_judge[1]);
        
        FILE* to_p1 = fdopen(judge_to_p1[1], "w");
        FILE* fr_p1 = fdopen(p1_to_judge[0], "r");

        // Read data from player1 via the read end of the pipe
        char ok[1024];
        fgets(ok, sizeof(ok), fr_p1);
        printf("%s", ok);
        
        // string response(buffer);

        // cout << "Response from player1: " << response << endl;

        // Wait for player1 to finish
        wait(NULL);
    }

    return 0;
}