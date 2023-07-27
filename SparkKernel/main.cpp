#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <math.h>
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
#include<chrono>
#include <iterator>
#include <list>
using namespace std::chrono;

// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t currentArrSem = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readyArrSem = PTHREAD_MUTEX_INITIALIZER;

int numPro = 4, timeCounter=0;
#include "main.h"
#include<queue>
using namespace std;
string outputFilename;
double start = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

void* processorFunction(void* arg){
	sleep(1);
	int cpuId = *(int*) arg ;	
	cout<<endl<<"U r in \t"<<cpuId<<'\t'<<currentArr[cpuId].id<<"."<<currentArr[cpuId].runningProcess.name<<"."<<endl;
	while(1){
		if(currentArr[cpuId].runningProcess.name == "Idle"){
			cout<<"Idle  "<< currentArr[cpuId].id<<"  "<<currentArr[cpuId].runningProcess.name <<endl;
			idle(currentArr[cpuId].id);
		}
		else
			{
				if(currentArr[cpuId].runningProcess.cpuTime>0){
					currentArr[cpuId].runningProcess.cpuTime--;
					cout<<currentArr[cpuId].runningProcess.name<<'\t'<<currentArr[cpuId].runningProcess.cpuTime<< endl;
					sleep(1);
				}
				else if(currentArr[cpuId].runningProcess.cpuTime==0 && currentArr[cpuId].runningProcess.ioTime<=0){
					cout<<"Finish"<<endl;

					terminate(currentArr[cpuId].id);
				}
				else if(currentArr[cpuId].runningProcess.cpuTime==0 && currentArr[cpuId].runningProcess.ioTime>0){
					cout<<"yield"<<endl;
					yield(currentArr[cpuId].id);
				}
			}	
	}
	cout<<"\nEnd\n";
}


void* controllerFunction(void* arg){
	while(1){
		auto millisec_since_epoch = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();	
		int cls =  millisec_since_epoch - start - 1;
		if(readyQueue.empty() && newList.empty() && currentArr[0].runningProcess.name == "Idle" && currentArr[1].runningProcess.name == "Idle" && currentArr[2].runningProcess.name == "Idle" && currentArr[3].runningProcess.name == "Idle"){
			cout<<"Number of context switches: "<<switchCounter<<endl;
			cout<<"Execution Time: "<<cls<<endl;
			exit(1);
		}
		// while(1){
		// 	if(waitList.front().newTime == timeCounter && waitList.front().name!="Idle" ){
		// 		cout<<"\n\n\n ***********returning from wait "<<waitList.front().name<<'\t'<<waitList.front().newTime<<endl;
		// 		if(waitList.front().name=="Idle"){waitList.pop();}
		// 		else{
		// 			readyQueue.push_back(waitList.front());waitList.pop();
		// 			pthread_cond_signal(&cond1);
		// 		}
		// 	}
		// 	else{
		
				if(waitList.front().name=="Idle"){
							cout<<"\n\n\n ***********returning from wait "<<waitList.front().name<<'\t'<<waitList.front().newTime<<endl;
					waitList.pop();
					}
		// 		break;
		// 	}	
		// }
		if(newList.front().arrivalTime==timeCounter){
			readyQueue.push_back(newList.front());
			newList.pop();
			pthread_cond_signal(&cond1);
		}

	

		string output = "\t------------------------" + to_string(cls) +'\t' + to_string(readyQueue.size()) +'\t'+ to_string(waitList.size());

		//cout<<"-----------------------"<<cls<<'\t'<<currentArr[0].runningProcess.name<<'\t'<<currentArr[1].runningProcess.name<<endl;
		for(int i=0;i<numPro;i++){
			output += '\t'+currentArr[i].runningProcess.name;
		}
		cout<<output+'\t'+to_string(switchCounter)<<endl;
		writeFile(outputFilename,output);
		sleep(1);
		timeCounter++;
	}
}



int main(int args, char** argv) {
	cout<<"PName\tArrivalTime\tProcTupe\tcpuTime\tioTime"<<endl;
	int intArr[4] = {0,1,2,3};
	string filename = argv[1];
	numPro = atoi(argv[2]);
	outputFilename = argv[5];
	waitList.push(idleProcess);
	_start(filename);
	pthread_t* proThreads = new pthread_t[numPro];
	pthread_t conThread;


	for(int i=0;i<numPro;i++){currentArr[i].id=i;}

	for (int i = 0; i < numPro; i++) {
			if(i==0){
			pthread_create(&conThread, NULL,controllerFunction,NULL);
		}
		pthread_create(&proThreads[i],NULL,processorFunction,(void*)&intArr[i]);
	
	}
	pthread_join(conThread, NULL);
}