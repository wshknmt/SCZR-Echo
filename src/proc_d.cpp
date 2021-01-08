#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "consts.h"

struct proc_d_settings{
    int workState;
    int deltaVal;
    int delayVal;
    int l1Val;
    int l2Val;
    int l3Val;
    int l4Val;
    int l5Val;
    int l6Val;
    int l7Val;
    int l8Val;
    int l9Val;
    int l10Val;
};

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

    // SETTING UP PROCESS SETTINGS (HEHE)
    struct proc_d_settings settings;
    settings.workState = PAUSED_M;
    settings.deltaVal = DELTA_DELAY_VALUE_M & 0;
    settings.delayVal = DELTA_DELAY_VALUE_M & 0;
    settings.l1Val = LAYER_1_M & 100;
    settings.l2Val = LAYER_2_M & 0;
    settings.l3Val = LAYER_3_M & 0;
    settings.l4Val = LAYER_4_M & 0;
    settings.l5Val = LAYER_5_M & 0;
    settings.l6Val = LAYER_6_M & 0;
    settings.l7Val = LAYER_7_M & 0;
    settings.l8Val = LAYER_8_M & 0;
    settings.l9Val = LAYER_9_M & 0;
    settings.l10Val = LAYER_10_M & 0;

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

    while (true){
        char c = getc(stdin);
        if (c == 's') {
            settings.workState = WORKING_M;
            mq_send(dtoa_mq_dec, (const char*) &(settings.workState), sizeof(int), 0);
        } else if (c == 'p') {
            settings.workState = PAUSED_M;
            mq_send(dtoa_mq_dec, (const char*) &(settings.workState), sizeof(int), 0);
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

    sleep(0.5);
    mq_unlink("/dtoa_mq");
    mq_unlink("/dtob_mq");
    mq_unlink("/dtoc_mq");
    mq_unlink("/atob_mq");
    mq_unlink("/btoc_mq");

    return 0;
}