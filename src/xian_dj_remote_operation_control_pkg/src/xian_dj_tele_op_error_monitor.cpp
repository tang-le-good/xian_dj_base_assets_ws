#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include "xian_dj_remote_operation_control_pkg/xian_dj_tele_op_controller_server.h"
#include <std_msgs/String.h>

class XianDjTeleOpErrorMonitor
{
    public:
        XianDjTeleOpErrorMonitor()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            xian_dj_tele_op_controller_server_state_sub = nh.subscribe<xian_dj_remote_operation_control_pkg::xian_dj_tele_op_controller_server>("xian_dj_tele_op_controller_server_msg", 10, &XianDjTeleOpErrorMonitor::tele_op_controller_server_callback, this);
            xian_dj_tele_op_error_monitor_pub = nh.advertise<std_msgs::String>("xian_dj_tele_op_error_monitor_msg", 1);
        }

        ros::WallTimer m_timer_control;

        void tele_op_controller_server_callback(const xian_dj_remote_operation_control_pkg::xian_dj_tele_op_controller_server::ConstPtr &data)
        {
            xian_dj_tele_op_controller_client_tcp_heart_beat = data->xian_dj_tele_op_controller_client_tcp_heart_beat;   
            printf("xian_dj_tele_op_controller_client_tcp_heart_beat: %d \n", xian_dj_tele_op_controller_client_tcp_heart_beat);  
        }

        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            
            // 监控 xian_dj_tele_op_controller_client_tcp_heart_beat
            xian_dj_tele_op_controller_client_tcp_heart_beat_pre = xian_dj_tele_op_controller_client_tcp_heart_beat_cur;
            xian_dj_tele_op_controller_client_tcp_heart_beat_cur = xian_dj_tele_op_controller_client_tcp_heart_beat;
            if(xian_dj_tele_op_controller_client_tcp_heart_beat_pre == xian_dj_tele_op_controller_client_tcp_heart_beat_cur)
            {
                xian_dj_tele_op_controller_client_tcp_restart_flag ++;
                // ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_tcp_error", 1); 
            }
            else
            {
                xian_dj_tele_op_controller_client_tcp_restart_flag = 0;
                // ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_tcp_error", 0);
            }
            if(xian_dj_tele_op_controller_client_tcp_restart_flag > time_out)
            {
                xian_dj_tele_op_controller_client_tcp_error = 1;
            }
            else
            {
                xian_dj_tele_op_controller_client_tcp_error = 0;
            }

            error_sum = xian_dj_tele_op_controller_client_tcp_error ;
            xian_dj_tele_op_error_code = error_sum + 500;
            std::string pub_sting = std::to_string(xian_dj_tele_op_error_code) 
                                    + "   " + std::to_string(xian_dj_tele_op_controller_client_tcp_error);
            pub_msg.data = pub_sting.c_str();

            xian_dj_tele_op_error_monitor_pub.publish(pub_msg);

        }
    private:
        ros::Subscriber xian_dj_tele_op_controller_server_state_sub;
        ros::Publisher xian_dj_tele_op_error_monitor_pub;
        std_msgs::String pub_msg;
        int xian_dj_tele_op_controller_client_tcp_heart_beat = 0;

        int xian_dj_tele_op_controller_client_tcp_heart_beat_cur = 0;
        int xian_dj_tele_op_controller_client_tcp_heart_beat_pre = 0;

        int xian_dj_tele_op_controller_client_tcp_restart_flag = 0;

        int xian_dj_tele_op_controller_client_tcp_error = 0;

        int time_out = 5;

        int error_sum = 0;
        int xian_dj_tele_op_error_code = 0;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_tele_op_error_monitor");
    XianDjTeleOpErrorMonitor xian_dj_tele_op_error_monitor;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_tele_op_error_monitor.m_timer_control = nh_2.createWallTimer(ros::WallDuration(1), &XianDjTeleOpErrorMonitor::m_timer_control_func, &xian_dj_tele_op_error_monitor);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}