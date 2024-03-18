import csv  
  
# 初始化两个空数组  
A = []  
B = []  
  
# 使用'utf-8'编码打开文件并读取内容  
with open('L2_rate_ordinary.txt', 'r', encoding='utf-8') as file:  
    for line in file:  
        # 检查行是否包含所需的计数器  
        if 'armv8_pmuv3_0/l2d_cache/u' in line:  
            # 使用split()方法分割字符串，并获取计数器前的数字  
            number_str = line.split('armv8_pmuv3_0/l2d_cache/u')[0].strip().split()[-1]  
            # 去除逗号，并将字符串转换为整数  
            number = int(number_str.replace(',', ''))  
            A.append(number)  # 将整数添加到数组A  
        elif 'armv8_pmuv3_0/l2d_cache_refill/u' in line:  
            number_str = line.split('armv8_pmuv3_0/l2d_cache_refill/u')[0].strip().split()[-1]  
            # 去除逗号，并将字符串转换为整数  
            number = int(number_str.replace(',', ''))  
            B.append(number)  # 将整数添加到数组B  
  
# 确保A和B的长度相同  
assert len(A) == len(B), "数组A和B的长度不一致，无法进行计算"  
  
# 创建一个空的列表来保存results  
results = []  
  
# 计算并保存(A[i] - B[i]) / A[i]的结果  
for i in range(len(A)):  
    # 避免除以零的情况  
    if A[i] != 0:  
        result =(1-( (A[i] - B[i]) / A[i]) )*100 
        results.append(result)  
    else:  
        results.append(None)  # 如果A[i]为零，则保存None  
  
# 将结果写入CSV文件  
with open('L2_rate_ordinary.csv', 'w', newline='', encoding='utf-8') as csvfile:  
    fieldnames = ['L2_rate_ordinary']  # 定义CSV文件的列名  
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)  
  
    writer.writeheader()  # 写入列名  
    for result in results:  
        writer.writerow({'L2_rate_ordinary': result})  # 写入每一行的结果  
  
print("结果已写入L2_rate_ordinary.csv文件。")