#include "remote.h"
#include "Arduino.h"

Remote::Remote()
{
}

void Remote::Remote_init(void)
{
  remote_data = channel_A;
  printmsgln("init Remote");
  
}

bool Remote::check_remote_data(int *buf, int count)
{
  int channel = channel_NG;
  bool isOK = false;
  int i,index=0 , c=0;
//  byte data[20]={};
//  Serial.println("====buffer=====start===========");        
//  for(int i=1;i<count;i++){
//    Serial.print(buf[i],DEC);
//    Serial.print(", ");    
//  }
//  Serial.println("====buffer=======end=========");   
//  for(int i=3;i<count;i=i+2){
//    data[index] = (high(buf[i]))? (data[index]>>1)+0x80:(data[index]>>1);    
//    c++;    
//    if(c % 8 == 0){
//        index++;
//    }
//  }
//  Serial.println("====data=======start=========");            
//  for(int i=0;i<20;i++){
//    Serial.print("0x");      
//    Serial.print(data[i],HEX);
//    Serial.print(", ");    
//  }  
//  Serial.println("====data=======end=========");        
  if(startkey(buf[0])){
      if(startkey(buf[1])){
           for(i=3 ; i< 66; i++){
                if(i%2 == 0){                    
                    if(cmdfirst(buf[i])){
                        isOK = true;
                    }else{
                        //Serial.println("count:");
                        //Serial.println(i);                      
                        isOK = false;
                        return isOK;  
                    }
                }
           }        
           for(i=3;i<=17; i=i+2){
               if(cmdcheck(buf[i],buf[i+16])){
                 isOK = true;
               }else{
                 isOK = false;
                 //Serial.println("mapping:");
                 //Serial.println(i);  
                 return isOK;  
               }
           }         
           if(high(buf[58])||(high(buf[60]))){
               isOK = false;
               //Serial.println("sig error");
               return isOK;  
           }           
      }
      if(high(buf[47])&&high(buf[63])){
          //Serial.println("Channel B");
          channel = channel_B;
      }else if(low(buf[47])&&low(buf[63])){
          //Serial.println("Channel A");    
          channel = channel_A;
      }else{
          //Serial.println("Channel NG");    
          channel = channel_NG;        
      }
        
      if(remote_data == channel){   
          //Serial.println("OK");
      }else{
         //Serial.println("Channel Error");
         isOK = false;
         return isOK;      
      }      
  }else{
       //Serial.println("Start Error");    
  }
  Serial.print("OK : ");
  Serial.println(isOK);
  return isOK;    
}

int Remote::checktemp(int *buf)
{
  int temp = 0xff;
  int offset = 0;
  for(int i=0;i<4;i++){
    int index = (i*2)+3;
    if(high(buf[index])){
        offset = offset + (1<<i);
    }
  }  
  if(low(buf[59])){
       printmsg("offset:");
       printmsgM(offset,DEC);
       if(offset != 0)        
            temp = ((offset+15)<16)? 16:offset+15;    
  }else{
    printmsgln("Temperture NG");
  }                   
  return temp;
}

int Remote::checkfan(int *buf)
{
  int fan = 0;
  if(high(buf[11])&&high(buf[27])&&high(buf[77])){
        fan = fan + (1<<0);
  }  
  if(high(buf[13])&&high(buf[29])&&high(buf[79])){
        fan = fan + (1<<1);
  }  
  if(high(buf[15])&&high(buf[31])&&high(buf[81])){
        fan = fan + (1<<2);
  }  
  if(high(buf[17])&&high(buf[33])&&high(buf[83])){
        fan = fan + (1<<3);
  }  
  
  switch(fan)
  {
    case 6:
        fan = Fan_HI;
        printmsgln("Fan HIGH");
        break;
    case 4:
        fan = Fan_ME;    
        printmsgln("Fan MID");
        break;
    case 2:
        fan = Fan_LO;        
        printmsgln("Fan LOW");
        break;
    case 15:
        fan = Fan_AUTO;            
        printmsgln("Fan Auto");
        break;        
    default:
        break;
  }
       
  return fan;
}

bool Remote::checkpwr(int *buf)
{
  bool pwr = false;
  if( low(buf[41]) && low(buf[43]) && low(buf[45]) && low(buf[57]) && low(buf[59]) && low(buf[61]) ){
        pwr = true;
  }  
  printmsg("pwr:");
  printmsgln(pwr);
  return pwr;
}

int Remote::checkmode(int *buf)
{
  int mode = 0;
  if(high(buf[35])&&high(buf[51])){
        mode = mode + (1<<0);
  }  
  if(high(buf[37])&&high(buf[53])){
        mode = mode + (1<<1);
  }  
  if(high(buf[39])&&high(buf[57])){
        mode = mode + (1<<2);
  }  
  
  switch(mode)
  {
    case 2:
        mode = Mode_Cool;
        printmsgln("Cold Mode");
        break;
    case 3:
        mode = Mode_DRY;    
        printmsgln("Dry Mode");
        break;
    case 0:
        mode = Mode_FAN;    
        printmsgln("FAN Mode");
        break;
    case 4:
        mode = Mode_HEAT;    
        printmsgln("WARM Mode");
        break;    
    case 6:
        mode = Mode_Auto;    
        printmsgln("AUTO Mode");
        break;         
    default:
        break;
  }
  printmsg("mode:");
  printmsgln(mode);
  return mode;
}

void Remote::process(int *buf,pAC_State pac_state){
     int fan = checkfan(buf);
     int temp = checktemp(buf);
     int mode = checkmode(buf);
     boolean ispwr = checkpwr(buf);
     if(temp_check(temp)){
          //if(mode == Mode_Cool)
          {
                pac_state->iTemperture = temp;
                pac_state->iFanSpeed = fan;
                pac_state->iMode = mode;
          }         
          if(ispwr){
                pac_state->isPowerState = !pac_state->isPowerState;
          }                                 
     }  
}

bool Remote::checkSleep(int *buf){
  bool isCheck = false;  
  if(high(buf[3])&&low(buf[5])&&low(buf[7])&&low(buf[9])&&low(buf[11])&&low(buf[13])&&low(buf[15])&&low(buf[17])&&low(buf[35])&&low(buf[37])&&low(buf[39])&&low(buf[41])&&high(buf[43])&&low(buf[45])){
     isCheck = true;
  }
  return isCheck;
}

bool Remote::checkFanArrOn(int *buf){
  bool isCheck = false;  
  if(low(buf[3])&&low(buf[5])&&low(buf[7])&&low(buf[9])&&low(buf[11])&&low(buf[13])&&low(buf[15])&&high(buf[17])&&low(buf[35])&&low(buf[37])&&low(buf[39])&&low(buf[41])&&high(buf[43])&&high(buf[45])){
     isCheck = true;
  }
  return isCheck;
}

bool Remote::checkFanArrOff(int *buf){
  bool isCheck = false;  
  if(low(buf[3])&&high(buf[5])&&low(buf[7])&&low(buf[9])&&low(buf[11])&&low(buf[13])&&low(buf[15])&&low(buf[17])&&low(buf[35])&&low(buf[37])&&low(buf[39])&&low(buf[41])&&high(buf[43])&&high(buf[45])){
     isCheck = true;
  }
  return isCheck;
}


