#include "ros/ros.h"
#include "chassis2/control.h"
#include "chassis2/vcu_fbk.h"
#include "chassis2/cmd.h"
#include "chassis2/pid.hpp"
namespace vcu_ns
{
    class VCU_TOPIC
    {
    private:
        ros::NodeHandle nh_;
        ros::Subscriber sub1;
        ros::Publisher pub1;
        chassis2::vcu_fbk *vcu_msg1;
        Pid vcu_pid;
        // int able,gear,par,linght;
        // float ster,bra,driv;
        int able=0, gear=0, par=0, linght=0;
        double kp, ki,kd;
        float ster=0.0, bra=0.0, driv=0.0,driv_limit=0.0;
        double _g_vel = 0;
    public:
        VCU_TOPIC(ros::NodeHandle &nh, double *speed) :
                 nh_(nh),
                 vcu_pid(&zhongyun_obj.vcu_state.Vehicle_Spd,
                     &zhongyun_obj.user_set_ctl.Drive,
                     &_g_vel)
        {
            sub1 = nh_.subscribe<chassis2::cmd>("vcuget", 100, &VCU_TOPIC::chatterCallback1, this);
            pub1 = nh_.advertise<chassis2::vcu_fbk>("vcuback", 100);
            vcu_msg1 = (chassis2::vcu_fbk *)&zhongyun_obj.vcu_state;
        };
        void chatterCallback1(const chassis2::cmd::ConstPtr &msg)
        {
                   able = msg->control_enable;
                    gear= msg->control_gear;
                    par= msg->control_parking;
                    ster= msg->control_steering;
                    bra= msg->control_brake;
                    linght=msg->control_electric;
                    driv_limit= msg->control_drive;

                    _g_vel = msg->control_vel;
                    kp = msg->kp;
                    ki = msg->ki;
                     kd = msg->kd;
            // 将接受到的消息打印出来
            ROS_DEBUG("I hear msg->gear: [%d]", msg->control_gear);
            ROS_DEBUG("conrol_drive: [%.f]", msg->control_drive);
            // ROS_INFO("i also hear :[%d]",zhongyun_obj.vcu_state.gear);
        };
        ~VCU_TOPIC(){};
        bool Run()
        {
            
            // vcu_msg1->Vehicle_Spd=zhongyun_obj.vcu_state.Vehicle_Spd;
            pub1.publish(*vcu_msg1);
            // zhongyun_obj.user_set_ctl.Drive=vcu_pid.realize(kp,ki,kd);
            if(zhongyun_obj.user_set_ctl.Gear==3&&zhongyun_obj.user_set_ctl.Enb==1)driv=vcu_pid.realize(1.6,0.0,0.3);
            // if(driv>driv_limit) driv=driv_limit;
            // else if (driv<-driv_limit)driv=-driv_limit; //放算法里面进行限制
            zhongyun_can_set_ENABLE(able);
            zhongyun_can_set_gear(gear);
            zhongyun_can_set_light(linght);
            zhongyun_can_set_steering(ster);
            zhongyun_can_set_drive_torque(driv);
            zhongyun_can_set_brake_torque(bra);
            zhongyun_can_set_parking_enable(par); // 1驻车

        };
    };

}