#!/usr/bin/env python3
#!coding=utf-8
import rospy
import serial
from typing import Dict
import time
from std_msgs.msg import Float32MultiArray
import socket
from xian_dj_stewart_platform_control_pkg.msg import xian_dj_stewart_platform_base_electric
from std_msgs.msg import UInt16
from dataclasses import dataclass
from typing import Optional

class TCPClient:
    def __init__(self, ip: str, port: int):
        self.server_ip = ip
        self.server_port = port
        self.sockfd: Optional[socket.socket] = None
        self.connected = False
        self.max_retry = 900000000
        self.retry_interval = 3  # 3s重连一次
    
    
    def __del__(self):
        self.disconnect()
    
    def connect_to_server(self) -> bool:
        retry_count = 0
        
        while retry_count < self.max_retry and not self.connected:
            try:
                # 创建套接字
                self.sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                # 设置超时选项
                self.sockfd.settimeout(5)  # 连接超时5秒
                
                # 尝试连接
                self.sockfd.connect((self.server_ip, self.server_port))
                self.connected = True
                print(f"成功连接到服务器 {self.server_ip}:{self.server_port}")
                return True
                
            except (socket.error, OSError) as e:
                print(f"连接失败，尝试重连 ({(retry_count + 1)}/{self.max_retry})... 错误: {e}")
                if self.sockfd:
                    self.sockfd.close()
                    self.sockfd = None
                retry_count += 1
                time.sleep(self.retry_interval)
                continue
        
        return False
    
    def disconnect(self):
        if self.sockfd:
            self.sockfd.close()
            self.sockfd = None
        self.connected = False
    
    def send_data(self, client_data) -> bool:
        if not self.connected and not self.connect_to_server():
            print("无法发送数据：未连接到服务器")
            return False
        
        try:
            # # 将数据打包成二进制格式
            # data_bytes = struct.pack(self.client_data_format,
            #                        client_data.client_heart_beat,
            #                        client_data.a,
            #                        client_data.b)
            
            # 发送数据
            self.sockfd.sendall(client_data)
            return True
            
        except (socket.error, OSError) as e:
            print(f"发送数据失败: {e}")
            self.connected = False
            return False
    
    def receive_data(self):
        if not self.connected:
            return None
        
        try:
            # 设置超时3秒
            self.sockfd.settimeout(3.0)
            
            # 接收数据
            data = self.sockfd.recv(1024)
            if not data:
                print("接收数据失败或连接已关闭")
                self.connected = False
                return None
            else:
                return len(data)
            
                
        except socket.timeout:
            print("接收数据超时（3秒）")
            return None
        except (socket.error, OSError) as e:
            print(f"接收数据错误: {e}")
            self.connected = False
            return None
    
    def is_connected(self) -> bool:
        return self.connected

     
class XianDjStewartPlatformBaseElectric:
    def __init__(self):
        rospy.init_node('xian_dj_stewart_platform_base_electric', anonymous=True)

        # 订阅话题
        rospy.Subscriber("xian_dj_stewart_platform_base_electric_msg", xian_dj_stewart_platform_base_electric, self.callback)
        # 发布话题
        self.pub_msg = rospy.Publisher("xian_dj_stewart_platform_base_electric_state_msg", UInt16, queue_size=1)
        self.heart_beat_msg = UInt16()

        
        #TCP参数
        self.server_ip   = '192.168.1.11'   # NE2-T1 静态 IP
        self.server_port = 8886             # NE2-T1 本地端口
        self.retry_interval = 3
        self.client = TCPClient(self.server_ip, self.server_port)
        self.init_tcp()
        
        # 舵机控制命令数据结构
        self.init_servo_commands()
        
        # 心跳计数器
        self.counter = 0
        self.xian_dj_stewart_platform_base_electric_heart_beat = 0

        self.xian_dj_stewart_platform_arm1_cmd = 0
        self.xian_dj_stewart_platform_arm2_cmd = 0
        self.xian_dj_stewart_platform_arm3_cmd = 0
        self.xian_dj_stewart_platform_arm4_cmd = 0
        self.xian_dj_stewart_platform_arm5_cmd = 0
        self.xian_dj_stewart_platform_arm6_cmd = 0

        # 请求电压数据结构
        self.CMD_REQ_BATTERY_VOLTAGE = {"frame_head_1": 0x55, # 帧头1，0x55固定不变
                                        "frame_head_2": 0x55, # 帧头2，0x55固定不变
                                        "message_length": 0x02, # 数据长度
                                        "mode": 0x0F,   #指令名：请求控制板电压指令
                                        }

    def callback(self, data):
        self.xian_dj_stewart_platform_arm1_cmd = data.xian_dj_stewart_platform_arm1_cmd
        self.xian_dj_stewart_platform_arm2_cmd = data.xian_dj_stewart_platform_arm2_cmd
        self.xian_dj_stewart_platform_arm3_cmd = data.xian_dj_stewart_platform_arm3_cmd
        self.xian_dj_stewart_platform_arm4_cmd = data.xian_dj_stewart_platform_arm4_cmd
        self.xian_dj_stewart_platform_arm5_cmd = data.xian_dj_stewart_platform_arm5_cmd
        self.xian_dj_stewart_platform_arm6_cmd = data.xian_dj_stewart_platform_arm6_cmd

    
    def xian_dj_stewart_platform_move_callback_fun(self, event):
        try:
            if not self.client.is_connected():
                print("连接已断开，尝试重新连接...")
                if not self.client.connect_to_server():
                    time.sleep(self.retry_interval)
                    return -1
                
            ttx_data = self.encode_CMD_REQ_BATTERY_VOLTAGE(self.CMD_REQ_BATTERY_VOLTAGE)
            if not self.client.send_data(ttx_data):
                print("send v to server failed!")

            if self.counter % 30 == 0:
                if not self.client.receive_data():
                    # 心跳
                    #     if self.counter > 1000:
                    #         self.counter = 0
                    #     self.counter += 1

                        
                    #     self.heart_beat_msg.data = self.counter
                    #     self.pub_msg.publish(self.heart_beat_msg)
                    # else:
                        self.counter = 30
                        self.heart_beat_msg.data = self.counter
                        self.pub_msg.publish(self.heart_beat_msg)
                        return -1


            # 设置响应时间
            param_time_high, param_time_low = self.int_to_hex_bytes(100)
            self.HEXA_CMD_SERVO_MOVE["param_time_low"] = param_time_low
            self.HEXA_CMD_SERVO_MOVE["param_time_high"] = param_time_high

            # 设置舵机1-6的旋转角度
            param_plus_high_num1, param_plus_low_num1 =self.angle_to_pulse(self.xian_dj_stewart_platform_arm1_cmd)
            param_plus_high_num2, param_plus_low_num2 =self.angle_to_pulse(self.xian_dj_stewart_platform_arm2_cmd * -1.0)
            param_plus_high_num3, param_plus_low_num3 =self.angle_to_pulse(self.xian_dj_stewart_platform_arm3_cmd)
            param_plus_high_num4, param_plus_low_num4 =self.angle_to_pulse(self.xian_dj_stewart_platform_arm4_cmd * -1.0)
            param_plus_high_num5, param_plus_low_num5 =self.angle_to_pulse(self.xian_dj_stewart_platform_arm5_cmd)
            param_plus_high_num6, param_plus_low_num6 =self.angle_to_pulse(self.xian_dj_stewart_platform_arm6_cmd * -1.0)
                    
            self.HEXA_CMD_SERVO_MOVE["param_plus_low_num1"] = param_plus_low_num1
            self.HEXA_CMD_SERVO_MOVE["param_plus_low_num2"] = param_plus_low_num2
            self.HEXA_CMD_SERVO_MOVE["param_plus_low_num3"] = param_plus_low_num3
            self.HEXA_CMD_SERVO_MOVE["param_plus_low_num4"] = param_plus_low_num4
            self.HEXA_CMD_SERVO_MOVE["param_plus_low_num5"] = param_plus_low_num5
            self.HEXA_CMD_SERVO_MOVE["param_plus_low_num6"] = param_plus_low_num6
            self.HEXA_CMD_SERVO_MOVE["param_plus_high_num1"] = param_plus_high_num1
            self.HEXA_CMD_SERVO_MOVE["param_plus_high_num2"] = param_plus_high_num2
            self.HEXA_CMD_SERVO_MOVE["param_plus_high_num3"] = param_plus_high_num3
            self.HEXA_CMD_SERVO_MOVE["param_plus_high_num4"] = param_plus_high_num4
            self.HEXA_CMD_SERVO_MOVE["param_plus_high_num5"] = param_plus_high_num5
            self.HEXA_CMD_SERVO_MOVE["param_plus_high_num6"] = param_plus_high_num6

            # 发送命令
            tx_data = self.encode_HEXA_CMD_SERVO_MOVE(self.HEXA_CMD_SERVO_MOVE)

            if self.client.send_data(tx_data):
                print("counter:", self.counter)
                print("angle 1: % 0.3f, angle 2: % 0.3f, angle 3: % 0.3f, angle 4: % 0.3f, angle 5: % 0.3f, angle 6: % 0.3f" % 
                              (self.xian_dj_stewart_platform_arm1_cmd,self.xian_dj_stewart_platform_arm2_cmd,self.xian_dj_stewart_platform_arm3_cmd,self.xian_dj_stewart_platform_arm4_cmd,self.xian_dj_stewart_platform_arm5_cmd,self.xian_dj_stewart_platform_arm6_cmd))
                
                # 心跳
                if self.counter > 1000:
                    self.counter = 0
                self.counter += 1
                self.heart_beat_msg.data = self.counter
                self.pub_msg.publish(self.heart_beat_msg)

            # else:
            #     self.heart_beat_msg.data = 0
            #     self.pub_msg.publish(self.heart_beat_msg)
            #     print("send to server failed!")
            #     return -1


            


                
        except Exception as e:
            rospy.logerr(f"Error processing servo command: {str(e)}")
    
    
    # def xian_dj_stewart_platform_power_callback(self, event):
    #     ttx_data = self.encode_CMD_REQ_BATTERY_VOLTAGE(self.CMD_REQ_BATTERY_VOLTAGE)
    #     # self.serial_port.write(ttx_data)
    #     self.sock.sendall(ttx_data)
    #     # # 接收响应（带超时重试机制）
    #     start_time = time.time()
    #     while time.time() - start_time < 5:  # 5秒超时
    #         rx_data = self.sock.recv(6)
    #         if len(rx_data) ==6:  # 预期最小数据长度
    #             voltage_data = self.decode_CMD_GET_BATTERY_VOLTAGE(rx_data)
                
    #             if voltage_data:
    #                 # 计算实际电压值（假设高8位+低8位组成16位整数，单位mV）
    #                 voltage_mv = (voltage_data["param_vol_high"] << 8) + voltage_data["param_vol_low"]
    #                 print(f"[RX] Decoded: {voltage_data}")
    #                 print(f"Battery Voltage: {voltage_mv / 1000:.2f}V")
    #                 # rospy.set_param("/xian_dj_stewart_platform_params_server/xian_is_duoji_driver_power", 1)
    #                 break
    #             else:
    #                 print(f"[RX] Invalid data: {rx_data.hex(' ').upper()}")
    #         # time.sleep(0.01)
    #         else:
    #             # rospy.set_param("/xian_dj_stewart_platform_params_server/xian_is_duoji_driver_power", 0)
    #             print("Timeout: No valid response received")
    #         pass

    def init_tcp(self):
        #初始化TCP
        # 初始连接
        if not self.client.connect_to_server():
            print("无法连接到服务器，退出程序")
            return -1
        

    def init_servo_commands(self):
        """初始化舵机控制命令数据结构"""
        # 六个舵机转动数据结构
        self.HEXA_CMD_SERVO_MOVE = {
            "frame_head_1": 0x55, "frame_head_2": 0x55,
            "message_length": 0x17, "mode": 0x03,
            "param_number_hts": 0x06, 
            "param_time_low": 0xE8, "param_time_high": 0x03,
            "param_id_num1": 0x01, "param_plus_low_num1": 0x20, "param_plus_high_num1": 0x03,
            "param_id_num2": 0x02, "param_plus_low_num2": 0x20, "param_plus_high_num2": 0x03,
            "param_id_num3": 0x03, "param_plus_low_num3": 0x20, "param_plus_high_num3": 0x03,
            "param_id_num4": 0x04, "param_plus_low_num4": 0x20, "param_plus_high_num4": 0x03,
            "param_id_num5": 0x05, "param_plus_low_num5": 0x20, "param_plus_high_num5": 0x03,
            "param_id_num6": 0x06, "param_plus_low_num6": 0x20, "param_plus_high_num6": 0x03
        }

    # 编码“请求电压数据结构”
    def encode_CMD_REQ_BATTERY_VOLTAGE(self, data_dict):
        """将字典转换为字节流，按固定字段顺序处理"""
        field_order = [
            "frame_head_1", "frame_head_2", "message_length", "mode"]
        return bytes([data_dict[field] for field in field_order])
    
    # 解码电压
    def decode_CMD_GET_BATTERY_VOLTAGE(self, raw_data: bytes):
        """解析电压响应数据包"""
        if len(raw_data) < 6:  # 最小长度检查（帧头2 + 长度1 + 模式1 + 数据2）
            return None
        
        if raw_data[0] != 0x55 or raw_data[1] != 0x55:
            return None
        
        response = self.CMD_GET_BATTERY_VOLTAGE.copy()
        response.update({
            "message_length": raw_data[2],
            "mode": raw_data[3],
            "param_vol_low": raw_data[4],
            "param_vol_high": raw_data[5]
        })
        return response

    def encode_HEXA_CMD_SERVO_MOVE(self, data_dict: Dict[str, int]) -> bytes:
        """将字典转换为字节流，按固定字段顺序处理"""
        field_order = [
            "frame_head_1", "frame_head_2", "message_length", "mode",
            "param_number_hts", "param_time_low", "param_time_high",
            "param_id_num1", "param_plus_low_num1", "param_plus_high_num1",
            "param_id_num2", "param_plus_low_num2", "param_plus_high_num2",
            "param_id_num3", "param_plus_low_num3", "param_plus_high_num3",
            "param_id_num4", "param_plus_low_num4", "param_plus_high_num4",
            "param_id_num5", "param_plus_low_num5", "param_plus_high_num5",
            "param_id_num6", "param_plus_low_num6", "param_plus_high_num6"
        ]
        return bytes([data_dict[field] for field in field_order])

    def int_to_hex_bytes(self, num):
        """将整数转换为2字节的有符号字节数组（大端序）"""
        bytes_data = num.to_bytes(2, byteorder='big', signed=True)
        return bytes_data[0], bytes_data[1]

    def angle_to_pulse(self, angle):
        """
        将角度(-120°到120°)转换为脉冲值(0-1000)
        其中0-1000脉冲对应0-240°转角
        """
        if angle > 120:
            angle = 120
        elif angle < -120:
            angle = -120

        mapped_angle = angle + 120  # 将范围转换为0-240°
        pulse = int((mapped_angle / 240) * 1000)
        pulse = max(0, min(1000, pulse))
        return self.int_to_hex_bytes(pulse)

    def __del__(self):
        """析构函数，关闭网口"""
        if self.sock:
            self.sock.close()
            rospy.loginfo("TCP socket closed")

if __name__ == '__main__':
    try:
        tt = XianDjStewartPlatformBaseElectric()
        rospy.init_node('xian_dj_stewart_platform_base_electric', anonymous=True)  # 初始化ROS节点
        rospy.Timer(rospy.Duration(0.1), tt.xian_dj_stewart_platform_move_callback_fun, oneshot=False) # 控制舵臂转动线程
        rospy.spin()  # 添加这行确保节点持续运行

    except rospy.ROSInterruptException:
        pass