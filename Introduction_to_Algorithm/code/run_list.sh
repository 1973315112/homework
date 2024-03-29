#!/bin/bash  
  
# 定义函数来运行list程序  
run_list() {  
    local arg1=$1  
    local arg2=$2  
    ./list "$arg1" "$arg2"  
}  
  
# 遍历参数组合  
for i in {1..6}  
do  
    for j in {1..5}  
    do  
        # 调用run_list函数并传递参数  
        run_list "$i" "$j"  
    done  
done