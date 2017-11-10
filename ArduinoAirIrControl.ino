#include "remote.h"
#include "led_display.h"
#include <IRremote.h>
#include "Common.h"
#include "SerialCommand.h"
#include <SoftwareSerial.h>


IRrecv irrecv(RECV_PIN);
Remote rc;
LED_Display ld;
AC_State ac_state;
Factory_State factory_state;
SerialCommand sc;
SoftwareSerial SerialCMD = SoftwareSerial(Rx_Pin, Tx_Pin);

unsigned long tone_cmd_time = 0;   
unsigned long ir_cmd_time = 0;
unsigned long led_cmd_time = 0;
unsigned long factory_cmd_time = 0;
decode_results results;
boolean isCompression = true;
int cindex = 0;
int buffer[200];
byte cbuffer[30];
SerialState state = CMD;
boolean isFactory = false;
int factory_count = 0;
int pos = FM_B_Mode;

int data_init()
{
  ac_state.iMode = Mode_Cool;
  ac_state.isPowerState = false;
  ac_state.isCompressor = false;
  ac_state.iTemperture = 24;
  ac_state.iFanSpeed = Fan_HI;
  ac_state.iError = 0x00;
}

void setup() {
  Debug.begin(115200);  
  SerialCMD.begin(2400); 
  sc.SerialCommand_init(); 
  ld.led_init();
  irrecv.enableIRIn(); // Start the receiver
  rc.Remote_init();
  data_init();
  pinMode(ToneKey, OUTPUT);
  digitalWrite(ToneKey,HIGH);
}

void loop() {  

  cmd_serial_process();
  cmd_control_process();
  remote_process();
  millis_process(); 
}

void millis_process(){
  
  if(abs(millis() - led_cmd_time) > Led_update_time){
       if(isFactory == false){
         ld.led_dsiplay_number(&ac_state,isCompression);
         isCompression = !isCompression; 
       }else{
         ld.led_dsiplayFactory_number(&factory_state,pos);
         if(ac_state.isPowerState == false){
            isFactory = false;
         }
       }       
       led_cmd_time = millis();
  }      
  if(abs(millis() - ir_cmd_time) > Serial_update_time){  
       sendscdata();
  }
  if(abs(millis() - ir_cmd_time) > ToneTime){  
       digitalWrite(ToneKey,HIGH);              
  }  
}

void remote_control_process(decode_results *results, int data_control)
{
//  int buf[results->rawlen-1];
  if(data_control == 0){
      for(int i=1 ; i<results->rawlen ; i++){
        if ((i % 2) == 1) {
          buffer[i-1] = results->rawbuf[i]*USECPERTICK;  
        } 
        else {
          buffer[i-1] = abs(-(int)results->rawbuf[i]*USECPERTICK);  
        }    
      }
  }
  else if(data_control == 1){
      for(int i=1 ; i<results->rawlen ; i++){
        if ((i % 2) == 1) {
          buffer[100+i-1] = results->rawbuf[i]*USECPERTICK;  
        } 
        else {
          buffer[100+i-1] = abs(-(int)results->rawbuf[i]*USECPERTICK);  
        }    
      }    
      if(rc.check_remote_data(buffer,200)){
              if(isFactory == false){
                  if((ac_state.iMode == Mode_Cool)&&(ac_state.iFanSpeed == Fan_AUTO)&&(ac_state.isPowerState == true)&&(ac_state.iTemperture == 30)){
                        factory_count++;
                        if(factory_count > 5){
                            isFactory = true;
                            factory_count = 0;
                        }
                        Serial.print("factory_count:");                        
                        Serial.println(factory_count,DEC);                                                
                  }                
                  rc.process(buffer,&ac_state);
                  sendscdata();
                  if((ac_state.isPowerState)||(rc.checkpwr(buffer)))
                       digitalWrite(ToneKey,LOW);                   
              }else{
                 if(rc.checkpwr(buffer) == true){
                    rc.process(buffer,&ac_state);
                    sendscdata();
                    if((ac_state.isPowerState)||(rc.checkpwr(buffer)))
                       digitalWrite(ToneKey,LOW);                                      
                 }else if(rc.checkSleep(buffer) == true){
                       up_factorydata(&factory_state,pos);
                       ld.led_dsiplayFactory_number(&factory_state,pos);                                       
                 }else if((rc.checkFanArrOn(buffer) == true)||(rc.checkFanArrOff(buffer) == true)){
                       if((++pos) == FM_Max){
                          pos = FM_B_Mode;
                       }                   
                       ld.led_dsiplayFactory_number(&factory_state,pos);
                 }
              }
              tone_cmd_time = millis();
              print_Data();              
              //tone(ToneKey, ToneAudio, ToneTime);
//              ld.led_dsiplay_number(&ac_state,isCompression);
      }
   }
}



void remote_process()
{
  if (irrecv.decode(&results)) 
  {
    int count = dump(&results);
    if(count > 0){
         remote_control_process(&results,0);  
    }    
    Serial.println("============1===========");    
    //print_receive_data(&results,count);    
    irrecv.resume(); // Receive the next value
    delay(200);
    if (irrecv.decode(&results)) 
    {
       int count = dump(&results);
       if(count > 0){
          remote_control_process(&results,1);  
       }    
       Serial.println("============2===========");
       //print_receive_data(&results,count);           
       irrecv.resume(); // Receive the next value
    }    
  }
}

void cmd_control_process()
{  
  if (SerialCMD.available()){
    cbuffer[cindex] = SerialCMD.read();
    cindex++;
    if(cindex > 30){
        cindex = 0;          
    }    
  }
  if(cindex > 10){
//      Serial.println("=========old==============");       
//     for(int i = 0;i<cindex;i++){
//        Serial.print("0x");       
//        Serial.print(cbuffer[i],HEX);              
//        Serial.print(" ,");
//     }
//      Serial.println("=======================");           
     int index = sc.checkdata(cbuffer,cindex);
     if(index != 0xff){
              byte buffer[11];
              for(int i=index;i<index+11;i++){
                  buffer[i-index] = cbuffer[i];
              }
//               Serial.println("========new===============");       
//              for(int i = 0;i<11;i++){
//                 Serial.print("0x");       
//                 Serial.print(buffer[i],HEX);              
//                 Serial.print(" ,");
//              }
//              Serial.println("=======================");       
              sc.process(buffer,&ac_state);              
              //ld.led_dsiplay_number(&ac_state,isCompression);           
              cindex = 0;    
     } 
  }
}

void cmd_serial_process()
{
  if (Serial.available() > 0) 
  {
       static int temp_s = 0;
       char cmd = Serial.read();
       if(cmd == '?'){
             display_cmd_list();
       }else{
             switch(state){
             case CMD:
                  if(cmd == '1'){
                    state = TEMPH;
                  }else if(cmd == '2'){   
                    state = FAN;
                  }else if(cmd == '3'){   
                    state = PWR;                    
                  }else if(cmd == '4'){   
                    state = CHANNEL;                                        
                  }else{
                    Serial.println("input error");
                  }               
                  break;
             case TEMPH:
                  temp_s = 0;
                  if((cmd >= '1')&&(cmd <= '3')){
                    temp_s = (cmd-0x30)*10;
                    state = TEMPL;               
                  }else{
                    Serial.println("input error");
                  }   
                  break;             
             case TEMPL:
                  if((cmd >= '0')&&(cmd <= '9')){
                    temp_s = temp_s+(cmd-0x30); 
                    if(temp_check(temp_s)){
                        ac_state.iTemperture = temp_s;                      
                        Serial.print("Temperture : ");                   
                        Serial.println(temp_s);            
                        sendscdata();                        
                    }else{
                        Serial.println("Temperture Error!!");                   
                    }
                    state = CMD;               
                  }else{
                    Serial.println("input error");
                  }   
                  break;                               
             case FAN:
                  if((cmd >= '0')&&(cmd <= '4')){
                    state = CMD;               
                    if(cmd == '0'){
                      ac_state.iFanSpeed = Fan_AUTO;
                      Serial.println("Fan Auto");
                    }else if(cmd == '1'){
                      ac_state.iFanSpeed = Fan_HI;
                      Serial.println("Fan HIGH");                      
                    }else if(cmd == '2'){
                      ac_state.iFanSpeed = Fan_ME;
                      Serial.println("Fan MID");
                    }else if(cmd == '3'){
                      ac_state.iFanSpeed = Fan_LO;
                      Serial.println("Fan LOW");                      
                    }                   
                    sendscdata();                                            
                  }else{
                    Serial.println("input error");
                  }
                  break;                                            
             case PWR:
                  if((cmd >= '0')&&(cmd <= '1')){
                    state = CMD;               
                    ac_state.isPowerState = (cmd == '0')? false:true;
                    sendscdata();                                            
                  }else{
                    Serial.println("input error");                    
                  }
                  break;                                               
             case CHANNEL:
                  if((cmd >= '0')&&(cmd <= '1')){
                    state = CMD;               
                    rc.remote_data = (cmd == '1')? channel_A:channel_B;
                  }else{
                    Serial.println("input error");                    
                  }
                  break;                                                            
             default:
                 break;                 
           }      
           display_cmd_list();                       
       }
  }            
}

void display_cmd_list(){
     switch(state){
             case CMD:
                  Serial.println("1.Temperture");
                  Serial.println("2.FAN Speed");
                  Serial.println("3.Power On/Off");                  
                  Serial.println("4.Remote Channel A/B");                              
                  break;
             case TEMPH:
                  Serial.println("Input temperture(10) :");
                  break;             
             case TEMPL:
                  Serial.println("Input temperture(1) :");
                  break;                               
             case FAN:
                  Serial.println("Input Fan Speed (0-4):");
                  break;                                            
             case PWR:
                  Serial.println("Input Power (1:On,0:Off) :");             
                  break;                                               
             case CHANNEL:
                  Serial.println("Input Channel (1:A,0:B) :");             
                  break;                                               
                  
             default:
                 break; 
     }          
}

int dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
//    Serial.print("Unknown encoding: ");
  } 
  else if (results->decode_type == NEC) {
    printmsg("Decoded NEC: ");
  } 
  else if (results->decode_type == SONY) {
    printmsg("Decoded SONY: ");
  } 
  else if (results->decode_type == RC5) {
    printmsg("Decoded RC5: ");
  } 
  else if (results->decode_type == RC6) {
    printmsg("Decoded RC6: ");
  }
  else if (results->decode_type == JVC) {
     printmsg("Decoded JVC: ");
  }
  return count;
}

void print_receive_data(decode_results *results, int count){
      Serial.print(results->value, HEX);
      Serial.print(" (");
      Serial.print(results->bits, DEC);
      Serial.println(" bits)");
      Serial.print("unsigned int YourVariableName1 [");
      Serial.print(count-1, DEC);
      Serial.print("]={");
    
      for (int i = 1; i < count; i++) {//ingore data of i=0
        if ((i % 2) == 1) {
                 Serial.print(results->rawbuf[i]*USECPERTICK,DEC);      
        } 
        else {
                Serial.print(abs(-(int)results->rawbuf[i]*USECPERTICK),DEC);
        }
        if(i<count-1){
            Serial.print(" ");
        }
      }
      Serial.println("};");  
}

void sendscdata(){
   byte buf[11]={0x55, 0xAA, 0x00, 0x00, 0x01, 0x19, 0x02, 0x84, 0x18, 0x00, 0x49};
   sc.senddata(&ac_state,buf);
   //Serial.println("SerialCommand send!!");
   SerialCMD.write(buf,11);             
   digitalWrite(11, LOW);     
   delay(40);  
   digitalWrite(11, HIGH);        
   ir_cmd_time = millis();        
   //print_Data(); 
}
  
void print_Data(){
    Serial.println("ac_state --------");
    Serial.print("M:0x");
    Serial.print(ac_state.iMode,HEX);
    Serial.print(",P:0x");    
    Serial.print(ac_state.isPowerState,HEX);    
    Serial.print(",C:0x");        
    Serial.print(ac_state.isCompressor,HEX);        
    Serial.print(",T:0x");            
    Serial.print(ac_state.iTemperture,DEC);            
    Serial.print(",F:0x");                
    Serial.print(ac_state.iFanSpeed,HEX);            
    Serial.print(",E:0x");                    
    Serial.print(ac_state.iError,HEX);  
    Serial.println("--------------------");    
}

void up_factorydata(pFactory_State pstate,int ppos)
{
  switch(ppos){
      case FM_B_Mode:
        if(++pstate->B_Mode >9)
            pstate->B_Mode = 0;
        break;
      case FM_C_Mode:
        if(++pstate->C_Mode >9)
            pstate->C_Mode = 0;
        break;      
      case FM_D_Mode:
        if(++pstate->D_Mode >7)
            pstate->D_Mode = 0;  
        break;            
      case FM_L_Mode:
        if(++pstate->L_Mode >9)
            pstate->L_Mode = 0;
        break;            
      case FM_H_Mode:
        if(++pstate->H_Mode >9)
            pstate->H_Mode = 0;
        break;            
      case FM_Max:  
      default:        
        break;            
   }      
}
