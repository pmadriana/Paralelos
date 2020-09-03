
constexpr auto MAX = 10000;
#include <iostream>
#include<stdlib.h>
#include<time.h>
#include <ctime>

double A[MAX][MAX];
double x[MAX];
double y[MAX];


void generateRandomNumbers() {
	for (int i = 0; i < MAX; ++i)
	{
		x[i] = rand();
		y[i] = 0.0;
		for (int j = 0; j < MAX; ++j)
		{
			A[i][j] = rand();
		}
	}


}

void primer_loop(){ //4 misses
	init_y();
	for (int i = 0; i < MAX; i++){  
		for (int j = 0; j < MAX; j++){
			y[i] += A[i][j]*x[j];
		}
	}		
}
	
	
void segundo_loop(){ //16 misses
	init_y();
	for (int j = 0; j < MAX; j++){
		for (int i = 0; i < MAX; i++){
			y[i] += A[i][j]*x[j];
		}
	}
}


int main()
{
	srand(time(0));
	generateRandomNumbers();
	unsigned t0, t1;
	t0 = clock();
	primer_loop();
	t1 = clock();
	double time = (double(t1 - t0) / CLOCKS_PER_SEC);
	std::cout << "Tiempo de ejecucion primera funcion: " << time << std::endl;
	generateRandomNumbers();
	t0 = clock();
	segundo_loop();
	t1 = clock();
	time = (double(t1 - t0) / CLOCKS_PER_SEC);
	std::cout << "Tiempo de ejecucion segunda funcion: " << time << std::endl;


	return 0;
	
}
