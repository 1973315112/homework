import re
import csv

def extract_percentages(file_path):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            content = file.read()
            matches = re.findall(r'(\d+\.\d+)%', content)
            percentages = [100 - float(match) for match in matches]  # 计算百分比
            return percentages
    except FileNotFoundError:
        print("文件未找到！")
        return []

def write_to_csv(file_path, percentages):
    try:
        with open(file_path, 'w', newline='', encoding='utf-8') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['n', 'L1_rate_cache'])  # 写入列标题
            for i, rate in enumerate(percentages, start=1):
                writer.writerow([i, rate])  # 写入数据
        print("结果已成功写入 CSV 文件：", file_path)
    except Exception as e:
        print("写入文件时出现错误：", e)

file_path = 'L1_rate_cache.txt.csv'  # 要写入的 CSV 文件路径
percentages = extract_percentages('L1_rate_cache.txt')
write_to_csv(file_path, percentages)
