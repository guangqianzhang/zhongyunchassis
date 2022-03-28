#ifndef CONTROL_H_
#define CONTROL_H_
#include "zhongyun_def.h"
#include "canif.h"
#include "zhongyun.h"



struct error_tt
{
	err_handler cb_error;
	void *err_ctx;
};



typedef struct _cmd_str
{
	double Drive;
	int Enb;
	int Gear;
	int  Electric;    ///按位 0x01左转  0x02右转  0x04 大灯  0x08 制动灯  0x0f  全亮
	int Praking;
	double Steering;
	double Brake;
}cmd_str;
typedef enum _ZHONGYUN_STATE_T {
  ZHONGYUN_SLEFCHECK = 1,
  ZHONGYUN_AUTODRIVE = 2,
  ZHONGYUN_ERRORMODE = 3,
  ZHONGYUN_SUSPEND   = 0,
} ZHONGYUN_STATE_T;
 struct zhongyun_t
{
	 int AutoDrive_flag;//1  处于自动驾驶   0处于手动驾驶
     int car_currentstate;
     int car_slefcheck_state;
     cmd_str slef_smb_str;
     cmd_str user_set_ctl;
     vcu_state_t     vcu_state;
     bms_t           bms;
     enable_state_t  enable_state;
     errorcode_t     errorcode;
     currenr_state_t     carsate;
     struct can_frame can_frame_t;
     struct can_frame connect_frame[6];
     struct can_frame slefcheck_frame[6];
     struct can_frame suspend_frame[6];
     struct can_frame autodrive_frame[6];
};
extern zhongyun_t zhongyun_obj;

int suspend_state(struct can_frame *frame);
int slefcheck_state(struct can_frame *frame);
int errormode_state(struct can_frame *frame);
int autodrive_state(struct can_frame *frame);

void control_err_handler(const int *error_code ,void *ctx);

vcu_state_t     *zhongyun_can_get_vcu_status();
bms_t           *zhongyun_can_get_bms_status();
enable_state_t  *zhongyun_can_get_enable_status();
errorcode_t     *zhongyun_can_get_errcode();
currenr_state_t *zhongyun_can_get_carstate();

///////////赋值函数///////////////
int zhongyun_can_set_steering(double value);
int zhongyun_can_set_drive_torque(double value);
int zhongyun_can_set_brake_torque(double value);
int zhongyun_can_set_parking_enable(int enable);
int zhongyun_can_set_gear(int value);
int zhongyun_can_set_light(int value);
int zhongyun_can_set_ENABLE(int value);
int zhongyun_can_emergency_stop();


/////////can的初始化操作/////
int      zhongyun_can_init();
void     zhongyun_can_deinit();

/////////////自动控制///////////////
int zhongyun_AutoDrive_start();
int zhongyun_AutoDrive_stop();
int  control_start(float speed);
int  control_stop();
#endif /* SRC_ZHONGYUN_DEF_H_ */
