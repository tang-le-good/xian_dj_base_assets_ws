#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include "xian_dj_remote_operation_control_pkg/xian_dj_remote_operation_back_end.h"

class XianDjTeleOpBackEnd
{
    public:
        XianDjTeleOpBackEnd()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            pub = nh.advertise<xian_dj_remote_operation_control_pkg::xian_dj_remote_operation_back_end>("xian_dj_remote_operation_back_end_msg", 1);
        }

        // ros::WallTimer m_timer_heart_beat;
        ros::WallTimer m_timer_control;

        // void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        // {
        //     ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_back_end_heart_beat", xian_dj_tele_op_back_end_heart_beat); 
        //     std::cout << "xian_dj_tele_op_back_end_heart_beat: " << xian_dj_tele_op_back_end_heart_beat << std::endl;
        //     counter = counter > 1000 ? 0 : (counter + 1);
        //     ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_back_end_heart_beat", counter);  // 自行替换
        // }

        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_back_end_heart_beat", xian_dj_tele_op_back_end_heart_beat); 
            std::cout << "xian_dj_tele_op_back_end_heart_beat: " << xian_dj_tele_op_back_end_heart_beat << std::endl;
            counter = counter > 1000 ? 0 : (counter + 1);
            ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_back_end_heart_beat", counter);  // 自行替换

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_params_server_heart_beat", xian_dj_tele_op_params_server_heart_beat);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_heart_beat", xian_dj_tele_op_controller_server_heart_beat);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_tcp_heart_beat", xian_dj_tele_op_controller_server_tcp_heart_beat);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_heart_beat", xian_dj_tele_op_controller_client_heart_beat);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_tcp_heart_beat", xian_dj_tele_op_controller_client_tcp_heart_beat);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_error_monitor_heart_beat", xian_dj_tele_op_error_monitor_heart_beat);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_back_end_heart_beat", xian_dj_tele_op_back_end_heart_beat);

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_client_cmd", xian_dj_tele_op_left_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_client_cmd", xian_dj_tele_op_right_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_up_client_cmd", xian_dj_tele_op_up_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_down_client_cmd", xian_dj_tele_op_down_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_x_client_cmd", xian_dj_tele_op_x_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_b_client_cmd", xian_dj_tele_op_b_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_y_client_cmd", xian_dj_tele_op_y_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_a_client_cmd", xian_dj_tele_op_a_client_cmd);

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_x_client_cmd", xian_dj_tele_op_left_rocker_x_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_y_client_cmd", xian_dj_tele_op_left_rocker_y_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_x_client_cmd", xian_dj_tele_op_right_rocker_x_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_y_client_cmd", xian_dj_tele_op_right_rocker_y_client_cmd);
  
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_r1_client_cmd", xian_dj_tele_op_r1_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_r2_client_cmd", xian_dj_tele_op_r2_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_l1_client_cmd", xian_dj_tele_op_l1_client_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_l2_client_cmd", xian_dj_tele_op_l2_client_cmd);

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_server_cmd", xian_dj_tele_op_left_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_server_cmd", xian_dj_tele_op_right_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_up_server_cmd", xian_dj_tele_op_up_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_down_server_cmd", xian_dj_tele_op_down_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_x_server_cmd", xian_dj_tele_op_x_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_b_server_cmd", xian_dj_tele_op_b_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_y_server_cmd", xian_dj_tele_op_y_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_a_server_cmd", xian_dj_tele_op_a_server_cmd);

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_x_server_cmd", xian_dj_tele_op_left_rocker_x_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_y_server_cmd", xian_dj_tele_op_left_rocker_y_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_x_server_cmd", xian_dj_tele_op_right_rocker_x_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_y_server_cmd", xian_dj_tele_op_right_rocker_y_server_cmd);

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_r1_server_cmd", xian_dj_tele_op_r1_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_r2_server_cmd", xian_dj_tele_op_r2_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_l1_server_cmd", xian_dj_tele_op_l1_server_cmd);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_l2_server_cmd", xian_dj_tele_op_l2_server_cmd);

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_error", xian_dj_tele_op_controller_server_error);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_error", xian_dj_tele_op_controller_client_error);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_tcp_error", xian_dj_tele_op_controller_server_tcp_error);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_tcp_error", xian_dj_tele_op_controller_client_tcp_error);
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_error_code", xian_dj_tele_op_error_code);

            web_show.xian_dj_tele_op_params_server_heart_beat = xian_dj_tele_op_params_server_heart_beat;
            web_show.xian_dj_tele_op_controller_server_heart_beat = xian_dj_tele_op_controller_server_heart_beat;
            web_show.xian_dj_tele_op_controller_server_tcp_heart_beat = xian_dj_tele_op_controller_server_tcp_heart_beat;
            web_show.xian_dj_tele_op_controller_client_heart_beat = xian_dj_tele_op_controller_client_heart_beat;
            web_show.xian_dj_tele_op_controller_client_tcp_heart_beat = xian_dj_tele_op_controller_client_tcp_heart_beat;
            web_show.xian_dj_tele_op_error_monitor_heart_beat = xian_dj_tele_op_error_monitor_heart_beat;
            web_show.xian_dj_tele_op_back_end_heart_beat = xian_dj_tele_op_back_end_heart_beat;

            web_show.xian_dj_tele_op_left_client_cmd = xian_dj_tele_op_left_client_cmd;
            web_show.xian_dj_tele_op_right_client_cmd = xian_dj_tele_op_right_client_cmd;
            web_show.xian_dj_tele_op_up_client_cmd = xian_dj_tele_op_up_client_cmd;
            web_show.xian_dj_tele_op_down_client_cmd = xian_dj_tele_op_down_client_cmd;
            web_show.xian_dj_tele_op_x_client_cmd = xian_dj_tele_op_x_client_cmd;
            web_show.xian_dj_tele_op_b_client_cmd = xian_dj_tele_op_b_client_cmd;
            web_show.xian_dj_tele_op_y_client_cmd = xian_dj_tele_op_y_client_cmd;
            web_show.xian_dj_tele_op_a_client_cmd = xian_dj_tele_op_a_client_cmd;

            web_show.xian_dj_tele_op_left_rocker_x_client_cmd = xian_dj_tele_op_left_rocker_x_client_cmd;
            web_show.xian_dj_tele_op_left_rocker_y_client_cmd = xian_dj_tele_op_left_rocker_y_client_cmd;
            web_show.xian_dj_tele_op_right_rocker_x_client_cmd = xian_dj_tele_op_right_rocker_x_client_cmd;
            web_show.xian_dj_tele_op_right_rocker_y_client_cmd = xian_dj_tele_op_right_rocker_y_client_cmd;

            web_show.xian_dj_tele_op_r1_client_cmd = xian_dj_tele_op_r1_client_cmd;
            web_show.xian_dj_tele_op_r2_client_cmd = xian_dj_tele_op_r2_client_cmd;
            web_show.xian_dj_tele_op_l1_client_cmd = xian_dj_tele_op_l1_client_cmd;
            web_show.xian_dj_tele_op_l2_client_cmd = xian_dj_tele_op_l2_client_cmd;

            web_show.xian_dj_tele_op_left_server_cmd = xian_dj_tele_op_left_server_cmd;
            web_show.xian_dj_tele_op_right_server_cmd = xian_dj_tele_op_right_server_cmd;
            web_show.xian_dj_tele_op_up_server_cmd = xian_dj_tele_op_up_server_cmd;
            web_show.xian_dj_tele_op_down_server_cmd = xian_dj_tele_op_down_server_cmd;
            web_show.xian_dj_tele_op_x_server_cmd = xian_dj_tele_op_x_server_cmd;
            web_show.xian_dj_tele_op_b_server_cmd = xian_dj_tele_op_b_server_cmd;
            web_show.xian_dj_tele_op_y_server_cmd = xian_dj_tele_op_y_server_cmd;
            web_show.xian_dj_tele_op_a_server_cmd = xian_dj_tele_op_a_server_cmd;

            web_show.xian_dj_tele_op_left_rocker_x_server_cmd = xian_dj_tele_op_left_rocker_x_server_cmd;
            web_show.xian_dj_tele_op_left_rocker_y_server_cmd = xian_dj_tele_op_left_rocker_y_server_cmd;
            web_show.xian_dj_tele_op_right_rocker_x_server_cmd = xian_dj_tele_op_right_rocker_x_server_cmd;
            web_show.xian_dj_tele_op_right_rocker_y_server_cmd = xian_dj_tele_op_right_rocker_y_server_cmd;

            web_show.xian_dj_tele_op_r1_server_cmd = xian_dj_tele_op_r1_server_cmd;
            web_show.xian_dj_tele_op_r2_server_cmd = xian_dj_tele_op_r2_server_cmd;
            web_show.xian_dj_tele_op_l1_server_cmd = xian_dj_tele_op_l1_server_cmd;
            web_show.xian_dj_tele_op_l2_server_cmd = xian_dj_tele_op_l2_server_cmd;

            web_show.xian_dj_tele_op_controller_server_error = xian_dj_tele_op_controller_server_error;
            web_show.xian_dj_tele_op_controller_client_error = xian_dj_tele_op_controller_client_error;
            web_show.xian_dj_tele_op_controller_server_tcp_error = xian_dj_tele_op_controller_server_tcp_error;
            web_show.xian_dj_tele_op_controller_client_tcp_error = xian_dj_tele_op_controller_client_tcp_error;
            web_show.xian_dj_tele_op_error_code = xian_dj_tele_op_error_code;
          
            pub.publish(web_show);
             
        }

    private:
        int counter = 0;
        int xian_dj_tele_op_back_end_heart_beat = 0;
        ros::Publisher pub; // 声明发布者
        xian_dj_remote_operation_control_pkg::xian_dj_remote_operation_back_end web_show; // 声明要发布的消息类型

        int xian_dj_tele_op_params_server_heart_beat ;
        int xian_dj_tele_op_controller_server_heart_beat ;
        int xian_dj_tele_op_controller_server_tcp_heart_beat ;
        int xian_dj_tele_op_controller_client_heart_beat ;
        int xian_dj_tele_op_controller_client_tcp_heart_beat ;
        int xian_dj_tele_op_error_monitor_heart_beat ;
        int xian_dj_tele_op_left_client_cmd ;
        int xian_dj_tele_op_right_client_cmd ;
        int xian_dj_tele_op_up_client_cmd ;
        int xian_dj_tele_op_down_client_cmd;
        int xian_dj_tele_op_x_client_cmd ;
        int xian_dj_tele_op_b_client_cmd ;
        int xian_dj_tele_op_y_client_cmd ;
        int xian_dj_tele_op_a_client_cmd ;
        float xian_dj_tele_op_left_rocker_x_client_cmd ;
        float xian_dj_tele_op_left_rocker_y_client_cmd ;
        float xian_dj_tele_op_right_rocker_x_client_cmd ;
        float xian_dj_tele_op_right_rocker_y_client_cmd ;
        int xian_dj_tele_op_r1_client_cmd ;
        int xian_dj_tele_op_r2_client_cmd ;
        int xian_dj_tele_op_l1_client_cmd ;
        int xian_dj_tele_op_l2_client_cmd ;
        int xian_dj_tele_op_left_server_cmd ;
        int xian_dj_tele_op_right_server_cmd ;
        int xian_dj_tele_op_up_server_cmd ;
        int xian_dj_tele_op_down_server_cmd;
        int xian_dj_tele_op_x_server_cmd ;
        int xian_dj_tele_op_b_server_cmd ;
        int xian_dj_tele_op_y_server_cmd ;
        int xian_dj_tele_op_a_server_cmd ;
        float xian_dj_tele_op_left_rocker_x_server_cmd ;
        float xian_dj_tele_op_left_rocker_y_server_cmd ;
        float xian_dj_tele_op_right_rocker_x_server_cmd ;
        float xian_dj_tele_op_right_rocker_y_server_cmd ;
        int xian_dj_tele_op_r1_server_cmd ;
        int xian_dj_tele_op_r2_server_cmd ;
        int xian_dj_tele_op_l1_server_cmd ;
        int xian_dj_tele_op_l2_server_cmd ;
        int xian_dj_tele_op_controller_server_error ;
        int xian_dj_tele_op_controller_client_error ;
        int xian_dj_tele_op_controller_server_tcp_error ;
        int xian_dj_tele_op_controller_client_tcp_error ;
        int xian_dj_tele_op_error_code;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_tele_op_back_end");
    XianDjTeleOpBackEnd xian_dj_tele_op_back_end;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    // xian_dj_tele_op_back_end.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(1.0), &XianDjTeleOpBackEnd::m_timer_heart_beat_func, &xian_dj_tele_op_back_end);
    xian_dj_tele_op_back_end.m_timer_control = nh_2.createWallTimer(ros::WallDuration(1), &XianDjTeleOpBackEnd::m_timer_control_func, &xian_dj_tele_op_back_end);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}