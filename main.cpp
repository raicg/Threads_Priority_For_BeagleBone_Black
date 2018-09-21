
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <math.h>
#include "BlackGPIO/BlackGPIO.h"
#include <sys/resource.h>
#include <sys/syscall.h>
#include "ADC/Adc.h"

using namespace std;

using namespace BlackLib;

int valueADC0, valueADC1;

ADC adc0(AIN0);
ADC adc1(AIN4);
BlackGPIO led0(GPIO_48, output);
BlackGPIO led1(GPIO_60, output);

void carga(int k){
	float f = 0.999999;
	for(int i=0; i<k; i++){
		f = f*f*f*f*f;
		f = 1.56;
		for (int j=0; j<k; j++){
			f = sin(f)*sin(f)*f*f*f;
		}	
	}
}

void *thread_one_function(void *arg){
	int estado=1;
	int priority;
	pid_t tid = syscall(SYS_gettid);
	printf("Thread 1 criada \n");
	while (1){
		if (estado==0){
			printf("estado 0, thread 1, led ligado\n");		
			led0.setValue(high);
			carga(3000);
			estado = 1;
		}	
		else{
			
			priority = ((valueADC0-190)/98)-20;
			printf("estado 1, thread 1, led desligado, prioridade: %d \n", priority);
			int ret = setpriority(PRIO_PROCESS, tid, priority);
			if (ret){
				printf("falha ao setar prioridade do thread 1.\n");
			}
			led0.setValue(low);
			usleep(300000);
			estado = 0;		
		}
	}

}



void *thread_two_function(void *arg){
	int estado=1;
	int priority;
	pid_t tid = syscall(SYS_gettid);
	printf("Thread 2 criada \n");
	while (1){
		if (estado==0){
			printf("estado 0, thread 2, led ligado\n");		
			led1.setValue(high);
			carga(3000);
			estado = 1;
		}	
		else{
			priority = ((valueADC1-190)/98)-20;
			printf("estado 1, thread 2, led desligado, prioridade: %d \n", priority);	
			int ret = setpriority(PRIO_PROCESS, tid, priority);
			if (ret){
				printf("falha ao setar prioridade do thread 2.\n");
			}
			led1.setValue(low);
			usleep(300000);
			estado = 0;		
		}
	}

}



char message[1];

int main() {
	int retorno;

	pthread_t vetorThread[2];

	valueADC0 = adc0.getIntValue();
	printf(" valor do ADC0: %d\n", valueADC0);
	valueADC1 = adc1.getIntValue();
	printf(" valor do ADC1: %d\n", valueADC1);

	printf ("Main - criando a thread: %d \n", 1);
	// criar thread 1
	retorno = pthread_create(&vetorThread[0], NULL, thread_one_function, (void *)1);
	//verificando se foi criado
	if (retorno){
		printf("Erro ao criar a thread: %d\n", retorno);
		exit(-1);
	}

	printf ("Main - criando a thread: %d \n", 2);
	// criar thread 1
	retorno = pthread_create(&vetorThread[1], NULL, thread_two_function, (void *)1);
	//verificando se foi criado
	if (retorno){
		printf("Erro ao criar a thread: %d\n", retorno);
		exit(-1);
	}

	while(1){
		valueADC0 = adc0.getIntValue();
		valueADC1 = adc1.getIntValue();
		sleep(1);
	}

}


