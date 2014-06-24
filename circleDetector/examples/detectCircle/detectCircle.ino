/*
 * Demo name   : Seeed CV library Demo 
 * Usage       : CV framework demo
 * Author      : Oliver Wang from Seeed Studio
 * Version     : v0.1
 
 * <Description> 
 * LED L will flash five times when Arduino shakes its hands with Unix
 * If the ball is tracked, LED L will be turned on. Otherwise, it's off
 * </Description>
*/

#include <Arduino.h>
#include <circleDetector.h>


void ledDisable(void);
void ledEnable(void);

void setup()
{
    CircleDetector.begin();
}

void loop()
{
    /* Detect circleNum */
    uint8_t circleNum = CircleDetector.detectCircle();
    if(circleNum > 0)
    {
        ledEnable();    //LED will be turned on when the ball is tracked
    }
    else
    {
        ledDisable();   //LED will be turned off when the ball is tracked
    }
}

 
 
void ledDisable(void){
     digitalWrite(13, LOW);
 }
 
void ledEnable(void){
     digitalWrite(13, HIGH);
 }