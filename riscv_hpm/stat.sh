#!/bin/bash

# 定义日志文件
LOG_FILE="/tmp/system_usage.log"

# 获取当前时间戳
get_timestamp() {
    date "+%Y-%m-%d %H:%M:%S"
}

# 获取 CPU 使用情况、内存使用情况、swap 使用情况
get_system_stats() {
    # 使用 top 命令以批处理模式获取前三行（CPU、内存、swap 信息）
    stats=$(top -bn1 | head -n 3)

    # 输出结果
    echo "$stats"
}

# 记录到日志文件
log_stats() {
    timestamp=$(get_timestamp)
    
    # 获取并记录 CPU、内存和 swap 使用情况
    stats=$(get_system_stats)

    # 将日志写入文件
    echo "-------------------------" >> $LOG_FILE
    echo "$stats" >> $LOG_FILE
}

# 每秒执行一次
while true
do
    log_stats
    sleep 15
done
