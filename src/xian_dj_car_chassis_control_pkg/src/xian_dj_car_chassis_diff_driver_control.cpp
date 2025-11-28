#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include "xian_dj_car_chassis_control_pkg/xian_dj_car_chassis_diff_driver_control.h"

class XianDjCarChassisDiffDriverControl
{
    public:
        XianDjCarChassisDiffDriverControl()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            xian_dj_car_chassis_diff_driver_control_sub = nh.subscribe<xian_dj_car_chassis_control_pkg::xian_dj_car_chassis_diff_driver_control>("xian_dj_car_chassis_diff_driver_control_msg", 10, &XianDjCarChassisDiffDriverControl::controller_callback, this);

            xian_dj_car_chassis_diff_driver_control_state_pub = nh.advertise<std_msgs::Int32>("xian_dj_car_chassis_diff_driver_control_state_msg", 1);
            xian_dj_car_chassis_diff_driver_control_left_write_velocity_pub = nh.advertise<std_msgs::Int32>("xian_dj_car_chassis_diff_driver_control_left_write_velocity_msg", 1); 
            xian_dj_car_chassis_diff_driver_control_right_write_velocity_pub = nh.advertise<std_msgs::Int32>("xian_dj_car_chassis_diff_driver_control_right_write_velocity_msg", 1); 
        }

        ros::WallTimer m_timer_heart_beat;
        // ros::WallTimer m_timer_control;

        void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        {
            xian_dj_car_chassis_diff_driver_control_heart_beat = xian_dj_car_chassis_diff_driver_control_heart_beat > 1000 ? 0 : (xian_dj_car_chassis_diff_driver_control_heart_beat + 1);
            std::cout << "xian_dj_car_chassis_diff_driver_control_heart_beat: " << xian_dj_car_chassis_diff_driver_control_heart_beat << std::endl;
            heart_beat.data = xian_dj_car_chassis_diff_driver_control_heart_beat;
            xian_dj_car_chassis_diff_driver_control_state_pub.publish(heart_beat);
        }

        void controller_callback(const xian_dj_car_chassis_control_pkg::xian_dj_car_chassis_diff_driver_control::ConstPtr &data)
        {
            input_velocity = data->input_velocity_cmd;
            input_theta = data->input_theta_cmd;
            delta_v1c = input_velocity;
            delta_v2c = input_velocity;
            delta_theta_1 = delta_v1c - input_theta;
            delta_theta_2 = delta_v2c + input_theta;
            xian_dj_car_chassis_delta_theta1c.data = delta_theta_1;
            xian_dj_car_chassis_delta_theta2c.data = delta_theta_2;

            xian_dj_car_chassis_diff_driver_control_left_write_velocity_pub.publish(xian_dj_car_chassis_delta_theta1c);
            xian_dj_car_chassis_diff_driver_control_right_write_velocity_pub.publish(xian_dj_car_chassis_delta_theta2c);
            
        }


    private:
        // 心跳
        int xian_dj_car_chassis_diff_driver_control_heart_beat = 0;

        ros::Subscriber xian_dj_car_chassis_diff_driver_control_sub; // 订阅control相关的消息
        ros::Publisher xian_dj_car_chassis_diff_driver_control_state_pub; 
        ros::Publisher xian_dj_car_chassis_diff_driver_control_left_write_velocity_pub; 
        ros::Publisher xian_dj_car_chassis_diff_driver_control_right_write_velocity_pub; 
        std_msgs::Int32 heart_beat;
        std_msgs::Int32 xian_dj_car_chassis_delta_theta1c;
        std_msgs::Int32 xian_dj_car_chassis_delta_theta2c;

        // 输入量
        double input_velocity_cmd = 0;
        double input_theta_cmd = 0;
        // 中间变量
        double input_velocity = 0;
        double input_theta = 0;
        double delta_v1c = 0;
        double delta_v2c = 0;
        double delta_theta_1 = 0;
        double delta_theta_2 = 0;
        // //输出量
        // double xian_dj_car_chassis_delta_theta1c = 0;
        // double xian_dj_car_chassis_delta_theta2c = 0;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_car_chassis_diff_driver_control");
    XianDjCarChassisDiffDriverControl xian_dj_car_chassis_diff_driver_control;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_car_chassis_diff_driver_control.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(1), &XianDjCarChassisDiffDriverControl::m_timer_heart_beat_func, &xian_dj_car_chassis_diff_driver_control);
    
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}