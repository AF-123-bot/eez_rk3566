#!/bin/bash

# 等待图形环境完全准备好
sleep 30

# 设置环境变量
export DISPLAY=:0
export XAUTHORITY=/home/cat/.Xauthority

# 允许root访问X11（可选，如程序内部需要）
xhost +SI:localuser:root

# 启动你的程序
/home/cat/test_1/build/project_test >> /home/cat/project_test_run.log 2>&1

