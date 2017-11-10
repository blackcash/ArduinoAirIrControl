#include "Arduino.h"
#include "SerialCommand.h"

SerialCommand::SerialCommand()
{
}

void SerialCommand::SerialCommand_init(void)
{
   printmsgln("SerialCommand init!!");
}

void SerialCommand::senddata(pAC_State pstate,byte* data){
    int checksum = 0x00;
    data[3] = pstate->isCompressor? 0x3f: 0x00;
    data[4] = pstate->iMode;
    data[5] = pstate->iTemperture;
    data[6] = pstate->iFanSpeed;
    data[7] = (pstate->isPowerState)? 0x80:0x00;    
    data[9] = 0x00;
    for(int i=0;i<10;i++){
         checksum = checksum + data[i];
    }
    data[10] = 0x100 - (checksum & 0xff);
}

int SerialCommand::checkdata(byte *buf, int count)
{
  bool isStart = false;
  int checksum = 0,i = 0, j = 0;
  for(i=0;i<count-10;i++){
      if((buf[i] == 0x55)&&(buf[i+1] == 0xAA)){
           isStart = true;
           break;
      }
      if((buf[i] == 0x55)&&(buf[i+1] == 0x03)){
           isStart = true;
           break;
      }      
  }  
  if(isStart){
      for(j=i;j<i+10;j++){
           checksum = checksum + buf[j];
         }
      checksum = 0x100 - (checksum & 0xff);
      if(buf[j] == checksum){
        return i;
      }
  }
  return 0xff;  
}

int SerialCommand::checktemp(byte *buf)
{
    printmsgM(buf[5],DEC);
    printmsgln(" C");
    return buf[5];
} 

bool SerialCommand::checkpwr(byte *buf)
{
    bool isPwr = false;
    if(buf[7] & 0x80){
        isPwr = true;
        printmsgln("Power On!!");
    }else{
        printmsgln("Power Off!!");      
    }
    return isPwr;
}

bool SerialCommand::checkcmp(byte *buf)
{
    bool isCmp = false;
    if(buf[3] != 0x00){
        isCmp = true;
        printmsgln("CMP On!!");
    }else{
        printmsgln("CMP Off!!");      
    }
    return isCmp;
}

int SerialCommand::checkfan(byte *buf)
{
    switch(buf[6]){
      case 0:
           printmsgln("fan auto mode!");
           break;
      case 1:
           printmsgln("fan lo mode!");
           break;
      case 2:
           printmsgln("fan me mode!");
           break;
      case 3:
           printmsgln("fan hi mode!");
           break;
      default:
           printmsgln("fan mode ????!!");
           break;     
    }  
    return buf[6];  
}

int SerialCommand::checkmode(byte *buf)
{
    switch(buf[4]){
      case 0:
           printmsgln("auto mode!");
           break;
      case 1:
           printmsgln("cool mode!");
           break;
      case 2:
           printmsgln("dry mode!");
           break;
      case 3:
           printmsgln("fan mode!");
           break;
      case 4:
           printmsgln("heat mode!");
           break;           
      default:
           printmsgln("mode ????!!");
           break;     
    }  
    return buf[4];
}

int SerialCommand::checkerror(byte *buf)
{  
    return buf[9];
}

void SerialCommand::process(byte* buffer,pAC_State pac_state)
{
    int fan = checkfan(buffer);
    int temp = checktemp(buffer);
    int mode = checkmode(buffer);
    int iserr = checkerror(buffer);
    boolean ispwr = checkpwr(buffer);
    boolean iscmp = checkcmp(buffer);
    if(mode == 1){
            pac_state->iTemperture = temp;
            pac_state->iFanSpeed = fan;
            pac_state->iMode = mode;
    }         
    pac_state->isPowerState = ispwr;               
    pac_state->iError = iserr;     
    pac_state->isCompressor = iscmp;    
}
