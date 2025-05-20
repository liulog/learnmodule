#!/bin/sh

insmod ./hpm_driver.ko

# 设置起始值和结束值
START=3
END=31

# 1. 停止所有
./hpm_call inhibit 0xFFFFFFF8
# 2. 清零 counter, 配置 event
for i in $(seq $START $END); do
    ./hpm_call setcounter $i 0
done
./hpm_call defaultevent

# 3. 使能所有
./hpm_call enable 0xFFFFFFFF
# 4. 开始计数
./hpm_call inhibit 0
# 5. 启动负载... 根据需要进行修改
./stress-ng --cpu 1 --vm 2 --vm-bytes 512M --timeout 1m

# 6. 停止计数
./hpm_call inhibit 0xFFFFFFFF
# 7. 读取数据
./hpm_call getcounter 0
for i in $(seq $START $END); do
    ./hpm_call getcounter $i
done

