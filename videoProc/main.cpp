/*
 * main.c
 * computer vision library for seeeduino cloud
 * Based on Zdenek Kalal's matlab version and OpenCV
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

#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>  //for cvtColor
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "serial.h"
#include "common.h"

using namespace cv;
using namespace std;

const char *serial_port = "/dev/ttyUSB0";
int serial_fd = 0;

const string color_txt[] = {
		"Yellow",
		"Green",
		"Blue",
		"indigo",
		"purple",
		"pink",
		"red"
};

//hide the local functions in an anon namespace
namespace
{
    void help(char** av)
    {
        cout << "\nCall from Arduino: detectCircle()\n"
        "Usage:\n./" << av[0] << " <video device number>\n" << "q,Q,esc -- quit\n"
        << "space   -- save frame\n\n"
        << "\n\n"
        << endl;
    }

    int dectColor(int h)
    {
      if(h>=1 && h <= 3)
      {
        return YELLOW;
      }
      else if(h>=4 && h<=6)
      {
        return GREEN;
      }
      else if(h>=7 && h<=9)
      {
        return BLUE;
      }
      else if(h>=10 && h<=11)
      {
        return INDIGO;
      }
      else if(h>=11 && h<=13)
      {
        return PURPLE;
      }
      else if(h>=13 && h<=15)
      {
        return PINK;
      }
      else
      {
        return RED;
      }
    }

    int process(VideoCapture& capture)
    {
        int n = 0;
        char filename[200];
        string window_name = "video | q or esc to quit";
        cout << "press space to save a picture. q or esc to quit" << endl;
        namedWindow("Seeed CV", CV_WINDOW_KEEPRATIO);     //resizable window;
        namedWindow("ROI", CV_WINDOW_KEEPRATIO);          //display ROI area
        cvNamedWindow( "H-S Histogram", 1 );
        Mat frame;
        for (;;)
        {
            /* Block here wait for arduino */
        	/*
            char tmp[20]={0};
            int len = 0;
            while(!(DETECT_CIRCLE == tmp[0] && len > 0))
            {
            	len = read(serial_fd, tmp, 1);
            }*/

            /* video processing */
            capture >> frame;
            if (frame.empty())
                break;

            cv::Mat current_frame;
            cv::Mat gray;

            frame.copyTo(current_frame);

            medianBlur(current_frame, current_frame, 5);
            cv::cvtColor(current_frame, gray,COLOR_RGB2GRAY);
            vector<Vec3f> circles;

            HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 10,
           	                 100, 30, 1, 50   // change the last two parameters
           	                                  // (min_radius & max_radius) to detect larger circles
           	                 );
            printf("circle num:%d\n", circles.size());
            for( size_t i = 0; i < circles.size(); i++ )
            {
            	  int color;
            	  Vec3i c = circles[i];

                  /* Get ROI */
            	  int w = 2*c[2];               //width
                  int h = 2*c[2];               //height
                  int x = c[0] - c[2];          //x coordinator
                  int y = c[1] - c[2];          //y coordinator

                  Rect roi_rect(x, y, w, h);    //ROI rect
                  printf("ROI x:%d y:%d w:%d h:%d \n ",x, y, w, h);

                  cv::Mat roi_img;

                  if(x < 0 || y < 0 || x + w > current_frame.cols || y + h > current_frame.rows ) continue;
                  current_frame(roi_rect).copyTo(roi_img);

                  /* Statistics HSV color histogram in ROI */
                  IplImage src = IplImage(roi_img);
              	  IplImage* hsv = cvCreateImage( cvGetSize(&src), 8, 3 );
              	  IplImage* h_plane = cvCreateImage( cvGetSize(&src), 8, 1 );
              	  IplImage* s_plane = cvCreateImage( cvGetSize(&src), 8, 1 );
              	  IplImage* v_plane = cvCreateImage( cvGetSize(&src), 8, 1 );
              	  IplImage* planes[] = { h_plane, s_plane };

              	  /* h range 16 */
              	  int h_bins = 16, s_bins = 8;
              	  int hist_size[] = {h_bins, s_bins};

              	  float h_ranges[] = {0, 180};
              	  float s_ranges[] = {0, 255};

              	  float *ranges[] = {h_ranges, s_ranges};

              	  cvCvtColor(&src, hsv, CV_BGR2HSV);
              	  cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, 0);

              	  CvHistogram* hist = cvCreateHist(2, hist_size, CV_HIST_ARRAY, ranges, 1);
                  cvCalcHist(planes, hist, 0, 0);

                  float max_value;
                  cvGetMinMaxHistValue(hist, 0, &max_value, 0, 0);
                  printf("hist max val:%f\n", max_value);

                  int height = 240;
                  int width = (h_bins*s_bins*6);
                  IplImage* hist_img = cvCreateImage( cvSize(width,height), 8, 3 );
                  cvZero( hist_img );

              	 IplImage * hsv_color = cvCreateImage(cvSize(1,1),8,3);
              	 IplImage * rgb_color = cvCreateImage(cvSize(1,1),8,3);
               	 int bin_w = width / (h_bins * s_bins);
              	 for(int h = 0; h < h_bins; h++)
              	 {
              		for(int s = 0; s < s_bins; s++)
              		{
              			int i = h*s_bins + s;

              			float bin_val = cvQueryHistValue_2D( hist, h, s );
              			if(bin_val == max_value)
              			{
              				printf("max[%d, %d]\n", h, s);
              				color = dectColor(h);
              			}
              			int intensity = cvRound(bin_val*height/max_value);

              			cvSet2D(hsv_color,0,0,cvScalar(h*180.f / h_bins,s*255.f/s_bins,255,0));
              			cvCvtColor(hsv_color,rgb_color,CV_HSV2BGR);
              			CvScalar color = cvGet2D(rgb_color,0,0);

              			cvRectangle( hist_img, cvPoint(i*bin_w,height),
              				cvPoint((i+1)*bin_w,height - intensity),
              				color, -1, 8, 0 );
              		}
              	 }

              	cvShowImage( "H-S Histogram", hist_img );


                imshow("ROI", roi_img);

                Point textOrg(c[0], c[1]);
            	  /* Draw */
            	  circle( current_frame, Point(c[0], c[1]), c[2], Scalar(178, 34, 34), 3, CV_AA);
            	  circle( current_frame, Point(c[0], c[1]), 2, Scalar(178, 34, 34), 3, CV_AA);

            	  putText(current_frame, color_txt[color], textOrg, FONT_HERSHEY_PLAIN, 2,
            	                      Scalar::all(255), 3, 8);

            }

            /* Resp to Arduino */
           // unsigned char circleCnt = circles.size();
            //send_data(serial_fd, &circleCnt, 1);

            imshow("Seeed CV", current_frame);

            char key = (char) waitKey(5); //delay N millis, usually long enough to display and capture input
            switch (key)
            {
                case 'q':
                case 'Q':
                case 27:  //escape key
                    return 0;
                case ' ': //Save an image
                    sprintf(filename, "filename%.3d.jpg", n++);
                    imwrite(filename, frame);
                    cout << "Saved " << filename << endl;
                    break;
                default:
                    break;
            }
        }

        /* Close serial port */
        //close_port(serial_fd);
        return 0;
    }

}

int main(int ac, char** av)
{
    if (ac != 2)
    {
        help(av);
        return 1;
    }
    std::string arg = av[1];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file
    if (!capture.isOpened())   //if this fails, try to open as a video camera, through the use of an integer param
        capture.open(atoi(arg.c_str()));
    if (!capture.isOpened())
    {
        cerr << "Failed to open a video device or video file!\n" << endl;
        help(av);
        return 1;
    }

    /*
    serial_fd = open_port(serial_port);
    if(-1 == serial_fd)
    {
    	cerr << "Failed to open serial port!\n" << endl;
        return 1;
    }
    set_port(serial_fd);

    char tmp[20]={0};

    int len = 0;
    while(!(SHAKE_HANDS_REQ == tmp[0] && len > 0))
    {
    	len = read(serial_fd, tmp, 1);
    }

    char ack = SHAKE_HANDS_RESP;
    send_data(serial_fd, &ack, 1);
    */

    return process(capture);
}


