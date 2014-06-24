/*
 * circleDetector.h
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
 
  /* circle */
 typedef struct{
     uint16_t p_x;         
	 uint16_t p_y;
	 uint16_t radius;
 }tsCircle;
 
 /* frame */
/*  typedef struct{
     uint8_t delimiter;
	 uint8_t length;
	 enum payload{
	     uint8_t cmd;
		 tsCircle circleList;
	 };
	 uint8_t checkSum;
 }tsFrame; */
 
 typedef enum{
    SHAKE_HANDS_REQ = 0x2B,
    SHAKE_HANDS_RESP = 0x2D,	
	DETECT_CIRCLE = 0x7a
 }teFrameIdentifier;
 

 
 /* circle detector class */
 class circleDetector{
     public:
	 /* Constructor */
	 circleDetector();
	 void begin();
	 /* Open camera */
	 //uint8_t openCamera(uint8_t dev);
	 /* Detect circles from video or image */
	 uint8_t detectCircle(void);	
	 
	 private:
	 int LED;
	 /* Midlayer is used to communication */
	 void midLayerInit(void);
	 void ledBlink(uint8_t timers);
	 
 };
 
 extern circleDetector CircleDetector;