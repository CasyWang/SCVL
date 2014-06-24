/*
 * serial.c
 * computer vision library for seeeduino cloud
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include "serial.h"

int open_port(const char* port)
{
  int fd=open(port,O_RDWR|O_NOCTTY);
  if(fd==-1)
  {
    perror("serial port open fail\n");
    return -1;
  }
  return fd;
}

void close_port(int fd)
{
  close(fd);
}

int set_port(int port_fd)
{
  struct termios old_opt,new_opt;
  int baud_rate,parity;
  memset(&old_opt,0,sizeof(old_opt));
  memset(&new_opt,0,sizeof(old_opt));
  cfmakeraw(&new_opt);
  tcgetattr(port_fd,&old_opt);
  baud_rate=B115200;
  cfsetispeed(&new_opt,baud_rate);
  cfsetospeed(&new_opt,baud_rate);
  new_opt.c_cflag|=CLOCAL;
  new_opt.c_cflag|=CREAD;
  new_opt.c_cflag|=IXON|IXOFF|IXANY;
  new_opt.c_cflag|=CS8;
  new_opt.c_cflag&=~PARENB;
  new_opt.c_cflag&=~CSTOPB;
  new_opt.c_oflag&=~OPOST;
  new_opt.c_cc[VMIN]=1;
  new_opt.c_cc[VTIME]=1;
  tcflush(port_fd,TCIFLUSH);
  int result;
  result=tcsetattr(port_fd,TCSANOW,&new_opt);
  if(result==-1)
  {
      perror("cannot set the serial port parameters\n");
      return -1;
  }
  tcgetattr(port_fd,&old_opt);
  return result;
}


int send_data(int fd,void *data,int data_len)
{
  int len=write(fd,data,data_len);
  if(len == data_len)
    return len;
  else
  {
    tcflush(fd,TCOFLUSH);
    return -1;
  }
}

int receive_data(int fd,void *data,int data_len)
{
  int len,fs_sel;
  fd_set fs_read;
  struct timeval time;
  FD_ZERO(&fs_read);
  FD_SET(fd,&fs_read);
  time.tv_sec=0;
  time.tv_usec=50000;
  fs_sel=select(fd+1,&fs_read,NULL,NULL,&time);
  if(fs_sel)
  {
    len=read(fd,data,data_len);
    return len;
  }
  else
  {
    return -1;
  }
}








