#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include "xian_dj_car_chassis_control_pkg/xian_dj_car_chassis_back_end.h"

class XianDjCarBackEnd
{
    public:
        XianDjCarBackEnd()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            pub = nh.advertise<xian_dj_car_chassis_control_pkg::xian_dj_car_chassis_back_end>("xian_dj_car_chassis_back_end_msg", 1);
        }

        // ros::WallTimer m_timer_heart_beat;
        ros::WallTimer m_timer_control;

        // void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        // {
        //     ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_back_end_heart_beat", xian_dj_car_chassis_back_end_heart_beat); 
        //     std::cout << "xian_dj_car_chassis_back_end_heart_beat: " << xian_dj_car_chassis_back_end_heart_beat << std::endl;
        //     counter = counter > 1000 ? 0 : (counter + 1);
        //     ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_back_end_heart_beat", counter);  // 自行替换
        // }

        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_back_end_heart_beat", xian_dj_car_chassis_back_end_heart_beat); 
            std::cout << "xian_dj_car_chassis_back_end_heart_beat: " << xian_dj_car_chassis_back_end_heart_beat << std::endl;
            counter = counter > 1000 ? 0 : (counter + 1);
            ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_back_end_heart_beat", counter);  // 自行替换

            ros::param::get("/xian_dj_car_chassis_params_server/input_velocity_cmd", input_velocity_cmd); 
            ros::param::get("/xian_dj_car_chassis_params_server/input_theta_cmd", input_theta_cmd); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_params_server_heart_beat", xian_dj_car_chassis_params_server_heart_beat); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_diff_driver_control_heart_beat", xian_dj_car_chassis_diff_driver_control_heart_beat); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_left_heart_beat", xian_dj_car_chassis_base_electric_left_heart_beat); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_right_heart_beat", xian_dj_car_chassis_base_electric_right_heart_beat); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_error_monitor_heart_beat", xian_dj_car_chassis_error_monitor_heart_beat); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_back_end_heart_beat", xian_dj_car_chassis_back_end_heart_beat); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_left_wheel_write_velocity", xian_dj_car_chassis_left_wheel_write_velocity); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_left_wheel_read_velocity", xian_dj_car_chassis_left_wheel_read_velocity); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_right_wheel_write_velocity", xian_dj_car_chassis_right_wheel_write_velocity); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_right_wheel_read_velocity", xian_dj_car_chassis_right_wheel_read_velocity); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_delta_theta1c", xian_dj_car_chassis_delta_theta1c); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_delta_theta2c", xian_dj_car_chassis_delta_theta2c); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_diff_driver_control_error", xian_dj_car_chassis_diff_driver_control_error); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_params_server_error", xian_dj_car_chassis_params_server_error); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_error", xian_dj_car_chassis_base_electric_error); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_back_end_error", xian_dj_car_chassis_back_end_error); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_error_code", xian_dj_car_chassis_error_code); 

            web_show.input_velocity_cmd = input_velocity_cmd;
            web_show.input_theta_cmd =input_theta_cmd;
            web_show.xian_dj_car_chassis_params_server_heart_beat = xian_dj_car_chassis_params_server_heart_beat;
            web_show.xian_dj_car_chassis_diff_driver_control_heart_beat = xian_dj_car_chassis_diff_driver_control_heart_beat;
            web_show.xian_dj_car_chassis_base_electric_left_heart_beat = xian_dj_car_chassis_base_electric_left_heart_beat;
            web_show.xian_dj_car_chassis_base_electric_right_heart_beat = xian_dj_car_chassis_base_electric_right_heart_beat;
            web_show.xian_dj_car_chassis_error_monitor_heart_beat = xian_dj_car_chassis_error_monitor_heart_beat;
            web_show.xian_dj_car_chassis_back_end_heart_beat = xian_dj_car_chassis_back_end_heart_beat;
            web_show.xian_dj_car_chassis_left_wheel_write_velocity = xian_dj_car_chassis_left_wheel_write_velocity;
            web_show.xian_dj_car_chassis_left_wheel_read_velocity = xian_dj_car_chassis_left_wheel_read_velocity;
            web_show.xian_dj_car_chassis_right_wheel_write_velocity = xian_dj_car_chassis_right_wheel_write_velocity;
            web_show.xian_dj_car_chassis_right_wheel_read_velocity = xian_dj_car_chassis_right_wheel_read_velocity;
            web_show.xian_dj_car_chassis_delta_theta1c = xian_dj_car_chassis_delta_theta1c;
            web_show.xian_dj_car_chassis_delta_theta2c = xian_dj_car_chassis_delta_theta2c;
            web_show.xian_dj_car_chassis_params_server_error = xian_dj_car_chassis_params_server_error;
            web_show.xian_dj_car_chassis_diff_driver_control_error = xian_dj_car_chassis_diff_driver_control_error;
            web_show.xian_dj_car_chassis_base_electric_error = xian_dj_car_chassis_base_electric_error;
            web_show.xian_dj_car_chassis_back_end_error = xian_dj_car_chassis_back_end_error;
            web_show.xian_dj_car_chassis_error_code = xian_dj_car_chassis_error_code;

            pub.publish(web_show);
             
        }

    private:
        int counter = 0;
        int xian_dj_car_chassis_back_end_heart_beat = 0;
        ros::Publisher pub; // 声明发布者
        xian_dj_car_chassis_control_pkg::xian_dj_car_chassis_back_end web_show; // 声明要发布的消息类型

        double input_velocity_cmd;
        double input_theta_cmd;
        int xian_dj_car_chassis_params_server_heart_beat;
        int xian_dj_car_chassis_diff_driver_control_heart_beat;
        int xian_dj_car_chassis_base_electric_left_heart_beat;
        int xian_dj_car_chassis_base_electric_right_heart_beat;
        int xian_dj_car_chassis_error_monitor_heart_beat;
        double xian_dj_car_chassis_left_wheel_write_velocity;
        double xian_dj_car_chassis_left_wheel_read_velocity;
        double xian_dj_car_chassis_right_wheel_write_velocity;
        double xian_dj_car_chassis_right_wheel_read_velocity;
        double xian_dj_car_chassis_delta_theta1c;
        double xian_dj_car_chassis_delta_theta2c;
        int xian_dj_car_chassis_params_server_error;
        int xian_dj_car_chassis_diff_driver_control_error;
        int xian_dj_car_chassis_base_electric_error;
        int xian_dj_car_chassis_back_end_error;
        int xian_dj_car_chassis_error_code;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_car_chassis_back_end");
    XianDjCarBackEnd xian_dj_car_chassis_back_end;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    // xian_dj_car_chassis_back_end.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(2.0), &XianDjCarBackEnd::m_timer_heart_beat_func, &xian_dj_car_chassis_back_end);
    xian_dj_car_chassis_back_end.m_timer_control = nh_2.createWallTimer(ros::WallDuration(1), &XianDjCarBackEnd::m_timer_control_func, &xian_dj_car_chassis_back_end);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}