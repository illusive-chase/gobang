#include "MCT.h"

void mcTN::updateBestChild(const double& fac) {
	mcTN* res = nullptr;
	double maxUCT = 0;
	double cal;
	//设置最优子节点为UCB1值最高的子节点
	for (mcTN* p : ch) {
		cal = UCB1(p, fac);
		if (maxUCT < cal) {
			maxUCT = cal;
			res = p;
		}
	}
	bc = res;
}

inline double UCB1(mcTN * p, const double& fac) {
	return (p->dp % 2 == 0 ? (1.0 - double(p->val) / p->vis) : (double(p->val) / p->vis)) + fac * std::sqrt(1.96 * std::log(double(p->pa->vis)) / p->vis);
}

bool iTrain::UCTSearch(mcTN * p, const double& fac) {
	for (; p && fullyExpanded(p); p = p->bc);//寻找未完全扩展的节点
	if (!p) return false;//提前终止
	setState(p);//按该决策树节点设置对应的状态
	vector<bool> close(POLNUM);//用于标记出不可行的策略
	for (mcTN* it : p->ch) close[it->pol] = true;//已有的策略不可行
	
	for (int i = 0; i < POLNUM; ++i) {
		//跳过不可行决策
		if (!close[i] && isValid(p, i)) {
			mcTN* tmp = p->addChild(i);//扩展一个未被访问的子节点
			setState(tmp);//按该决策树节点设置对应的状态
			if (OB) print();
			turn = tmp->dp % 2 + 1;//turn标记接下来轮到哪个玩家
			expand();//模拟结果
			backPropagation(tmp, BPValue(), fac);//从起始节点反向传播直到根节点
			if (OB) system("pause");
			return true;//成功
		}
	}
	return false;
}

void iTrain::expand() {
	//快速模拟结果
	while (!isFinish()) fastPolicy();
	if (OB) cout << BPValue() << endl;
}

void iTrain::backPropagation(mcTN * p, int bp_val, const double& fac) {
	if (~bp_val) {
		//如果不是平局，根据bp_val更新路径上的节点
		bp_val <<= 1;
		while (p) {
			p->val += bp_val;
			p->vis += 2;
			p->updateBestChild(fac);
			p = p->pa;
		}
	} else {
		//如果平局，即 bp_val=-1
		while (p) {
			++p->val;
			p->vis += 2;
			p->updateBestChild(fac);
			p = p->pa;
		}
	}
}



