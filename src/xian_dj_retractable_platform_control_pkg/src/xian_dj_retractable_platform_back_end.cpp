#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include "xian_dj_retractable_platform_control_pkg/xian_dj_retractable_platform_back_end.h"

class XianDjRetractablePlatformBackEnd
{
    public:
        XianDjRetractablePlatformBackEnd()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            pub = nh.advertise<xian_dj_retractable_platform_control_pkg::xian_dj_retractable_platform_back_end>("xian_dj_retractable_platform_back_end_msg", 1);
        }

        ros::WallTimer m_timer_heart_beat;
        ros::WallTimer m_timer_control;

        void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        {
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_back_end_heart_beat", xian_dj_retractable_platform_back_end_heart_beat); 
            std::cout << "xian_dj_retractable_platform_back_end_heart_beat: " << xian_dj_retractable_platform_back_end_heart_beat << std::endl;
            counter = counter > 1000 ? 0 : (counter + 1);
            ros::param::set("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_back_end_heart_beat", counter);  // 自行替换
        }

        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_params_server_heart_beat", xian_dj_retractable_platform_params_server_heart_beat);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_control_heart_beat", xian_dj_retractable_platform_control_heart_beat);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_base_electric_heart_beat", xian_dj_retractable_platform_base_electric_heart_beat);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_error_monitor_heart_beat", xian_dj_retractable_platform_error_monitor_heart_beat);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_back_end_heart_beat", xian_dj_retractable_platform_back_end_heart_beat);

            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_enble", xian_dj_retractable_platform_stand_linear_actuator_enble);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_move", xian_dj_retractable_platform_stand_linear_actuator_move);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_enble", xian_dj_retractable_platform_first_linear_actuator_enble);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_move", xian_dj_retractable_platform_first_linear_actuator_move);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_enble", xian_dj_retractable_platform_second_linear_actuator_enble);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_move", xian_dj_retractable_platform_second_linear_actuator_move);

            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_stand_cmd", xian_dj_retractable_platform_stand_linear_actuator_stand_cmd);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_stand_linear_actuator_sit_cmd", xian_dj_retractable_platform_stand_linear_actuator_sit_cmd);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_up_cmd", xian_dj_retractable_platform_first_linear_actuator_up_cmd);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_first_linear_actuator_down_cmd", xian_dj_retractable_platform_first_linear_actuator_down_cmd);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_up_cmd", xian_dj_retractable_platform_second_linear_actuator_up_cmd);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_second_linear_actuator_down_cmd", xian_dj_retractable_platform_second_linear_actuator_down_cmd);

            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_params_server_error", xian_dj_retractable_platform_params_server_error);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_control_error", xian_dj_retractable_platform_control_error);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_base_electric_error", xian_dj_retractable_platform_base_electric_error);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_back_end_error", xian_dj_retractable_platform_back_end_error);
            ros::param::get("/xian_dj_retractable_platform_params_server/xian_dj_retractable_platform_error_code", xian_dj_retractable_platform_error_code);

            web_show.xian_dj_retractable_platform_params_server_heart_beat = xian_dj_retractable_platform_params_server_heart_beat;
            web_show.xian_dj_retractable_platform_control_heart_beat = xian_dj_retractable_platform_control_heart_beat;
            web_show.xian_dj_retractable_platform_base_electric_heart_beat = xian_dj_retractable_platform_base_electric_heart_beat;
            web_show.xian_dj_retractable_platform_error_monitor_heart_beat = xian_dj_retractable_platform_error_monitor_heart_beat;
            web_show.xian_dj_retractable_platform_back_end_heart_beat = xian_dj_retractable_platform_back_end_heart_beat;

            web_show.xian_dj_retractable_platform_stand_linear_actuator_enble = xian_dj_retractable_platform_stand_linear_actuator_enble;
            web_show.xian_dj_retractable_platform_stand_linear_actuator_move = xian_dj_retractable_platform_stand_linear_actuator_move;
            web_show.xian_dj_retractable_platform_first_linear_actuator_enble = xian_dj_retractable_platform_first_linear_actuator_enble;
            web_show.xian_dj_retractable_platform_first_linear_actuator_move = xian_dj_retractable_platform_first_linear_actuator_move;
            web_show.xian_dj_retractable_platform_second_linear_actuator_enble = xian_dj_retractable_platform_second_linear_actuator_enble;
            web_show.xian_dj_retractable_platform_second_linear_actuator_move = xian_dj_retractable_platform_second_linear_actuator_move;

            web_show.xian_dj_retractable_platform_stand_linear_actuator_stand_cmd = xian_dj_retractable_platform_stand_linear_actuator_stand_cmd;
            web_show.xian_dj_retractable_platform_stand_linear_actuator_sit_cmd = xian_dj_retractable_platform_stand_linear_actuator_sit_cmd;
            web_show.xian_dj_retractable_platform_first_linear_actuator_up_cmd = xian_dj_retractable_platform_first_linear_actuator_up_cmd;
            web_show.xian_dj_retractable_platform_first_linear_actuator_down_cmd = xian_dj_retractable_platform_first_linear_actuator_down_cmd;
            web_show.xian_dj_retractable_platform_second_linear_actuator_up_cmd = xian_dj_retractable_platform_second_linear_actuator_up_cmd;
            web_show.xian_dj_retractable_platform_second_linear_actuator_down_cmd = xian_dj_retractable_platform_second_linear_actuator_down_cmd;

            web_show.xian_dj_retractable_platform_params_server_error = xian_dj_retractable_platform_params_server_error;
            web_show.xian_dj_retractable_platform_control_error = xian_dj_retractable_platform_control_error;
            web_show.xian_dj_retractable_platform_base_electric_error = xian_dj_retractable_platform_base_electric_error;
            web_show.xian_dj_retractable_platform_back_end_error = xian_dj_retractable_platform_back_end_error;
            web_show.xian_dj_retractable_platform_error_code = xian_dj_retractable_platform_error_code;
          
            pub.publish(web_show);
             
        }

    private:
        int counter = 0;
        int xian_dj_retractable_platform_back_end_heart_beat = 0;
        ros::Publisher pub; // 声明发布者
        xian_dj_retractable_platform_control_pkg::xian_dj_retractable_platform_back_end web_show; // 声明要发布的消息类型

        int xian_dj_retractable_platform_params_server_heart_beat ;
        int xian_dj_retractable_platform_control_heart_beat ;
        int xian_dj_retractable_platform_base_electric_heart_beat ;
        int xian_dj_retractable_platform_error_monitor_heart_beat ;
        int xian_dj_retractable_platform_stand_linear_actuator_enble ;
        int xian_dj_retractable_platform_stand_linear_actuator_move ;
        int xian_dj_retractable_platform_first_linear_actuator_enble ;
        int xian_dj_retractable_platform_first_linear_actuator_move ;
        int xian_dj_retractable_platform_second_linear_actuator_enble ;
        int xian_dj_retractable_platform_second_linear_actuator_move ;
        int xian_dj_retractable_platform_stand_linear_actuator_stand_cmd ;
        int xian_dj_retractable_platform_stand_linear_actuator_sit_cmd ;
        int xian_dj_retractable_platform_first_linear_actuator_up_cmd ;
        int xian_dj_retractable_platform_first_linear_actuator_down_cmd ;
        int xian_dj_retractable_platform_second_linear_actuator_up_cmd ;
        int xian_dj_retractable_platform_second_linear_actuator_down_cmd ;
        int xian_dj_retractable_platform_params_server_error ;
        int xian_dj_retractable_platform_control_error ;
        int xian_dj_retractable_platform_base_electric_error ;
        int xian_dj_retractable_platform_back_end_error ;

        int xian_dj_retractable_platform_error_code ;

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_retractable_platform_back_end");
    XianDjRetractablePlatformBackEnd xian_dj_retractable_platform_back_end;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_retractable_platform_back_end.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(1.0), &XianDjRetractablePlatformBackEnd::m_timer_heart_beat_func, &xian_dj_retractable_platform_back_end);
    xian_dj_retractable_platform_back_end.m_timer_control = nh_2.createWallTimer(ros::WallDuration(0.1), &XianDjRetractablePlatformBackEnd::m_timer_control_func, &xian_dj_retractable_platform_back_end);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}