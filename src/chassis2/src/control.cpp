/*
一个通过can通讯实现的逻辑控制文件
作用控制 can.cpp等文件的功能
*/
#include <iostream>
#include "chassis2/control.h"
#include "chassis2/protocol.h"
#include "chassis2/can.h"
#include "ros/ros.h"
zhongyun_t zhongyun_obj;
extern canif_t can;
error_tt error;
__u8 auto_flag=0;
//注册回调接收回调函数
void register_can_in_handler(canin handler, void *ctx)
{
	can.cb_canin = handler;
	can.can_in_ctx = ctx;
}
//注册发送回调函数
void register_get_canframe(can_do_packet pack, void *ctx)
{
	can.cb_pack = pack;
	can.can_in_ctx = ctx;
}
void zhongyun_can_register_err_handler(err_handler handle, void *ctx)
{
	error.cb_error = handle;
	error.err_ctx = ctx;
}

// CAN初始化，报文解析初始化
int zhongyun_can_init()
{
	int ret = canif_init();
	if (ret == -1)
	{
		printf("*******zhongyun_can_init is failed********\r\n");
	}
	register_can_in_handler(canin_s, NULL);		  //输入报文打包
	register_get_canframe(can_do_packet_S, NULL); //输出报文打包
	return 0;
}
//////////////////报文设置函数////////////////////////
// frame传出
int suspend_state(struct can_frame *frame) //用户手动驾驶或调试模式
{

	zhongyun_carset(frame, &zhongyun_obj.user_set_ctl);

	return ZHONGYUN_SUSPEND;
}
int slefcheck_state(struct can_frame *frame) //自检状态
{
	int slef_check_ret = ZHONGYUN_SLEFCHECK;
	static int clink = 0;
	if (zhongyun_obj.car_slefcheck_state == 0)
	{
		memset(&zhongyun_obj.slef_smb_str, 0, sizeof(cmd_str)); //清除上次自检的值
		zhongyun_obj.slef_smb_str.Enb = 1;
		zhongyun_obj.slef_smb_str.Steering = 5;
		zhongyun_obj.car_slefcheck_state |= 0x01;
		zhongyun_obj.slef_smb_str.Brake = 10;
	}

	if ((zhongyun_obj.car_slefcheck_state >= 1) && (zhongyun_obj.enable_state.Auto_Enb == 1))
	{
		clink++;
		switch (zhongyun_obj.car_slefcheck_state)
		{
		case 0x01:
			if ((clink > 100) && (clink < 160))
			{
				if ((zhongyun_obj.vcu_state.Steering_Ang > 4) && (zhongyun_obj.vcu_state.Steering_Ang < 6))
				{
					zhongyun_obj.slef_smb_str.Steering = 0;
					zhongyun_obj.slef_smb_str.Gear = 1;
					zhongyun_obj.car_slefcheck_state |= 0x02;
					zhongyun_obj.slef_smb_str.Praking = 1;
				}
			}
			break;
		case 0x03:
			if ((clink > 170) && (clink < 330))
			{
				if (zhongyun_obj.vcu_state.gear == 1)
				{
					zhongyun_obj.slef_smb_str.Gear = 2;
					zhongyun_obj.slef_smb_str.Praking = 0;
					zhongyun_obj.car_slefcheck_state |= 0x04;
				}
			}
			break;
		case 0x07:
			if ((clink > 330) && (clink < 350))
			{
				zhongyun_obj.slef_smb_str.Electric = 0x01;
				zhongyun_obj.car_slefcheck_state |= 0x08;
			}
			break;
		case 0x0F:
			if ((clink > 350) && (clink < 400))
			{
				{
					zhongyun_obj.slef_smb_str.Electric = 0x00;
					zhongyun_obj.slef_smb_str.Brake = 10;
					zhongyun_obj.car_slefcheck_state = 0;
					slef_check_ret = ZHONGYUN_AUTODRIVE;
					clink = 0;
				}
			}
			break;
		}
	}
//printf("zhongyun_obj.car_slefcheck_state=%d \n",zhongyun_obj.car_slefcheck_state);

	if (clink > 500)
		slef_check_ret = ZHONGYUN_ERRORMODE;
	zhongyun_carset(frame, &zhongyun_obj.slef_smb_str);
//printf("clink==============%d\n",clink);
	return slef_check_ret;
}
int errormode_state(struct can_frame *frame)
{
	ROS_INFO("err state!!!!!!!!!!!!!!!");
	zhongyun_obj.user_set_ctl.Enb = 0;

	memset(&zhongyun_obj.user_set_ctl, 0, sizeof(zhongyun_obj.user_set_ctl));

	memset(&zhongyun_obj.slef_smb_str, 0, sizeof(zhongyun_obj.slef_smb_str));

	return ZHONGYUN_ERRORMODE;
}
int autodrive_state(struct can_frame *frame)
{
	int autodrive_state = ZHONGYUN_AUTODRIVE;

	if (zhongyun_obj.enable_state.Auto_Enb == 1)
	{ //车辆在自动驾驶
		

		if (zhongyun_obj.errorcode.Code_Num == 0)
		{ //车辆无故障
		auto_flag=1;
			zhongyun_carset(frame, &zhongyun_obj.user_set_ctl);
		}
		else
		{ //车辆有故障
			zhongyun_carset(frame, &zhongyun_obj.user_set_ctl);
		}
	}
	else
	{
auto_flag=0;
ROS_INFO("err state in auto!!!!!!!!!!!!!!!");
		zhongyun_obj.user_set_ctl.Enb = 0; //车辆退出自动驾驶

		memset(&zhongyun_obj.user_set_ctl, 0, sizeof(cmd_str)); //清除本次控制内容

		autodrive_state = ZHONGYUN_SUSPEND;
	}
	return autodrive_state;
}

///////////////状态获取函数///////////////
vcu_state_t *zhongyun_can_get_vcu_status()
{
	return &zhongyun_obj.vcu_state;
}
bms_t *zhongyun_can_get_bms_status()
{
	return &zhongyun_obj.bms;
}
enable_state_t *zhongyun_can_get_enable_status()
{
	return &zhongyun_obj.enable_state;
}
errorcode_t *zhongyun_can_get_errcode()
{
	return &zhongyun_obj.errorcode;
}
currenr_state_t *zhongyun_can_get_carstate()
{
	return &zhongyun_obj.carsate;
}
///////////////待补充//////////////////
void control_err_handler(const int *error_code, void *ctx)
{
	// TODO add your error handler here;
}

int control_start(float speed)
{
	while(zhongyun_obj.carsate.car_state==ZHONGYUN_AUTODRIVE)
	{
printf("auto_flag=%d=================\n",zhongyun_obj.carsate.car_state);
	}
	
	return 0;
}
int control_stop()
{
	return 0;
}

int zhongyun_AutoDrive_start() //启动后进入自检模式
{

	int old_car_currentstate = zhongyun_obj.car_currentstate;
	if (old_car_currentstate == ZHONGYUN_AUTODRIVE)
		return 0;
		
	zhongyun_obj.car_currentstate = ZHONGYUN_SLEFCHECK;
	return -1;
}

int zhongyun_AutoDrive_stop()
{
	zhongyun_obj.car_currentstate = ZHONGYUN_SUSPEND;

	memset(&zhongyun_obj.user_set_ctl, 0, sizeof(zhongyun_obj.user_set_ctl));

	memset(&zhongyun_obj.slef_smb_str, 0, sizeof(zhongyun_obj.slef_smb_str));

	return 0;
}
