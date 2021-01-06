#include <iostream>
#include <fstream> 
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>

int main(){
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(2, &set);
    
    std::cout << "proc_b: Starting..." << std::endl;
    std::ofstream outfile ("b_out.txt");
    outfile << "proc_b here, kisses!" << std::endl;
    outfile.close();


    sleep(2);

    int flags = O_RDONLY | O_CREAT;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attr;
    attr.mq_flags = flags;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = sizeof(int);
    //attr.mq_curmsgs = 0;
    mqd_t atob_mq_dec = mq_open ("/atob_mq", flags,  mode, &attr);

    mq_getattr (atob_mq_dec, &attr);
    std::cout << "proc_b: " << attr.mq_curmsgs << " atributes in queueueue as for the beginning of my work." << std::endl;
    int i = 0;
    while (true){
        mq_getattr(atob_mq_dec, &attr);
        if (attr.mq_curmsgs > 0){
            mq_receive(atob_mq_dec, (char*) &i, sizeof(int), 0);
            std::cout << "proc_b: Received " << i << ". " << attr.mq_curmsgs << " atributes left in queueueue." << std::endl;
        }
    }
    std::cout << "proc_b: " << attr.mq_curmsgs << " atributes in queueueue, my job is done here, bye!." << std::endl;
    mq_close(atob_mq_dec);
    return 0;
}