#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include <std_msgs/UInt16.h>
#include"xian_dj_stewart_platform_control_pkg/xian_dj_stewart_platform_manual_controller.h"
#include "xian_dj_stewart_platform_control_pkg/xian_dj_stewart_platform_control.h"

class XianDjStewartPlatformManualControl
{
    public:
        XianDjStewartPlatformManualControl()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            manual_controller = nh.subscribe<xian_dj_stewart_platform_control_pkg::xian_dj_stewart_platform_manual_controller>("xian_dj_stewart_platform_manual_controller_msg", 10,&XianDjStewartPlatformManualControl::manual_controller_Callback, this);
            manual_controller_state_pub = nh.advertise<std_msgs::UInt16>("xian_dj_stewart_platform_manual_controller_state_msg", 1);
            manual_controller_pub = nh.advertise<xian_dj_stewart_platform_control_pkg::xian_dj_stewart_platform_control>("xian_dj_stewart_platform_control_msg", 1);
        }

        ros::WallTimer m_timer_heart_beat;

        void manual_controller_Callback(const xian_dj_stewart_platform_control_pkg::xian_dj_stewart_platform_manual_controller::ConstPtr &data)
        {
            xian_dj_stewart_platform_input_alpha_positive_cmd = data->xian_dj_stewart_platform_input_alpha_positive_cmd;
            xian_dj_stewart_platform_input_alpha_negative_cmd = data->xian_dj_stewart_platform_input_alpha_negative_cmd;
            xian_dj_stewart_platform_input_beta_positive_cmd = data->xian_dj_stewart_platform_input_beta_positive_cmd;
            xian_dj_stewart_platform_input_beta_negative_cmd = data->xian_dj_stewart_platform_input_beta_negative_cmd;
            xian_dj_stewart_platform_input_gamma_positive_cmd = data->xian_dj_stewart_platform_input_gamma_positive_cmd;
            xian_dj_stewart_platform_input_gamma_negative_cmd = data->xian_dj_stewart_platform_input_gamma_negative_cmd;
            xian_dj_stewart_platform_input_x_positive_cmd = data->xian_dj_stewart_platform_input_x_positive_cmd;
            xian_dj_stewart_platform_input_x_negative_cmd = data->xian_dj_stewart_platform_input_x_negative_cmd;
            xian_dj_stewart_platform_input_y_positive_cmd = data->xian_dj_stewart_platform_input_y_positive_cmd;
            xian_dj_stewart_platform_input_y_negative_cmd = data->xian_dj_stewart_platform_input_y_negative_cmd;
            xian_dj_stewart_platform_input_z_positive_cmd = data->xian_dj_stewart_platform_input_z_positive_cmd;
            xian_dj_stewart_platform_input_z_negative_cmd = data->xian_dj_stewart_platform_input_z_negative_cmd;

            if(xian_dj_stewart_platform_input_x_positive_cmd == 1)
            {
                input_x += step_length;
            }
            if(xian_dj_stewart_platform_input_x_negative_cmd == 1)
            {
                input_x -= step_length;
            }
            // ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_x_cmd", input_x);
            pub_msg.xian_dj_stewart_platform_input_x_cmd = input_x;

            if(xian_dj_stewart_platform_input_y_positive_cmd == 1)
            {
                input_y += step_length;
            }
            if(xian_dj_stewart_platform_input_y_negative_cmd == 1)
            {
                input_y -= step_length;
            }
            // ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_y_cmd", input_y);
            pub_msg.xian_dj_stewart_platform_input_y_cmd = input_y;

            if(xian_dj_stewart_platform_input_z_positive_cmd == 1)
            {
                input_z += step_length;
            }
            if(xian_dj_stewart_platform_input_z_negative_cmd == 1)
            {
                input_z -= step_length;
            }
            // ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_z_cmd", input_z);
            pub_msg.xian_dj_stewart_platform_input_z_cmd = input_z;
            printf("input_x: %0.2f, input_y: %0.2f, input_z: %0.2f \n", input_x, input_y, input_z);


            if(xian_dj_stewart_platform_input_alpha_positive_cmd == 1)
            {
                input_alpha += step_length;
            }
            if(xian_dj_stewart_platform_input_alpha_negative_cmd == 1)
            {
                input_alpha -= step_length;
            }
            // ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_alpha_cmd", input_alpha);
            pub_msg.xian_dj_stewart_platform_input_alpha_cmd = input_alpha;

            if(xian_dj_stewart_platform_input_beta_positive_cmd == 1)
            {
                input_beta += step_length;
            }
            if(xian_dj_stewart_platform_input_beta_negative_cmd == 1)
            {
                input_beta -= step_length;
            }
            // ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_beta_cmd", input_beta);
            pub_msg.xian_dj_stewart_platform_input_beta_cmd = input_beta;

            if(xian_dj_stewart_platform_input_gamma_positive_cmd == 1)
            {
                input_gamma += step_length;
            }
            if(xian_dj_stewart_platform_input_gamma_negative_cmd == 1)
            {
                input_gamma -= step_length;
            }
            // ros::param::set("/xian_dj_stewart_platform_params_server/xian_dj_stewart_platform_input_gamma_cmd", input_gamma);
            pub_msg.xian_dj_stewart_platform_input_gamma_cmd = input_gamma;

            printf("input_alpha: %0.2f, input_beta: %0.2f, input_gamma: %0.2f \n", input_alpha, input_beta, input_gamma);
            manual_controller_pub.publish(pub_msg);
            
        }


        // 节点心跳
        void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        {
            
            xian_dj_stewart_platform_manual_controller_heart_beat = xian_dj_stewart_platform_manual_controller_heart_beat > 1000 ? 0 : (xian_dj_stewart_platform_manual_controller_heart_beat + 1);
            std::cout << "xian_dj_stewart_platform_manual_controller_heart_beat: " << xian_dj_stewart_platform_manual_controller_heart_beat << std::endl;
            heart_beat_msg.data = xian_dj_stewart_platform_manual_controller_heart_beat;
            manual_controller_state_pub.publish(heart_beat_msg);
        }
 
        

    private:
        ros::Subscriber manual_controller;
        ros::Publisher manual_controller_state_pub;
        ros::Publisher manual_controller_pub;
        std_msgs::UInt16 heart_beat_msg;
        xian_dj_stewart_platform_control_pkg::xian_dj_stewart_platform_control pub_msg;

        int xian_dj_stewart_platform_manual_controller_heart_beat = 0;

        double input_alpha = 0;
        double input_beta = 0;
        double input_gamma = 0;
        double input_x = 0;
        double input_y = 0;
        double input_z = 0;  //杆长为160时，动平台水平对应的Z高度
        double step_length = 0.25 ;

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

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_stewart_platform_manual_controller");
    XianDjStewartPlatformManualControl xian_dj_stewart_platform_manual_controller;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_stewart_platform_manual_controller.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(1.0), &XianDjStewartPlatformManualControl::m_timer_heart_beat_func, &xian_dj_stewart_platform_manual_controller);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}