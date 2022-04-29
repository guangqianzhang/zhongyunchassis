/*
 * zhongyun_def.h
 *
 *  Created on: 2021年3月30日
 *      Author: gongping.xie
 */

#ifndef SRC_ZHONGYUN_DEF_H_
#define SRC_ZHONGYUN_DEF_H_
#define  ZHONGYUAN_GEAR_ID 	 			    0xA1
#define  ZHONGYUAN_STEERING_ID 	 			0xA2
#define  ZHONGYUAN_DRIVE_ID 	 			0xA3
#define  ZHONGYUAN_BREAK_ID 	 			0xA4
#define  ZHONGYUAN_PARKING_ID 	 			0xA5
#define  ZHONGYUAN_ELECTRIC_ID 	 			0xA6

#define  VCU_FeedBack 	 			0xC1
#define  BMS_FeedBack 	 			0xC2
#define  Enable_Fbk 	 			0xC3
#define  Err_Fbk                    0xE1


typedef struct _vcu_state{
	double Vehicle_Spd;       //当前车速
	double Torque_fbk;        //当前力矩
	int parking;              //手刹信息
	int gear;                 //档位信息
	double Steering_Ang;      //方向盘角度
}vcu_state_t;

typedef struct _bms{
	int SOC;                 //soc
	double Bat_Vol;          //电压
	int Bat_Discharge_Cur;   //放电电流
}bms_t;


typedef struct _enable_state{
	int Auto_Enb;
	int Gear_Enb_fbk;
	int Steering_Enb_fbk;
	int Drive_Enb_fbk;
	int Brake_Enb_fbk;
	int Parking_Enb_fbk;
	int RC_Takeover_flg;	//接管标志位
}enable_state_t;

typedef struct _errorcode{
	int Error_code;  //故障码
	int Code_Num;    //具体故障码
}errorcode_t;

typedef struct _currenr_state_{
	int car_state;
}currenr_state_t;



#endif /* SRC_ZHONGYUN_DEF_H_ */
