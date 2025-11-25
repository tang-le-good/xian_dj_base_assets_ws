#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include "xian_dj_stewart_platform_control_pkg/xian_dj_stewart_platform.h"

class XianDjStewartPlatformBackEnd
{
    public:
        XianDjStewartPlatformBackEnd()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            pub = nh.advertise<xian_dj_stewart_platform_control_pkg::xian_dj_stewart_platform>("xian_dj_stewart_platform_back_end_msg", 1);
        }

        // ros::WallTimer m_timer_heart_beat;
        ros::WallTimer m_timer_control;

        // void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        // {
        //     ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_back_end_heart_beat", xian_dj_stewart_platform_back_end_heart_beat); 
        //     std::cout << "xian_dj_stewart_platform_back_end_heart_beat: " << xian_dj_stewart_platform_back_end_heart_beat << std::endl;
        //     counter = counter > 1000 ? 0 : (counter + 1);
        //     ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_back_end_heart_beat", counter);  // 自行替换
        // }

        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_back_end_heart_beat", xian_dj_stewart_platform_back_end_heart_beat); 
            std::cout << "xian_dj_stewart_platform_back_end_heart_beat: " << xian_dj_stewart_platform_back_end_heart_beat << std::endl;
            counter = counter > 1000 ? 0 : (counter + 1);
            ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_back_end_heart_beat", counter);  // 自行替换

            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_params_server_heart_beat", xian_dj_stewart_platform_params_server_heart_beat);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_control_heart_beat", xian_dj_stewart_platform_control_heart_beat);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_base_electric_heart_beat", xian_dj_stewart_platform_base_electric_heart_beat);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_error_monitor_heart_beat", xian_dj_stewart_platform_error_monitor_heart_beat);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_manual_controller_heart_beat", xian_dj_stewart_platform_manual_controller_heart_beat);

            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_alpha_cmd", xian_dj_stewart_platform_input_alpha_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_beta_cmd", xian_dj_stewart_platform_input_beta_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_gamma_cmd", xian_dj_stewart_platform_input_gamma_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_x_cmd", xian_dj_stewart_platform_input_x_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_y_cmd", xian_dj_stewart_platform_input_y_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_z_cmd", xian_dj_stewart_platform_input_z_cmd);

            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_alpha_positive_cmd", xian_dj_stewart_platform_input_alpha_positive_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_alpha_negative_cmd", xian_dj_stewart_platform_input_alpha_negative_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_beta_positive_cmd", xian_dj_stewart_platform_input_beta_positive_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_beta_negative_cmd", xian_dj_stewart_platform_input_beta_negative_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_gamma_positive_cmd", xian_dj_stewart_platform_input_gamma_positive_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_gamma_negative_cmd", xian_dj_stewart_platform_input_gamma_negative_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_x_positive_cmd", xian_dj_stewart_platform_input_x_positive_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_x_negative_cmd", xian_dj_stewart_platform_input_x_negative_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_y_positive_cmd", xian_dj_stewart_platform_input_y_positive_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_y_negative_cmd", xian_dj_stewart_platform_input_y_negative_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_z_positive_cmd", xian_dj_stewart_platform_input_z_positive_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_z_negative_cmd", xian_dj_stewart_platform_input_z_negative_cmd);

            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_arm1_cmd", xian_dj_stewart_platform_arm1_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_arm2_cmd", xian_dj_stewart_platform_arm2_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_arm3_cmd", xian_dj_stewart_platform_arm3_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_arm4_cmd", xian_dj_stewart_platform_arm4_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_arm5_cmd", xian_dj_stewart_platform_arm5_cmd);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_arm6_cmd", xian_dj_stewart_platform_arm6_cmd);

            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_params_server_error", xian_dj_stewart_platform_params_server_error);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_control_error", xian_dj_stewart_platform_control_error);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_base_electric_error", xian_dj_stewart_platform_base_electric_error);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_back_end_error", xian_dj_stewart_platform_back_end_error);
            ros::param::get("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_error_code", xian_dj_stewart_platform_error_code);

            web_show.xian_dj_stewart_platform_params_server_heart_beat = xian_dj_stewart_platform_params_server_heart_beat;
            web_show.xian_dj_stewart_platform_control_heart_beat = xian_dj_stewart_platform_control_heart_beat;
            web_show.xian_dj_stewart_platform_base_electric_heart_beat = xian_dj_stewart_platform_base_electric_heart_beat;
            web_show.xian_dj_stewart_platform_error_monitor_heart_beat = xian_dj_stewart_platform_error_monitor_heart_beat;
            web_show.xian_dj_stewart_platform_manual_controller_heart_beat = xian_dj_stewart_platform_manual_controller_heart_beat;

            web_show.xian_dj_stewart_platform_input_alpha_cmd = xian_dj_stewart_platform_input_alpha_cmd;
            web_show.xian_dj_stewart_platform_input_beta_cmd = xian_dj_stewart_platform_input_beta_cmd;
            web_show.xian_dj_stewart_platform_input_gamma_cmd = xian_dj_stewart_platform_input_gamma_cmd;
            web_show.xian_dj_stewart_platform_input_x_cmd = xian_dj_stewart_platform_input_x_cmd;
            web_show.xian_dj_stewart_platform_input_y_cmd = xian_dj_stewart_platform_input_y_cmd;
            web_show.xian_dj_stewart_platform_input_z_cmd = xian_dj_stewart_platform_input_z_cmd;

            web_show.xian_dj_stewart_platform_input_alpha_positive_cmd = xian_dj_stewart_platform_input_alpha_positive_cmd;
            web_show.xian_dj_stewart_platform_input_alpha_negative_cmd = xian_dj_stewart_platform_input_alpha_negative_cmd;
            web_show.xian_dj_stewart_platform_input_beta_positive_cmd = xian_dj_stewart_platform_input_beta_positive_cmd;
            web_show.xian_dj_stewart_platform_input_beta_negative_cmd = xian_dj_stewart_platform_input_beta_negative_cmd;
            web_show.xian_dj_stewart_platform_input_gamma_positive_cmd = xian_dj_stewart_platform_input_gamma_positive_cmd;
            web_show.xian_dj_stewart_platform_input_gamma_negative_cmd = xian_dj_stewart_platform_input_gamma_negative_cmd;
            web_show.xian_dj_stewart_platform_input_x_positive_cmd = xian_dj_stewart_platform_input_x_positive_cmd;
            web_show.xian_dj_stewart_platform_input_x_negative_cmd = xian_dj_stewart_platform_input_x_negative_cmd;
            web_show.xian_dj_stewart_platform_input_y_positive_cmd = xian_dj_stewart_platform_input_y_positive_cmd;
            web_show.xian_dj_stewart_platform_input_y_negative_cmd = xian_dj_stewart_platform_input_y_negative_cmd;
            web_show.xian_dj_stewart_platform_input_z_positive_cmd = xian_dj_stewart_platform_input_z_positive_cmd;
            web_show.xian_dj_stewart_platform_input_z_negative_cmd = xian_dj_stewart_platform_input_z_negative_cmd;

            web_show.xian_dj_stewart_platform_arm1_cmd = xian_dj_stewart_platform_arm1_cmd;
            web_show.xian_dj_stewart_platform_arm2_cmd = xian_dj_stewart_platform_arm2_cmd;
            web_show.xian_dj_stewart_platform_arm3_cmd = xian_dj_stewart_platform_arm3_cmd;
            web_show.xian_dj_stewart_platform_arm4_cmd = xian_dj_stewart_platform_arm4_cmd;
            web_show.xian_dj_stewart_platform_arm5_cmd = xian_dj_stewart_platform_arm5_cmd;
            web_show.xian_dj_stewart_platform_arm6_cmd = xian_dj_stewart_platform_arm6_cmd;

            web_show.xian_dj_stewart_platform_params_server_error = xian_dj_stewart_platform_params_server_error;
            web_show.xian_dj_stewart_platform_control_error = xian_dj_stewart_platform_control_error;
            web_show.xian_dj_stewart_platform_base_electric_error = xian_dj_stewart_platform_base_electric_error;
            web_show.xian_dj_stewart_platform_back_end_error = xian_dj_stewart_platform_back_end_error;
            web_show.xian_dj_stewart_platform_error_code = xian_dj_stewart_platform_error_code;
                
            pub.publish(web_show);
             
        }

    private:
        int counter = 0;
        int xian_dj_stewart_platform_back_end_heart_beat = 0;
        ros::Publisher pub; // 声明发布者
        xian_dj_stewart_platform_control_pkg::xian_dj_stewart_platform web_show; // 声明要发布的消息类型

        int xian_dj_stewart_platform_params_server_heart_beat = 0;
        int xian_dj_stewart_platform_control_heart_beat = 0;
        int xian_dj_stewart_platform_base_electric_heart_beat = 0;
        int xian_dj_stewart_platform_error_monitor_heart_beat = 0;
        int xian_dj_stewart_platform_manual_controller_heart_beat = 0;

        double xian_dj_stewart_platform_input_alpha_cmd = 0;
        double xian_dj_stewart_platform_input_beta_cmd = 0;
        double xian_dj_stewart_platform_input_gamma_cmd = 0;
        double xian_dj_stewart_platform_input_x_cmd = 0;
        double xian_dj_stewart_platform_input_y_cmd = 0;
        double xian_dj_stewart_platform_input_z_cmd = 0;

        int xian_dj_stewart_platform_input_alpha_positive_cmd = 0;
        int xian_dj_stewart_platform_input_alpha_negative_cmd = 0;
        int xian_dj_stewart_platform_input_beta_positive_cmd = 0;
        int xian_dj_stewart_platform_input_beta_negative_cmd = 0;
        int xian_dj_stewart_platform_input_gamma_positive_cmd = 0;
        int xian_dj_stewart_platform_input_gamma_negative_cmd = 0;
        int xian_dj_stewart_platform_input_x_positive_cmd = 0;
        int xian_dj_stewart_platform_input_x_negative_cmd = 0;
        int xian_dj_stewart_platform_input_y_positive_cmd = 0;
        int xian_dj_stewart_platform_input_y_negative_cmd = 0;
        int xian_dj_stewart_platform_input_z_positive_cmd = 0;
        int xian_dj_stewart_platform_input_z_negative_cmd = 0;

        double xian_dj_stewart_platform_arm1_cmd = 0;
        double xian_dj_stewart_platform_arm2_cmd = 0;
        double xian_dj_stewart_platform_arm3_cmd = 0;
        double xian_dj_stewart_platform_arm4_cmd = 0;
        double xian_dj_stewart_platform_arm5_cmd = 0;
        double xian_dj_stewart_platform_arm6_cmd = 0;

        int xian_dj_stewart_platform_params_server_error = 0;
        int xian_dj_stewart_platform_control_error = 0;
        int xian_dj_stewart_platform_base_electric_error = 0;
        int xian_dj_stewart_platform_back_end_error = 0;
        int xian_dj_stewart_platform_error_code = 300;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_stewart_platform_back_end");
    XianDjStewartPlatformBackEnd xian_dj_stewart_platform_back_end;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    // xian_dj_stewart_platform_back_end.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(1.0), &XianDjStewartPlatformBackEnd::m_timer_heart_beat_func, &xian_dj_stewart_platform_back_end);
    xian_dj_stewart_platform_back_end.m_timer_control = nh_2.createWallTimer(ros::WallDuration(1), &XianDjStewartPlatformBackEnd::m_timer_control_func, &xian_dj_stewart_platform_back_end);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}