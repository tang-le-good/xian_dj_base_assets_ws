#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include"xian_dj_retractable_platform_control_pkg/xian_dj_retractable_platform_base_electric.h"
#include"xian_dj_retractable_platform_control_pkg/xian_dj_retractable_platform_control.h"
#include <std_msgs/UInt16.h>
class XianDjRetractablePlatformControl
{
    public:
        XianDjRetractablePlatformControl()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            xian_dj_retractable_platform_control_state_pub = nh.advertise<std_msgs::UInt16>("xian_dj_retractable_platform_control_state_msg", 1);
            xian_dj_retractable_platform_base_electric_pub = nh.advertise<xian_dj_retractable_platform_control_pkg::xian_dj_retractable_platform_base_electric>("xian_dj_retractable_platform_base_electric_msg", 1);
            xian_dj_retractable_platform_control_sub = nh.subscribe<xian_dj_retractable_platform_control_pkg::xian_dj_retractable_platform_control>("xian_dj_retractable_platform_control_msg", 10, &XianDjRetractablePlatformControl::controller_callback, this);
        }

        ros::WallTimer m_timer_heart_beat;

        void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        {
            xian_dj_retractable_platform_control_heart_beat = xian_dj_retractable_platform_control_heart_beat > 1000 ? 0 : (xian_dj_retractable_platform_control_heart_beat + 1);
            std::cout << "xian_dj_retractable_platform_control_heart_beat: " << xian_dj_retractable_platform_control_heart_beat << std::endl;
            heart_beat.data = xian_dj_retractable_platform_control_heart_beat;
            xian_dj_retractable_platform_control_state_pub.publish(heart_beat);
        }

        void controller_callback(const xian_dj_retractable_platform_control_pkg::xian_dj_retractable_platform_control::ConstPtr &data)
        {
            xian_dj_retractable_platform_stand_linear_actuator_stand_cmd = data->xian_dj_retractable_platform_stand_linear_actuator_stand_cmd;
            xian_dj_retractable_platform_stand_linear_actuator_sit_cmd = data->xian_dj_retractable_platform_stand_linear_actuator_sit_cmd;
            xian_dj_retractable_platform_first_linear_actuator_up_cmd = data->xian_dj_retractable_platform_first_linear_actuator_up_cmd;
            xian_dj_retractable_platform_first_linear_actuator_down_cmd = data->xian_dj_retractable_platform_first_linear_actuator_down_cmd;
            xian_dj_retractable_platform_second_linear_actuator_up_cmd = data->xian_dj_retractable_platform_second_linear_actuator_up_cmd;
            xian_dj_retractable_platform_second_linear_actuator_down_cmd = data->xian_dj_retractable_platform_second_linear_actuator_down_cmd;

            // 控制倒伏伸缩杆运动状态
            if (xian_dj_retractable_platform_stand_linear_actuator_stand_cmd==1 && xian_dj_retractable_platform_stand_linear_actuator_sit_cmd==0)
            {
                // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_enble", 1);
                // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_move", 1);
                xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_stand_linear_actuator_enble = 1;
                xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_stand_linear_actuator_move = 1;
            }
            else
            {
                if (xian_dj_retractable_platform_stand_linear_actuator_stand_cmd==0 && xian_dj_retractable_platform_stand_linear_actuator_sit_cmd==1)
                {
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_enble", 1);
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_move", 0);
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_stand_linear_actuator_enble = 1;
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_stand_linear_actuator_move = 0;
                }
                else
                {
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_enble", 0);
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_move", 0);
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_stand_linear_actuator_enble = 0;
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_stand_linear_actuator_move = 0;
                }
            }

            // 控制一级伸缩杆运动状态
            if (xian_dj_retractable_platform_first_linear_actuator_up_cmd==1 && xian_dj_retractable_platform_first_linear_actuator_down_cmd==0)
            {
                // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_enble", 1);
                // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_move", 1);
                xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_first_linear_actuator_enble = 1;
                xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_first_linear_actuator_move = 1;
            }
            else
            {
                if (xian_dj_retractable_platform_first_linear_actuator_up_cmd==0 && xian_dj_retractable_platform_first_linear_actuator_down_cmd==1)
                {
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_enble", 1);
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_move", 0);
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_first_linear_actuator_enble = 1;
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_first_linear_actuator_move = 0;
                }
                else
                {
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_enble", 0);
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_move", 0);
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_first_linear_actuator_enble = 0;
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_first_linear_actuator_move = 0;
                }
            }

            // 控制二级伸缩杆运动状态
            if (xian_dj_retractable_platform_second_linear_actuator_up_cmd==1 && xian_dj_retractable_platform_second_linear_actuator_down_cmd==0)
            {
                // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_enble", 1);
                // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_move", 1);
                xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_second_linear_actuator_enble = 1;
                xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_second_linear_actuator_move = 1;
            }
            else
            {
                if (xian_dj_retractable_platform_second_linear_actuator_up_cmd==0 && xian_dj_retractable_platform_second_linear_actuator_down_cmd==1)
                {
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_enble", 1);
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_move", 0);
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_second_linear_actuator_enble = 1;
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_second_linear_actuator_move = 0;
                }
                else
                {
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_enble", 0);
                    // ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_move", 0);
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_second_linear_actuator_enble = 0;
                    xian_dj_retractable_platform_base_electric_pub_msg.xian_dj_retractable_platform_second_linear_actuator_move = 0;
                }
            }
            xian_dj_retractable_platform_base_electric_pub.publish(xian_dj_retractable_platform_base_electric_pub_msg);
        }

        
    private:
        int counter = 0;
        int xian_dj_retractable_platform_control_heart_beat = 0;
        ros::Subscriber xian_dj_retractable_platform_control_sub; // 订阅control相关的消息
        ros::Publisher xian_dj_retractable_platform_control_state_pub; // 发布心跳topic
        ros::Publisher xian_dj_retractable_platform_base_electric_pub; // 发布伸缩的基础电控
        xian_dj_retractable_platform_control_pkg::xian_dj_retractable_platform_base_electric xian_dj_retractable_platform_base_electric_pub_msg;
        std_msgs::UInt16 heart_beat;

        int xian_dj_retractable_platform_stand_linear_actuator_enble = 0;
        int xian_dj_retractable_platform_stand_linear_actuator_move = 0;
        int xian_dj_retractable_platform_first_linear_actuator_enble = 0;
        int xian_dj_retractable_platform_first_linear_actuator_move = 0;
        int xian_dj_retractable_platform_second_linear_actuator_enble = 0;
        int xian_dj_retractable_platform_second_linear_actuator_move = 0;
        int xian_dj_retractable_platform_stand_linear_actuator_stand_cmd = 0;
        int xian_dj_retractable_platform_stand_linear_actuator_sit_cmd = 0;
        int xian_dj_retractable_platform_first_linear_actuator_up_cmd = 0;
        int xian_dj_retractable_platform_first_linear_actuator_down_cmd = 0;
        int xian_dj_retractable_platform_second_linear_actuator_up_cmd = 0;
        int xian_dj_retractable_platform_second_linear_actuator_down_cmd = 0;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_retractable_platform_control");
    XianDjRetractablePlatformControl xian_dj_retractable_platform_control;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_retractable_platform_control.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(1), &XianDjRetractablePlatformControl::m_timer_heart_beat_func, &xian_dj_retractable_platform_control);

    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}