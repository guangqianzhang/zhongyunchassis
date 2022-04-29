
#include <iostream>
#include "chassis2/canif.h"
#include "ros/ros.h"
#include "chassis2/zhongyuncan.h"
#include "chassis2/zhongyun.h"
#include "chassis2/can.h"
#include <pthread.h>
#include "chassis2/control.h"
unsigned int pCanID_usr[5] = {0xC1, 0xC2, 0xC3, 0xC4, 0xE1}; //接收id
zhongyuncan can_send("can0", 500000);
canif_t can;
//建立can通讯套接字，实现can总线初始化
int canif_init()
{

    can.socket_tx = can_send.can_socket(sizeof(pCanID_usr), pCanID_usr);
    if (can.socket_tx == 0)
    {
        printf("*******canif_init is failed***********\r\n");
        return -1;
    }
    can.flag_in = 1; //初始化标志
    can.flag_out = 1;
    return 0;
}
void *canSend(void *ctx)
{
    static int i = 0;
    static int n = 0;
    while (can.flag_out)
    {
        // printf("*********************task_can_out***************\r\n");
        memset(can.frame_data, 0, sizeof(struct can_frame) * 6);
        //不明觉厉 把数据赋值给can.frame
        if (can.cb_pack != NULL)
            can.cb_pack(can.frame_data, &can.send_size, can.can_in_ctx);

        (i < 5) ? (i++) : (i = 0);
        can_send.zy_CanSendFrame(can.frame_data[i], false);
        n++;

        // can_send.CANSendFrame(ZHONGYUAN_GEAR_ID,zhongyun_obj.autodrive_frame->data,sizeof(zhongyun_obj.autodrive_frame->data),false);

        usleep(20000);
        ROS_INFO("e=%d,ger=%d,ster=%f,tor=%f",
                 zhongyun_obj.user_set_ctl.Enb,
                 
                 zhongyun_obj.user_set_ctl.Gear,
                 zhongyun_obj.user_set_ctl.Steering,
                 zhongyun_obj.user_set_ctl.Drive
                 
                 );
    }

    return 0;
}
void *canGet(void *ctx)
{
    while (can.flag_in)
    {
        // printf("*********************task_can_in***************\r\n");

        struct can_frame frame;

        int ret = can_send.zy_CanGetFrame(can.socket_tx, &frame); // dengdai 1ms

        // printf("*********************task_can_in id=%x***************\r\n",frame.can_id);

        if (can.cb_canin != NULL)
        {
            can.cb_canin(&frame, can.can_in_ctx);
        }
        // usleep(10000);
        usleep(1000);
    }
    return 0;
}
int canif_start()
{
    //    thread a(canSend);
    // a.detach();
    // thread b(canGet);
    // b.detach();
    int ret = pthread_create(&can.task_recv, NULL, canGet, NULL);
    if (ret == -1)
        printf("****pthread_create can_task_can_in is failed********");

    ret = pthread_create(&can.task_send, NULL, canSend, NULL);
    if (ret == -1)
        ROS_WARN("****pthread_create can_task_can_in is failed********");

    return 0;
}
int zhongyun_can_start()
{
    canif_start();

    return 0;
}
