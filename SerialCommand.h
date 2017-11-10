#ifndef SERIALCOMMAND_H  //预编译指令，防止重复定义类
#define SERIALCOMMAND_H
#include "Common.h"



class SerialCommand
{
private:    
    int checktemp(byte *buf);
    bool checkpwr(byte *buf);
    int checkfan(byte *buf);
    int checkmode(byte *buf);
    int checkerror(byte *buf);
    bool checkcmp(byte *buf);    

public:
    SerialCommand();
    void senddata(pAC_State pstate,byte* data);
    void SerialCommand_init(void);
    int checkdata(byte *buf, int count);  
    void process(byte* buffer,pAC_State pac_state);
};


#endif // RELAYCONTROL_H



