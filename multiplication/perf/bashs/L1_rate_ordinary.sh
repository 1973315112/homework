#!/bin/bash

# 定义程序名称
program="/home/s2213917/perf/ordinary_hit_rate"

# 循环运行不同的数据规模
for ((n = 100; n <= 5000; n += 100)); do
    echo "Running Perf for data size: $n"
    
    
    # 运行 Perf 命令并收集 L1 数据缓存命中率
    perf stat -e L1-dcache-load-misses,L1-dcache-loads $program $n
done
