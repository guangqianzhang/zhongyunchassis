#include <iostream>
#include <pthread.h>
#include "chassis2/zhongyun.h"
#include "chassis2/can.h"
#include "chassis2/socket_can.h"
#include "chassis2/control.h"
#include "chassis2/protocol.h"

#include <thread>
#include "ros/ros.h"
#include "chassis2/vcu_topic.hpp"
using namespace std;
pthread_t task_set;
pthread_t task_get;
//定义变量
vcu_state_t *vcu_state_t_get = NULL;
bms_t *bms_t_get = NULL;
enable_state_t *enable_state_t_get = NULL;

errorcode_t *errorcode_t_get = NULL;
currenr_state_t *currenr_state_get = NULL;

float _goal_Speed;
// void *task_test_set(void *ctx)
int task_test_set()
{
    static int i = 0;

#if 0 //测试退出自动驾驶
		if(currenr_state_get->car_state==2)
		{
			zhongyun_AutoDrive_stop();
		}

#else //测试参数设置功能

    i++;

    zhongyun_can_set_light(4);
    zhongyun_can_set_ENABLE(1); //使能信号控制
    zhongyun_can_set_gear(3);   //档位请求1zhuche 2空档 3前进

    if (i > 3000)
    {
        zhongyun_can_set_steering(20);
        zhongyun_can_set_drive_torque(10);
    }
    else
    {
        zhongyun_can_set_steering(-20); //转向
        zhongyun_can_set_drive_torque(20);
    }
    // printf("set count=%d\n", i);
    zhongyun_can_set_brake_torque(0);
    zhongyun_can_set_parking_enable(0); // 1驻车

#endif
    usleep(10000);

    return 0;
}

// void *task_test_get(void *ctx)
// {
//     while(1)
//     {
//     	vcu_state_t_get      = zhongyun_can_get_vcu_status();
//     	bms_t_get            = zhongyun_can_get_bms_status();
//     	enable_state_t_get   = zhongyun_can_get_enable_status();
//     	errorcode_t_get      = zhongyun_can_get_errcode();
//     	currenr_state_get    = zhongyun_can_get_carstate();
//     	printf("vcu_state Steering_Ang=%f,Torque_fbk=%f,Vehicle_Spd=%f,gear=%d,parking=%d \r\n",
//     			vcu_state_t_get->Steering_Ang,vcu_state_t_get->Torque_fbk,
//     			vcu_state_t_get->Vehicle_Spd,vcu_state_t_get->gear,vcu_state_t_get->parking);
//     	//printf("bms_t_get param1=%d,param1=%f,param1=%d \r\n",bms_t_get->SOC,bms_t_get->Bat_Vol,bms_t_get->Bat_Discharge_Cur);
//     	printf("current_state_get=%d ********************** \r\n",currenr_state_get->car_state);
//     	printf("enable          =%d ********************** \r\n" ,enable_state_t_get->Auto_Enb);
//     	usleep(10000);
//     }
//     return 0;
// }



int main(int argc, char *argv[])
{

    setlocale(LC_ALL, "");
    ros::init(argc, argv, "vcu", ros::init_options::NoSigintHandler);
    ros::NodeHandle nh;
    ROS_INFO("%s", ros::this_node::getName().c_str());

    // 3.创建服务器对象
    // dynamic_reconfigure::Server<control::controlConfig> server;
    // // 4.创建回调对象(使用回调函数，打印修改后的参数)
    // dynamic_reconfigure::Server<control::controlConfig>::CallbackType cbType;
    // cbType = boost::bind(&callback,_1,_2);
    // // 5.服务器对象调用回调对象
    // server.setCallback(cbType);
    double *drive_vel=NULL;
    ros::Rate rate(100);
    vcu_ns::VCU_TOPIC vcu_topic(nh,drive_vel);
    ros::AsyncSpinner spinner(2); // Use 4 threads
    spinner.start();
    //！ 1 can总线接口初始化
    if (0 != zhongyun_can_init())
    {
        printf("canbase init failed !\r\n");
        return -1;
    }
    zhongyun_can_register_err_handler(control_err_handler, NULL);
    //！ 4 开启can总线模块
    zhongyun_can_start();
    //进入自动驾驶
    zhongyun_AutoDrive_start();
    printf("auto start......................\n");
    //! 6 进入终端信号事件循环
    // char c = getchar();
    // while (c == 'q')
    //     break;

    while (ros::ok)
    {
        rate.sleep();
        // task_test_set();
        vcu_topic.Run();
        // ROS_INFO("data update>>>>>>>>>>");
    }

    return 0;
}
