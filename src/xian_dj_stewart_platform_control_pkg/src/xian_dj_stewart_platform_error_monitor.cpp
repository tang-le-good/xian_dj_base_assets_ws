#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/String.h>

class XianDjStewartPlatformErrorMonitor
{
    public:
        XianDjStewartPlatformErrorMonitor()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            xian_dj_stewart_platform_control_state_sub = nh.subscribe<std_msgs::UInt16>("xian_dj_stewart_platform_control_state_msg", 10, &XianDjStewartPlatformErrorMonitor::stewart_platform_control_state_callback, this);
            xian_dj_stewart_platform_base_electric_state_sub = nh.subscribe<std_msgs::UInt16>("xian_dj_stewart_platform_base_electric_state_msg", 10, &XianDjStewartPlatformErrorMonitor::stewart_platform_base_electric_state_callback, this);
            xian_dj_stewart_platform_manual_controller_state_sub = nh.subscribe<std_msgs::UInt16>("xian_dj_stewart_platform_manual_controller_state_msg", 10, &XianDjStewartPlatformErrorMonitor::stewart_platform_manual_controller_state_callback, this);

            xian_dj_stewart_platform_error_monitor_pub = nh.advertise<std_msgs::String>("xian_dj_stewart_platform_error_monitor_msg", 1);
        }

        ros::WallTimer m_timer_control;

        void stewart_platform_control_state_callback(const std_msgs::UInt16::ConstPtr &data)
        {
            xian_dj_stewart_platform_control_heart_beat = data->data;
        }

        void stewart_platform_base_electric_state_callback(const std_msgs::UInt16::ConstPtr &data)
        {
            xian_dj_stewart_platform_base_electric_heart_beat = data->data;
        }

        void stewart_platform_manual_controller_state_callback(const std_msgs::UInt16::ConstPtr &data)
        {
            xian_dj_stewart_platform_manual_controller_heart_beat = data->data;
        }


        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            
            // 监控 xian_dj_stewart_platform_control_heart_beat
            xian_dj_stewart_platform_control_heart_beat_pre = xian_dj_stewart_platform_control_heart_beat_cur;
            xian_dj_stewart_platform_control_heart_beat_cur = xian_dj_stewart_platform_control_heart_beat;
            if(xian_dj_stewart_platform_control_heart_beat_pre == xian_dj_stewart_platform_control_heart_beat_cur)
            {
                xian_dj_stewart_platform_control_node_restart_flag ++;
            }
            else
            {
                xian_dj_stewart_platform_control_node_restart_flag = 0;
            }
            if(xian_dj_stewart_platform_control_node_restart_flag > time_out)
            {
                xian_dj_stewart_platform_control_error = 1;
            }
            else
            {
                xian_dj_stewart_platform_control_error = 0;
            }

            // 监控 xian_dj_stewart_platform_base_electric_heart_beat
            xian_dj_stewart_platform_base_electric_heart_beat_pre = xian_dj_stewart_platform_base_electric_heart_beat_cur;
            xian_dj_stewart_platform_base_electric_heart_beat_cur = xian_dj_stewart_platform_base_electric_heart_beat;
            if(xian_dj_stewart_platform_base_electric_heart_beat_pre == xian_dj_stewart_platform_base_electric_heart_beat_cur)
            {
                xian_dj_stewart_platform_base_electric_node_restart_flag ++;
            }
            else
            {
                xian_dj_stewart_platform_base_electric_node_restart_flag = 0;
            }
            if(xian_dj_stewart_platform_base_electric_node_restart_flag > time_out)
            {
                xian_dj_stewart_platform_base_electric_error = 1;
            }
            else
            {
                xian_dj_stewart_platform_base_electric_error = 0;
            }

            // 监控 xian_dj_stewart_platform_manual_controller_heart_beat
            xian_dj_stewart_platform_manual_controller_heart_beat_pre = xian_dj_stewart_platform_manual_controller_heart_beat_cur;
            xian_dj_stewart_platform_manual_controller_heart_beat_cur = xian_dj_stewart_platform_manual_controller_heart_beat;
            if(xian_dj_stewart_platform_manual_controller_heart_beat_pre == xian_dj_stewart_platform_manual_controller_heart_beat_cur)
            {
                xian_dj_stewart_platform_manual_controller_node_restart_flag ++;
            }
            else
            {
                xian_dj_stewart_platform_manual_controller_node_restart_flag = 0;
            }
            if(xian_dj_stewart_platform_manual_controller_node_restart_flag > time_out)
            {
                xian_dj_stewart_platform_manual_controller_error = 1;
            }
            else
            {
                xian_dj_stewart_platform_manual_controller_error = 0;
            }
            error_sum = xian_dj_stewart_platform_control_error + xian_dj_stewart_platform_base_electric_error + xian_dj_stewart_platform_manual_controller_error ;
            xian_dj_stewart_platform_error_code = error_sum + 200;
            std::string pub_sting = std::to_string(xian_dj_stewart_platform_error_code) 
                                    + "   " + std::to_string(xian_dj_stewart_platform_control_error)
                                    + "   " + std::to_string(xian_dj_stewart_platform_base_electric_error)
                                    + "   " + std::to_string(xian_dj_stewart_platform_manual_controller_error);
            pub_msg.data = pub_sting.c_str();
            xian_dj_stewart_platform_error_monitor_pub.publish(pub_msg);

        }
    private:
        ros::Subscriber xian_dj_stewart_platform_control_state_sub;
        ros::Subscriber xian_dj_stewart_platform_base_electric_state_sub;
        ros::Subscriber xian_dj_stewart_platform_manual_controller_state_sub;
        ros::Publisher xian_dj_stewart_platform_error_monitor_pub;
        std_msgs::String pub_msg;
        int xian_dj_stewart_platform_control_heart_beat = 0;
        int xian_dj_stewart_platform_base_electric_heart_beat = 0;
        int xian_dj_stewart_platform_manual_controller_heart_beat = 0;

        int xian_dj_stewart_platform_control_heart_beat_cur = 0;
        int xian_dj_stewart_platform_control_heart_beat_pre = 0;
        int xian_dj_stewart_platform_base_electric_heart_beat_cur = 0;
        int xian_dj_stewart_platform_base_electric_heart_beat_pre = 0;
        int xian_dj_stewart_platform_manual_controller_heart_beat_cur = 0;
        int xian_dj_stewart_platform_manual_controller_heart_beat_pre = 0;

        int xian_dj_stewart_platform_control_node_restart_flag = 0;
        int xian_dj_stewart_platform_base_electric_node_restart_flag = 0;
        int xian_dj_stewart_platform_manual_controller_node_restart_flag = 0;

        int xian_dj_stewart_platform_control_error = 0;
        int xian_dj_stewart_platform_base_electric_error = 0;
        int xian_dj_stewart_platform_manual_controller_error = 0;

        int time_out = 5;
        int error_sum = 0;
        int xian_dj_stewart_platform_error_code = 0;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_stewart_platform_error_monitor");
    XianDjStewartPlatformErrorMonitor xian_dj_stewart_platform_error_monitor;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_stewart_platform_error_monitor.m_timer_control = nh_2.createWallTimer(ros::WallDuration(1), &XianDjStewartPlatformErrorMonitor::m_timer_control_func, &xian_dj_stewart_platform_error_monitor);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}