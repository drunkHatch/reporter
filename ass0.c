#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include <time.h>
#include <stdlib.h>

void sig_handler(int signo){

	if (signo == SIGINT)
		{printf("received SIGINT\n");}
	if (signo == SIGALRM)
		system("date");


}

int main(void)
{
	
	signal(SIGALRM, sig_handler);

	signal(SIGINT, sig_handler);

	while(1)
		{
		alarm(5);
		sleep(5);}
	return 0;
}

