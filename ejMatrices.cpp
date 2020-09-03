#include <iostream>
#include <stdlib.h>
#include<time.h>
#include <ctime>
#include <chrono>
	
using namespace std;

#define MAX 100

void fillMatrix(int a[MAX][MAX]){
	for(int i = 0; i < MAX; i++){
		for(int j = 0; j < MAX; j++){
			a[i][j] = rand()%10;
		}
	}
}

	
void printMatrix(int a[MAX][MAX]){
	for(int i = 0; i < MAX; i++){
		for(int j = 0; j < MAX; j++){
			cout << a[i][j] << "-";
		}
		cout << endl;
	}
	cout << endl;
}
	
void mult_clasica(int a[MAX][MAX],int b[MAX][MAX]){
	int ans[MAX][MAX] = {0};
		for(int i = 0; i < MAX; ++i){
			for(int j = 0; j < MAX; ++j){
				for(int k = 0; k < MAX; ++k){
					ans[i][j] += a[i][k] * b[k][j];
				}
			}   
		}
	//	printMatrix(ans);
}
			

	void mult_bloques(int a[MAX][MAX],int b[MAX][MAX], int B){
		int ans[MAX][MAX] = {0};
		int i,j,k,m;
		for (i=0; i<MAX; i++){
			for (j=0; j<MAX; j++){
				ans[i][j] = 0;
				for (j=0; j<MAX; j+=B){
					for (i=0; i<MAX; i++){
						for (k=0; k<MAX; k++){ 
							int aux = a[i][k];
							for (m=j; m<j+B; m++)
								ans[i][m] = ans[i][m] + aux * b[k][m];
						}
					}
				}
			}
		}
		//printMatrix(ans);
	}
int main(){
	srand(time(NULL));
	int a[MAX][MAX];
	int b[MAX][MAX];
	fillMatrix(a);
	fillMatrix(b);
	unsigned t0, t1;
	t0 = clock();
	mult_clasica(a,b);
	t1 = clock();
	double time = (double(t1 - t0) / CLOCKS_PER_SEC);
	cout<<"Multiplicacion de Matrices: " << time << endl;
	
	t0 = clock();
	mult_bloques(a,b,20);
	t1 = clock();
	time = (double(t1-t0)/CLOCKS_PER_SEC);
	cout<< "Multiplicacion de Matrices por Bloques: " << time << endl;
	

	return 0;
	}
