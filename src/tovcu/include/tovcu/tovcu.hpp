#pragma once
#include "ros/ros.h"
#include "tovcu/vcu_fbk.h"
#include "tovcu/cmd.h"
#include <dynamic_reconfigure/server.h>
#include "tovcu/controlConfig.h"
namespace tovcu
{
    class TO_VCU
    {
    private:
        ros::NodeHandle nh_;
        // ros::NodeHandle private_nh("~");
        ros::Subscriber sub;
        ros::Publisher pub;
        ros::Publisher pub1;
        tovcu::vcu_fbk fbk;
        tovcu::cmd cmd;

        dynamic_reconfigure::Server<control::controlConfig> server;
        dynamic_reconfigure::Server<control::controlConfig>::CallbackType cbType;

    public:
        TO_VCU(ros::NodeHandle &nh) : nh_(nh)
        {
            pub = nh_.advertise<tovcu::cmd>("vcuget", 100);
            pub1 = nh_.advertise<tovcu::vcu_fbk>("toplot", 100);
            sub = nh_.subscribe<tovcu::vcu_fbk>("vcuback", 100, &TO_VCU::chatterCallback1, this);
            cbType = boost::bind(&TO_VCU::callback, this, _1, _2);
            server.setCallback(cbType);

            ROS_INFO("init tovcu...............");
        };
        void chatterCallback1(const tovcu::vcu_fbk::ConstPtr &msg)
        {
            fbk.Vehicle_Spd=msg->Vehicle_Spd;
            fbk.Torque_fbk=msg->Torque_fbk;
            // fbk.Steering_Ang=msg->Steering_Ang;
            fbk.gear=cmd.control_vel;
            ROS_INFO("torque%.2f",fbk.Torque_fbk);
            pub1.publish(fbk);
            ROS_INFO("speed=%.2f", msg->Vehicle_Spd);
            //     ROS_INFO("torque=%.2f", msg->Torque_fbk);
            //     ROS_INFO("gear=%d", msg->gear);
        }
        void callback(control::controlConfig &config, uint32_t level)
        {
            ROS_INFO("Reconfigure Request");

            cmd.control_vel = config.drive_vel;
            cmd.control_enable = config.ENABLE;
            cmd.control_gear = config.gear;
            cmd.control_electric = config.light;
            cmd.control_parking = config.parking_able;
            cmd.control_brake = config.brake_torque;
            cmd.control_drive = config.drive_torque;
            cmd.control_steering = config.steer_angle;
            cmd.kp = config.pid_p;
            cmd.ki = config.pid_i;
            cmd.kd = config.pid_d;
            fbk.Steering_Ang=config.drive_vel;
        }

        bool Run()
        {

            pub.publish(cmd);
            
            // ROS_INFO("cmd:%0.2f", cmd.control_drive);
        };
    };
}