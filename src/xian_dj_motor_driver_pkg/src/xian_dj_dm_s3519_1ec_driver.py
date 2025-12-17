#!/usr/bin/env python3
#!coding=utf-8
import rospy
import signal
import sys
import numpy as np
import time, datetime, os, json, logging
from std_msgs.msg import Int32,UInt16, Float32,String
import can
import time
import struct

class DualMotorController:
    def __init__(self, channel='can0'):
        """
        初始化CAN总线连接
        """
        try:
            # 使用socketcan接口[5,8](@ref)
            self.bus = can.interface.Bus(channel=channel, bustype='socketcan')
            print(f"成功连接到CAN接口 {channel}")
        except Exception as e:
            print(f"CAN总线初始化失败: {e}")
            raise
    
    # def send_motor_command(self, left_motor_data, right_motor_data):
    #     """
    #     参数:
    #     left_motor_data: 左电机控制数据字节数组(8字节)
    #     right_motor_data: 右电机控制数据字节数组(8字节)
    #     """
    #     # 创建左电机CAN消息 (标准帧, ID=0x201)[2,3](@ref)
    #     left_msg = can.Message
    #     (
    #         arbitration_id=0x201,        # 左电机ID
    #         data=left_motor_data,        # 8字节数据
    #         is_extended_id=False         # 标准帧[4]
    #     )
        
    #     # 创建右电机CAN消息 (标准帧, ID=0x202)
    #     right_msg = can.Message
    #     (
    #         arbitration_id=0x202,        # 右电机ID
    #         data=right_motor_data,       # 8字节数据
    #         is_extended_id=False         # 标准帧
    #     )
        
    #     try:
    #         # 发送消息到CAN总线[2]
    #         self.bus.send(left_msg)
    #         self.bus.send(right_msg)
    #         return True
    #     except can.CanError as e:
    #         print(f"CAN消息发送失败: {e}")
    #         return False
    def send_motor_command(self, motor_id,motor_data):
        """
        参数:
        left_motor_data: 左电机控制数据字节数组(8字节)
        right_motor_data: 右电机控制数据字节数组(8字节)
        """
        # 创建左电机CAN消息 (标准帧, ID=0x201)[2,3](@ref)
        send_msg = can.Message(
            arbitration_id = motor_id,        # 左电机ID
            data=motor_data,        # 8字节数据
            is_extended_id=False         # 标准帧[4]
        )
             
        try:
            # 发送消息到CAN总线[2]
            self.bus.send(send_msg)
            return True
        except can.CanError as e:
            print(f"CAN消息发送失败: {e}")
            return False

    def receive_feedback(self, timeout=0.1):
        """
        接收电机反馈数据
        """
        feedback_messages = []
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            msg = self.bus.recv(timeout=0.01)  # 非阻塞接收[3](@ref)
            if msg is not None:
                feedback_messages.append(msg)
        
        return feedback_messages
    
    def close(self):
        """
        关闭CAN连接[2]
        """
        if self.bus:
            self.bus.shutdown()
            print("CAN连接已关闭")
    
class XianDjDmS35191ec:
    def __init__(self):
        self.counter = 0
        self.motor_controller = DualMotorController()

        # 参数初始化
        self.left_motor_id  = 0x201
        self.right_motor_id = 0x202
        self.left_velocity  = [0]*8
        self.right_velocity = [0]*8
        self.xian_dj_dm_s3519_1ec_left_driver_heart_beat = 0
        self.xian_dj_dm_s3519_1ec_right_driver_heart_beat = 0
        self.xian_dj_dm_driver_write_velocity_left = 0
        self.xian_dj_dm_driver_write_velocity_right = 0

        # 订阅话题
        rospy.Subscriber("xian_dj_car_chassis_diff_driver_control_left_write_velocity_msg", Int32, self.left_driver_callback)
        rospy.Subscriber("xian_dj_car_chassis_diff_driver_control_right_write_velocity_msg", Int32, self.right_driver_callback)

        self.left_driver_state_pub_msg = rospy.Publisher("xian_dj_dm_s3519_1ec_driver_left_msg", String, queue_size=1)
        self.left_driver_msg = String()

        self.right_driver_state_pub_msg = rospy.Publisher("xian_dj_dm_s3519_1ec_driver_right_msg", String, queue_size=1)
        self.right_driver_msg = String()

    def left_driver_callback(self, msg):
        xian_dj_dm_driver_write_velocity_left = msg.data
        int32_value = np.int32(-(xian_dj_dm_driver_write_velocity_left))
        float32_value = np.float32(int32_value) /100
        if float32_value >= 45.0:
            float32_value = 45.0
        elif float32_value <= -45.0:
            float32_value = -45.0
        self.left_velocity = float_to_hex_bytes_list(float32_value) + [0x00, 0x00, 0x00, 0x00]
        
        
    def right_driver_callback(self, msg):
        xian_dj_dm_driver_write_velocity_right = msg.data
        int32_value = np.int32(xian_dj_dm_driver_write_velocity_right)
        float32_value = np.float32(int32_value) / 100
        if float32_value >= 45.0:
            float32_value = 45.0
        elif float32_value <= -45.0:
            float32_value = -45.0
        self.right_velocity = float_to_hex_bytes_list(float32_value) + [0x00, 0x00, 0x00, 0x00]

    def driver_control_fun(self, event):

        self.motor_controller.send_motor_command(self.left_motor_id, self.left_velocity)
        self.motor_controller.send_motor_command(self.right_motor_id, self.right_velocity)
        self.feedback = self.motor_controller.receive_feedback()
        if self.feedback:
            print(f"收到 {len(self.feedback)} 条反馈消息:", self.feedback)


        parsed = parse_motor_feedback(self.feedback)
        
        left_info  = {'motor_id': 1, 'err': -1, 'speed_rpm': 0.0}   # 默认值
        right_info = {'motor_id': 2, 'err': -1, 'speed_rpm': 0.0}
        
        
        for m in parsed:
            if m['motor_id'] == 1:
                left_info = m
            elif m['motor_id'] == 2:
                right_info = m
        for m in parsed:
            if m['motor_id'] == 1:
                if self.xian_dj_dm_s3519_1ec_left_driver_heart_beat > 1000:
                    self.xian_dj_dm_s3519_1ec_left_driver_heart_beat = 0
                else:
                    self.xian_dj_dm_s3519_1ec_left_driver_heart_beat += 1
                if m['err'] == 0:
                    left_enable = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC]  # 左电机使能
                    self.motor_controller.send_motor_command(self.left_motor_id, left_enable)
                    self.motor_controller.send_motor_command(self.left_motor_id, self.left_velocity)# 使能后重新给速度指令
                else :
                    print('1 号电机转速', m['speed_rpm'])
                
            if m['motor_id'] == 2:
                if self.xian_dj_dm_s3519_1ec_right_driver_heart_beat > 1000:
                    self.xian_dj_dm_s3519_1ec_right_driver_heart_beat = 0
                else:
                    self.xian_dj_dm_s3519_1ec_right_driver_heart_beat += 1
                if m['err'] == 0:
                    right_enable = [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC]  # 左电机使能
                    self.motor_controller.send_motor_command(self.right_motor_id, right_enable)
                    self.motor_controller.send_motor_command(self.right_motor_id, self.right_velocity)# 使能后重新给速度指令
                else :
                    print('2 号电机转速', m['speed_rpm'])

        self.left_driver_msg.data  = f"{left_info['motor_id']}    {left_info['err']}    {left_info['speed_rpm']:.2f}    {self.xian_dj_dm_s3519_1ec_left_driver_heart_beat}"
        self.right_driver_msg.data = f"{right_info['motor_id']}    {right_info['err']}    {right_info['speed_rpm']:.2f}    {self.xian_dj_dm_s3519_1ec_right_driver_heart_beat}"

        self.left_driver_state_pub_msg.publish(self.left_driver_msg)
        self.right_driver_state_pub_msg.publish(self.right_driver_msg)
         

def float_to_hex_bytes_list(float_value):
    # 将浮点数转换为十六进制字节序列（低位在前，高位在后）
    # 将浮点数打包为4字节（单精度浮点数）
    packed = struct.pack('<f', float_value)  # '<f' 表示小端序的float
    # 将每个字节转换为十六进制整数
    hex_bytes = [byte for byte in packed]
    return hex_bytes

def parse_motor_feedback(messages, vmax=200):
    """
    返回 list，每条元素是 dict：
    {
        'motor_id' : int,        # 电机编号（data[0] 低 4 位）
        'err'      : int,        # 高 4 位状态码
        'speed_rpm': float,      # 换算后的转速
        'raw'      : list[int]   # 原始 8 字节 data，方便调试
    }
    """
    results = []
    for msg in messages:
        if not (msg.data and len(msg.data) >= 6):
            continue
        first = msg.data[0]
        motor_id = first & 0x0F          # 低 4 位
        err_code = (first >> 4) & 0x0F   # 高 4 位

        d3, d4 = msg.data[3], msg.data[4]
        v_int = (d3 << 4) | (d4 >> 4)
        speed = (v_int / 4095.0) * (2 * vmax) - vmax

        results.append({
            'motor_id': motor_id,
            'err': err_code,
            'speed_rpm': speed,
            'raw': list(msg.data)          # 万一后面要查原始帧
        })

        # 实时打印也带上 ID，一眼就知道是谁
        print(f"[电机 #{motor_id}]  err=0x{err_code:X}  "
              f"speed={speed:+.1f} rpm  raw={list(msg.data)}")
    return results

if __name__ == '__main__':
    # motor_controller = None
    try:
        mc = DualMotorController('can0')
        tt = XianDjDmS35191ec()
        rospy.init_node('xian_dj_dm_s3519_1ec_driver', anonymous=True)  # 初始化ROS节点
        # rospy.Timer(rospy.Duration(1), tt.xian_heart_beat_fun, oneshot=False) # 心跳线程
        rospy.Timer(rospy.Duration(0.1), tt.driver_control_fun, oneshot=False) # 心跳线程
        rospy.spin()  # 添加这行确保节点持续运行

    except rospy.ROSInterruptException:
        pass