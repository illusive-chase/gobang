#include "MCT.h"

void mcTN::updateBestChild(const double& fac) {
	mcTN* res = nullptr;
	double maxUCT = 0;
	double cal;
	//���������ӽڵ�ΪUCB1ֵ��ߵ��ӽڵ�
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
	for (; p && fullyExpanded(p); p = p->bc);//Ѱ��δ��ȫ��չ�Ľڵ�
	if (!p) return false;//��ǰ��ֹ
	setState(p);//���þ������ڵ����ö�Ӧ��״̬
	vector<bool> close(POLNUM);//���ڱ�ǳ������еĲ���
	for (mcTN* it : p->ch) close[it->pol] = true;//���еĲ��Բ�����
	
	for (int i = 0; i < POLNUM; ++i) {
		//���������о���
		if (!close[i] && isValid(p, i)) {
			mcTN* tmp = p->addChild(i);//��չһ��δ�����ʵ��ӽڵ�
			setState(tmp);//���þ������ڵ����ö�Ӧ��״̬
			if (OB) print();
			turn = tmp->dp % 2 + 1;//turn��ǽ������ֵ��ĸ����
			expand();//ģ����
			backPropagation(tmp, BPValue(), fac);//����ʼ�ڵ㷴�򴫲�ֱ�����ڵ�
			if (OB) system("pause");
			return true;//�ɹ�
		}
	}
	return false;
}

void iTrain::expand() {
	//����ģ����
	while (!isFinish()) fastPolicy();
	if (OB) cout << BPValue() << endl;
}

void iTrain::backPropagation(mcTN * p, int bp_val, const double& fac) {
	if (~bp_val) {
		//�������ƽ�֣�����bp_val����·���ϵĽڵ�
		bp_val <<= 1;
		while (p) {
			p->val += bp_val;
			p->vis += 2;
			p->updateBestChild(fac);
			p = p->pa;
		}
	} else {
		//���ƽ�֣��� bp_val=-1
		while (p) {
			++p->val;
			p->vis += 2;
			p->updateBestChild(fac);
			p = p->pa;
		}
	}
}



