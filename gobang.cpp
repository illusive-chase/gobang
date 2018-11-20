#include "MCT.h"
//定义棋盘大小
constexpr int SIZE = 11;
//继承iTrain接口
class Gobang :public iTrain {
public:

	int bpv;
	bool fin;
	vector<vector<int>> density;
	void checkFin(int x, int y);

	//实现iTrain接口

	void fastPolicy();
	int BPValue();
	bool isFinish();
	void setState(mcTN * p);
	void print();
	void train(mcTN * pos, int times, const double & fac);
	bool fullyExpanded(mcTN* p);
	void simulate(int extra_times, const double & fac);
	bool isValid(mcTN* p, int pol);

	//设置随机种子，设置最大决策分支数为SIZE*SIZE，初始化状态
	Gobang() :iTrain(SIZE*SIZE) {
		fin = false;
		bpv = -1;
		srand((unsigned)time(0));
		root = new mcTN(0);
		state = vector<vector<int>>(SIZE, vector<int>(SIZE));
		density = vector<vector<int>>(SIZE, vector<int>(SIZE));
	}
};

bool Gobang::fullyExpanded(mcTN* p) {
	return p->ch.size() == POLNUM - p->dp;//判断是否完全扩展
}

void Gobang::checkFin(int x, int y) {
	int role = state[x][y];
	int cnt;

	for (int i = x - 2; i < x + 3; ++i) {
		if (i >= 0 && i < SIZE) {
			for (int j = y - 2; j < y + 3; ++j) {
				if (j >= 0 && j < SIZE && !state[i][j]) ++density[i][j];
			}
		}
	}
	density[x][y] = 0;

	//列计数
	cnt = 1;
	for (int i = x - 1; ~i && state[i][y] == role; --i) ++cnt;
	for (int i = x + 1; i < SIZE && state[i][y] == role; ++i) ++cnt;
	if (cnt > 4) {
		fin = true;
		bpv = 2 - role;
		return;
	}
	//行计数
	cnt = 1;
	for (int i = y - 1; ~i && state[x][i] == role; --i) ++cnt;
	for (int i = y + 1; i < SIZE && state[x][i] == role; ++i) ++cnt;
	if (cnt > 4) {
		fin = true;
		bpv = 2 - role;
		return;
	}
	//主对角线
	cnt = 1;
	for (int i = x - 1, j = y - 1; ~i && ~j && state[i][j] == role; --i, --j) ++cnt;
	for (int i = x + 1, j = y + 1; i < SIZE && j < SIZE && state[i][j] == role; ++i, ++j) ++cnt;
	if (cnt > 4) {
		fin = true;
		bpv = 2 - role;
		return;
	}
	//副对角线
	cnt = 1;
	for (int i = x + 1, j = y - 1; i < SIZE && ~j && state[i][j] == role; ++i, --j) ++cnt;
	for (int i = x - 1, j = y + 1; ~i && j < SIZE && state[i][j] == role; --i, ++j) ++cnt;
	if (cnt > 4) {
		fin = true;
		bpv = 2 - role;
		return;
	}
}

void Gobang::fastPolicy() {
	/*
	vector<int> pols(POLNUM);//待选决策列表
	for (int i = 0; i < POLNUM; ++i) pols[i] = i;//初始化，加入所有决策
	//随机选取
	for (;;) {
		if (!pols.size()) {
			fin = true;
			bpv = -1;
			return;
		}
		int r = rand() % pols.size();
		int a = pols[r] / SIZE, b = pols[r] % SIZE;
		if (state[a][b]) pols.erase(pols.begin() + r);
		else {
			state[a][b] = turn;//更新状态
			if (OB) print();
			checkFin(a, b);
			turn = 3 - turn;//接下来轮到另一个玩家行动
			return;
		}
	}
	*/
	vector<int> pols(POLNUM);//待选决策列表
	for (int i = 0; i < POLNUM; ++i) pols[i] = i;//初始化，加入所有决策
	//随机选取
	int a = -1, b = -1;
	for (int i = 0; i < SIZE;) {
		int r = rand() % pols.size();
		pols.erase(pols.begin() + r);
		if (!state[pols[r] / SIZE][pols[r] % SIZE]) {
			++i;
			a = pols[r] / SIZE, b = pols[r] % SIZE;
			if (density[a][b]) break;
		}
		if (!pols.size()) break;
	}
	if (~a) {
		state[a][b] = turn;//更新状态
		if (OB) print();
		checkFin(a, b);
		turn = 3 - turn;//接	下来轮到另一个玩家行动
	} else {
		fin = true;
		bpv = -1;
	}
}

int Gobang::BPValue() {
	return bpv;
}

bool Gobang::isFinish() {
	return fin;
}

void Gobang::setState(mcTN* p) {
	//重设状态和标记
	fin = false;
	bpv = -1;
	state = vector<vector<int>>(SIZE, vector<int>(SIZE));
	density = vector<vector<int>>(SIZE, vector<int>(SIZE));

	stack<int> pols;//保存决策序列
	for (; p->pa; p = p->pa) pols.push(p->pol);//沿决策树向根节点移动，得到决策序列
	//根据决策序列，推导当前状态
	for (int i = 1; !pols.empty(); ++i) {
		int tmp = pols.top();
		state[tmp / SIZE][tmp % SIZE] = 2 - i % 2;
		checkFin(tmp / SIZE, tmp % SIZE);
		if (isFinish()) return;
		pols.pop();
	}
}

void Gobang::print() {
	//打印状态
	cout << "*  ";
	for (int j = 0; j < 10 && j < SIZE; ++j) cout << j << "  ";
	for (int j = 10; j < SIZE; ++j) cout << j << " ";
	cout << endl;
	for (int i = 0; i < 10 && i < SIZE; ++i) {
		cout << i << "  ";
		for (int j = 0; j < SIZE; ++j) cout << (state[i][j] ? char(state[i][j] + '0') : '.') << "  ";
		cout << endl;
	}
	for (int i = 10; i < SIZE; ++i) {
		cout << i << " ";
		for (int j = 0; j < SIZE; ++j) cout << (state[i][j] ? char(state[i][j] + '0') : '.') << "  ";
		cout << endl;
	}
	cout << endl;
}

void Gobang::train(mcTN * pos, int times, const double& fac) {
	int cnt = 0;
	for (; cnt < times; cnt++) {
		if (OB) cout << "train " << cnt + 1 << endl;
		if (!UCTSearch(pos, fac)) break;//如果已经没有未完全扩展的节点，提前结束训练
	}
	//重置标记
	fin = false;
	bpv = -1;
	cout << "Studying for " << cnt << " times" << endl;
}

void Gobang::simulate(int extra_times, const double & fac) {
	//用于临时保存状态
	vector<vector<int>> si_state(SIZE, vector<int>(SIZE));
	vector<vector<int>> si_density(SIZE, vector<int>(SIZE));

	setState(root);
	print();
	int op;//从控制台读入决策
	int finish;//临时记录游戏结果
	mcTN* rbt = root;//记录AI在决策树中的节点位置
	for (;;) {

		//临时进行额外训练
		swap(si_state, state);
		swap(si_density, density);
		train(rbt, extra_times, fac);
		swap(si_state, state);
		swap(si_density, density);

		//根据访问次数排序，访问次数相同时，再根据获胜期望排序
		//这是因为访问次数多的节点一定是更可靠的，而获胜期望高的不一定
		sort(rbt->ch.begin(), rbt->ch.end(), [](mcTN* a, mcTN* b) {
			if (a->vis == b->vis) {
				return a->val > b->val;
			} else return a->vis > b->vis;
		});
		//输出先手方在决策树中接下来的决策，按从优到劣排序
		for (int len = std::min((int)rbt->ch.size(), 10), i = 0; i < len; ++i) {
			cout << "Your choice " << i + 1 << " is " << rbt->ch[i]->pol / SIZE << "," << rbt->ch[i]->pol%SIZE;
			cout << " (" << rbt->ch[i]->val << "/" << rbt->ch[i]->vis << ")" << endl;
		}

		//输入决策
		int op_a, op_b;
		cin >> op_a >> op_b;

		//如果选择了不可行的决策，提示重新输入
		while (op_a < 0 || op_a >= SIZE || op_b < 0 || op_b >= SIZE || state[op_a][op_b]) {
			cout << "invalid!" << endl;
			cin >> op_a >> op_b;
		}
		op = op_a * SIZE + op_b;

		//更新状态
		state[op_a][op_b] = 1;
		checkFin(op_a, op_b);
		//判断是否结束
		if (isFinish()) {
			finish = 2 - BPValue();
			if (finish == 3) cout << "Drawn" << endl;//输出和棋
			else cout << (finish == 1 ? "Player wins" : "AI wins") << endl;//输出赢家
			break;
		}
		print();


		//根据玩家的输入，寻找决策树中对应的下一个节点
		vector<mcTN*>::iterator fd = std::find_if(rbt->ch.begin(), rbt->ch.end(), [&op](mcTN* p) {
			return p->pol == op;
		});
		if (fd == rbt->ch.end()) {
			if (rbt->dp == POLNUM) {
				cout << "Drawn" << endl;
				break;
			}
			cout << "AI can't play!" << endl;//如果决策树不存在这个节点，输出AI无法继续，中止游戏
			break;
		} else {
			//更新记录的当前节点
			rbt = *fd;
			//删除父节点的其他子节点
			rbt->pa->delChildExcept(rbt);
		}

		//临时进行额外训练
		swap(si_state, state);
		swap(si_density, density);
		train(rbt, extra_times, fac);
		swap(si_state, state);
		swap(si_density, density);

		if (rbt->ch.empty()) {
			cout << "AI can't play!" << endl;//如果决策树不存在下一个节点，输出AI无法继续，中止游戏
			break;
		} else {
			sort(rbt->ch.begin(), rbt->ch.end(), [](mcTN* a, mcTN* b) {
				if (a->vis == b->vis) {
					return a->val < b->val;
				} else return a->vis > b->vis;
			});
			//输出后手方在决策树中接下来的决策，按从优到劣排序
			for (int len = std::min((int)rbt->ch.size(), 10), i = 0; i < len; ++i) {
				cout << "My choice " << i + 1 << " is " << rbt->ch[i]->pol / SIZE << "," << rbt->ch[i]->pol%SIZE;
				cout << " (" << rbt->ch[i]->vis - rbt->ch[i]->val << "/" << rbt->ch[i]->vis << ")" << endl;
			}
			rbt = rbt->ch[0];//AI选择最优决策
			rbt->pa->delChildExcept(rbt);
		}
		//更新状态
		state[rbt->pol / SIZE][rbt->pol % SIZE] = 2;
		checkFin(rbt->pol / SIZE, rbt->pol % SIZE);
		//判断是否结束
		if (isFinish()) {
			finish = 2 - BPValue();
			if (finish == 3) cout << "Drawn" << endl;//输出和棋
			else cout << (finish == 1 ? "Player wins" : "AI wins") << endl;//输出赢家
			break;
		}
		print();
	}
	system("pause");
}

bool Gobang::isValid(mcTN * p, int pol) {
	return !state[pol / SIZE][pol % SIZE];//必须下在非空格子里
}

int main() {
	Gobang gb;
	constexpr int times = 400000;
	constexpr double fac = 0.8;
	gb.train(gb.root, times, fac);
	gb.simulate(times, fac);
}