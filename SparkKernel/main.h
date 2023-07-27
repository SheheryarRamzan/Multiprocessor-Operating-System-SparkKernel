#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include<cmath>
#include<fstream>
#include<cstring>
#include<sstream>
#include<iomanip>
#include<semaphore.h>
#include<time.h>
#include<signal.h>
#include<queue>
#include <iterator>
#include <list>
using namespace std;
//sem_t currentArrSem;
void idle(int);
void context_switch();
void sortReadyQueue();
int switchCounter=0;

void inputProcess(string, int);
struct process {
	string name, processType;
	int priority, ioTime;
	float arrivalTime, cpuTime, newTime;

	process() {
		name = "Idle"; processType = "";
		priority = 0; ioTime = 0;
		arrivalTime = 0; cpuTime = 0;
	}

}processes[20], idleProcess;

struct PCB {
	string pName, state;

};
void readFile(string filename) {
	string fileline;
	ifstream fin;
	int counter = 0;
	fin.open(filename);
	getline(fin, fileline);

	while (!fin.eof()) {
		getline(fin, fileline);
		if (fileline == "") { continue; }
		inputProcess(fileline, counter);
		counter++;
	}

	fin.close();
}
void writeFile(string filename, string output){
	ofstream fout;
	fout.open(filename,ios::app);
	fout<<output<<endl;
	fout.close();
}

queue<process> newList;
queue<process> waitList;
list<process> readyQueue;


void inputProcess(string line, int processCounter) {
	char* charline = &line[0];
	char* token;
	string reline = "";
	token = strtok(charline, "\t");
	reline = token;
	processes[processCounter].name = reline;
	stringstream geek(reline);


	int tokenCounter = 1;
	while (token != NULL) {

		token = strtok(NULL, "\t");
		if (token == NULL) { break; }
		reline = token;
		stringstream geek(reline);
		int value = 0;
		geek >> value;

		if (tokenCounter == 1)
			processes[processCounter].priority = value;
		else if (tokenCounter == 2)
			processes[processCounter].arrivalTime = value;
		else if (tokenCounter == 3)
			processes[processCounter].processType = reline;
		else if (tokenCounter == 4)
			processes[processCounter].cpuTime = value;
		else if (tokenCounter == 5)
			processes[processCounter].ioTime = value;
		tokenCounter++;
	}
	if(tokenCounter == 4){
		processes[processCounter].cpuTime = rand()%20;
	}
	cout<<processes[processCounter].name <<'\t'<<processes[processCounter].arrivalTime <<'\t'<<processes[processCounter].processType <<'\t'<<processes[processCounter].cpuTime <<'\t'<<processes[processCounter].ioTime <<endl;
    newList.push(processes[processCounter]);
}

void _start(string filename) {
	readFile(filename);
}

struct CPUs{
	int id;
	process runningProcess;
	CPUs(){
		id=-1;
	}
};CPUs* currentArr = new CPUs[numPro];

void schedule(int cpuId){  
	cout<<"schedule fucntion beign called "<<cpuId<<endl;
    pthread_mutex_lock(&readyArrSem);
	if(readyQueue.empty()){idle(cpuId);}
    pthread_mutex_unlock(&readyArrSem);

    pthread_mutex_lock(&currentArrSem);
	cout<<"enter schedule fucntion beign called "<<cpuId<<endl;
	sortReadyQueue();
	process nextP = readyQueue.front();readyQueue.pop_front();
	context_switch();
    currentArr[cpuId].runningProcess = nextP;
	cout<<"oyut schedule fucntion beign called "<<cpuId<<endl;

    pthread_mutex_unlock(&currentArrSem);
}

void preempt(int cpuId){
	pthread_mutex_lock(&currentArrSem);
        process p = currentArr[cpuId].runningProcess;

    readyQueue.push_back(p);
    pthread_mutex_unlock(&currentArrSem);
    schedule(cpuId);
}
void terminate(int cpuId){
	pthread_mutex_lock(&currentArrSem);
	currentArr[cpuId].runningProcess=idleProcess;
    pthread_mutex_unlock(&currentArrSem);
    schedule(cpuId);
}

void yield(int cpuId){
	pthread_mutex_lock(&currentArrSem);

	currentArr[cpuId].runningProcess.newTime=timeCounter+1;
	currentArr[cpuId].runningProcess.newTime= currentArr[cpuId].runningProcess.newTime+2;
	currentArr[cpuId].runningProcess.ioTime--;
	waitList.push(currentArr[cpuId].runningProcess);
    pthread_mutex_unlock(&currentArrSem);
    schedule(cpuId);
}
void context_switch(){switchCounter++;}
void wake_up(int cpuId){}


void idle(int cpuId){
	pthread_mutex_lock(&currentArrSem);
		cout<<"Idle fucntion beign called "<<cpuId<<endl;
    while (readyQueue.size() == 0) {
		currentArr[cpuId].runningProcess=idleProcess;
        pthread_cond_wait(&cond1, &currentArrSem);
		cout<<"Idle empty fucntion beign called "<<cpuId<<endl;
    }
    pthread_mutex_unlock(&currentArrSem);
	cout<<"...."<<endl;
    schedule(cpuId);
}

void sortReadyQueue(){
	
	process* arr = new process[readyQueue.size()];
	int size = readyQueue.size();
	for(int i=0;i<size;i++){
		arr[i] = readyQueue.front();readyQueue.pop_front();
	}
	process temp;
    for (int i = 0; i < size - 1; i++){
        for (int j = 0; j < size - i - 1; j++){
            if (arr[j].arrivalTime > arr[j+1].arrivalTime)
               {
				   temp = arr[j];
				   arr[j]=arr[j+1];
				   arr[j+1]=temp;
			   }
		}
	}

	for(int i=0;i<size;i++){
		readyQueue.push_back(arr[i]);
	}
}
