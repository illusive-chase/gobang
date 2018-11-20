#include "MCT.h"
//�������̴�С
constexpr int SIZE = 11;
//�̳�iTrain�ӿ�
class Gobang :public iTrain {
public:

	int bpv;
	bool fin;
	vector<vector<int>> density;
	void checkFin(int x, int y);

	//ʵ��iTrain�ӿ�

	void fastPolicy();
	int BPValue();
	bool isFinish();
	void setState(mcTN * p);
	void print();
	void train(mcTN * pos, int times, const double & fac);
	bool fullyExpanded(mcTN* p);
	void simulate(int extra_times, const double & fac);
	bool isValid(mcTN* p, int pol);

	//����������ӣ����������߷�֧��ΪSIZE*SIZE����ʼ��״̬
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
	return p->ch.size() == POLNUM - p->dp;//�ж��Ƿ���ȫ��չ
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

	//�м���
	cnt = 1;
	for (int i = x - 1; ~i && state[i][y] == role; --i) ++cnt;
	for (int i = x + 1; i < SIZE && state[i][y] == role; ++i) ++cnt;
	if (cnt > 4) {
		fin = true;
		bpv = 2 - role;
		return;
	}
	//�м���
	cnt = 1;
	for (int i = y - 1; ~i && state[x][i] == role; --i) ++cnt;
	for (int i = y + 1; i < SIZE && state[x][i] == role; ++i) ++cnt;
	if (cnt > 4) {
		fin = true;
		bpv = 2 - role;
		return;
	}
	//���Խ���
	cnt = 1;
	for (int i = x - 1, j = y - 1; ~i && ~j && state[i][j] == role; --i, --j) ++cnt;
	for (int i = x + 1, j = y + 1; i < SIZE && j < SIZE && state[i][j] == role; ++i, ++j) ++cnt;
	if (cnt > 4) {
		fin = true;
		bpv = 2 - role;
		return;
	}
	//���Խ���
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
	vector<int> pols(POLNUM);//��ѡ�����б�
	for (int i = 0; i < POLNUM; ++i) pols[i] = i;//��ʼ�����������о���
	//���ѡȡ
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
			state[a][b] = turn;//����״̬
			if (OB) print();
			checkFin(a, b);
			turn = 3 - turn;//�������ֵ���һ������ж�
			return;
		}
	}
	*/
	vector<int> pols(POLNUM);//��ѡ�����б�
	for (int i = 0; i < POLNUM; ++i) pols[i] = i;//��ʼ�����������о���
	//���ѡȡ
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
		state[a][b] = turn;//����״̬
		if (OB) print();
		checkFin(a, b);
		turn = 3 - turn;//��	�����ֵ���һ������ж�
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
	//����״̬�ͱ��
	fin = false;
	bpv = -1;
	state = vector<vector<int>>(SIZE, vector<int>(SIZE));
	density = vector<vector<int>>(SIZE, vector<int>(SIZE));

	stack<int> pols;//�����������
	for (; p->pa; p = p->pa) pols.push(p->pol);//�ؾ���������ڵ��ƶ����õ���������
	//���ݾ������У��Ƶ���ǰ״̬
	for (int i = 1; !pols.empty(); ++i) {
		int tmp = pols.top();
		state[tmp / SIZE][tmp % SIZE] = 2 - i % 2;
		checkFin(tmp / SIZE, tmp % SIZE);
		if (isFinish()) return;
		pols.pop();
	}
}

void Gobang::print() {
	//��ӡ״̬
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
		if (!UCTSearch(pos, fac)) break;//����Ѿ�û��δ��ȫ��չ�Ľڵ㣬��ǰ����ѵ��
	}
	//���ñ��
	fin = false;
	bpv = -1;
	cout << "Studying for " << cnt << " times" << endl;
}

void Gobang::simulate(int extra_times, const double & fac) {
	//������ʱ����״̬
	vector<vector<int>> si_state(SIZE, vector<int>(SIZE));
	vector<vector<int>> si_density(SIZE, vector<int>(SIZE));

	setState(root);
	print();
	int op;//�ӿ���̨�������
	int finish;//��ʱ��¼��Ϸ���
	mcTN* rbt = root;//��¼AI�ھ������еĽڵ�λ��
	for (;;) {

		//��ʱ���ж���ѵ��
		swap(si_state, state);
		swap(si_density, density);
		train(rbt, extra_times, fac);
		swap(si_state, state);
		swap(si_density, density);

		//���ݷ��ʴ������򣬷��ʴ�����ͬʱ���ٸ��ݻ�ʤ��������
		//������Ϊ���ʴ�����Ľڵ�һ���Ǹ��ɿ��ģ�����ʤ�����ߵĲ�һ��
		sort(rbt->ch.begin(), rbt->ch.end(), [](mcTN* a, mcTN* b) {
			if (a->vis == b->vis) {
				return a->val > b->val;
			} else return a->vis > b->vis;
		});
		//������ַ��ھ������н������ľ��ߣ������ŵ�������
		for (int len = std::min((int)rbt->ch.size(), 10), i = 0; i < len; ++i) {
			cout << "Your choice " << i + 1 << " is " << rbt->ch[i]->pol / SIZE << "," << rbt->ch[i]->pol%SIZE;
			cout << " (" << rbt->ch[i]->val << "/" << rbt->ch[i]->vis << ")" << endl;
		}

		//�������
		int op_a, op_b;
		cin >> op_a >> op_b;

		//���ѡ���˲����еľ��ߣ���ʾ��������
		while (op_a < 0 || op_a >= SIZE || op_b < 0 || op_b >= SIZE || state[op_a][op_b]) {
			cout << "invalid!" << endl;
			cin >> op_a >> op_b;
		}
		op = op_a * SIZE + op_b;

		//����״̬
		state[op_a][op_b] = 1;
		checkFin(op_a, op_b);
		//�ж��Ƿ����
		if (isFinish()) {
			finish = 2 - BPValue();
			if (finish == 3) cout << "Drawn" << endl;//�������
			else cout << (finish == 1 ? "Player wins" : "AI wins") << endl;//���Ӯ��
			break;
		}
		print();


		//������ҵ����룬Ѱ�Ҿ������ж�Ӧ����һ���ڵ�
		vector<mcTN*>::iterator fd = std::find_if(rbt->ch.begin(), rbt->ch.end(), [&op](mcTN* p) {
			return p->pol == op;
		});
		if (fd == rbt->ch.end()) {
			if (rbt->dp == POLNUM) {
				cout << "Drawn" << endl;
				break;
			}
			cout << "AI can't play!" << endl;//�������������������ڵ㣬���AI�޷���������ֹ��Ϸ
			break;
		} else {
			//���¼�¼�ĵ�ǰ�ڵ�
			rbt = *fd;
			//ɾ�����ڵ�������ӽڵ�
			rbt->pa->delChildExcept(rbt);
		}

		//��ʱ���ж���ѵ��
		swap(si_state, state);
		swap(si_density, density);
		train(rbt, extra_times, fac);
		swap(si_state, state);
		swap(si_density, density);

		if (rbt->ch.empty()) {
			cout << "AI can't play!" << endl;//�����������������һ���ڵ㣬���AI�޷���������ֹ��Ϸ
			break;
		} else {
			sort(rbt->ch.begin(), rbt->ch.end(), [](mcTN* a, mcTN* b) {
				if (a->vis == b->vis) {
					return a->val < b->val;
				} else return a->vis > b->vis;
			});
			//������ַ��ھ������н������ľ��ߣ������ŵ�������
			for (int len = std::min((int)rbt->ch.size(), 10), i = 0; i < len; ++i) {
				cout << "My choice " << i + 1 << " is " << rbt->ch[i]->pol / SIZE << "," << rbt->ch[i]->pol%SIZE;
				cout << " (" << rbt->ch[i]->vis - rbt->ch[i]->val << "/" << rbt->ch[i]->vis << ")" << endl;
			}
			rbt = rbt->ch[0];//AIѡ�����ž���
			rbt->pa->delChildExcept(rbt);
		}
		//����״̬
		state[rbt->pol / SIZE][rbt->pol % SIZE] = 2;
		checkFin(rbt->pol / SIZE, rbt->pol % SIZE);
		//�ж��Ƿ����
		if (isFinish()) {
			finish = 2 - BPValue();
			if (finish == 3) cout << "Drawn" << endl;//�������
			else cout << (finish == 1 ? "Player wins" : "AI wins") << endl;//���Ӯ��
			break;
		}
		print();
	}
	system("pause");
}

bool Gobang::isValid(mcTN * p, int pol) {
	return !state[pol / SIZE][pol % SIZE];//�������ڷǿո�����
}

int main() {
	Gobang gb;
	constexpr int times = 400000;
	constexpr double fac = 0.8;
	gb.train(gb.root, times, fac);
	gb.simulate(times, fac);
}