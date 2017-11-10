#ifndef COMMON_H
#define COMMON_H
#include "pitches.h"

#define Debug    Serial
//#define SerialCMD  SoftwareSerial(10, 11) // RX, TX

#define high(data) (abs(2500-data)<150)
#define low(data) (abs(960-data)<150)
#define cmdfirst(data) (abs(800-data)<150)
#define startkey(data) (abs(3500-data)<150)
#define cmdcheck(a,b) (abs(a-b)<150)


const int ToneKey = 12;
const int ToneAudio = NOTE_A2;
const int ToneTime = 100;
const int RECV_PIN = 2;
const int Rx_Pin = 10;
const int Tx_Pin = 11;
const int Led_update_time = 1000;
const int Serial_update_time = 2000;


#if 0
#define printmsg(b) Debug.print(b)
#define printmsgM(a,b) Debug.print(a,b)
#define printmsgln(b) Debug.println(b)
#define printmsglnM(a) Debug.println(a,b)
#else
#define printmsg(b)  
#define printmsgM(a,b) 
#define printmsgln(b) 
#define printmsglnM(a) 
#endif

#define temp_check(temp_s) (temp_s != 0xff)&&(temp_s<40)&&(temp_s>15)

#define Mode_Auto  0x00
#define Mode_Cool  0x01
#define Mode_DRY   0x02
#define Mode_FAN   0x03
#define Mode_HEAT  0x04

#define Fan_AUTO    0x00
#define Fan_LO      0x01
#define Fan_ME      0x02
#define Fan_HI      0x03

typedef struct AC_State_Type{
    int iMode;
    bool isPowerState;
    bool isCompressor;
    int iTemperture;
    int iFanSpeed;
    int iError;
}AC_State ,*pAC_State;

typedef struct Factory_State_Type{
    int B_Mode;
    int C_Mode;
    int D_Mode;    
    int L_Mode;        
    int H_Mode;            
}Factory_State ,*pFactory_State;

enum SerialState{CMD=0,TEMPH,TEMPL,FAN,PWR,CHANNEL};
enum Channel{channel_NG=0,channel_A,channel_B};
enum FactoryMode{FM_B_Mode=0,FM_C_Mode,FM_D_Mode,FM_L_Mode,FM_H_Mode,FM_Max};

#endif


