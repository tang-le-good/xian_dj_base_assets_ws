#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>
#include <array>
#include <ros/callback_queue.h>
#include "boost/thread.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstdint>

// #define SERVER_IP "192.168.1.132"
// #define SERVER_PORT 7886
#define MAX_RETRY 900000000 
#define RETRY_INTERVAL 3 // 重试间隔(秒)     3秒重连1次，900000000次，时间大概是10年

// client发送的数据
struct client2server 
{
    unsigned char address_0; 
    unsigned char address_1;
    unsigned char address_2; 
    unsigned char address_3; 
    unsigned char address_4;
    unsigned char address_5; 
    unsigned char address_6; 
    unsigned char address_7;
    // unsigned char address_8; 
    // unsigned char address_9; 
    // unsigned char address_10;
    // unsigned char address_11;
    // unsigned char address_12; 
    // unsigned char address_13; 
    // unsigned char address_14;
    // unsigned char address_15; 
    // unsigned char address_16; 
    // unsigned char address_17;
    // unsigned char address_18; 
    // unsigned char address_19; 
    // unsigned char address_20;
    
};

// 接收到的server的数据
struct server2client
{
    unsigned char address_0; 
    unsigned char address_1;
    unsigned char address_2; 
    unsigned char address_3; 
    unsigned char address_4;
    unsigned char address_5; 
    unsigned char address_6; 
    unsigned char address_7;
    // unsigned char address_8; 
    // unsigned char address_9; 
    // unsigned char address_10;
    // unsigned char address_11;
    // unsigned char address_12; 
    // unsigned char address_13; 
    // unsigned char address_14;
    // unsigned char address_15; 
    // unsigned char address_16; 
    // unsigned char address_17;
    // unsigned char address_18; 
    // unsigned char address_19; 
    // unsigned char address_20; 

};

class TCPClient 
{
    private:
        int sockfd;
        struct sockaddr_in server_addr;
        bool connected;
        std::string server_ip_;
        int server_port_;
        
    public:
        TCPClient(const std::string& ip, int port) : server_ip_(ip), server_port_(port), sockfd(-1), connected(false) 
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
        
        bool connectToServer(int max_retry = MAX_RETRY) 
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
            
            if (recv_len <= 0) {
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

class XianDjCarChassisBaseElectric
{

    public:
        XianDjCarChassisBaseElectric(const std::string& ip, int port, const std::string& motor_driver):server_ip_(ip), server_port_(port), motor_driver_(motor_driver)
        {
            std::cout << "xian_dj_car_chassis_base_electric:  节点已启动" << std::endl;
            // server_ip_ = ip;
            // server_port_ = port;
            init();
        }
        ros::WallTimer m_timer_Main_Func;
        ros::WallTimer m_timer_HeartBeat;

        void m_timer_Main_Func_f(const ros::WallTimerEvent& event)
        {
            // state
            client_address_0 = 0x02; 
            client_address_1 = 0x03;
            client_address_2 = 0x00; 
            client_address_3 = 0xE3; 
            client_address_4 = 0x00;
            client_address_5 = 0x01; 
            tcp_server_date = this->command_callback(client_address_0, client_address_1,client_address_2,
                                                     client_address_3, client_address_4,client_address_5);
            if(tcp_server_date!=nullptr)
            {
                unsigned char state_low = *(tcp_server_date+4);
                auto result = convertByteToBools(state_low);
                printf("运行状态： %d, 过流： %d, 过压： %d, 编码器故障： %d, 位置偏差过大： %d, 欠压： %d, 过载标志： %d, 外部控制标志： %d  \n",
                                result[0], result[1],result[2],result[3],result[4],result[5],result[6],result[7]);
                
                if(result[0]==0) // 如果没有使能
                {
                    client_address_0 = 0x02; 
                    client_address_1 = 0x06;
                    client_address_2 = 0x00; 
                    client_address_3 = 0x10; 
                    client_address_4 = 0x00;
                    client_address_5 = 0x00; 
                    tcp_server_date = this->command_callback(client_address_0, client_address_1,client_address_2,
                                                            client_address_3, client_address_4,client_address_5);
                    // enable
                    client_address_0 = 0x02; 
                    client_address_1 = 0x06;
                    client_address_2 = 0x00; 
                    client_address_3 = 0x00; 
                    client_address_4 = 0x00;
                    client_address_5 = 0x01; 
                    
                    tcp_server_date = this->command_callback(client_address_0, client_address_1,client_address_2,
                                                            client_address_3, client_address_4,client_address_5);
                }
            }
            

            // velocity_read
            client_address_0 = 0x02; 
            client_address_1 = 0x03;
            client_address_2 = 0x00; 
            client_address_3 = 0xE4; 
            client_address_4 = 0x00;
            client_address_5 = 0x01; 
            tcp_server_date = this->command_callback(client_address_0, client_address_1,client_address_2,
                                                     client_address_3, client_address_4,client_address_5);
            int velocity_read = 0;
            if(tcp_server_date!=nullptr)
            {
                unsigned char velocity_read_high = *(tcp_server_date+3);
                unsigned char velocity_read_low = *(tcp_server_date+4);
                velocity_read = combineBytes(velocity_read_high, velocity_read_low);
                printf("读取速度 : %d \n", velocity_read);
            }
            

            // velocity_write
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_left_wheel_write_velocity", xian_dj_car_chassis_left_wheel_write_velocity); 
            ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_right_wheel_write_velocity", xian_dj_car_chassis_right_wheel_write_velocity); 
            int16_t write_velocity = 0;
            if(motor_driver_ == "left_driver")
            {
                write_velocity = xian_dj_car_chassis_left_wheel_write_velocity;
                ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_left_wheel_read_velocity", velocity_read); 
            }
            else if(motor_driver_ == "right_driver")
            {
                write_velocity = xian_dj_car_chassis_right_wheel_write_velocity;
                ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_right_wheel_read_velocity", velocity_read);
            }  

            unsigned char high_byte, low_byte;
            SplitInt16ToBytes(write_velocity, high_byte, low_byte);
            client_address_0 = 0x02; 
            client_address_1 = 0x06;
            client_address_2 = 0x00; 
            client_address_3 = 0x10; 
            client_address_4 = high_byte;
            client_address_5 = low_byte; 
            tcp_server_date = this->command_callback(client_address_0, client_address_1,client_address_2,
                                                     client_address_3, client_address_4,client_address_5);

            if(tcp_server_date!=nullptr)
            {
                // printf("Tcp Server Data: 0: %X, 1: %X, 2: %X, 3: %X, 4: %X, 5: %X \n", *(tcp_server_date+0), 
                //                                                                         *(tcp_server_date+1),
                //                                                                         *(tcp_server_date+2),
                //                                                                         *(tcp_server_date+3),
                //                                                                         *(tcp_server_date+4),
                //                                                                         *(tcp_server_date+5)
                //                                                                         );
            }
        }
        void m_timer_HeartBeat_f(const ros::WallTimerEvent& event)
        {
            date_recive_counter_pre = date_recive_counter_cur;
            date_recive_counter_cur = date_recive_counter;

            if(date_recive_counter_pre == date_recive_counter_cur)
            {
                timeout_counter += 1;
                timeout_counter = timeout_counter > 1000 ? 5 : (timeout_counter + 1);
            }
            else
            {
                timeout_counter = 0;
            }
            if(timeout_counter >= 5)
            {
                if(motor_driver_ == "left_driver")
                {
                    ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_left_heart_beat", xian_dj_car_chassis_base_electric_left_heart_beat); 
                    std::cout << "xian_dj_car_chassis_base_electric_left_heart_beat: " << xian_dj_car_chassis_base_electric_left_heart_beat << std::endl;
                    counter = 0;
                    ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_left_heart_beat", counter); 
                }
                else if(motor_driver_ == "right_driver")
                {
                    ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_right_heart_beat", xian_dj_car_chassis_base_electric_right_heart_beat); 
                    std::cout << "xian_dj_car_chassis_base_electric_right_heart_beat: " << xian_dj_car_chassis_base_electric_right_heart_beat << std::endl;
                    counter = 0;
                    ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_right_heart_beat", counter);
                }  
            }
            else
            {
                if(motor_driver_ == "left_driver")
                {
                    ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_left_heart_beat", xian_dj_car_chassis_base_electric_left_heart_beat); 
                    std::cout << "xian_dj_car_chassis_base_electric_left_heart_beat: " << xian_dj_car_chassis_base_electric_left_heart_beat << std::endl;
                    counter = counter > 1000 ? 0 : (counter + 1);
                    ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_left_heart_beat", counter); 
                }
                else if(motor_driver_ == "right_driver")
                {
                    ros::param::get("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_right_heart_beat", xian_dj_car_chassis_base_electric_right_heart_beat); 
                    std::cout << "xian_dj_car_chassis_base_electric_right_heart_beat: " << xian_dj_car_chassis_base_electric_right_heart_beat << std::endl;
                    counter = counter > 1000 ? 0 : (counter + 1);
                    ros::param::set("/xian_dj_car_chassis_params_server/xian_dj_car_chassis_base_electric_right_heart_beat", counter);
                } 
            }
        }
    private:
        int counter = 0;
        int date_recive_counter = 0;
        int timeout_counter = 0;
        int date_recive_counter_cur = 0;
        int date_recive_counter_pre = 0;
        int xian_dj_car_chassis_base_electric_left_heart_beat = 0;
        int xian_dj_car_chassis_base_electric_right_heart_beat = 0;
        std::string server_ip_;
        std::string motor_driver_;
        int server_port_;
        TCPClient client{server_ip_, server_port_};
        struct client2server client_data;
        struct server2client server_data;
        unsigned char client_address_0;
        unsigned char client_address_1;
        unsigned char client_address_2;
        unsigned char client_address_3;
        unsigned char client_address_4;
        unsigned char client_address_5;
        unsigned char * tcp_server_date;
        int xian_dj_car_chassis_left_wheel_write_velocity=0;
        int xian_dj_car_chassis_right_wheel_write_velocity=0;

    
               
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

        unsigned char* command_callback(unsigned char address_0, unsigned char address_1, unsigned char address_2, 
                                        unsigned char address_3, unsigned char address_4, unsigned char address_5)
        {
            static unsigned char tcp_date[8] ; 
            if (!client.isConnected()) 
            {
                std::cout << "连接已断开，尝试重新连接..." << std::endl;
                if (!client.connectToServer()) 
                {
                    std::this_thread::sleep_for(std::chrono::seconds(RETRY_INTERVAL));
                    return 0x00;
                }
            }
                
            // 发送数据
            // 电机使能
            client_data.address_0 = address_0;
            client_data.address_1 = address_1;
            client_data.address_2 = address_2;
            client_data.address_3 = address_3;
            client_data.address_4 = address_4;
            client_data.address_5 = address_5;            

            // 计算CRC校验值
            uint16_t crc = CalculateStructCRC(client_data);
            
            unsigned char high_byte, low_byte;
            SplitInt16ToBytes(crc, high_byte, low_byte);
            // 校验位
            client_data.address_6 = low_byte;
            client_data.address_7 = high_byte;

            // printf(" Client send: address_0=%X, address_1=%X, address_2=%X, address_3=%X, address_4=%X, address_5=%X, address_6=%X, address_7=%X \n", 
            //        client_data.address_0,
            //        client_data.address_1,
            //        client_data.address_2,
            //        client_data.address_3,
            //        client_data.address_4,
            //        client_data.address_5,
            //        client_data.address_6,
            //        client_data.address_7);

            if (!client.sendData(client_data)) 
            {
                return 0x00;
            }

            // 接收响应（带3秒超时）
            if (client.receiveData(server_data)) 
            {
                date_recive_counter = date_recive_counter > 1000 ? 0 : (date_recive_counter + 1);
                printf("date_recive_counter:%d \n", date_recive_counter);
                
                tcp_date[0] = server_data.address_0;
                tcp_date[1] = server_data.address_1;
                tcp_date[2] = server_data.address_2;
                tcp_date[3] = server_data.address_3;
                tcp_date[4] = server_data.address_4;
                tcp_date[5] = server_data.address_5;
                tcp_date[6] = server_data.address_6;
                tcp_date[7] = server_data.address_7;

                return tcp_date;
            } 
            else 
            {
                // 3秒超时后会执行到这里                
                return nullptr;
            }
        }

        // CRC校验
        uint16_t ModbusGetCRC(uint8_t* data, uint8_t count)
        {
            uint8_t i, j, xdabit;
            uint16_t reg_crc = 0xFFFF;  // CRC初始值
            
            // 遍历每个数据字节
            for (i = 0; i < count; i++)
            {
                reg_crc ^= (uint16_t)data[i];  // 与当前字节进行异或
                
                // 处理每个字节的8个位
                for (j = 0; j < 8; j++)
                {
                    xdabit = (uint8_t)(reg_crc & 0x01);  // 获取最低位
                    reg_crc >>= 1;  // 右移一位
                    
                    // 如果最低位为1，进行多项式异或
                    if (xdabit == 1)
                    {
                        reg_crc ^= 0xA001;  // Modbus CRC16多项式
                    }
                }
            }
            
            return reg_crc;
        }

        /**
        * 为client2server结构体计算CRC16校验值
        * @param data 结构体引用
        * @return CRC16校验值
        */
        uint16_t CalculateStructCRC(const client2server& data)
        {
            // 将结构体转换为字节数组进行CRC计算
            return ModbusGetCRC((uint8_t*)&data, 6);
        }

        /**
        * 将16位拆分为高字节和低字节
        * @param crc_value 16位CRC值
        * @param high_byte 返回的高字节
        * @param low_byte 返回的低字节
        */
        void SplitInt16ToBytes(uint16_t crc_value, unsigned char& high_byte, unsigned char& low_byte)
        {
            // 方法1：使用位操作（推荐）
            high_byte = (crc_value >> 8) & 0xFF;  // 获取高8位
            low_byte = crc_value & 0xFF;          // 获取低8位
        }

        /**
        * 将unsigned char变量的每一位转换为8个bool值
        * @param state_high 输入的8位无符号字符
        * @return 包含8个bool值的std::array，索引0对应最低位(LSB)，索引7对应最高位(MSB)
        */
        std::array<bool, 8> convertByteToBools(unsigned char state_high) {
            std::array<bool, 8> boolArray{};
            
            for (int i = 0; i < 8; ++i) {
                // 方法1：右移后与1进行按位与操作[1,4](@ref)
                boolArray[i] = (state_high >> i) & 1;
                
                // 方法2（等效）：使用位掩码检查特定位[2,6](@ref)
                // boolArray[i] = (state_high & (1 << i)) != 0;
            }
            
            return boolArray;
        }

        // 把2个把为拼成int16_t
        int16_t combineBytes(uint8_t velocity_read_high, uint8_t velocity_read_low)
        {
            // 方法1：高位在前 (Big-endian style)
            int16_t result = (velocity_read_high << 8) | velocity_read_low;
            return result;

            // 方法2：低位在前 (Little-endian style)，如果需要的话
            // int16_t result = (velocity_read_low << 8) | velocity_read_high;
            // return result;
        }

        

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_car_chassis_base_electric");

    

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;

    std::string server_ip_;
    std::string motor_driver_;
    int server_port_;

    ros::NodeHandle private_nh("~");
        
    // 从参数服务器获取私有参数[10](@ref)
    private_nh.param<int>("server_port", server_port_, 7886);
    private_nh.param<std::string>("server_ip", server_ip_, "192.168.1.132");
    private_nh.param<std::string>("motor_driver", motor_driver_, "left_driver");
    
    ROS_INFO("server_ip: %s, server_port: %d, motor_driver: %s", 
                server_ip_.c_str(), server_port_, motor_driver_.c_str());
    XianDjCarChassisBaseElectric xian_dj_car_chassis_base_electric_node{server_ip_, server_port_, motor_driver_};

    ros::AsyncSpinner spinner(0);
    spinner.start();

    xian_dj_car_chassis_base_electric_node.m_timer_HeartBeat = nh_2.createWallTimer(ros::WallDuration(1), &XianDjCarChassisBaseElectric::m_timer_HeartBeat_f, &xian_dj_car_chassis_base_electric_node);
    xian_dj_car_chassis_base_electric_node.m_timer_Main_Func = nh_2.createWallTimer(ros::WallDuration(0.02), &XianDjCarChassisBaseElectric::m_timer_Main_Func_f, &xian_dj_car_chassis_base_electric_node);

    ros::waitForShutdown();

    // ros::spin();
    return 0;
}