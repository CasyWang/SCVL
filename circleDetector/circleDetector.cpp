/*
 * circleDetector.cpp
 * SCVL (Seeed computer vision library)
 *
 * Copyright (c) Seeed Inc. 2014.
 * Author     : Oliver Wang
 * Create Time: 2014/06
 * Change Log :  
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
 
 #include <Arduino.h>
 #include "circleDetector.h"
 
 #define COM_PORT Serial

 
 circleDetector CircleDetector;
 
 /* Constructor */
 circleDetector::circleDetector(){     
	LED = 13;
 }
 
 
 void circleDetector::begin(){
      midLayerInit();
 }
 /****************************************************************************
 *
 * NAME: detectCircle
 *
 * DESCRIPTION:
 * detect the circles from real time video
 *
 * PARAMETERS: Name         RW  Usage
 *             circleAr     W   pointer to circle array              
 *
 * RETURNS:
 * uint8_t: detected circle number
 *
 ****************************************************************************/
/*  uint8_t detectCircle(tsCircle* circleAr){
     
 } */
 uint8_t circleDetector::detectCircle(void){
     COM_PORT.write(DETECT_CIRCLE);
	 while(!COM_PORT.available()); 
	 return COM_PORT.read();		
 }
/****************************************************************************
 *
 * NAME: midLayerInit
 *
 * DESCRIPTION:
 * Open camera
 *
 * PARAMETERS: Name         RW  Usage
 *                      
 * RETURNS:
 * none
 *
 ****************************************************************************/
 void circleDetector::midLayerInit(void){
     
	 /* Serial Initialize */
	 COM_PORT.begin(115200);
	 
	 /* LED Pin initialize */
	 pinMode(LED, OUTPUT);
	 
	 /* Shake hands one time */
	 COM_PORT.write(SHAKE_HANDS_REQ);
	 while(!COM_PORT.available());
	 if(SHAKE_HANDS_RESP == COM_PORT.read()){
		 ledBlink(5);
	 }			 
 }
 
/****************************************************************************
 *
 * NAME: ledBlink
 *
 * DESCRIPTION:
 * Open camera
 *
 * PARAMETERS: Name         RW  Usage
 *                      
 * RETURNS:
 * none
 *
 ****************************************************************************/
void circleDetector::ledBlink(uint8_t timers){
     int i;
     for(i = timers; i > 0; i--){
	       digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
           delay(500);                // wait for half a second
           digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
           delay(500); 
	 }
 }