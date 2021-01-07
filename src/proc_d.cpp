#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <mqueue.h>

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

    int flags = O_WRONLY | O_CREAT;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    // OPENING D->A QUEUE
    struct mq_attr attrDA;
    attrDA.mq_flags = flags;       
    attrDA.mq_maxmsg = 10;      
    attrDA.mq_msgsize = sizeof(int);
    attrDA.mq_curmsgs = 0;
    mqd_t dtoa_mq_dec = mq_open("/dtoa_mq", flags, mode, &attrDA);

    // OPENING D->B QUEUE
    struct mq_attr attrDB;
    attrDB.mq_flags = flags;       
    attrDB.mq_maxmsg = 10;      
    attrDB.mq_msgsize = sizeof(int);
    attrDB.mq_curmsgs = 0;
    mqd_t dtob_mq_dec = mq_open("/dtob_mq", flags, mode, &attrDB);

    // OPENING D->C QUEUE
    struct mq_attr attrDC;
    attrDC.mq_flags = flags;       
    attrDC.mq_maxmsg = 10;      
    attrDC.mq_msgsize = sizeof(int);
    attrDC.mq_curmsgs = 0;
    mqd_t dtoc_mq_dec = mq_open("/dtoc_mq", flags, mode, &attrDC);


    pid_t proc_a_pid = startProc("bin/proc_a", NULL);
    pid_t proc_b_pid = startProc("bin/proc_b", NULL);
    pid_t proc_c_pid = startProc("bin/proc_c", NULL);

    int command = 0;
    while (true){
        char c = getc(stdin);
        if (c == 's') {
            command = 1;
            mq_send(dtoa_mq_dec, (const char*) &command, sizeof(int), 0);
        } else if (c == 'p') {
            command = -1;
            mq_send(dtoa_mq_dec, (const char*) &command, sizeof(int), 0);
        } else if (c == 'x'){
            break;
        }
    }

    kill (proc_a_pid, SIGKILL);
    kill (proc_b_pid, SIGKILL);
    kill (proc_c_pid, SIGKILL);

    mq_close(dtoa_mq_dec);
    mq_close(dtob_mq_dec);
    mq_close(dtoc_mq_dec);

    return 0;
}