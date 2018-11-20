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

//OB��ʾ�Ƿ��ӡѵ���Ĺ���
constexpr bool OB = false;

class mcTN {
public:
	int vis, val;//vis��ʾ���ʴ���*2��val��ʾ���ֻ�ʤ����*2+ƽ�ִ���
	mcTN* bc;//��������ӽڵ㣬��ʾ����������Ѿ���
	mcTN* pa;//���ڵ�
	vector<mcTN*> ch;//�ӽڵ�ļ���
	int pol;//���ڵ㵽��ǰ�ڵ���ʹ�õľ���
	int dp;//�ھ������е����

	mcTN(int pol, mcTN* pa = nullptr) :vis(0), val(0), bc(nullptr), pa(pa), pol(pol), dp(pa ? pa->dp + 1 : 0) {}
	~mcTN() { for (mcTN* p : ch) delete p; }

	//���������ӽڵ�
	void updateBestChild(const double& fac = 1.0);

	//����ӽڵ㲢������
	mcTN* addChild(int pol) { 
		ch.push_back(new mcTN(pol, this));
		return *(ch.end() - 1);
	}

	//ɾ��������ӽڵ�
	void delChildExcept(mcTN* p) {
		for (mcTN* cn : ch) {
			if (cn != p) delete cn;
		}
		ch.clear();
		ch.push_back(p);
	}
};

//����UCB1ֵ��facԽС��ʾ������Խ�����ڿ��ǵ�ǰ��������ǽ���������
inline double UCB1(mcTN* p, const double& fac = 1.0);

class iTrain {
public:
	//���������߷�֧��
	iTrain(int polnum) :POLNUM(polnum) {}

	virtual bool isValid(mcTN* p, int pol) = 0;//�����Ƿ�Ϸ�
	virtual void fastPolicy() = 0;//����ģ�⵽��һ��״̬
	virtual int BPValue() = 0;//��Ϸ�����1��ʾ����ʤ����0��ʾ����ʤ����-1��ʾƽ��
	virtual bool isFinish() = 0;//��ǰ״̬�Ƿ�ﵽ��Ϸ����
	virtual void setState(mcTN* p) = 0;//���ݾ������ڵ�����״̬
	virtual void print() = 0;//��ӡ״̬
	virtual bool fullyExpanded(mcTN* p) = 0;//p�Ƿ���ȫ��չ
	virtual void simulate(int extra_times, const double & fac) = 0;//ģ��Ծ֣���������ÿһ�������ѵ������
	virtual void train(mcTN * pos, int times, const double & fac) = 0;//ѡȡ�ڵ�pѵ��һ������

	const int POLNUM;//�����߷�֧��
	int turn;//����ֵ��ĸ����
	vector<vector<int>> state;//״̬�����̣�
	mcTN* root;//�������ĸ��ڵ�
	bool UCTSearch(mcTN * p, const double& fac);//UCT�㷨ѡȡ�ڵ�p����ȡ��ѧϰ
	void expand();//����ģ����
	void backPropagation(mcTN* p, int new_val, const double& fac);//���򴫲����
	
};



