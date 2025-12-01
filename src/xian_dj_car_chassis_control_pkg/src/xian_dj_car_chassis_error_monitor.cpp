#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include <std_msgs/String.h>
#include <std_msgs/UInt16.h>
#include <std_msgs/Int32.h>

class XianDjCarChassisErrorMonitor
{
    public:
        XianDjCarChassisErrorMonitor()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            xian_dj_car_chassis_diff_driver_control_state_sub = nh.subscribe<std_msgs::Int32>("xian_dj_car_chassis_diff_driver_control_state_msg", 10, &XianDjCarChassisErrorMonitor::diff_driver_control_state_callback, this);
            xian_dj_car_chassis_base_electric_left_state_sub = nh.subscribe<std_msgs::String>("xian_dj_car_chassis_base_electric_left_state_msg", 10, &XianDjCarChassisErrorMonitor::base_electric_left_state_callback, this);
            xian_dj_car_chassis_base_electric_right_state_sub = nh.subscribe<std_msgs::String>("xian_dj_car_chassis_base_electric_right_state_msg", 10, &XianDjCarChassisErrorMonitor::base_electric_right_state_callback, this);
            
            xian_dj_car_chassis_error_monitor_pub = nh.advertise<std_msgs::String>("xian_dj_car_chassis_error_monitor_msg", 1);
        }

        ros::WallTimer m_timer_control;

        void diff_driver_control_state_callback(const std_msgs::Int32::ConstPtr &data)
        {
            xian_dj_car_chassis_diff_driver_control_heart_beat = data->data;   
            printf("xian_dj_car_chassis_diff_driver_control_heart_beat: %d \n", xian_dj_car_chassis_diff_driver_control_heart_beat);  
        }

        void base_electric_left_state_callback(const std_msgs::String::ConstPtr &data)
        {
            std::string received_str = data->data;
            int velocicy;
            std::istringstream iss(received_str);
            iss >> velocicy >> xian_dj_car_chassis_base_electric_heart_beat_left;

            // 转换为整型 
            printf("xian_dj_car_chassis_base_electric_heart_beat_left: %d \n", xian_dj_car_chassis_base_electric_heart_beat_left);

        }

        void base_electric_right_state_callback(const std_msgs::String::ConstPtr &data)
        {
            std::string received_str = data->data;
            int velocicy;
            std::istringstream iss(received_str);
            iss >> velocicy >> xian_dj_car_chassis_base_electric_heart_beat_right;

            // 转换为整型 
            printf("xian_dj_car_chassis_base_electric_heart_beat_right: %d \n", xian_dj_car_chassis_base_electric_heart_beat_right);   
        }


        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            
            // 监控 xian_dj_car_chassis_diff_driver_control_heart_beat
            xian_dj_car_chassis_diff_driver_control_heart_beat_pre = xian_dj_car_chassis_diff_driver_control_heart_beat_cur;
            xian_dj_car_chassis_diff_driver_control_heart_beat_cur = xian_dj_car_chassis_diff_driver_control_heart_beat;
            if(xian_dj_car_chassis_diff_driver_control_heart_beat_pre == xian_dj_car_chassis_diff_driver_control_heart_beat_cur)
            {
                xian_dj_car_chassis_diff_driver_control_node_restart_flag++;
            }
            else
            {
                xian_dj_car_chassis_diff_driver_control_node_restart_flag = 0;
            }

            if(xian_dj_car_chassis_diff_driver_control_node_restart_flag > time_out)
            {
                xian_dj_car_chassis_diff_driver_control_error = 1;
            }
            else
            {
                xian_dj_car_chassis_diff_driver_control_error = 0;
            }


            // 监控 xian_dj_car_chassis_base_electric_heart_beat_left
            xian_dj_car_chassis_base_electric_heart_beat_left_pre = xian_dj_car_chassis_base_electric_heart_beat_left_beat_cur;
            xian_dj_car_chassis_base_electric_heart_beat_left_beat_cur = xian_dj_car_chassis_base_electric_heart_beat_left;
            if(xian_dj_car_chassis_base_electric_heart_beat_left_pre == xian_dj_car_chassis_base_electric_heart_beat_left_beat_cur)
            {
                xian_dj_car_chassis_base_electric_left_node_restart_flag++;
            }
            else
            {
                xian_dj_car_chassis_base_electric_left_node_restart_flag = 0;
            }
            if(xian_dj_car_chassis_base_electric_left_node_restart_flag > time_out)
            {
                xian_dj_car_chassis_base_electric_left_error = 1;
            }
            else
            {
                xian_dj_car_chassis_base_electric_left_error = 0;
            }
            

            // 监控 xian_dj_car_chassis_base_electric_heart_beat_right
            xian_dj_car_chassis_base_electric_heart_beat_right_pre = xian_dj_car_chassis_base_electric_heart_beat_right_beat_cur;
            xian_dj_car_chassis_base_electric_heart_beat_right_beat_cur = xian_dj_car_chassis_base_electric_heart_beat_right;
            if(xian_dj_car_chassis_base_electric_heart_beat_right_pre == xian_dj_car_chassis_base_electric_heart_beat_right_beat_cur)
            {
                xian_dj_car_chassis_base_electric_right_node_restart_flag++;
            }
            else
            {
                xian_dj_car_chassis_base_electric_right_node_restart_flag = 0;
            }
            if(xian_dj_car_chassis_base_electric_right_node_restart_flag > time_out)
            {
                xian_dj_car_chassis_base_electric_right_error = 1;
            }
            else
            {
                xian_dj_car_chassis_base_electric_right_error = 0;
            }
        
            error_sum = xian_dj_car_chassis_diff_driver_control_error + xian_dj_car_chassis_base_electric_left_error + xian_dj_car_chassis_base_electric_right_error ;
            xian_dj_car_chassis_error_code = error_sum + 100;

            std::string pub_sting = std::to_string(xian_dj_car_chassis_error_code) 
                                    + "   " + std::to_string(xian_dj_car_chassis_diff_driver_control_error)
                                    + "   " + std::to_string(xian_dj_car_chassis_base_electric_left_error)
                                    + "   " + std::to_string(xian_dj_car_chassis_base_electric_right_error);
            pub_msg.data = pub_sting.c_str();
            xian_dj_car_chassis_error_monitor_pub.publish(pub_msg);
        }
    private:
        ros::Subscriber xian_dj_car_chassis_diff_driver_control_state_sub;
        ros::Subscriber xian_dj_car_chassis_base_electric_left_state_sub;
        ros::Subscriber xian_dj_car_chassis_base_electric_right_state_sub; 

        ros::Publisher xian_dj_car_chassis_error_monitor_pub; 
        std_msgs::String pub_msg;

        std::string command_kill_current_node = "";
        std::string command_restart_current_node = "";


        int xian_dj_car_chassis_error_monitor_heart_beat = 0;

        int xian_dj_car_chassis_diff_driver_control_heart_beat = 0;
        int xian_dj_car_chassis_base_electric_heart_beat_left = 0;
        int xian_dj_car_chassis_base_electric_heart_beat_right = 0;

        int xian_dj_car_chassis_diff_driver_control_heart_beat_cur = 0;
        int xian_dj_car_chassis_diff_driver_control_heart_beat_pre = 0;
        int xian_dj_car_chassis_base_electric_heart_beat_left_beat_cur = 0;
        int xian_dj_car_chassis_base_electric_heart_beat_left_pre = 0;
        int xian_dj_car_chassis_base_electric_heart_beat_right_beat_cur = 0;
        int xian_dj_car_chassis_base_electric_heart_beat_right_pre = 0;

        int xian_dj_car_chassis_diff_driver_control_error = 0;
        int xian_dj_car_chassis_base_electric_left_error = 0;
        int xian_dj_car_chassis_base_electric_right_error = 0;

        int xian_dj_car_chassis_diff_driver_control_node_restart_flag = 0;
        int xian_dj_car_chassis_base_electric_left_node_restart_flag = 0;
        int xian_dj_car_chassis_base_electric_right_node_restart_flag = 0;
        int time_out = 5;


        int error_sum = 0;
        int xian_dj_car_chassis_error_code = 0;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_car_chassis_error_monitor");
    XianDjCarChassisErrorMonitor xian_dj_car_chassis_error_monitor;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_car_chassis_error_monitor.m_timer_control = nh_2.createWallTimer(ros::WallDuration(1), &XianDjCarChassisErrorMonitor::m_timer_control_func, &xian_dj_car_chassis_error_monitor);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}