#include <iostream>  
#include <fstream>  
#include <sstream>  
#include <vector>  
  
using namespace std;
  
int main() {  
    int row, col1, col2;  
    string n;
    std::cout << "请输入row, col1, col2的值（用空格分隔）: ";  
    std::cin >> row >> col1 >> col2;  
  	std::cout << "请输入第几个文件: "; 
  	cin>>n;
    // 打开test1.txt文件以写入数据  
    std::ofstream outfile("C:/Users/13033/Desktop/state/tests/test"+n+".txt");  
    if (!outfile.is_open()) {  
        std::cerr << "无法打开文件 test1.txt 进行写入" << std::endl;  
        return 1;  
    }  
  
    // 写入第一行  
    outfile << row << " " << col1 << " " << col2 << std::endl;  
  
    // 定义文件路径  
    const std::vector<std::string> filenames = {"消元子.txt", "被消元行.txt", "消元结果.txt"};  
  
    for (const std::string& filename : filenames) {  
        std::ifstream infile(filename);  
        if (!infile.is_open()) {  
            std::cerr << "无法打开文件 " << filename << std::endl;  
            continue;  
        }  
  
        std::string line;  
        while (std::getline(infile, line)) {  
            std::istringstream iss(line);  
            int num;  
            int count = 0; // 统计当前行的数字个数  
  
            // 读取当前行的所有数字，并统计数量  
            while (iss >> num) {  
                ++count;  
            }  
  
            // 将数字个数写入新文件的当前行  
            outfile << count << " ";  
  
            // 重置istringstream，重新读取当前行的数字  
            iss.clear();  
            iss.str(line);  
  
            // 读取并转换当前行的每个数字  
            while (iss >> num) {  
                outfile << row - 1 - num << " ";  
            }  
            outfile << std::endl; // 换行  
        }  
        infile.close();  
    }  
  
    outfile.close();  
    std::cout << "文件写入完成" << std::endl;  
  
    return 0;  
}