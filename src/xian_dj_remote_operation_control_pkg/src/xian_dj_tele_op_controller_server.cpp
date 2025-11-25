#include<ros/ros.h>
#include<stdio.h>
#include<sys/types.h>

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <errno.h>
#include <poll.h>    // 新增：用于poll相关功能
#include <fcntl.h>   // 新增：用于文件控制操作
// #define PORT 3241
// #define BACKLOG 2048

// 接收client的数据
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

// server发送的数据
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

class TCPServer 
{
private:
    int server_fd;              // 服务器套接字描述符
    struct sockaddr_in server_addr;  // 服务器地址结构
    bool is_running;            // 服务器运行状态标志
    int port_;
    int BACKLOG_;

    // 使用vector管理多个客户端连接
    std::vector<int> client_fds; 
    fd_set active_fds;          // select 使用的文件描述符集合
    int max_fd;                 // 当前最大的文件描述符

public:
    // 构造函数
    TCPServer(int port, int BACKLOG) : port_(port), BACKLOG_(BACKLOG), server_fd(-1), is_running(false) 
    {
        memset(&server_addr, 0, sizeof(server_addr));
        FD_ZERO(&active_fds);
        max_fd = -1;
        client_fds.clear();
    }
    
    // 析构函数
    ~TCPServer() 
    {
        stop();
    }
    
    // 初始化服务器
    bool init() 
    {
        // 1. 创建套接字
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        {
            std::cerr << "创建套接字失败: " << strerror(errno) << std::endl;
            return false;
        }

        // 设置发送和接收超时
        struct timeval timeout;
        timeout.tv_sec = 3;  // 秒
        timeout.tv_usec = 0;
        
        if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
        {
            std::cerr << "设置接收超时失败: " << strerror(errno) << std::endl;
        }
        if (setsockopt(server_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) 
        {
            std::cerr << "设置发送超时失败: " << strerror(errno) << std::endl;
        }
        
        // 2. 设置SO_REUSEADDR选项，解决TIME_WAIT状态导致的绑定失败问题[4](@ref)
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
        {
            std::cerr << "设置SO_REUSEADDR失败: " << strerror(errno) << std::endl;
            close(server_fd);
            return false;
        }
        
        // 3. 配置服务器地址
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port_);
        
        return true;
    }
    
    // 启动服务器
    bool start() {
        if (server_fd == -1 && !init()) 
        {
            return false;
        }
        
        // 4. 绑定套接字到端口
        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) 
        {
            std::cerr << "绑定失败: " << strerror(errno) << std::endl;
            return false;
        }
        
        // 5. 开始监听
        if (listen(server_fd, BACKLOG_) < 0) {
            std::cerr << "监听失败: " << strerror(errno) << std::endl;
            return false;
        }
        
        // 将服务器socket加入监控集合
        FD_SET(server_fd, &active_fds);
        max_fd = server_fd;
        
        is_running = true;
        std::cout << "服务器启动成功，监听端口 " << port_ << "..." << std::endl;
        
        return true;
    }
    
    // 停止服务器
    void stop() 
    {
        is_running = false;
        
        // 关闭所有客户端连接
        for (int fd : client_fds) 
        {
            if (fd != -1) 
            {
                close(fd);
            }
        }
        client_fds.clear();
        
        if (server_fd != -1) 
        {
            close(server_fd);
            server_fd = -1;
        }
        
        FD_ZERO(&active_fds);
        max_fd = -1;
        
        std::cout << "服务器已停止" << std::endl;
    }
    
    // 接受客户端连接，并进行健康检查[5](@ref)
    bool acceptClient() 
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int new_client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (new_client_fd < 0) 
        {
            std::cerr << "接受连接失败: " << strerror(errno) << std::endl;
            return false;
        }

        // 健康检查：使用poll立即检查连接是否可用[5](@ref)
        struct pollfd pfd;
        pfd.fd = new_client_fd;
        pfd.events = POLLIN | POLLHUP | POLLERR;
        pfd.revents = 0;
        
        // 超时设为0，立即返回
        if (poll(&pfd, 1, 0) > 0 && (pfd.revents & (POLLHUP | POLLERR))) 
        {
            std::cout << "检测到幽灵连接，已关闭" << std::endl;
            close(new_client_fd);
            return false;
        }
        
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        std::cout << "客户端连接成功：" << client_ip << ":" << ntohs(client_addr.sin_port) << "，FD: " << new_client_fd << std::endl;
        
        // 设置新客户端socket为非阻塞模式以适应超时设置
        int flags = fcntl(new_client_fd, F_GETFL, 0);
        fcntl(new_client_fd, F_SETFL, flags | O_NONBLOCK);
        
        // 添加到客户端列表和监控集合
        client_fds.push_back(new_client_fd);
        FD_SET(new_client_fd, &active_fds);
        if (new_client_fd > max_fd) 
        {
            max_fd = new_client_fd;
        }
        
        return true;
    }
    
    // 接收客户端数据
    int receiveData(int client_fd, client2server& client_data) 
    {
        if (client_fd == -1) 
        {
            return -1;
        }
        
        memset(&client_data, 0, sizeof(client_data)); // 清空结构体
        ssize_t recv_len = recv(client_fd, &client_data, sizeof(client_data), 0);
        
        if (recv_len <= 0) 
        {
            if (recv_len == 0) 
            {
                std::cout << "客户端 " << client_fd << " 正常断开连接" << std::endl;
            } 
            else 
            {
                // 处理非阻塞模式下的错误
                if (errno == EAGAIN || errno == EWOULDBLOCK) 
                {
                    return 0; // 超时，没有数据可读，不是错误
                }
                std::cerr << "从客户端 " << client_fd << " 接收错误: " << strerror(errno) << std::endl;
            }
            return -1;
        }
        
        // 检查是否收到完整的数据包
        if (recv_len != sizeof(client_data)) 
        {
            std::cerr << "从客户端 " << client_fd << " 接收数据不完整，期望" 
                      << sizeof(client_data) << "字节，实际" << recv_len << "字节" << std::endl;
            return -1;
        }
        
        return recv_len;
    }
    
    // 发送数据到客户端，确保完整发送[4](@ref)
    bool sendData(int client_fd, server2client& server_data) 
    {
        if (client_fd == -1) 
        {
            return false;
        }
        
        size_t total_sent = 0;
        const char* data_ptr = reinterpret_cast<const char*>(&server_data);
        size_t data_size = sizeof(server_data);
        
        while (total_sent < data_size) 
        {
            ssize_t sent = send(client_fd, data_ptr + total_sent, data_size - total_sent, 0);
            if (sent < 0) 
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) 
                {
                    // 超时，重试
                    usleep(10000); // 10ms
                    continue;
                }
                std::cerr << "向客户端 " << client_fd << " 发送失败: " << strerror(errno) << std::endl;
                return false;
            }
            total_sent += sent;
        }
        
        return true;
    }
    
    // 关闭特定客户端连接
    void closeClient(int client_fd) 
    {
        if (client_fd != -1) 
        {
            // 从监控集合中移除
            FD_CLR(client_fd, &active_fds);
            
            // 从客户端列表中移除
            for (auto it = client_fds.begin(); it != client_fds.end(); ++it) 
            {
                if (*it == client_fd) 
                {
                    client_fds.erase(it);
                    break;
                }
            }
            
            // 关闭socket
            close(client_fd);
            std::cout << "已关闭客户端连接 FD: " << client_fd << std::endl;
            
            // 更新max_fd
            if (client_fd == max_fd) 
            {
                max_fd = server_fd;
                for (int fd : client_fds) 
                {
                    if (fd > max_fd) 
                    {
                        max_fd = fd;
                    }
                }
            }
        }
    }
    
    // 获取所有客户端文件描述符
    const std::vector<int>& getClientFDs() const 
    {
        return client_fds;
    }
    
    // 检查服务器是否运行
    bool isRunning() const 
    {
        return is_running;
    }
    
    // 获取准备就绪的文件描述符
    int waitForEvents(struct timeval* timeout) 
    {
        fd_set read_fds = active_fds; // 复制监控集合
        int ready = select(max_fd + 1, &read_fds, NULL, NULL, timeout);
        
        if (ready < 0 && errno != EINTR) 
        {
            std::cerr << "select错误: " << strerror(errno) << std::endl;
            return -1;
        }
        
        // 检查服务器socket是否有新连接
        if (FD_ISSET(server_fd, &read_fds)) 
        {
            acceptClient();
            ready--;
        }
        
        return ready;
    }
    
    // 获取可读的客户端列表
    void getReadyClients(fd_set& read_fds, std::vector<int>& ready_clients) 
    {
        ready_clients.clear();
        for (int fd : client_fds) 
        {
            if (FD_ISSET(fd, &read_fds)) 
            {
                ready_clients.push_back(fd);
            }
        }
    }
};



class XianDjTeleOpControllerServer
{
    public:
        XianDjTeleOpControllerServer()
        {
            // 创建一个ROS节点句柄
            // ros::NodeHandle nh;

            // 启动服务器
            if (!server.start()) 
            {
                return ;
            }
        }

        // ros::WallTimer m_timer_heart_beat;
        ros::WallTimer m_timer_control;

        // void m_timer_heart_beat_func(const ros::WallTimerEvent& event)
        // {
        //     ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_heart_beat", xian_dj_tele_op_controller_server_heart_beat); 
        //     std::cout << "xian_dj_tele_op_controller_server_heart_beat: " << xian_dj_tele_op_controller_server_heart_beat << std::endl;
        //     counter = counter > 1000 ? 0 : (counter + 1);
        //     ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_heart_beat", counter);  // 自行替换
        // }

        void m_timer_control_func(const ros::WallTimerEvent& event)
        {
            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_heart_beat", xian_dj_tele_op_controller_server_heart_beat); 
            std::cout << "xian_dj_tele_op_controller_server_heart_beat: " << xian_dj_tele_op_controller_server_heart_beat << std::endl;
            counter = counter > 1000 ? 0 : (counter + 1);
            ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_server_heart_beat", counter);  // 自行替换

            ros::param::get("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_tcp_error", xian_dj_tele_op_controller_client_tcp_error); 
            if(xian_dj_tele_op_controller_client_tcp_error != 0)
            {
                // 什么也不用做，等待
            }
            else
            {
                this->command_callback();
            }
            
        }

        void someMethod() 
        {
            auto now = std::chrono::system_clock::now();
            auto secs = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        now.time_since_epoch()) % 1000;
            std::cout << "HH:MM:SS.mmm: "
                    << std::put_time(std::localtime(&secs), "%H:%M:%S")
                    << '.' << std::setfill('0') << std::setw(3) << ms.count() << '\n';
        }

    private:
        int counter = 0;
        int xian_dj_tele_op_controller_server_heart_beat = 0;
        int xian_dj_tele_op_controller_client_tcp_heart_beat = 0;
        int xian_dj_tele_op_controller_client_tcp_error = 0;

         // 声明鲁班猫接收到的变量
        int xian_dj_tele_op_left_server_cmd;
        int xian_dj_tele_op_right_server_cmd;
        int xian_dj_tele_op_up_server_cmd;
        int xian_dj_tele_op_down_server_cmd;
        int xian_dj_tele_op_x_server_cmd;
        int xian_dj_tele_op_b_server_cmd;
        int xian_dj_tele_op_y_server_cmd;
        int xian_dj_tele_op_a_server_cmd;
        double xian_dj_tele_op_left_rocker_x_server_cmd;
        double xian_dj_tele_op_left_rocker_y_server_cmd;
        double xian_dj_tele_op_right_rocker_x_server_cmd;
        double xian_dj_tele_op_right_rocker_y_server_cmd;
        int xian_dj_tele_op_r1_server_cmd;
        int xian_dj_tele_op_r2_server_cmd;
        int xian_dj_tele_op_l1_server_cmd;
        int xian_dj_tele_op_l2_server_cmd;


        TCPServer server{3241, 2048};
        client2server client_data;
        server2client server_data;
        
        int command_callback()
        {
            // // 接受客户端连接
            // if (!server.acceptClient()) 
            // {
            //     return -1;
            // }

            // 设置select超时
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        // 等待事件发生
        int ready = server.waitForEvents(&timeout);
        
        if (ready < 0) 
        {
            // 严重错误，退出循环
            return -1;
        }
        
        if (ready == 0) 
        {
            // 超时，没有事件发生，可以执行一些后台任务
            return -1;
        }
        
        // 处理所有已连接的客户端
        std::vector<int> client_fds = server.getClientFDs();
        for (int client_fd : client_fds) 
        {
            // 接收数据（非阻塞）
            int recv_len = server.receiveData(client_fd, client_data);
            if (recv_len > 0) 
            {
                // printf("收到数据：xian_dj_tele_op_controller_client_tcp_heart_beat= %d \n", client_data.xian_dj_tele_op_controller_client_tcp_heart_beat);
                // printf("收到数据：xian_dj_tele_op_left_client_cmd = %d \n", client_data.xian_dj_tele_op_left_client_cmd);
                // printf("收到数据：xian_dj_tele_op_right_client_cmd = %d \n", client_data.xian_dj_tele_op_right_client_cmd);
                // printf("收到数据：xian_dj_tele_op_up_client_cmd = %d \n", client_data.xian_dj_tele_op_up_client_cmd);
                // printf("收到数据：xian_dj_tele_op_down_client_cmd= %d \n", client_data.xian_dj_tele_op_down_client_cmd);
                // printf("收到数据：xian_dj_tele_op_x_client_cmd= %d \n", client_data.xian_dj_tele_op_x_client_cmd);
                // printf("收到数据：xian_dj_tele_op_b_client_cmd= %d \n", client_data.xian_dj_tele_op_b_client_cmd);
                // printf("收到数据：xian_dj_tele_op_y_client_cmd= %d \n", client_data.xian_dj_tele_op_y_client_cmd);
                // printf("收到数据：xian_dj_tele_op_a_client_cmd= %d \n", client_data.xian_dj_tele_op_a_client_cmd);
                printf("收到数据：xian_dj_tele_op_left_rocker_x_client_cmd= %f \n", client_data.xian_dj_tele_op_left_rocker_x_client_cmd);
                // printf("收到数据：xian_dj_tele_op_left_rocker_y_client_cmd= %f \n", client_data.xian_dj_tele_op_left_rocker_y_client_cmd);
                // printf("收到数据：xian_dj_tele_op_right_rocker_x_client_cmd= %f \n", client_data.xian_dj_tele_op_right_rocker_x_client_cmd);
                // printf("收到数据：xian_dj_tele_op_right_rocker_y_client_cmd= %f \n", client_data.xian_dj_tele_op_right_rocker_y_client_cmd);
                // printf("收到数据：xian_dj_tele_op_r1_client_cmd= %d \n", client_data.xian_dj_tele_op_r1_client_cmd);
                // printf("收到数据：xian_dj_tele_op_r2_client_cmd= %d \n", client_data.xian_dj_tele_op_r2_client_cmd);
                // printf("收到数据：xian_dj_tele_op_l1_client_cmd= %d \n", client_data.xian_dj_tele_op_l1_client_cmd);
                // printf("收到数据：xian_dj_tele_op_l2_client_cmd= %d \n", client_data.xian_dj_tele_op_l2_client_cmd);
                someMethod();

                xian_dj_tele_op_controller_client_tcp_heart_beat = client_data.xian_dj_tele_op_controller_client_tcp_heart_beat;
                xian_dj_tele_op_left_server_cmd = client_data.xian_dj_tele_op_left_client_cmd;
                xian_dj_tele_op_right_server_cmd = client_data.xian_dj_tele_op_right_client_cmd;
                xian_dj_tele_op_up_server_cmd = client_data.xian_dj_tele_op_up_client_cmd;
                xian_dj_tele_op_down_server_cmd = client_data.xian_dj_tele_op_down_client_cmd;
                xian_dj_tele_op_x_server_cmd = client_data.xian_dj_tele_op_x_client_cmd;
                xian_dj_tele_op_b_server_cmd = client_data.xian_dj_tele_op_b_client_cmd;
                xian_dj_tele_op_y_server_cmd = client_data.xian_dj_tele_op_y_client_cmd;
                xian_dj_tele_op_a_server_cmd = client_data.xian_dj_tele_op_a_client_cmd;
                xian_dj_tele_op_left_rocker_x_server_cmd = client_data.xian_dj_tele_op_left_rocker_x_client_cmd;
                xian_dj_tele_op_left_rocker_y_server_cmd = client_data.xian_dj_tele_op_left_rocker_y_client_cmd;
                xian_dj_tele_op_right_rocker_x_server_cmd = client_data.xian_dj_tele_op_right_rocker_x_client_cmd;
                xian_dj_tele_op_right_rocker_y_server_cmd = client_data.xian_dj_tele_op_right_rocker_y_client_cmd;
                xian_dj_tele_op_r1_server_cmd = client_data.xian_dj_tele_op_r1_client_cmd;
                xian_dj_tele_op_r2_server_cmd = client_data.xian_dj_tele_op_r2_client_cmd;
                xian_dj_tele_op_l1_server_cmd = client_data.xian_dj_tele_op_l1_client_cmd;
                xian_dj_tele_op_l2_server_cmd = client_data.xian_dj_tele_op_l2_client_cmd;
                // set param server
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_server_cmd", xian_dj_tele_op_left_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_server_cmd", xian_dj_tele_op_right_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_up_server_cmd", xian_dj_tele_op_up_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_down_server_cmd", xian_dj_tele_op_down_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_x_server_cmd", xian_dj_tele_op_x_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_b_server_cmd", xian_dj_tele_op_b_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_y_server_cmd", xian_dj_tele_op_y_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_a_server_cmd", xian_dj_tele_op_a_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_x_server_cmd", xian_dj_tele_op_left_rocker_x_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_y_server_cmd", xian_dj_tele_op_left_rocker_y_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_x_server_cmd", xian_dj_tele_op_right_rocker_x_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_y_server_cmd", xian_dj_tele_op_right_rocker_y_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_r1_server_cmd", xian_dj_tele_op_r1_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_r2_server_cmd", xian_dj_tele_op_r2_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_l1_server_cmd", xian_dj_tele_op_l1_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_l2_server_cmd", xian_dj_tele_op_l2_server_cmd); 
                ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_tcp_heart_beat", xian_dj_tele_op_controller_client_tcp_heart_beat); 
                

                // 发送响应
                server_data.xian_dj_tele_op_controller_server_tcp_heart_beat = xian_dj_tele_op_controller_server_heart_beat;
                if (!server.sendData(client_fd, server_data)) 
                {
                    std::cout << "向客户端 " << client_fd << " 发送失败，关闭连接" << std::endl;
                    server.closeClient(client_fd);
                }

            }
            else if (recv_len < 0) 
            {
                // 连接出错或断开
                server.closeClient(client_fd);
            }
            // recv_len == 0 表示超时，没有数据，继续处理其他客户端


        }
        // 短暂休眠，避免CPU占用过高
        usleep(10000); // 10ms
        return 0;




            
        //     // 数据交互循环
        //     while (true) 
        //     {
        //         usleep(100 * 1000); // 100 ms
        //         // 接收数据
        //         int recv_len = server.receiveData(client_data);
        //         if (recv_len <= 0) 
        //         {
        //             break;
        //         }
        //         printf("收到数据：xian_dj_tele_op_controller_client_tcp_heart_beat= %d \n", client_data.xian_dj_tele_op_controller_client_tcp_heart_beat);
        //         printf("收到数据：xian_dj_tele_op_left_client_cmd = %d \n", client_data.xian_dj_tele_op_left_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_right_client_cmd = %d \n", client_data.xian_dj_tele_op_right_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_up_client_cmd = %d \n", client_data.xian_dj_tele_op_up_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_down_client_cmd= %d \n", client_data.xian_dj_tele_op_down_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_x_client_cmd= %d \n", client_data.xian_dj_tele_op_x_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_b_client_cmd= %d \n", client_data.xian_dj_tele_op_b_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_y_client_cmd= %d \n", client_data.xian_dj_tele_op_y_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_a_client_cmd= %d \n", client_data.xian_dj_tele_op_a_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_left_rocker_x_client_cmd= %f \n", client_data.xian_dj_tele_op_left_rocker_x_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_left_rocker_y_client_cmd= %f \n", client_data.xian_dj_tele_op_left_rocker_y_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_right_rocker_x_client_cmd= %f \n", client_data.xian_dj_tele_op_right_rocker_x_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_right_rocker_y_client_cmd= %f \n", client_data.xian_dj_tele_op_right_rocker_y_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_r1_client_cmd= %d \n", client_data.xian_dj_tele_op_r1_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_r2_client_cmd= %d \n", client_data.xian_dj_tele_op_r2_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_l1_client_cmd= %d \n", client_data.xian_dj_tele_op_l1_client_cmd);
        //         printf("收到数据：xian_dj_tele_op_l2_client_cmd= %d \n", client_data.xian_dj_tele_op_l2_client_cmd);

        //         xian_dj_tele_op_controller_client_tcp_heart_beat = client_data.xian_dj_tele_op_controller_client_tcp_heart_beat;
        //         xian_dj_tele_op_left_server_cmd = client_data.xian_dj_tele_op_left_client_cmd;
        //         xian_dj_tele_op_right_server_cmd = client_data.xian_dj_tele_op_right_client_cmd;
        //         xian_dj_tele_op_up_server_cmd = client_data.xian_dj_tele_op_up_client_cmd;
        //         xian_dj_tele_op_down_server_cmd = client_data.xian_dj_tele_op_down_client_cmd;
        //         xian_dj_tele_op_x_server_cmd = client_data.xian_dj_tele_op_x_client_cmd;
        //         xian_dj_tele_op_b_server_cmd = client_data.xian_dj_tele_op_b_client_cmd;
        //         xian_dj_tele_op_y_server_cmd = client_data.xian_dj_tele_op_y_client_cmd;
        //         xian_dj_tele_op_a_server_cmd = client_data.xian_dj_tele_op_a_client_cmd;
        //         xian_dj_tele_op_left_rocker_x_server_cmd = client_data.xian_dj_tele_op_left_rocker_x_client_cmd;
        //         xian_dj_tele_op_left_rocker_y_server_cmd = client_data.xian_dj_tele_op_left_rocker_y_client_cmd;
        //         xian_dj_tele_op_right_rocker_x_server_cmd = client_data.xian_dj_tele_op_right_rocker_x_client_cmd;
        //         xian_dj_tele_op_right_rocker_y_server_cmd = client_data.xian_dj_tele_op_right_rocker_y_client_cmd;
        //         xian_dj_tele_op_r1_server_cmd = client_data.xian_dj_tele_op_r1_client_cmd;
        //         xian_dj_tele_op_r2_server_cmd = client_data.xian_dj_tele_op_r2_client_cmd;
        //         xian_dj_tele_op_l1_server_cmd = client_data.xian_dj_tele_op_l1_client_cmd;
        //         xian_dj_tele_op_l2_server_cmd = client_data.xian_dj_tele_op_l2_client_cmd;
        //         // set param server
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_server_cmd", xian_dj_tele_op_left_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_server_cmd", xian_dj_tele_op_right_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_up_server_cmd", xian_dj_tele_op_up_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_down_server_cmd", xian_dj_tele_op_down_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_x_server_cmd", xian_dj_tele_op_x_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_b_server_cmd", xian_dj_tele_op_b_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_y_server_cmd", xian_dj_tele_op_y_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_a_server_cmd", xian_dj_tele_op_a_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_x_server_cmd", xian_dj_tele_op_left_rocker_x_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_left_rocker_y_server_cmd", xian_dj_tele_op_left_rocker_y_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_x_server_cmd", xian_dj_tele_op_right_rocker_x_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_right_rocker_y_server_cmd", xian_dj_tele_op_right_rocker_y_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_r1_server_cmd", xian_dj_tele_op_r1_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_r2_server_cmd", xian_dj_tele_op_r2_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_l1_server_cmd", xian_dj_tele_op_l1_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_l2_server_cmd", xian_dj_tele_op_l2_server_cmd); 
        //         ros::param::set("/xian_dj_tele_op_params_server/xian_dj_tele_op_controller_client_tcp_heart_beat", xian_dj_tele_op_controller_client_tcp_heart_beat); 

        //         // 构造并发送响应
        //         server_data.xian_dj_tele_op_controller_server_tcp_heart_beat = xian_dj_tele_op_controller_server_heart_beat;
        //         printf("发送client的数据：xian_dj_tele_op_controller_server_tcp_heart_beat= %d \n", server_data.xian_dj_tele_op_controller_server_tcp_heart_beat);
        //         if (!server.sendData(server_data)) 
        //         {
        //             break;
        //         }
        //     }
            
        //     // 关闭当前客户端连接
        //     server.closeClient();
        //     return 0;
        }

};

int main(int argc, char** argv)
{
    //initial and name node
    ros::init(argc,argv,"xian_dj_tele_op_controller_server");
    XianDjTeleOpControllerServer xian_dj_tele_op_controller_server;

    // 创建一个ROS节点句柄
    ros::NodeHandle nh_2;
    ros::AsyncSpinner spinner(0);
    spinner.start();

    // xian_dj_tele_op_controller_server.m_timer_heart_beat = nh_2.createWallTimer(ros::WallDuration(1.0), &XianDjTeleOpControllerServer::m_timer_heart_beat_func, &xian_dj_tele_op_controller_server);
    xian_dj_tele_op_controller_server.m_timer_control = nh_2.createWallTimer(ros::WallDuration(0.2), &XianDjTeleOpControllerServer::m_timer_control_func, &xian_dj_tele_op_controller_server);
    ros::waitForShutdown();
    
    // ros::spin();
    return 0;
}