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

    int process(VideoCapture& capture)
    {
        int n = 0;
        char filename[200];
        string window_name = "video | q or esc to quit";
        cout << "press space to save a picture. q or esc to quit" << endl;
        namedWindow("Seeed CV", CV_WINDOW_KEEPRATIO);     //resizable window;
        Mat frame;
        for (;;)
        {
            /* Block here wait for arduino */
            char tmp[20]={0};
            int len = 0;
            while(!(DETECT_CIRCLE == tmp[0] && len > 0))
            {
            	len = read(serial_fd, tmp, 1);
            }

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
            Point textOrg(20, 100);
            for( size_t i = 0; i < circles.size(); i++ )
            {

            	  Vec3i c = circles[i];
            	  circle( current_frame, Point(c[0], c[1]), c[2], Scalar(178, 34, 34), 3, CV_AA);
            	  circle( current_frame, Point(c[0], c[1]), 2, Scalar(178, 34, 34), 3, CV_AA);
            	  //text
            	  putText(current_frame, "Tracked", textOrg, FONT_HERSHEY_PLAIN, 2,
            	                      Scalar::all(255), 3, 8);
            }

            /* Resp to Arduino */
            unsigned char circleCnt = circles.size();
            send_data(serial_fd, &circleCnt, 1);

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
        close_port(serial_fd);
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

    return process(capture);
}


