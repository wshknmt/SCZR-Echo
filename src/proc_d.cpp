#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int startProc(const char *proc_name, char **proc_args){
    pid_t pid = fork();
    if (pid != 0){
        return pid;
    } else {
        execvp(proc_name, proc_args);
        fprintf(stderr, "Some kind of error in child proccess, fuck this.");
        abort();
    }
}


int main(){
    std::cout << "proc_d: Starting..." << std::endl;

    pid_t proc_a_pid = startProc("bin/proc_a", NULL);
    pid_t proc_b_pid = startProc("bin/proc_b", NULL);
    pid_t proc_c_pid = startProc("bin/proc_c", NULL);

    while (true){
        char c = getc(stdin);
        if (c == 'x') {
            break;
        }   
    }
    kill (proc_a_pid, SIGKILL);
    kill (proc_b_pid, SIGKILL);
    kill (proc_c_pid, SIGKILL);
    return 0;
}