#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/UInt16.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

// #define SERVER_IP "192.168.1.12"
// #define SERVER_PORT 3241
// #define MAX_RETRY 900000000 
// #define RETRY_INTERVAL 3 // 重试间隔(秒)     3秒重连1次，900000000次，时间大概是10年

// client发送的数据
struct client2server 
{
    int xian_dj_tele_op_controller_client_tcp_heart_beat; 
    int xian_dj_tele_op_left_client_cmd;
    int xian_dj_tele_op_right_client_cmd;
    int xian_dj_tele_op_up_client_cmd;
    int xian_dj_tele_op_down_client_cmd;
    int xian_dj_tele_op_x_client_cmd;
    int xian_dj_tele_op_b_client_cmd;
    int xian_dj_tele_op_y_client_cmd;
    int xian_dj_tele_op_a_client_cmd;
    double xian_dj_tele_op_left_rocker_x_client_cmd;
    double xian_dj_tele_op_left_rocker_y_client_cmd;
    double xian_dj_tele_op_right_rocker_x_client_cmd;
    double xian_dj_tele_op_right_rocker_y_client_cmd;
    int xian_dj_tele_op_r1_client_cmd;
    int xian_dj_tele_op_r2_client_cmd;
    int xian_dj_tele_op_l1_client_cmd;
    int xian_dj_tele_op_l2_client_cmd;
};

// 接收到的server的数据
struct server2client
{
    int xian_dj_tele_op_controller_server_tcp_heart_beat; 
    // int xian_dj_tele_op_left_server_cmd;
    // int xian_dj_tele_op_right_server_cmd;
    // int xian_dj_tele_op_up_server_cmd;
    // int xian_dj_tele_op_down_server_cmd;
    // int xian_dj_tele_op_x_server_cmd;
    // int xian_dj_tele_op_b_server_cmd;
    // int xian_dj_tele_op_y_server_cmd;
    // int xian_dj_tele_op_a_server_cmd;
    // double xian_dj_tele_op_left_rocker_x_server_cmd;
    // double xian_dj_tele_op_left_rocker_y_server_cmd;
    // double xian_dj_tele_op_right_rocker_x_server_cmd;
    // double xian_dj_tele_op_right_rocker_y_server_cmd;
    // int xian_dj_tele_op_r1_server_cmd;
    // int xian_dj_tele_op_r2_server_cmd;
    // int xian_dj_tele_op_l1_server_cmd;
    // int xian_dj_tele_op_l2_server_cmd;
};

class TCPClient 
{
    private:
        int sockfd;
        struct sockaddr_in server_addr;
        bool connected;
        std::string server_ip_;
        int server_port_;
        int max_retry=900000000;
        int RETRY_INTERVAL=3; // 3s重连一次
        
    public:
        TCPClient(const std::string& ip, int port):server_ip_(ip), server_port_(port), sockfd(-1), connected(false) 
        {
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(server_port_);
            
            if (inet_pton(AF_INET, server_ip_.c_str(), &server_addr.sin_addr) <= 0) 
            {
                std::cerr << "无效的地址或地址不支持: " << server_ip_ << std::endl;
            }
        }
        
        ~TCPClient() 
        {
            disconnect();
        }
        
        bool connectToServer() 
        {
            int retry_count = 0;
            
            while (retry_count < max_retry && !connected) 
            {
                // 创建套接字
                if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
                {
                    std::cerr << "创建套接字失败" << std::endl;
                    return false;
                }
                
                // 尝试连接
                if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
                {
                    std::cerr << "连接失败，尝试重连 (" << (retry_count + 1) << "/" << max_retry << ")..." << std::endl;
                    close(sockfd);
                    sockfd = -1;
                    retry_count++;
                    std::this_thread::sleep_for(std::chrono::seconds(RETRY_INTERVAL));
                    continue;
                }
                
                connected = true;
                std::cout << "成功连接到服务器 " << server_ip_ << ":" << server_port_ << std::endl;
                return true;
            }
            
            return false;
        }
        
        void disconnect() 
        {
            if (sockfd != -1) 
            {
                close(sockfd);
                sockfd = -1;
            }
            connected = false;
        }
        
        bool sendData(const client2server& client_data) 
        {
            if (!connected && !connectToServer()) 
            {
                std::cerr << "无法发送数据：未连接到服务器" << std::endl;
                return false;
            }
            
            if (send(sockfd, &client_data, sizeof(client_data), 0) < 0) 
            {
                std::cerr << "发送数据失败" << std::endl;
                connected = false;
                return false;
            }
            
            return true;
        }
        
        bool receiveData(server2client& server_data) 
        {
            // 设置文件描述符集合
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);
            
            // 设置3秒超时
            struct timeval timeout;
            timeout.tv_sec = 3;
            timeout.tv_usec = 0;
            
            // 等待socket可读
            int select_result = select(sockfd + 1, &readfds, NULL, NULL, &timeout);
            
            if (select_result == 0) {
                std::cerr << "接收数据超时（3秒）" << std::endl;
                // connected = false;
                return false;
            } else if (select_result < 0) {
                std::cerr << "select错误: " << strerror(errno) << std::endl;
                // connected = false;
                return false;
            }

            // 有数据可读，正常接收
            int recv_len = recv(sockfd, &server_data, sizeof(server_data), 0);
            
            if (recv_len <= 0) 
            {
                std::cerr << "接收数据失败或连接已关闭" << std::endl;
                connected = false;
                return false;
            }
            
            return true;
        }
        
        bool isConnected() const 
        {
            return connected;
        }
};

class XianDjTeleOpControllerClient
{
    public:
        XianDjTeleOpControllerClient()
        {
            // 创建一个ROS节点句柄
            ros::NodeHandle nh;
            init();
            xian_dj_tele_op_controller_client_pub = nh.advertise<std_msgs::UInt16>("xian_dj_tele_op_controller_client_msg", 1);
            controller_sub = nh.subscribe<sensor_msgs::Joy>("joy", 10, &XianDjTeleOpControllerClient::controller_callback, this);
        }

        ros::WallTimer m_timer_control;

        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            this->command_callback();
        }

        void controller_callback(const sensor_msgs::Joy::ConstPtr &Joy)
        {
            axes_6_common = Joy->axes[6];
            switch (axes_6_common)
            {
                case 0:
                    xian_dj_tele_op_left_client_cmd = 0;
                    xian_dj_tele_op_right_client_cmd = 0;
                    break;
                case 1:
                    xian_dj_tele_op_left_client_cmd = 1;
                    xian_dj_tele_op_right_client_cmd = 0;
                    break;  
                case -1:
                    xian_dj_tele_op_left_client_cmd = 0;
                    xian_dj_tele_op_right_client_cmd = 1;
                    break;
            }

            axes_7_common = Joy->axes[7];
            switch (axes_7_common)
            {
                case 0:
                    xian_dj_tele_op_up_client_cmd = 0;
                    xian_dj_tele_op_down_client_cmd = 0;
                    break;
                case 1:
                    xian_dj_tele_op_up_client_cmd = 1;
                    xian_dj_tele_op_down_client_cmd = 0;
                    break;  
                case -1:
                    xian_dj_tele_op_up_client_cmd = 0;
                    xian_dj_tele_op_down_client_cmd = 1;
                    break;
            }
            // xian_dj_tele_op_left_client_cmd = Joy->axes[6];
            // xian_dj_tele_op_right_client_cmd = Joy->axes[6];
            // xian_dj_tele_op_up_client_cmd = Joy->axes[7];
            // xian_dj_tele_op_down_client_cmd = Joy->axes[7];
            xian_dj_tele_op_x_client_cmd = Joy->buttons[3];
            xian_dj_tele_op_b_client_cmd = Joy->buttons[1];
            xian_dj_tele_op_y_client_cmd = Joy->buttons[4];
            xian_dj_tele_op_a_client_cmd = Joy->buttons[0];
            xian_dj_tele_op_left_rocker_x_client_cmd = Joy->axes[0];
            xian_dj_tele_op_left_rocker_y_client_cmd = Joy->axes[1];
            xian_dj_tele_op_right_rocker_x_client_cmd = Joy->axes[2];
            xian_dj_tele_op_right_rocker_y_client_cmd = Joy->axes[3];
            xian_dj_tele_op_r1_client_cmd = Joy->buttons[7];
            xian_dj_tele_op_r2_client_cmd = Joy->buttons[9];
            xian_dj_tele_op_l1_client_cmd = Joy->buttons[6];
            xian_dj_tele_op_l2_client_cmd = Joy->buttons[8];
           
            // printf("xian_dj_tele_op_x_client_cmd: %d\n", xian_dj_tele_op_x_client_cmd);
            // printf("xian_dj_tele_op_b_client_cmd: %d\n", xian_dj_tele_op_b_client_cmd);
            // printf("xian_dj_tele_op_y_client_cmd: %d\n", xian_dj_tele_op_y_client_cmd);
            // printf("xian_dj_tele_op_a_client_cmd: %d\n", xian_dj_tele_op_a_client_cmd);
            // printf("xian_dj_tele_op_left_rocker_x_client_cmd: %.2f\n", xian_dj_tele_op_left_rocker_x_client_cmd);
            // printf("xian_dj_tele_op_left_rocker_y_client_cmd: %.2f\n", xian_dj_tele_op_left_rocker_y_client_cmd);
            // printf("xian_dj_tele_op_right_rocker_x_client_cmd: %.2f\n", xian_dj_tele_op_right_rocker_x_client_cmd);
            // printf("xian_dj_tele_op_right_rocker_y_client_cmd: %.2f\n", xian_dj_tele_op_right_rocker_y_client_cmd);
            // printf("xian_dj_tele_op_r1_client_cmd: %d\n", xian_dj_tele_op_r1_client_cmd);
            // printf("xian_dj_tele_op_r2_client_cmd: %d\n", xian_dj_tele_op_r2_client_cmd);
            // printf("xian_dj_tele_op_l1_client_cmd: %d\n", xian_dj_tele_op_l1_client_cmd);
            // printf("xian_dj_tele_op_l2_client_cmd: %d\n", xian_dj_tele_op_l2_client_cmd);
            // printf("xian_dj_tele_op_left_client_cmd: %d\n", xian_dj_tele_op_left_client_cmd);
            // printf("xian_dj_tele_op_right_client_cmd: %d\n", xian_dj_tele_op_right_client_cmd);
            // printf("xian_dj_tele_op_up_client_cmd: %d\n", xian_dj_tele_op_up_client_cmd);
            // printf("xian_dj_tele_op_down_client_cmd: %d\n", xian_dj_tele_op_down_client_cmd);
        }

    private:
        ros::Subscriber controller_sub; // 订阅遥控器/joy消息
        ros::Publisher xian_dj_tele_op_controller_client_pub; // 发布client的心跳topic
        std_msgs::UInt16 pub_msg; // client的心跳topic的type
        int xian_dj_tele_op_controller_client_heart_beat = 0;
        int xian_dj_tele_op_controller_server_tcp_heart_beat = 0;
        // int xian_dj_tele_op_controller_server_tcp_error = 0;
      
        // 声明遥控器接收到的变量
        int xian_dj_tele_op_left_client_cmd = 0;
        int xian_dj_tele_op_right_client_cmd = 0;
        int xian_dj_tele_op_up_client_cmd = 0;
        int xian_dj_tele_op_down_client_cmd = 0;
        int xian_dj_tele_op_x_client_cmd = 0;
        int xian_dj_tele_op_b_client_cmd = 0;
        int xian_dj_tele_op_y_client_cmd = 0;
        int xian_dj_tele_op_a_client_cmd = 0;
        double xian_dj_tele_op_left_rocker_x_client_cmd = 0;
        double xian_dj_tele_op_left_rocker_y_client_cmd = 0;
        double xian_dj_tele_op_right_rocker_x_client_cmd = 0;
        double xian_dj_tele_op_right_rocker_y_client_cmd = 0;
        int xian_dj_tele_op_r1_client_cmd = 0;
        int xian_dj_tele_op_r2_client_cmd = 0;
        int xian_dj_tele_op_l1_client_cmd = 0;
        int xian_dj_tele_op_l2_client_cmd = 0;
        
        int axes_6_common = 0;
        int axes_7_common = 0;

        std::string server_ip_ = "192.168.1.12";
        int server_port_ = 3241;
        int RETRY_INTERVAL = 3; // 断开后，3s重连一次
        TCPClient client{server_ip_, server_port_};
        struct client2server client_data;
        struct server2client server_data;

        int init()
        {
            // 初始连接
            if (!client.connectToServer()) 
            {
                std::cerr << "无法连接到服务器，退出程序" << std::endl;
                return -1;
            }
            return 1;
        }

        int command_callback()
        {
            if (!client.isConnected()) 
            {
                std::cout << "连接已断开，尝试重新连接..." << std::endl;
                if (!client.connectToServer()) 
                {
                    std::this_thread::sleep_for(std::chrono::seconds(RETRY_INTERVAL));
                    return -1;
                }
            }

            // client心跳
            xian_dj_tele_op_controller_client_heart_beat = xian_dj_tele_op_controller_client_heart_beat > 1000 ? 0 : (xian_dj_tele_op_controller_client_heart_beat + 1);
            printf("xian_dj_tele_op_controller_client_heart_beat: %d \n", xian_dj_tele_op_controller_client_heart_beat);    
            // 发送数据
            client_data.xian_dj_tele_op_controller_client_tcp_heart_beat = xian_dj_tele_op_controller_client_heart_beat;
            client_data.xian_dj_tele_op_left_client_cmd = xian_dj_tele_op_left_client_cmd;
            client_data.xian_dj_tele_op_right_client_cmd = xian_dj_tele_op_right_client_cmd;
            client_data.xian_dj_tele_op_up_client_cmd = xian_dj_tele_op_up_client_cmd;
            client_data.xian_dj_tele_op_down_client_cmd = xian_dj_tele_op_down_client_cmd;
            client_data.xian_dj_tele_op_x_client_cmd = xian_dj_tele_op_x_client_cmd;
            client_data.xian_dj_tele_op_b_client_cmd = xian_dj_tele_op_b_client_cmd;
            client_data.xian_dj_tele_op_y_client_cmd = xian_dj_tele_op_y_client_cmd;
            client_data.xian_dj_tele_op_a_client_cmd = xian_dj_tele_op_a_client_cmd;
            client_data.xian_dj_tele_op_left_rocker_x_client_cmd = xian_dj_tele_op_left_rocker_x_client_cmd;
            client_data.xian_dj_tele_op_left_rocker_y_client_cmd = xian_dj_tele_op_left_rocker_y_client_cmd;
            client_data.xian_dj_tele_op_right_rocker_x_client_cmd = xian_dj_tele_op_right_rocker_x_client_cmd;
            client_data.xian_dj_tele_op_right_rocker_y_client_cmd = xian_dj_tele_op_right_rocker_y_client_cmd;
            client_data.xian_dj_tele_op_r1_client_cmd = xian_dj_tele_op_r1_client_cmd;
            client_data.xian_dj_tele_op_r2_client_cmd = xian_dj_tele_op_r2_client_cmd;
            client_data.xian_dj_tele_op_l1_client_cmd = xian_dj_tele_op_l1_client_cmd;
            client_data.xian_dj_tele_op_l2_client_cmd = xian_dj_tele_op_l2_client_cmd;


            // printf("client_heart_beat=%d, a= %d, b= %d,c= %d, d= %d,e= %d, f= %d,g= %d, h= %d,i= %d, j= %d,k= %d, l= %d,m= %d, n= %d,o= %d, p= %d \n", 
            //                                                     client_data.xian_dj_tele_op_controller_client_tcp_heart_beat, 
            //                                                     client_data.xian_dj_tele_op_left_client_cmd, 
            //                                                     client_data.xian_dj_tele_op_right_client_cmd,
            //                                                     client_data.xian_dj_tele_op_up_client_cmd,
            //                                                     client_data.xian_dj_tele_op_down_client_cmd,
            //                                                     client_data.xian_dj_tele_op_x_client_cmd,
            //                                                     client_data.xian_dj_tele_op_b_client_cmd,
            //                                                     client_data.xian_dj_tele_op_y_client_cmd,
            //                                                     client_data.xian_dj_tele_op_a_client_cmd,
            //                                                     client_data.xian_dj_tele_op_left_rocker_x_client_cmd,
            //                                                     client_data.xian_dj_tele_op_left_rocker_y_client_cmd,
            //                                                     client_data.xian_dj_tele_op_right_rocker_x_client_cmd,
            //                                                     client_data.xian_dj_tele_op_right_rocker_y_client_cmd,
            //                                                     client_data.xian_dj_tele_op_r1_client_cmd,
            //                                                     client_data.xian_dj_tele_op_r2_client_cmd,
            //                                                     client_data.xian_dj_tele_op_l1_client_cmd,
            //                                                     client_data.xian_dj_tele_op_l2_client_cmd
            //                                                 );
            if (!client.sendData(client_data)) 
            {
                return -1;
            }
            
            // 接收响应（带3秒超时）
            if (client.receiveData(server_data)) 
            {
                xian_dj_tele_op_controller_server_tcp_heart_beat = server_data.xian_dj_tele_op_controller_server_tcp_heart_beat;
                // ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_tcp_heart_beat", xian_dj_tele_op_controller_server_tcp_heart_beat); 
                printf("返回的结果：server_heart_beat=%d, \n", xian_dj_tele_op_controller_server_tcp_heart_beat);
            }
            else
            {
                xian_dj_tele_op_controller_server_tcp_heart_beat = 0;
                printf("recieve server data failed \n");
            }
            pub_msg.data = xian_dj_tele_op_controller_server_tcp_heart_beat;
            xian_dj_tele_op_controller_client_pub.publish(pub_msg);
            return 1;
        }


};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_tele_op_controller_client");
    XianDjTeleOpControllerClient xian_dj_tele_op_controller_client;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_tele_op_controller_client.m_timer_control = nh_2.createWallTimer(ros::WallDuration(0.1), &XianDjTeleOpControllerClient::m_timer_control_func, &xian_dj_tele_op_controller_client);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}