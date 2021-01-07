#include <iostream>
#include <fstream> 
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

#include "consts.h"

struct proc_b_settings{
    int workState;
    int deltaVal;
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

int main(){
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(2, &set);
    
    std::cout << "proc_b: Starting..." << std::endl;
    std::ofstream outfile ("b_out.txt");
    outfile << "proc_b here, kisses!" << std::endl;

    // SETTING UP PROCESS SETTINGS (HEHE)
    struct proc_b_settings settings;
    settings.workState = WORKING_M;
    settings.deltaVal = DELTA_DELAY_VALUE_M & 0;
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

    // OPENING D->B QUEUE
    int flagsDB = O_RDONLY | O_CREAT;
    mode_t modeDB = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attrDB;
    attrDB.mq_flags = flagsDB;
    attrDB.mq_maxmsg = 10;
    attrDB.mq_msgsize = sizeof(int);
    mqd_t dtob_mq_dec = mq_open ("/dtob_mq", flagsDB,  modeDB, &attrDB);

    // OPENING A->B QUEUE
    int flagsAB = O_RDONLY | O_CREAT;
    mode_t modeAB = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attrAB;
    attrAB.mq_flags = flagsAB;
    attrAB.mq_maxmsg = 100;
    attrAB.mq_msgsize = sizeof(int);
    mqd_t atob_mq_dec = mq_open ("/atob_mq", flagsAB,  modeAB, &attrAB);

    // OPENING B->C QUEUE
    int flagsBC = O_WRONLY | O_CREAT;
    mode_t modeBC = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attrBC;
    attrBC.mq_flags = flagsBC;       
    attrBC.mq_maxmsg = 100;      
    attrBC.mq_msgsize = sizeof(int);
    attrBC.mq_curmsgs = 0;
    mqd_t btoc_mq_dec = mq_open("/btoc_mq", flagsBC, modeBC, &attrBC);

    /// MAIN PROGRAM

    mq_getattr (atob_mq_dec, &attrAB);
    outfile << "proc_b: " << attrAB.mq_curmsgs << " atributes in queueueue as for the beginning of my work." << std::endl;
    int i = 0;
    while (true){
        mq_getattr(atob_mq_dec, &attrAB);
        if (attrAB.mq_curmsgs > 0){
            mq_receive(atob_mq_dec, (char*) &i, sizeof(int), 0);
            outfile << "proc_b: Received " << i << ". " << attrAB.mq_curmsgs << " atributes left in queueueue." << std::endl;
            ++i;
            mq_send(btoc_mq_dec, (const char*) &i, sizeof(int), 0);
        }
    }
    outfile << "proc_b: " << attrAB.mq_curmsgs << " atributes in queueueue, my job is done here, bye!." << std::endl;

    /// MAIN PROGRAM END

    mq_close(dtob_mq_dec);
    mq_close(atob_mq_dec);
    mq_close(btoc_mq_dec);
    outfile.close();

    return 0;
}