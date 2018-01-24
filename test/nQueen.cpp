#include <vector>
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

const static int MAX_THREAD = 8;

bool verify(vector<int> status){
	for (int i = 0; i < status.size(); i++){
		for (int j = i + 1; j < status.size(); j++){
			if (status[i] == status[j] ||
				abs(status[i] - status[j]) == abs(i - j)){
				return false;
			}
		}
	}
	return true;
}

int nQueenSolver(vector<int> status, int q, int left){
	if (!verify(status)){
		return 0;
	}
	if (left == 0){
		return 1;
	}
	int result = 0;
	for (int i = 0; i < q; i++){
		vector<int> temp = status;
		temp.push_back(i);
		result += nQueenSolver(temp, q, left - 1);
	}
	return result;
}

typedef struct threadData{
	int start;
	int end;
	int q;
	int* result;
} threadData;

void* threadSolver(void* data){
	threadData* ptr = (threadData*)data;
	vector<int> status(ptr->q);
	status.clear();
	int result = 0;
	for(int i=ptr->start; i<ptr->end; i++){
		vector<int> temp = status;
		temp.push_back(i);
		result += nQueenSolver(temp, ptr->q, (ptr->q)-1);
		cout << "thread " << ptr->start << " " << clock() << endl;
	}
	*(ptr->result) = result;
}

int main(int argc, char* argv[]){
	vector<int> status;
	if(argc!=3){
		cout << "wrong argc" << endl;
		return 0;
	}
	int n = atoi(argv[1]);
	int t = atoi(argv[2]);
	
	pthread_t pid[MAX_THREAD];
	int result[MAX_THREAD] = {0};
	threadData datas[MAX_THREAD];
	clock_t t0, t1, mt0, mt1, mt2;

	t0 = clock();
	cout << clock() << endl;
	int slice = n / t;
	int leftTask = n - slice*t;
	vector<int> task(t);
	for(int i=0; i<t; i++){
		task[i] = slice;
		if(i<leftTask) task[i]++;
	}
	cout << clock() << endl;
	int total = 0;
	for(int i=0; i<t; i++){
		datas[i] = {total, total+task[i], n, &result[i]};
		pthread_create(&pid[i], nullptr, threadSolver, (void*)&datas[i]);
		total+=task[i];
	}
	cout << clock() << endl;
	for(int i=0; i<t; i++){
		pthread_join(pid[i], 0);
		cout << clock() << endl;
	}
	//int result = nQueenSolver(status, n, n);
	t1 = clock();
	int collection = 0;
	for(int i=0; i<t; i++){
		collection += result[i];
	}	
	
	cout << collection << " " << (t1-t0)*1000/CLOCKS_PER_SEC << "ms" << endl;
}
