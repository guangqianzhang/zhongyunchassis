/*
 * zhongyun_def.h
 *
 *  Created on: 2021��3��30��
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
	double Vehicle_Spd;       //��ǰ����
	double Torque_fbk;        //��ǰ����
	int parking;              //��ɲ��Ϣ
	int gear;                 //��λ��Ϣ
	double Steering_Ang;      //�����̽Ƕ�
}vcu_state_t;

typedef struct _bms{
	int SOC;                 //soc
	double Bat_Vol;          //��ѹ
	int Bat_Discharge_Cur;   //�ŵ����
}bms_t;


typedef struct _enable_state{
	int Auto_Enb;
	int Gear_Enb_fbk;
	int Steering_Enb_fbk;
	int Drive_Enb_fbk;
	int Brake_Enb_fbk;
	int Parking_Enb_fbk;
	int RC_Takeover_flg;	//�ӹܱ�־λ
}enable_state_t;

typedef struct _errorcode{
	int Error_code;  //������
	int Code_Num;    //���������
}errorcode_t;

typedef struct _currenr_state_{
	int car_state;
}currenr_state_t;



#endif /* SRC_ZHONGYUN_DEF_H_ */
