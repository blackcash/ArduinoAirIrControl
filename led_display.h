#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H
#include "Common.h"


class LED_Display
{
private:    

public:
    LED_Display();
    void led_init(void);
    void led_dsiplay_number(pAC_State pstate,bool isCompress);
    void led_dsiplayFactory_number(pFactory_State pstate,int pos);
};

#endif 



