#include "ros/ros.h"
#include "tovcu/tovcu.hpp"
#include <signal.h>
#include "std_msgs/String.h"

void mgSigintHandler(int sig)
{
    ros::shutdown();
    ros::waitForShutdown();
    exit(0);
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    signal(SIGINT, mgSigintHandler);
    ros::init(argc, argv, "to_vcu", ros::init_options::NoSigintHandler);
    ros::NodeHandle nh;
    
    
    
    tovcu::TO_VCU tovcu(nh);
    ros::AsyncSpinner spinner(2);
    spinner.start();
    ros::Rate rate(100);

    while (ros::ok)
    {

        rate.sleep();
        tovcu.Run();
        ROS_INFO("run");
    }

    return 0;
}
