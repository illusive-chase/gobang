#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <iostream>
#include <cstring>
#include <stack>
#include <cstdlib>
#include <iostream>
#include <map>
#include <ctime>

#undef min
#undef max
using std::cin;
using std::cout;
using std::endl;
using std::stack;
using std::map;
using std::vector;
using std::swap;

//OB表示是否打印训练的过程
constexpr bool OB = false;

class mcTN {
public:
	int vis, val;//vis表示访问次数*2，val表示先手获胜次数*2+平局次数
	mcTN* bc;//标记最优子节点，表示接下来的最佳决策
	mcTN* pa;//父节点
	vector<mcTN*> ch;//子节点的集合
	int pol;//父节点到当前节点所使用的决策
	int dp;//在决策树中的深度

	mcTN(int pol, mcTN* pa = nullptr) :vis(0), val(0), bc(nullptr), pa(pa), pol(pol), dp(pa ? pa->dp + 1 : 0) {}
	~mcTN() { for (mcTN* p : ch) delete p; }

	//更新最优子节点
	void updateBestChild(const double& fac = 1.0);

	//添加子节点并返回它
	mcTN* addChild(int pol) { 
		ch.push_back(new mcTN(pol, this));
		return *(ch.end() - 1);
	}

	//删除多余的子节点
	void delChildExcept(mcTN* p) {
		for (mcTN* cn : ch) {
			if (cn != p) delete cn;
		}
		ch.clear();
		ch.push_back(p);
	}
};

//计算UCB1值，fac越小表示决策树越倾向于考虑当前的利益而非将来的利益
inline double UCB1(mcTN* p, const double& fac = 1.0);

class iTrain {
public:
	//设置最大决策分支数
	iTrain(int polnum) :POLNUM(polnum) {}

	virtual bool isValid(mcTN* p, int pol) = 0;//策略是否合法
	virtual void fastPolicy() = 0;//快速模拟到下一个状态
	virtual int BPValue() = 0;//游戏结果：1表示先手胜利，0表示后手胜利，-1表示平局
	virtual bool isFinish() = 0;//当前状态是否达到游戏结束
	virtual void setState(mcTN* p) = 0;//根据决策树节点设置状态
	virtual void print() = 0;//打印状态
	virtual bool fullyExpanded(mcTN* p) = 0;//p是否完全扩展
	virtual void simulate(int extra_times, const double & fac) = 0;//模拟对局，参数决定每一步额外的训练次数
	virtual void train(mcTN * pos, int times, const double & fac) = 0;//选取节点p训练一定次数

	const int POLNUM;//最大决策分支数
	int turn;//标记轮到哪个玩家
	vector<vector<int>> state;//状态（棋盘）
	mcTN* root;//决策树的根节点
	bool UCTSearch(mcTN * p, const double& fac);//UCT算法选取节点p进行取样学习
	void expand();//快速模拟结果
	void backPropagation(mcTN* p, int new_val, const double& fac);//反向传播结果
	
};



