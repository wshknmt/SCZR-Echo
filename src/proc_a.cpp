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
#include <SFML/Audio.hpp>
#include <vector>
#include <thread>

#include "consts.h"

struct proc_a_settings{
    int workState;
};

void startRecording();
void sendSound(const sf::SoundBuffer & buffer);

int main(){    
    srand(time(NULL));
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(1, &set);
    
    std::cout << "proc_a: Starting..." << std::endl;
    std::ofstream outfile ("a_out.txt");
    outfile << "proc_a here, kisses!" << std::endl;

    // SETTING UP PROCESS SETTINGS (HEHE)
    struct proc_a_settings settings;
    settings.workState = PAUSED_M;

    // OPENING D->A QUEUE
    int flagsDA = O_RDONLY | O_CREAT;
    mode_t modeDA = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attrDA;
    attrDA.mq_flags = flagsDA;
    attrDA.mq_maxmsg = 10;
    attrDA.mq_msgsize = sizeof(int);
    mqd_t dtoa_mq_dec = mq_open ("/dtoa_mq", flagsDA, modeDA, &attrDA);

    // OPENING A->B QUEUE
    int flagsAB = O_WRONLY | O_CREAT;
    mode_t modeAB = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    struct mq_attr attrAB;
    attrAB.mq_flags = flagsAB;       
    attrAB.mq_maxmsg = 100;      
    attrAB.mq_msgsize = sizeof(int);
    attrAB.mq_curmsgs = 0;
    mqd_t atob_mq_dec = mq_open("/atob_mq", flagsAB, modeAB, &attrAB);

    /// MAIN PROGRAM

    mq_getattr (atob_mq_dec, &attrAB);
    outfile << attrAB.mq_curmsgs << " atributes in queueueue." << std::endl;
    while (true) {
        mq_getattr (dtoa_mq_dec, &attrDA);
        if (attrDA.mq_curmsgs > 0){
            int bufDA = 0;
            mq_receive(dtoa_mq_dec, (char*) &bufDA, sizeof(int), 0);
            if (bufDA == WORKING_M) {
                settings.workState = WORKING_M;
                outfile << "proc_a: I'm starting my job!" << std::endl;
            } else if (bufDA == PAUSED_M) {
                settings.workState = PAUSED_M;
                outfile << "proc_a: I'm quiting my job!" << std::endl;
            }
        } 
        if (settings.workState == WORKING_M) {
            mq_getattr (atob_mq_dec, &attrAB);
            if (attrAB.mq_curmsgs < 100){
                int i = rand() % 100;
                if (mq_send(atob_mq_dec, (const char*) &i, sizeof(int), 0) < 0) {
                    outfile << "proc_a: Ops, something went wrong, I think my time has come, adios!" << std::endl;
                    break;
                }
                mq_getattr (atob_mq_dec, &attrAB);
                outfile << "proc_a: " << i << " value sent to the message queueueue." << attrAB.mq_curmsgs << " atributes in queueueue." << std::endl;   
            }
        }      
    }

    /// MAIN PROGRAM END

    mq_close(dtoa_mq_dec);
    mq_close(atob_mq_dec);
    outfile.close();

    return 0;
}

void startRecording() {
    if (!sf::SoundBufferRecorder::isAvailable()) {
        std::cerr << "Error. Microphone is unavailable." << std::endl;
        exit(EXIT_FAILURE);
    }	
    else
        std::cout << "Microphone is available. Recording started." << std::endl;

    sf::SoundBufferRecorder recorder;
    sf::SoundBuffer tmpBuffer;
    recorder.start();
    while (true) {
        sleep(0.05);
        recorder.stop();
        tmpBuffer = recorder.getBuffer();
        recorder.start();
        std::thread sender(sendSound, tmpBuffer);
        sender.detach();
    }
}

void sendSound(const sf::SoundBuffer & buffer) {
    const sf::Int16 * samples = buffer.getSamples();
    for (unsigned int i = 0; i < buffer.getSampleCount(); ++i) {
        // add sample to message queue here please
    }
};