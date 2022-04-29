/*
一个关于can协议的解析文件
*/
#include <iostream>
#include "chassis2/protocol.h"
#include "chassis2/control.h"


extern zhongyun_t zhongyun_obj;
///////////////////////////////////////////
///////////////can线赋值////////////////////
int zhongyun_can_set_steering(double value)
{
	zhongyun_obj.user_set_ctl.Steering=value;

	return 1;
}
int zhongyun_can_set_drive_torque(double value)
{
	zhongyun_obj.user_set_ctl.Drive=value;
	return 1;
}
int zhongyun_can_set_brake_torque(double value)
{
	zhongyun_obj.user_set_ctl.Brake=value;
	return 1;
}
int zhongyun_can_set_parking_enable(int enable)
{
	zhongyun_obj.user_set_ctl.Praking=enable;
	return 1;
}
int zhongyun_can_set_gear(int value)
{
	zhongyun_obj.user_set_ctl.Gear=value;
	return 1;
}
int zhongyun_can_set_light(int value)
{
	zhongyun_obj.user_set_ctl.Electric=value;
	return 1;
}
int zhongyun_can_set_ENABLE(int value)
{
	zhongyun_obj.user_set_ctl.Enb=value;
	return 1;
}

//控制ID等赋值 具体还不明晰
// frame 传出
void zhongyun_carset(struct can_frame *frame,cmd_str *Tocar_smd_str)
{
	   frame[0].can_id =ZHONGYUAN_GEAR_ID;
	   frame[0].can_dlc=8;
	   frame[0].data[0]=Tocar_smd_str->Enb;
	   frame[0].data[1]=Tocar_smd_str->Gear;
	   frame[0].data[2]=Tocar_smd_str->Enb;
	   

	   frame[1].can_id =ZHONGYUAN_STEERING_ID;
	   frame[1].can_dlc=8;
	   frame[1].data[0]=Tocar_smd_str->Enb;

	   int stering     =(int)((Tocar_smd_str->Steering+28)*1170.27);
	   frame[1].data[1]= stering%256;
	   frame[1].data[2]= stering/256;


	   frame[2].can_id =ZHONGYUAN_DRIVE_ID;
	   frame[2].can_dlc=8;
	   frame[2].data[0]=Tocar_smd_str->Enb;
	   int Drive = (int)(Tocar_smd_str->Drive*655.74);
	   frame[2].data[1]= Drive%256;
	   frame[2].data[2]= Drive/256;


	   frame[3].can_id =ZHONGYUAN_BREAK_ID;
	   frame[3].can_dlc=8;
	   
	   int brake = (int)(Tocar_smd_str->Brake*655.74);
		(brake>300)?(Tocar_smd_str->Enb=0x01):(Tocar_smd_str->Enb=0x00);
	   frame[3].data[0]=Tocar_smd_str->Enb;
	   frame[3].data[1]=brake%256;
	   frame[3].data[2]=brake/256;

	   frame[4].can_id =ZHONGYUAN_PARKING_ID;
	   frame[4].can_dlc=8;
	   frame[4].data[0]=Tocar_smd_str->Enb;
	   frame[4].data[1]=Tocar_smd_str->Praking;


	   frame[5].can_id =ZHONGYUAN_ELECTRIC_ID;
	   frame[5].can_dlc=8;
	   int light=0;
	   ((Tocar_smd_str->Electric&0x01)==0x01)?(light=1):(light=0);
	   frame[5].data[0]=light;
	   ((Tocar_smd_str->Electric&0x02)==0x02)?(light=1):(light=0);
	   frame[5].data[1]=light;
	   ((Tocar_smd_str->Electric&0x04)==0x04)?(light=1):(light=0);
	   frame[5].data[2]=light;
	   ((Tocar_smd_str->Electric&0x08)==0x08)?(light=1):(light=0);
	   frame[5].data[3]=light;
}
///////////////////////////////
//////////can流程线控制//////////

//接收报文处理函数
void canin_s(const struct can_frame *frame, void *ctx)
{
	double speed_temp=0;
	double tor_temp=0;
    switch (frame->can_id)
    {
    case VCU_FeedBack:
    {
         speed_temp= (frame->data[1] * 256 + frame->data[0]) * 0.1;
        zhongyun_obj.vcu_state.Vehicle_Spd=speed_temp/36.0;
		printf("speed in protp=%.2f\n",zhongyun_obj.vcu_state.Vehicle_Spd);
		zhongyun_obj.vcu_state.Steering_Ang = (frame->data[3] * 256 + frame->data[2]) * 0.0008545 - 28;
        zhongyun_obj.vcu_state.gear = frame->data[4];
		tor_temp= frame->data[5] * 0.19607;
        zhongyun_obj.vcu_state.Torque_fbk =tor_temp;
		printf("torque inproto=%0.2f\n",zhongyun_obj.vcu_state.Torque_fbk);
    }
    break;
    case BMS_FeedBack:

        zhongyun_obj.bms.SOC = frame->data[0];
        zhongyun_obj.bms.Bat_Vol = (frame->data[1] + frame->data[2] * 256) * 0.1;
        zhongyun_obj.bms.Bat_Discharge_Cur = frame->data[3];

        break;

    case Enable_Fbk:
    {
        zhongyun_obj.enable_state.Auto_Enb = frame->data[0] | frame->data[1] | frame->data[2] |
                                             frame->data[3] | frame->data[4] | frame->data[5];
        zhongyun_obj.enable_state.Brake_Enb_fbk = frame->data[0];
        zhongyun_obj.enable_state.Drive_Enb_fbk = frame->data[0];
        zhongyun_obj.enable_state.Gear_Enb_fbk = frame->data[0];
        zhongyun_obj.enable_state.Parking_Enb_fbk = frame->data[0];
        zhongyun_obj.enable_state.RC_Takeover_flg = frame->data[0];
    }
    break;
    case 0xE1:
        zhongyun_obj.errorcode.Error_code = frame->data[0];
        zhongyun_obj.errorcode.Code_Num = frame->data[1];

        break;
    }
}
//发送报文打包状态机函数
//frame传出
void can_do_packet_S(struct can_frame *frame, int *size, void *ctx)
{
    switch (zhongyun_obj.car_currentstate)
    {
    case ZHONGYUN_SUSPEND: //静态模式

        zhongyun_obj.car_currentstate = suspend_state(frame);

        break;

    case ZHONGYUN_SLEFCHECK: //自检模式

        zhongyun_obj.car_currentstate = slefcheck_state(frame);

        break;

    case ZHONGYUN_AUTODRIVE: //自动驾驶模式

        zhongyun_obj.car_currentstate = autodrive_state(frame);

        break;

    case ZHONGYUN_ERRORMODE: //故障模式

        errormode_state(&zhongyun_obj.can_frame_t);

        break;
    }
    // zhongyun_obj.carsate=zhongyun_obj.car_currentstate;
    zhongyun_obj.carsate.car_state = zhongyun_obj.car_currentstate;

    *size = 6;
}
