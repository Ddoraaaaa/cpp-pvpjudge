#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <chrono>
#include <unistd.h>
#include <sstream>
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
    int cpuTime, ramMb;
    int timeLeft, lastResume;
    FILE *toF, *frF;
    bool restricted;

public:
    exFile(bool _restricted = true, int _timeLeft = INT32_MAX) : restricted(_restricted), timeLeft(_timeLeft), lastResume(-1),  cpuTime(1), ramMb(64) {}

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

            if(restricted) {
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

    void setResource(int _cpuTime, int _ramMb) {
        cpuTime = _cpuTime;
        ramMb = _ramMb;
    }

    void readLine(string &s) {
        s = "";
        char buf[1024];
        while(fgets(buf, sizeof(buf), frF) != nullptr) {
            s += buf;
        };
    }

    void readLine(stringstream &strin) {
        string s = "";
        readLine(s);
        strin.str(s);
    }

    void writeLine(string &s) {
        fprintf(toF, s.c_str());
    }

    void splitTime() {
        if(lastResume == -1) {
            return;
        }
        timeLeft -= getTime() - lastResume;
        lastResume = -1;
    }

    void resumeTime() {
        lastResume = getTime();
    }

private:
    int getTime() {
        return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

    void setRestriction(string fileRoot) {
        struct rlimit limits;

        // limit cpu time
        limits.rlim_cur = 0.001 * cpuTime;
        limits.rlim_max = 0.001 * cpuTime;
        setrlimit(RLIMIT_CPU, &limits);

        // limit mem usage
        limits.rlim_cur = ramMb * 1024 * 1024;
        limits.rlim_max = ramMb * 1024 * 1024;
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
    string type;
    string p1File = "player1", p2File = "player2", judgeFile = "judge";
    stringstream p1in, p2in, judgein;

    exFile *judge = new exFile(false);
    judge->runFile(judgeFile, judgeFile);

    // get time limit
    judge->readLine(judgein);
    int timeLimit;
    judgein >> type; assert(type=="time"); judgein >> timeLimit;

    exFile *player1 = new exFile(true, timeLimit), *player2 = new exFile(true, timeLimit);
    player1->runFile(p1File, p1File); player2->runFile(p2File, p2File);

    exFile *players[] = {player1, player2};

    int turn = 0;
    while(true) {
        judge->readLine(judgein);
        jug
    }
}