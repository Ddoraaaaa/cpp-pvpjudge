#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// not available on OS X, because why would it
#ifndef __APPLE__
#include <seccomp.h>
#endif

using namespace std;

class exFile {
    enum Languages {
        py, cpp, c, java, go, ruby, js
    };

    int toFile[2], fromFile[2];
    FILE *toF, *frF;
    bool _restricted;

public:
    exFile(bool restricted = true) : _restricted(restricted) {;}

    void runFile(string fileName, string fileRoot = "") {
        
        pid_t pid;

        if (pipe(toFile) == -1) {
            cerr << "Error: Failed to create pipe from main to file " << fileName << endl;
            exit(1);
        }
        if (pipe(fromFile) == -1) {
            cerr << "Error: Failed to create pipe from file " << fileName << " to main" <<endl;
            exit(1);
        }

        pid = fork();
        if (pid == -1) {
            cerr << "Error: Failed to create child process for file " << fileName << endl;
            exit(1);
        } else if (pid == 0) {
            // child process (File)

            // pipe from main
            {
                // close write end of the pipe
                close(toFile[1]);
                // redirect stdin to read end of the pipe
                dup2(toFile[0], STDIN_FILENO);
            }

            // pipe to main
            {
                // close write end of the pipe
                close(fromFile[0]);
                // redirect stdout to the write end of the pipe
                dup2(fromFile[1], STDOUT_FILENO);
            }

            if(_restricted) {
                setRestriction(fileRoot);
            }
            
            // execute file
            execl(("./" + fileName).c_str(), fileName.c_str(), NULL);

            // The code below is only executed if execl() fails
            cerr << "Error: Failed to execute player1" << endl;

            exit(1);
        } else {
            // parent process (main)

            // close unneeded pipes:
            close(toFile[0]);
            close(fromFile[1]);
            
            // open file streams
            toF = fdopen(toFile[1], "w");
            frF = fdopen(fromFile[0], "r");
            
            // cout << "Response from player1: " << response << endl;

            // Wait for player1 to finish
            wait(NULL);
                
        }
    }

    string readLine(string &s) {
        s = "";
        char buf[1024];
        while(fgets(buf, sizeof(buf), frF) != nullptr) {
            s += buf;
        };
    }

    void writeLine(string &s) {
        fprintf(toF, s.c_str());
    }

private:
    void setRestriction(string fileRoot) {
        struct rlimit limits;

        // limit cpu time (1s)
        limits.rlim_cur = 1;
        limits.rlim_max = 1;
        setrlimit(RLIMIT_CPU, &limits);

        // limit mem usage (1mb)
        limits.rlim_cur = 1024 * 1024;
        limits.rlim_max = 1024 * 1024;
        setrlimit(RLIMIT_AS, &limits);

        // sandbox process file access
        chdir("/");
        chroot(fileRoot.c_str());

        //kill syscalls
        #ifndef __APPLE__
        scmp_filter_ctx ctx;
        ctx = seccomp_init(SCMP_ACT_ALLOW);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 0);
        seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(socket), 0);
        seccomp_load(ctx);
        #endif
    }
};

int main() {
    exFile player1(false), player2(true), judge(false);
    string p1F = "player1", p2F = "player2", judgeF = "judge";
    judge.runFile(judgeF);
    player1.runFile(p1F);
    player2.runFile(p2F);
    while(true) {

    }
}