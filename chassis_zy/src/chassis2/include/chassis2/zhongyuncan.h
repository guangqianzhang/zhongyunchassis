/*
一个自定义子类
继承于是socket_can.cpp
*/
#ifndef ZHONGYUNCAN_H_
#define ZHONGYUNCAN_H_
#include "socket_can.h"

class zhongyuncan : public SocketCAN
{
private:
    /* data */
public:
    zhongyuncan(const char *CANPort, const int bitrate) : SocketCAN(CANPort, bitrate){};
    // ~zhongyuncan();
    int zy_CanSendFrame(can_frame frame, bool wait_send)
    {
        int ret;

        if (frame.can_dlc > 8)
        {
            printf("canSnd buf length is wrong.\n");
            return -1;
        }

        if (NULL != frame.data)
        {
            ret = write(skt, &frame, sizeof(frame)) == sizeof(frame);
            if (ret == -1)
            {
                perror("write data:");
            }
        }
        else //考虑要不要计数空数据
        {
            printf("there is nothing in frame!\n");
        }
#ifdef TEST
        printf("send can id = \\\
    %x can[%x %x %x %x %x %x %x %x ]\n",
               frame.can_id, frame.data[0], frame.data[1],
               frame.data[2], frame.data[3], frame.data[4],
               frame.data[5], frame.data[6], frame.data[7]);
#endif
        if (wait_send)
            usleep(2000);
        return (ret); // Send this frame out to the socketCAN handler

        // socketCAN works to keeping all packets in-order, so
        // no need to do anything special for wait-sent
    }

    int zy_CanGetFrame(int fd, can_frame *GET_frame)
    {
        static int i=0;
        int ret = -1;
        struct can_frame frame;
        bzero(&frame, sizeof(frame));
        //接收的poll
        struct pollfd canPoll[1];
        canPoll[0].fd = fd;
        canPoll[0].events = POLLIN | POLLPRI;
        ret = poll(canPoll, 1, 1);//用于标记数组fds中的结构体元素的总数量；poll函数调用阻塞的时间，单位：毫秒；
        
        //printf(" recvData1 fd =%d \n", fd);
        //printf(" recvData1 ret =%d \n", ret);
        //printf(" recvData3 i=%d\n",i);
        if (-1 == ret)//error
        {
            return -1;
        }
        else if (0 == ret)//timeout
        {
            return 0;
        }
        //printf(" recvData2 ret=%d \n",ret);

        if (canPoll[0].revents & (POLLIN | POLLPRI))
        {
            ret = read(fd, &frame, sizeof(frame));
            i++;
            
            // printf("get can id = 
            //         %x can[%x %x %x %x %x %x %x %x ]\n",
            //    GET_frame->can_id, GET_frame->data[0], GET_frame->data[1],
            //    GET_frame->data[2], GET_frame->data[3], GET_frame->data[4],
            //    GET_frame->data[5], GET_frame->data[6], GET_frame->data[7]);
            if (-1 == ret)
            {
                printf("sgl_canRcvProcess read failed, error:%s.\n", strerror(errno));
                return -1;
            }
            memcpy(GET_frame, &frame, sizeof(frame));
            return 1;
        }
        printf(" recvData3 fd=%d\n",fd);
        return -1;
        

        return (ret); // Send this frame out to the socketCAN handler

        // socketCAN works to keeping all packets in-order, so
        // no need to do anything special for wait-sent
    }
};
#endif
