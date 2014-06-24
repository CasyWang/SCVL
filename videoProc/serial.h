/*
 * serial.h
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

#ifndef SERIAL_H_
#define SERIAL_H_

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

typedef struct port_info
{
  int baud_rate;
  int port_fd;
  char parity;
  char stop_bits;
  char flow_ctrl;
  char data_bits;
}*pport_info;

/* serial part */
int open_port(const char* port);
void close_port(int fd);
int set_port(int port_fd);
int send_data(int fd,void *data,int data_len);
int receive_data(int fd,void *data,int data_len);

#endif /* SERIAL_H_ */
