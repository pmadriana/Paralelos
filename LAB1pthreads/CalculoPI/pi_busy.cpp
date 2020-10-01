#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;

#define thread_count 32
const int n = 100000000;

double sum = 0.0;
int flag = 0;

///GLOBAL SUM WITH CRITICAL SECTION AFTER LOOP
void* Thread_sum(void* rank)
{
    long my_rank = (long) rank;
    double factor, my_sum = 0.0;
    long long i;
    long long my_n = n/thread_count;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;

    if (my_first_i % 2 == 0)
    {
        factor = 1.0;
    }
    else
    {
        factor = -1.0;
    }

    for (i = my_first_i; i < my_last_i; i++, factor = -factor)
    {
        my_sum += factor/(2*i+1);
    }

    while(flag != my_rank);
    sum += my_sum;
    flag = (flag+1) % thread_count;

    return NULL;
}

int main()
{
   pthread_t threads[thread_count];
   long i;

   //Tiempo de inicio
    auto start = std::chrono::high_resolution_clock::now();

   for(i = 0; i < thread_count; i++ )
   {
      pthread_create(&threads[i], NULL, Thread_sum, (void *)i);
   }

   for(i= 0; i<thread_count; i++)
   {
       pthread_join(threads[i], NULL);
   }

   free(threads);
   sum = 4.0 * sum;

   //Tiempo de fin
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << "Tiempo de ejecucion: " << elapsed.count() << " segundos" << endl << endl;

    cout<<"Pi calculado: "<<  fixed << setprecision(10) << sum;

    return 0;
}
