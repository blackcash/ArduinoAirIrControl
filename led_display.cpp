#include "led_display.h"
#include "Arduino.h"

// Lab12 使用兩顆 74HC595 和三支腳位控制 16 顆 LED
// 接 74HC595 的 ST_CP (pin 12,latch pin)
const int latchPin = 8;
// 接 74HC595 的 SH_CP (pin 11, clock pin)
const int clockPin = 7;
// 接 74HC595 的 DS (pin 14)
const int dataPin = 6;
enum digitsNumber{ Number0=0, Number1,Number2,Number3,Number4,Number5,Number6,Number7,Number8,Number9,Number0FF,NumberA,Numberb,NumberC,Numberd,NumberE,NumberF,NumberP,NumberU,NumberH,Numbern,NumberL,Numberr};
const byte seven_seg_digits[] = { B01000000,  // = 0
                              B01111001,  // = 1
                              B00100100,  // = 2
                              B00110000,  // = 3
                              B00011001,  // = 4
                              B00010010,  // = 5
                              B00000010,  // = 6
                              B01111000,  // = 7
                              B00000000,  // = 8
                              B00010000,   // = 9
                              B01111111,   // = OFF
                              B01000000,   // = A                                                             
                              B00000011,   // = b                                                                                           
                              B01000110,   // = C                                                            
                              B00100001,   // = d                                                                                          
                              B00000110,   // = E                              
                              B00001110,   // = F                               
                              B00001100,   // = P                                                             
                              B01000001,   // = U                                                                                           
                              B00001001,   // = H
                              B00101011,   // = n                              
                              B01000111,   // = L
                              B00101111,   // = r                              
                             };

LED_Display::LED_Display()
{
}

void LED_Display::led_init(void)
{
  // 將 latchPin, clockPin, dataPin 設置為輸出
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  printmsgln("init LED_Display");
}

void LED_Display::led_dsiplay_number(pAC_State pstate,bool isCompress)
{
    
    if(pstate->isPowerState){
        // 送資料前要先把 latchPin 拉成低電位
        byte high = (pstate->iTemperture / 10) % 10;
        byte low = pstate->iTemperture%10;
        byte high_light = seven_seg_digits[high];
        byte low_light = seven_seg_digits[low];        
        digitalWrite(latchPin, LOW);
        if(pstate->isCompressor == false){
            if(!isCompress){
               high_light = (high_light | 0x80);
               low_light = (low_light | 0x80);
            }
        }
        if(pstate->iError != 0x00){
            Serial.print("error:");
            Serial.println(pstate->iError,HEX);            
            high_light = seven_seg_digits[NumberE];   // E
            switch(pstate->iError){
              case 0x80:
                  low_light = seven_seg_digits[Number0];    // 0              
                  break;              
              case 0x81:
                  low_light = seven_seg_digits[NumberC];    // C              
                  break;
              case 0x82:
                  low_light = seven_seg_digits[Number1];    // 1
                  break;
              case 0x83:
                  low_light = seven_seg_digits[Number2];    // 2
                  break;                  
              case 0x84:
                  low_light = seven_seg_digits[Number3];    // 3             
                  break;
              case 0x85:
                  low_light = seven_seg_digits[Number5];    // 5             
                  break;
              case 0x86:
                  low_light = seven_seg_digits[Number6];    // 6             
                  break;                  
              case 0x87:
                  low_light = seven_seg_digits[Number7];    // 7             
                  break;
              case 0x88:
                  low_light = seven_seg_digits[Number8];    // 8             
                  break;
              case 0x89:
                  low_light = seven_seg_digits[Number9];    // 9             
                  break;
              case 0x8A:
                  low_light = seven_seg_digits[NumberF];    // F             
                  break;
              case 0x8B:
                  low_light = seven_seg_digits[NumberA];    // A             
                  break;
              case 0x8C:
                  low_light = seven_seg_digits[NumberE];    // E             
                  break;
              case 0x8D:
                  low_light = seven_seg_digits[NumberP];    // P             
                  break;
              case 0x8E:
                  low_light = seven_seg_digits[NumberU];    // U             
                  break;
              case 0x8F:
                  low_light = seven_seg_digits[NumberH];    // H             
                  break;                  
              case 0x90:
                  low_light = seven_seg_digits[Number4];    // 4             
                  break;                  
              case 0x91:
                  low_light = seven_seg_digits[Numbern];    // n
                  break;                  
              case 0x92:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number1];    // 1
                  break;                   
              case 0x93:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number2];    // 2
                  break;                   
              case 0x94:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number4];    // 4
                  break;                   
              case 0x95:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number5];    // 5
                  break;                   
              case 0x96:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number6];    // 6
                  break;                   
              case 0x97:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number7];    // 7
                  break;                   
              case 0x98:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number8];    // 8
                  break;                   
              case 0x99:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number9];    // 9
                  break;                   
              case 0x9A:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Number0];    // 0
                  break;                   
              case 0x9B:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[NumberL];    // L
                  break;                   
              case 0x9C:
                  high_light = seven_seg_digits[NumberP];   // P              
                  low_light = seven_seg_digits[Numbern];    // n
                  break;                                     
              default:
                  low_light = seven_seg_digits[Numberr];    // r
                  break;
            }            

        }
        // 先送高位元組 (Hight Byte), 給離 Arduino 較遠的那顆 74HC595
        shiftOut(dataPin, clockPin, MSBFIRST, low_light); 
        // 再送低位元組 (Low Byte), 給離 Arduino 較近的那顆 74HC595
        shiftOut(dataPin, clockPin, MSBFIRST, high_light);  
    
        // 送完資料後要把 latchPin 拉回成高電位
        digitalWrite(latchPin, HIGH);
    }else{
        digitalWrite(latchPin, LOW);      
        // 先送高位元組 (Hight Byte), 給離 Arduino 較遠的那顆 74HC595
        shiftOut(dataPin, clockPin, MSBFIRST, seven_seg_digits[Number0FF]); 
        // 再送低位元組 (Low Byte), 給離 Arduino 較近的那顆 74HC595
        shiftOut(dataPin, clockPin, MSBFIRST, seven_seg_digits[Number0FF]);      
        // 送完資料後要把 latchPin 拉回成高電位
        digitalWrite(latchPin, HIGH);     
    }   
}

void LED_Display::led_dsiplayFactory_number(pFactory_State pstate,int pos)
{
    bool isError = false;
    byte high_light;
    byte low_light;
    int data;
        
    switch(pos){
      case FM_B_Mode:
        high_light = seven_seg_digits[Numberb];
        data = pstate->B_Mode;
        break;
      case FM_C_Mode:
        high_light = seven_seg_digits[NumberC];
        data = pstate->C_Mode;        
        break;      
      case FM_D_Mode:
        high_light = seven_seg_digits[Numberd];
        data = pstate->D_Mode;        
        break;            
      case FM_L_Mode:
        high_light = seven_seg_digits[NumberL];      
        data = pstate->L_Mode;        
        break;            
      case FM_H_Mode:
        high_light = seven_seg_digits[NumberH];            
        data = pstate->H_Mode;                
        break;            
      case FM_Max:  
      default:
        isError = true;
        break;            
    }
    
    if(isError ==  false){
        
        low_light = seven_seg_digits[data];
        // 先送高位元組 (Hight Byte), 給離 Arduino 較遠的那顆 74HC595
        shiftOut(dataPin, clockPin, MSBFIRST, low_light); 
        // 再送低位元組 (Low Byte), 給離 Arduino 較近的那顆 74HC595
        shiftOut(dataPin, clockPin, MSBFIRST, high_light);  
    
        // 送完資料後要把 latchPin 拉回成高電位
        digitalWrite(latchPin, HIGH);      
    }
}
