#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <stdlib.h>
using namespace std;

const int Weight = 10000; //背包最大容纳重量
const int MaxWeight = 1000; //随机生成的背包重量最大100
const double MaxValue = 100.0; //随机生成的背包价值最大为100.0
const int G = 1000; //得到1000代种群
const int MaxPopulations = 32; //初始种群的大小
const double Pc = 0.9; //选择染色体交换的概率
const double Pm = 0.0075; //染色体变异的可能性

struct Item {
    double value;
    int weight;
}; //背包的数据类型

void Knapsack(int n); //用于求解背包问题
double GA(int n, vector<struct Item> items); //遗传算法
double DP(int n, vector<struct Item> items); //动态规划
void generateItem(int n, vector<struct Item> &items); //生成背包
void initPopulations(int n, vector<string> &populations); //初始化种群
void initPopuWithMean(int n, vector<string> &populations, vector<struct Item> items); //生成均为可行解的种群
double getFitness(vector<struct Item> items, vector<string> populations, vector<double> &fitness); //得到各个体的适应度
int selection(vector<double> fitness); //选择染色体交换的个体
void crossover(string &female, string &male, int n); //交换两条染色体
void mutation(string &individuals, int n); //变异

int main() {
    int n; //背包数量
    
    srand((unsigned int)time(NULL)); //随机数种子，便于生成不同的随机数
    cout<<"Input number of items:";
    do {
        cin>>n;
    }while(n <= 0); //保证输入的背包个数大于0
    Knapsack(n);
    
    return 0;
}

void Knapsack(int n) {
    vector<struct Item> items; //定义物品
    
    generateItem(n, items); //生成物品
    for (int i = 0; i < n; ++i) cout<< items[i].value << " " <<items[i].weight <<endl; //输出背包
    cout<< GA(n, items) <<endl; //输出用遗传算法得到的近似解
    cout<< DP(n, items) <<endl; //输出用动态规划得到的精确解
}

double GA(int n, vector<struct Item> items) {
    int female, male, descendants = 0; //依次为雌性个体、雄性个体、第几代
    double maxValue = 0; //最大的价值
    string bestIndividual; //最优的个体
    vector<string> populations; //定义种群
    vector<double> fitness; //定义适应度
    
    for (int i = 0; i < MaxPopulations; ++i) fitness.push_back(0); //初始化适应度
//    initPopulations(n, populations); //初始化种群
    initPopuWithMean(n, populations, items);
    
    do {
        descendants += 1; //记录第几代
        maxValue = max(maxValue, getFitness(items, populations, fitness)); //得到最大价值
        for (int i = 0; i < MaxPopulations; ++i) { //对种群进行选择、交叉、变异操作的遗传算子
            if (1.0 * rand() / RAND_MAX < Pc) { //判断是否可以进行选择
                female = selection(fitness); //选择雌性个体
                male = selection(fitness); //选择雄性个体
                crossover(populations[female], populations[male], n); //染色体交叉
                if (1.0 * rand() / RAND_MAX < Pm) { //判断是否发生变异
                    mutation(populations[female], n);
                    mutation(populations[male], n);
                }
            }
        }
    }while(descendants < G); //1000代之后结束
    
    return maxValue; //返回最大价值
//    return bestIndividual; //返回近似解
}

void generateItem(int n, vector<struct Item> &items) {
    struct Item item;
    
    for (int i = 0; i < n; ++i) {
        item.value = (1.0 * rand() / RAND_MAX) * MaxValue;
        item.weight = rand() % MaxWeight;
        items.push_back(item); //将生成的物品放入待取区
    }
}

void initPopulations(int n, vector<string> &populations) {
    for (int i = 0; i < MaxPopulations; ++i) { //生成种群
        string tmp = "";
        for (int j = 0; j < n; ++j) //随机生成个体
            tmp += rand() % 2 + '0';
        populations.push_back(tmp); //将生成的个体放入种群
    }
}

double getFitness(vector<struct Item> items, vector<string> populations, vector<double> &fitness) {
    double allValue = 0, maxValue = 0; //分别表示种群的价值总和、最大价值
    string bestIndividual; //保留最优的个体
    
    for (int i = 0; i < MaxPopulations; ++i) { //对每个个体计算适应度
        double maxWeight = 0; //用于记录个体所需的背包容量
        fitness[i] = 0;
        for (int j = 0; j < populations[i].length(); ++j) {
            maxWeight += (populations[i][j] - '0') * items[j].weight;
            if (maxWeight > Weight) { //当个体所需的容量大于背包容量时，解不存在
                fitness[i] = 0;
                break;
            }
            fitness[i] += (populations[i][j] - '0') * items[j].value;
        }
        if (fitness[i] > maxValue) { //用于得到最优解
            maxValue = fitness[i];
            bestIndividual = populations[i];
        }
        allValue += fitness[i]; //计算种群价值
    }
    if (allValue == 0) allValue = 1; //防止所有的解都不存在
    for (int i = 0; i < MaxPopulations; ++i) //求解适应度，价值大的适应度大
        fitness[i] /= allValue;
    return maxValue; //返回最大价值
//    return bestIndividual; //返回选择方式
}

int selection(vector<double> fitness) {
    double m = 0;
    double r = 1.0  * rand() / RAND_MAX; //随机生成需要选择的个体

    for (int i = 0; i < MaxPopulations; ++i) { //类似转动圆盘，最后选择指针所指的区域
        m += fitness[i];
        if (r <= m) return i; //返回之后指针
    }
    return MaxPopulations - 1; //当所有值都不存在时，保证有值返回
}

void crossover(string &female, string &male, int n) {
    string subOfFemale = female.substr(n/4, n/2); //选择female中n/4-n/2之间的片段
    string subOfMale = male.substr(n/4, n/2); //选择male中n/4-n/2之间的片段

    female.replace(n/4, subOfFemale.length(), subOfMale); //将male中的染色体放入female中
    male.replace(n/4, subOfMale.length(), subOfFemale); //将female中的染色体放入male中
}

void mutation(string &individuals, int n) {
    int index = rand() % n; //随机某个基因位
    
    individuals[index] = !(individuals[index] - '0') + '0'; //更改基因
}

double DP(int n, vector<struct Item> items) {
    vector<double> value[Weight + 1];
    
    for (int i = 0; i < Weight + 1; ++i) //得到存储解的表
        for (int j = 0; j < n + 1; ++j)
            value[i].push_back(0);
    
    for (int i = 0; i < n + 1; ++i) value[0][i] = 0; //初始化
    for (int i = 0; i < Weight + 1; ++i) value[i][0] = 0; //初始化
    
    for (int i = 1; i < n + 1; ++i) { //用动态规划求解
        for (int j = 1; j < Weight + 1; ++j) { //以下时状态转移方程
            if (items[i - 1].weight > j) value[j][i] = value[j][i - 1];
            else value[j][i] = max(value[j][i - 1], value[j - items[i - 1].weight][i - 1] + items[i - 1].value);
        }
    }
    return value[Weight][n]; //返回最优值
}

void initPopuWithMean(int n, vector<string> &populations, vector<struct Item> items) {
    int index = 0, meanWeight = 0, preChoose;
    
    for (int i = 0; i < items.size(); ++i)
        meanWeight += items[i].weight;
    meanWeight /= items.size();
    preChoose = meanWeight > n? n: meanWeight; //判断是否可选数目，使得预先选择的想法可以实现
    for (int i = 0; i < MaxPopulations; ++i) { //生成种群
        string tmp(n,'0'); //初始化，预先不取任何物品
        for (int j = 0; j < preChoose; ++j) {//随机生成个体
            index = rand() % n; //选定需要更改的基因位
            tmp[index] = '1';
        }
        populations.push_back(tmp); //将生成的个体放入种群
    }
}

/*
 1. 物品数量和种群数量之间的关系对解的影响
 2. 后代多少对解的影响
*/
