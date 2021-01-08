#include <iostream>
#include <fstream> 
#include <sys/stat.h>
#include <mqueue.h>
#include <SFML/Audio.hpp>
#include <unistd.h>

#include "consts.h"

struct proc_c_settings{
    int workState;
    int delayValue;
};

int main(){
    std::cout << "proc_c: Starting..." << std::endl;
    std::ofstream outfile ("c_out.txt");
    outfile << "proc_c here, kisses!" << std::endl;

    // SETTING UP PROCESS SETTINGS (HEHE)
    struct proc_c_settings settings;
    settings.workState = PAUSED_M;
    settings.delayValue = DELTA_DELAY_VALUE_M & 0;

    // OPENING D->C QUEUE
    int flagsDC = O_RDONLY | O_CREAT;
    mode_t modeDC = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attrDC;
    attrDC.mq_flags = flagsDC;
    attrDC.mq_maxmsg = 10;
    attrDC.mq_msgsize = sizeof(int);
    mqd_t dtoc_mq_dec = mq_open ("/dtoc_mq", flagsDC,  modeDC, &attrDC);

    // OPENING B->C QUEUE
    int flagsBC = O_RDONLY | O_CREAT;
    mode_t modeBC = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attrBC;
    attrBC.mq_flags = flagsBC;
    attrBC.mq_maxmsg = SOUND_MQ_MAX;
    attrBC.mq_msgsize = sizeof(short);
    mqd_t btoc_mq_dec = mq_open ("/btoc_mq", flagsBC,  modeBC, &attrBC);

    /// MAIN PROGRAM

    mq_getattr (btoc_mq_dec, &attrBC);
    outfile << "proc_c: " << attrBC.mq_curmsgs << " atributes in queueueue as for the beginning of my work." << std::endl;
    int i = 0;
    while (true){
        mq_getattr (dtoc_mq_dec, &attrDC);
        if (attrDC.mq_curmsgs > 0){
            int bufDC = 0;
            mq_receive(dtoc_mq_dec, (char*) &bufDC, sizeof(int), 0);
            if (bufDC == WORKING_M) {
                settings.workState = WORKING_M;
                outfile << "proc_c: I'm starting my job!" << std::endl;
            } else if (bufDC == PAUSED_M) {
                settings.workState = PAUSED_M;
                outfile << "proc_c: I'm quiting my job!" << std::endl;
            }
        } 
        if (settings.workState == WORKING_M) {
            // DALEM TU KOD Z NAGRYWANIA, ROBCIE Z TYM CO CHCECIE
            sf::SoundBufferRecorder recorder;
            sf::SoundBuffer tmpBuffer;
            recorder.start();
            while (settings.workState == WORKING_M) {
                sleep(0.05);
                recorder.stop();
                tmpBuffer = recorder.getBuffer();
                recorder.start();
                //std::thread sender(sendSound, tmpBuffer, atob_mq_dec, std::ref(outfile));
                //sender.detach();
            }
        } 
        // A TUTAJ JEST KOD CO ODBIERA PROBKE OD B I JA WPISUJE DO PLIKU, SIE MOZE PRZYDAC
        // mq_getattr(btoc_mq_dec, &attrBC);
        // if (attrBC.mq_curmsgs > 0){
        //     mq_receive(btoc_mq_dec, (char*) &i, sizeof(short), 0);
        //     outfile << "proc_c: Received " << i << ". " << attrBC.mq_curmsgs << " atributes left in queueueue." << std::endl;
        // }

    }
    outfile << "proc_c: " << attrBC.mq_curmsgs << " atributes in queueueue, my job is done here, bye!." << std::endl;

    /// MAIN PROGRAM END

    mq_close(dtoc_mq_dec);
    mq_close(btoc_mq_dec);
    outfile.close();

    return 0;
}