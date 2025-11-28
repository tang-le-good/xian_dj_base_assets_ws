#!/usr/bin/env python3
#!coding=utf-8

import rospy
import signal
import sys
import numpy as np
import cv2, time, datetime, os, json, logging
from xian_dj_retractable_platform_control_pkg.msg import xian_dj_retractable_platform_base_electric
from std_msgs.msg import UInt16
     
class XianDjRetractablePlatformBaseElectric:
    def __init__(self):
        self.counter = 0

        # 订阅话题
        rospy.Subscriber("xian_dj_retractable_platform_base_electric_msg", xian_dj_retractable_platform_base_electric, self.callback)
        # 发布话题
        self.pub_msg = rospy.Publisher("xian_dj_retractable_platform_base_electric_state_msg", UInt16, queue_size=1)
        self.heart_beat_msg = UInt16()
        # GPIO全局编号
        self.GPIO_stand_linear_actuator_enable = 497  # 引脚12, 控制倒伏伸缩杆使能
        self.GPIO_stand_linear_actuator_move = 498  # 引脚16，控制倒伏伸缩杆运动

        self.GPIO_first_linear_actuator_enable = 499  # 引脚18, 控制一级伸缩杆使能
        self.GPIO_first_linear_actuator_move = 500  # 引脚22，控制一级伸缩杆运动

        self.GPIO_second_linear_actuator_enable = 479  # 引脚24, 控制二级伸缩杆使能
        self.GPIO_second_linear_actuator_move = 481  # 引脚26，控制二级伸缩杆运动
 
        self.xian_dj_retractable_platform_stand_linear_actuator_enble = 0
        self.xian_dj_retractable_platform_stand_linear_actuator_move = 0
        self.xian_dj_retractable_platform_first_linear_actuator_enble = 0
        self.xian_dj_retractable_platform_first_linear_actuator_move = 0
        self.xian_dj_retractable_platform_second_linear_actuator_enble = 0
        self.xian_dj_retractable_platform_second_linear_actuator_move = 0
        self.xian_dj_retractable_platform_base_electric_heart_beat = 0

        #引脚使能，并设置为输出模式
        self.gpio_export(self.GPIO_stand_linear_actuator_enable)
        self.gpio_export(self.GPIO_stand_linear_actuator_move)
        self.gpio_set_direction(self.GPIO_stand_linear_actuator_enable, "out")
        self.gpio_set_direction(self.GPIO_stand_linear_actuator_move, "out")

        self.gpio_export(self.GPIO_first_linear_actuator_enable)
        self.gpio_export(self.GPIO_first_linear_actuator_move)
        self.gpio_set_direction(self.GPIO_first_linear_actuator_enable, "out")
        self.gpio_set_direction(self.GPIO_first_linear_actuator_move, "out")

        self.gpio_export(self.GPIO_second_linear_actuator_enable)
        self.gpio_export(self.GPIO_second_linear_actuator_move)
        self.gpio_set_direction(self.GPIO_second_linear_actuator_enable, "out")
        self.gpio_set_direction(self.GPIO_second_linear_actuator_move, "out")
    

    def callback(self, data):
        self.xian_dj_retractable_platform_stand_linear_actuator_enble = data.xian_dj_retractable_platform_stand_linear_actuator_enble
        self.xian_dj_retractable_platform_stand_linear_actuator_move = data.xian_dj_retractable_platform_stand_linear_actuator_move
        self.xian_dj_retractable_platform_first_linear_actuator_enble = data.xian_dj_retractable_platform_first_linear_actuator_enble
        self.xian_dj_retractable_platform_first_linear_actuator_move = data.xian_dj_retractable_platform_first_linear_actuator_move
        self.xian_dj_retractable_platform_second_linear_actuator_enble = data.xian_dj_retractable_platform_second_linear_actuator_enble
        self.xian_dj_retractable_platform_second_linear_actuator_move = data.xian_dj_retractable_platform_second_linear_actuator_move

        # 倒伏
        self.gpio_set_value(self.GPIO_stand_linear_actuator_enable, self.xian_dj_retractable_platform_stand_linear_actuator_enble)
        self.gpio_set_value(self.GPIO_stand_linear_actuator_move, self.xian_dj_retractable_platform_stand_linear_actuator_move) 

        # 一级
        self.gpio_set_value(self.GPIO_first_linear_actuator_enable, self.xian_dj_retractable_platform_first_linear_actuator_enble)
        self.gpio_set_value(self.GPIO_first_linear_actuator_move, abs(self.xian_dj_retractable_platform_first_linear_actuator_move-1)) 

        # 二级
        self.gpio_set_value(self.GPIO_second_linear_actuator_enable, self.xian_dj_retractable_platform_second_linear_actuator_enble)
        self.gpio_set_value(self.GPIO_second_linear_actuator_move, abs(self.xian_dj_retractable_platform_second_linear_actuator_move-1)) 


    def xian_heart_beat_fun(self, event):
        # 心跳
        if self.xian_dj_retractable_platform_base_electric_heart_beat > 1000:
            self.xian_dj_retractable_platform_base_electric_heart_beat = 0
        self.xian_dj_retractable_platform_base_electric_heart_beat += 1
        print("xian_dj_retractable_platform_base_electric_heart_beat:", self.xian_dj_retractable_platform_base_electric_heart_beat)
        self.heart_beat_msg.data = self.xian_dj_retractable_platform_base_electric_heart_beat
        self.pub_msg.publish(self.heart_beat_msg)

        
    def gpio_export(self,pin):# 使能引脚
        export_path = f"/sys/class/gpio/gpio{pin}"
        if not os.path.exists(export_path):
            with open("/sys/class/gpio/export", "w") as f:
                f.write(str(pin))
            time.sleep(0.1)  # 等待系统创建目录
    
    def gpio_unexport(self, pin): # 复位引脚
        export_path = f"/sys/class/gpio/gpio{pin}"
        if os.path.exists(export_path):
            with open("/sys/class/gpio/unexport", "w") as f:
                f.write(str(pin))
    
    def gpio_set_direction(self,pin, direction):# 设置引脚为输出模式
        with open(f"/sys/class/gpio/gpio{pin}/direction", "w") as f:
            f.write(direction)

    def gpio_set_value(self,pin, value):# 设置引脚的高低电平
        with open(f"/sys/class/gpio/gpio{pin}/value", "w") as f:
            f.write(str(value))



if __name__ == '__main__':
    try:
        tt = XianDjRetractablePlatformBaseElectric()
        rospy.init_node('xian_dj_retractable_platform_base_electric', anonymous=True)  # 初始化ROS节点
        rospy.Timer(rospy.Duration(1), tt.xian_heart_beat_fun, oneshot=False) # 倒伏直线推杆线程
        rospy.spin()  # 添加这行确保节点持续运行

    except rospy.ROSInterruptException:
        pass