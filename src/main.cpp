/***************************************************************************************
                                b l u e T o o t h
                                                                      қuran dec 2024
**************************************************************************************/
#include <Arduino.h>
#include <BluetoothSerial.h>

#define TRUE                             1
#define FALSE                            0
#define WAIT_ONE_SEC                 10000
#define ON_BOARD_LED                     5
#define DAC                             25   // Trig
#define WHEEL_L                          2
#define WHEEL_R                         A4
#define WHEEL_L_DIRECTION               15 
#define WHEEL_R_DIRECTION               A5
#define BATTERY_LEVEL                   A3   // GPIO 39

void IRAM_ATTR myTimer(void);

volatile int flag; 
volatile int vL, vR;
BluetoothSerial SerialBT;

void setup() 
{
    hw_timer_t *timer = NULL;

    pinMode(ON_BOARD_LED, OUTPUT);
    pinMode(DAC, OUTPUT);
    pinMode(WHEEL_L, OUTPUT);
    pinMode(WHEEL_R, OUTPUT);
    pinMode(WHEEL_L_DIRECTION, OUTPUT);
    pinMode(WHEEL_R_DIRECTION, OUTPUT);


    digitalWrite(ON_BOARD_LED, LOW); // invers logic!
    digitalWrite(WHEEL_L_DIRECTION, LOW );
    digitalWrite(WHEEL_R_DIRECTION, HIGH);
    digitalWrite(WHEEL_L, LOW); // stop !
    digitalWrite(WHEEL_R, LOW); // stop !


    vR = vL = 0;

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &myTimer, true);
    timerAlarmWrite(timer, 100, true);  // 0.1 msec
    timerAlarmEnable(timer);

    Serial.begin(115200);
    SerialBT.begin("EL-ROBOT");  

    sei();                              // normaly not needed cause Arduino
                                        // uses it befor the loop starts.
                                        // but 'cause we want to use the 
                                        // serial imediately... 
                                        // therefore we call sei() first!
    printf("start!\n");

}  

void loop() 
{
    static int led = 0;
    char c;
    if (flag)
    {
        flag = 0; 
        printf("Elektronik und Technische Informatik");
    }

    if (Serial.available() > 0) 
    {
        String data = Serial.readString();
        if (data.indexOf('x') != -1)
        {
            // x received
            flag = TRUE;
            printf("\nx received!\n");

            led ^= 1; // toggle
            digitalWrite(ON_BOARD_LED, led);
        }
    }
    
    while(SerialBT.available())       
    {
        c = SerialBT.read();             
        if (c == 'x')         
        {
            // x received
            printf("\nblueTooth: x received!\n");
            led ^= 1; // toggle
            digitalWrite(ON_BOARD_LED, led);

            if (led) 
            {
                SerialBT.printf("LED off\n");
            }
            else 
            { 
                SerialBT.printf("LED on\n");
            }
        }
        c  = 0;
    }
}


void IRAM_ATTR myTimer(void)   // periodic timer interrupt, expires each 0.1 msec
{
    static int32_t count  = 0;
    static unsigned char ramp = 0;

    count++;
    ramp++;

    dacWrite(DAC, ramp);

    if (count >= WAIT_ONE_SEC) 
    {
        flag = TRUE;
        count = 0;
    }


    // PWM:

    if (ramp >= vL) digitalWrite(WHEEL_L, LOW);  else digitalWrite(WHEEL_L, HIGH);
    if (ramp >= vR) digitalWrite(WHEEL_R, LOW);  else digitalWrite(WHEEL_R, HIGH);

}
