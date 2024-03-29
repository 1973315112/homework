#!/bin/bash  
  
# 定义函数来运行state程序  
run_state() {  
    local arg1=$1  
    local arg2=$2  
    ./state "$arg1" "$arg2"  
}  
  
# 遍历参数组合  
for i in {1..8}  
do  
    for j in {1..5}  
    do  
        # 调用run_state函数并传递参数  
        run_state "$i" "$j"  
    done  
done