#include <iostream>
#include <string>
#include <regex>
#include <chrono>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <climits>
#include <time.h>
#include <iomanip>

using namespace std;
using namespace chrono;

double perl();
double astar();
double mcf();

int main() {
	system("chcp 65001");
	cout<<"欢迎使用本基准测试"<<endl;		
	cout<<"------------------"<<endl;
	cout<<"基准测试1 perl开始"<<endl;
	double score1=perl();
	cout<<"基准测试1 perl结束"<<endl;
	cout<<"------------------"<<endl;
	cout<<"基准测试2 astar开始"<<endl;
	double score2=astar();
	cout<<"基准测试2 astar结束"<<endl;
	cout<<"------------------"<<endl;	
	cout<<"基准测试3 mcf开始"<<endl;
	double score3=mcf();
	cout<<"基准测试3 mcf结束"<<endl;
	cout<<"------------------"<<endl;
	double score=cbrt(score1*score2*score3);
	cout<<"本次基准测试得分为："<<fixed<<setprecision(2)<<score<<"分"<<endl;	
	cout<<"基准测试结束"<<endl;	
	cout<<"------------------"<<endl;
    return 0;
}

double perl()
{
    // 定义一些测试用的字符串
    string str = "The quick brown fox jumps over the lazy dog";
    string pattern = "quick|lazy";
    // 开始计时
    clock_t startTime = clock();
    // 执行一些字符串操作和正则表达式匹配
    for (int i = 0; i < 1000000; i++) {
        // 字符串操作：查找子字符串
        size_t found = str.find("fox");
        if (found != std::string::npos) {
            // 正则表达式匹配
            regex reg(pattern);
            smatch match;
            regex_search(str, match, reg);
        }
    }
    // 停止计时并计算执行时间
    clock_t endTime = clock();
    double Time = 1000*double(endTime - startTime) / CLOCKS_PER_SEC;
    double score=(10626.67/Time);
    // 输出执行时间
	cout<<"基准测试1 perl:"<<Time<<"ms"<<endl;
	cout<<"基准测试1 perl:"<<fixed<<setprecision(2)<<score<<"分"<<endl;
	return score;
}




// 定义一个表示坐标的结构体
struct Point {
    int x, y;
    Point() {}
    Point(int _x, int _y) : x(_x), y(_y) {}
};

// 定义地图的大小
const int ROW = 5;
const int COL = 5;

// 定义地图，0表示可通过的空格，1表示障碍物
int grid[ROW][COL] = {
    {0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0}
};

// 定义启发式函数，这里简单地使用曼哈顿距离
int heuristic(Point a, Point b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// 定义节点的结构体
struct Node {
    Point point;
    int f, g, h; // f = g + h

    Node(Point _point, int _g, int _h) : point(_point), g(_g), h(_h) {
        f = g + h;
    }

    // 定义比较函数，用于优先队列
    bool operator<(const Node& other) const {
        return f > other.f; // 小顶堆
    }
};

// A*算法
void AStar(Point start, Point goal, vector<Point>& path) {
    // 定义一个优先队列来保存待访问的节点
    priority_queue<Node> openList;

    // 定义一个数组来记录节点是否已经访问过
    bool closedList[ROW][COL] = {false};

    // 定义一个数组来记录每个节点的父节点，用于最终回溯路径
    Point parents[ROW][COL];

    // 初始化起始节点
    openList.push(Node(start, 0, heuristic(start, goal)));

    while (!openList.empty()) {
        // 从优先队列中取出f值最小的节点
        Node current = openList.top();
        openList.pop();

        // 如果当前节点是目标节点，则路径找到
        if (current.point.x == goal.x && current.point.y == goal.y) {
            Point p = current.point;
            while (!(p.x == start.x && p.y == start.y)) {
                path.push_back(p);
                p = parents[p.x][p.y];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());
            return;
        }

        closedList[current.point.x][current.point.y] = true;

        // 访问当前节点的邻居节点
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;

                int newX = current.point.x + dx;
                int newY = current.point.y + dy;

                // 确保新坐标在地图范围内
                if (newX >= 0 && newX < ROW && newY >= 0 && newY < COL && grid[newX][newY] == 0 && !closedList[newX][newY]) {
                    // 计算新节点的g值和h值
                    int newG = current.g + 1;
                    int newH = heuristic(Point(newX, newY), goal);
                    int newF = newG + newH;

                    // 如果新节点已经在openList中，且新的路径代价更小，则更新节点信息
                    bool inOpenList = false;
                    priority_queue<Node> tempQueue; // 临时优先队列用于存储重新排序的节点
                    while (!openList.empty()) {
                        Node node = openList.top();
                        openList.pop();
                        if (node.point.x == newX && node.point.y == newY) {
                            inOpenList = true;
                            if (newF < node.f) {
                                node.f = newF;
                                node.g = newG;
                                node.h = newH;
                                parents[newX][newY] = current.point;
                            }
                        }
                        tempQueue.push(node); // 将节点放回临时队列
                    }

                    // 将临时队列中的节点重新放回优先队列
                    while (!tempQueue.empty()) {
                        openList.push(tempQueue.top());
                        tempQueue.pop();
                    }

                    // 如果新节点不在openList中，则将其加入
                    if (!inOpenList) {
                        openList.push(Node(Point(newX, newY), newG, newH));
                        parents[newX][newY] = current.point;
                    }
                }
            }
        }
    }
	return;
}



double astar() {
	
    // 定义起点和终点
    Point start(0, 0);
    Point goal(4, 4);

    vector<Point> path;

    // 运行A*算法并计算时间
    bool found;
    auto start_time = high_resolution_clock::now();
    
    clock_t startTime = clock();
    for (int i = 0; i < 20000; ++i) 
	{
        AStar(start, goal, path);
    }
    
    clock_t endTime = clock();
    double Time = 1000*double(endTime - startTime) / CLOCKS_PER_SEC;
    double score=(11253.33/Time);
	cout<<"基准测试2 astar:"<<Time<<"ms"<<endl;
	cout<<"基准测试2 astar:"<<fixed<<setprecision(2)<<score<<"分"<<endl;
	return score;
}


// 定义最大顶点数
#define V 6

// Ford-Fulkerson算法求最大流
int fordFulkerson(int graph[V][V], int source, int sink) {
    int u, v;

    // 创建残留图，并初始化为原始图的副本
    int residualGraph[V][V];
    for (u = 0; u < V; u++) {
        for (v = 0; v < V; v++) {
            residualGraph[u][v] = graph[u][v];
        }
    }

    // 记录从源到当前节点的路径
    int parent[V];

    int maxFlow = 0;  // 最大流量

    // 循环直到找不到增广路径为止
    while (true) {
        // 使用广度优先搜索查找增广路径
        fill(parent, parent + V, -1);
        queue<int> q;
        q.push(source);
        parent[source] = source;
        while (!q.empty() && parent[sink] == -1) {
            u = q.front();
            q.pop();
            for (v = 0; v < V; v++) {
                if (parent[v] == -1 && residualGraph[u][v] > 0) {
                    q.push(v);
                    parent[v] = u;
                }
            }
        }

        // 如果无法找到增广路径，则结束
        if (parent[sink] == -1) {
            break;
        }

        // 寻找增广路径上的最小剩余容量
        int pathFlow = INT_MAX;
        for (v = sink; v != source; v = parent[v]) {
            u = parent[v];
            pathFlow = min(pathFlow, residualGraph[u][v]);
        }

        // 更新残留图和最大流量
        for (v = sink; v != source; v = parent[v]) {
            u = parent[v];
            residualGraph[u][v] -= pathFlow;
            residualGraph[v][u] += pathFlow;
        }

        maxFlow += pathFlow;
    }

    return maxFlow;
}

double mcf() {
    // 定义图的邻接矩阵表示
    int graph[V][V] = {
        {0, 16, 13, 0, 0, 0},
        {0, 0, 10, 12, 0, 0},
        {0, 4, 0, 0, 14, 0},
        {0, 0, 9, 0, 0, 20},
        {0, 0, 0, 7, 0, 4},
        {0, 0, 0, 0, 0, 0}
    };
    // 源节点和汇点
    int source = 0, sink = 5;
    // 计时开始
    clock_t startTime = clock();
    for (int i = 0; i < 6000000; ++i) 
	{
        fordFulkerson(graph, source, sink);
    }
    // 计时结束
    clock_t endTime = clock();
    double Time = 1000*double(endTime - startTime) / CLOCKS_PER_SEC;
    double score=(9966/Time);
    cout<<"基准测试3 mcf："<<Time<<"ms"<<endl;
    cout<<"基准测试3 mcf:"<<fixed<<setprecision(2)<<score<<"分"<<endl;
    return score;
}
