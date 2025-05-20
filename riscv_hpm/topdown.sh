#!/bin/sh

insmod ./hpm_driver.ko

# 设置起始值和结束值
START=3
END=31

# 1. 停止所有
./hpm_call inhibit 0xFFFFFFFF
# 2. 清零 counter, 配置 event
./hpm_call setcounter 0 0   # cycle
./hpm_call setcounter 2 0   # instret
for i in $(seq $START $END); do
    ./hpm_call setcounter $i 0
done
./hpm_call topdown-event-l12

# 3. 使能所有
./hpm_call enable 0xFFFFFFFF
# 4. 开始计数
./hpm_call inhibit 0
# 5. 启动负载... 根据需要进行修改
./stress-ng --cpu 1 --vm 2 --vm-bytes 512M --timeout 1m

# 6. 停止计数
./hpm_call inhibit 0xFFFFFFFF
# 7. 读取数据
./hpm_call topdown-metric-l12

#####################################
# repeat for Level3

./hpm_call setcounter 0 0   # cycle
./hpm_call setcounter 2 0   # instret
for i in $(seq $START $END); do
    ./hpm_call setcounter $i 0
done
./hpm_call topdown-event-l3

# 3. 使能所有
./hpm_call enable 0xFFFFFFFF
# 4. 开始计数
./hpm_call inhibit 0
# 5. 启动负载... 根据需要进行修改
./stress-ng --cpu 1 --vm 2 --vm-bytes 512M --timeout 1m

# 6. 停止计数
./hpm_call inhibit 0xFFFFFFFF
# 7. 读取数据
./hpm_call topdown-metric-l3
