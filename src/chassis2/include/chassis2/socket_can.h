#ifndef SOCKET_CAN_H
#define SOCKET_CAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <wait.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>


using namespace std;
const int test = 1;
class SocketCAN
{
private:
    void set_port(const char *CANPort);
    void set_bitrate(const int bitrate);

public:
    SocketCAN(const char *CANPort, const int bitrate);
    ~SocketCAN();
    char can_Command[128];

    int can_set();
    int can_socket(__u8 num,unsigned int *mask);
    bool CANSendFrame(__u32 id, const __u8 *buf, __u8 len, bool wait_send);
    bool CANGetFrame(__u32 &id, __u8 *buf, __u8 &len);
    bool CANClose(int skt);
    protected:
    int _CAN_bitrate;
    const char *_CANPort;
    int *mask;
    __u8 num;
    int skt;
};

#endif