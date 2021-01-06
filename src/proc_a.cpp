#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctime>
#include <cstdlib>


int main(){
    srand(time(NULL));
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(1, &set);
    
    std::cout << "proc_a: Starting..." << std::endl;
    std::ofstream outfile ("a_out.txt");
    outfile << "proc_a here, kisses!" << std::endl;
    outfile.close();

    int flags = O_WRONLY | O_CREAT;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attr;
    attr.mq_flags = flags;       
    attr.mq_maxmsg = 100;      
    attr.mq_msgsize = sizeof(int);
    attr.mq_curmsgs = 0;

    mqd_t atob_mq_dec = mq_open("/atob_mq", flags, mode, &attr);

    mq_getattr (atob_mq_dec, &attr);
    std::cout << attr.mq_curmsgs << " atributes in queueueue." << std::endl;
    while (true) {
        mq_getattr (atob_mq_dec, &attr);
        if (attr.mq_curmsgs < 100){
            int i = rand();
            if (mq_send(atob_mq_dec, (const char*) &i, sizeof(int), 0) < 0) {
                std::cout << "proc_a: Ops, something went wrong, I think my time has come, adios!" << std::endl;
                break;
            }
            mq_getattr (atob_mq_dec, &attr);
            std::cout << "proc_a: " << i << " value sent to the message queueueue." << attr.mq_curmsgs << " atributes in queueueue." << std::endl;   
        }        
    }

    mq_close(atob_mq_dec);
    return 0;
}