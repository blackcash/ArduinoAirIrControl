#ifndef REMOTE_H  //预编译指令，防止重复定义类
#define REMOTE_H
#include "Common.h"

class Remote
{
private:    
    int checktemp(int *buf);
    int checkfan(int *buf);
    int checkmode(int *buf);

public:
    Remote();
    void Remote_init(void);
    bool check_remote_data(int *buf, int count);  
    int remote_data;
    bool checkpwr(int *buf);
    void process(int *buf,pAC_State pac_state);
    bool checkSleep(int *buf);
    bool checkFanArrOn(int *buf);
    bool checkFanArrOff(int *buf);   
};


#endif // RELAYCONTROL_H



