#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/* Random ints are less than MAX_KEY */
const int MAX_KEY = 100000000;


/* Struct for list nodes */
struct list_node_s {
   int    data;
   struct list_node_s* next;
};

/* Shared variables */
struct      list_node_s* head = NULL;  
int         thread_count;
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;
pthread_rwlock_t    rwlock;
pthread_mutex_t     count_mutex;
int         member_count = 0, insert_count = 0, delete_count = 0;

/* Setup and cleanup */
void        Usage(char* prog_name);
void        Get_input(int* inserts_in_main_p);

/* Thread function */
void*       Thread_work(void* rank);

/* List operations */
int         Insert(int value);
void        Print(void);
int         Member(int value);
int         Delete(int value);
void        Free_list(void);
int         Is_empty(void);

/* Peter Pacheco my_rand.c */
#define MR_MULTIPLIER 279470273 
#define MR_INCREMENT 0
#define MR_MODULUS 4294967291U
#define MR_DIVISOR ((double) 4294967291U)

unsigned my_rand(unsigned* seed_p) {
	long long z = *seed_p;
	z *= MR_MULTIPLIER; 
	// z += MR_INCREMENT;
	z %= MR_MODULUS;
	*seed_p = z;
	return *seed_p;
}

double my_drand(unsigned* seed_p) {
	unsigned x = my_rand(seed_p);
	double y = x/MR_DIVISOR;
	return y;
}

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int inserts_in_main;
   double start, finish;
   long i; 
   int key, success, attempts;
   pthread_t* thread_handles;
   unsigned seed = 1;
   
   printf("args %d\n", argc);

   if (argc != 4) Usage(argv[0]);
   thread_count = strtol(argv[1],NULL,10);
   
   inserts_in_main = 1000;
   total_ops = 100000;
   search_percent = atof(argv[2]);
   insert_percent = atof(argv[3]);
   delete_percent = 1.0 - (search_percent + insert_percent);
   
   printf("Threads: %d\n", thread_count);

   //Includes creation and deletion of
   //list and rwlocks
   /* Try to insert inserts_in_main keys, but give up after */
   /* 2*inserts_in_main attempts.                           */
   i = attempts = 0;
   while ( i < inserts_in_main && attempts < 2*inserts_in_main ) {
	   key = my_rand(&seed) % MAX_KEY;
	   success = Insert(key);
	   attempts++;
	   if (success) i++;
   }
   printf("Inserted %ld keys in empty list\n", i);
   
#  ifdef OUTPUT
   printf("Before starting threads, list = \n");
   Print();
   printf("\n");
#  endif
   
   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&count_mutex, NULL);
   pthread_rwlock_init(&rwlock, NULL);
   
   GET_TIME(start);
   for (i = 0; i < thread_count; i++)
	   pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);
   
   for (i = 0; i < thread_count; i++)
	   pthread_join(thread_handles[i], NULL);
   GET_TIME(finish);
   //printf("Elapsed time = %e seconds\n", finish - start);
   printf("Elapsed time = %f seconds\n", finish - start);
   printf("Total ops = %d\n", total_ops);
   printf("member ops = %d\n", member_count);
   printf("insert ops = %d\n", insert_count);
   printf("delete ops = %d\n\n", delete_count);
   
   printf("Member pct: %f\n", search_percent*100);
   printf("Insert pct: %f\n", insert_percent*100);
   printf("Delete pct: %f\n", delete_percent*100);
   
#  ifdef OUTPUT
   printf("After threads terminate, list = \n");
   Print();
   printf("\n");
#  endif
   
   Free_list();
   pthread_rwlock_destroy(&rwlock);
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);
   return 0;
}  /* main */


/*-----------------------------------------------------------------*/
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> <search_pct> <insert_pct>\n", prog_name);
   exit(0);
}  /* Usage */

/*-----------------------------------------------------------------*/

/*-----------------------------------------------------------------*/
/* Insert value in correct numerical location into list */
/* If value is not in list, return 1, else return 0 */
int Insert(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   struct list_node_s* temp;
   int rv = 1;
   
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else { /* value in list */
      rv = 0;
   }

   return rv;
}  /* Insert */

/*-----------------------------------------------------------------*/
void Print(void) {
   struct list_node_s* temp;

   printf("list = ");

   temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
}  /* Print */


/*-----------------------------------------------------------------*/
int  Member(int value) {
   struct list_node_s* temp;

   temp = head;
   while (temp != NULL && temp->data < value)
      temp = temp->next;

   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d is not in the list\n", value);
#     endif
      return 0;
   } else {
#     ifdef DEBUG
      printf("%d is in the list\n", value);
#     endif
      return 1;
   }
}  /* Member */

/*-----------------------------------------------------------------*/
/* Deletes value from list */
/* If value is in list, return 1, else return 0 */
int Delete(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   int rv = 1;

   /* Find value */
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { /* first element in list */
         head = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      } else { 
         pred->next = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      }
   } else { /* Not in list */
      rv = 0;
   }

   return rv;
}  /* Delete */

/*-----------------------------------------------------------------*/
void Free_list(void) {
   struct list_node_s* current;
   struct list_node_s* following;

   if (Is_empty()) return;
   current = head; 
   following = current->next;
   while (following != NULL) {
#     ifdef DEBUG
      printf("Freeing %d\n", current->data);
#     endif
      free(current);
      current = following;
      following = current->next;
   }
#  ifdef DEBUG
   printf("Freeing %d\n", current->data);
#  endif
   free(current);
}  /* Free_list */

/*-----------------------------------------------------------------*/
int  Is_empty(void) {
   if (head == NULL)
      return 1;
   else
      return 0;
}  /* Is_empty */

/*-----------------------------------------------------------------*/
void* Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member_count = 0, my_insert_count=0, my_delete_count=0;
   int ops_per_thread = total_ops/thread_count;

   for (i = 0; i < ops_per_thread; i++) {
      which_op = my_drand(&seed);
      val = my_rand(&seed) % MAX_KEY;
      if (which_op < search_percent) {
         pthread_rwlock_rdlock(&rwlock);
         Member(val);
         pthread_rwlock_unlock(&rwlock);
         my_member_count++;
      } else if (which_op < search_percent + insert_percent) {
         pthread_rwlock_wrlock(&rwlock);
         Insert(val);
         pthread_rwlock_unlock(&rwlock);
         my_insert_count++;
      } else { /* delete */
         pthread_rwlock_wrlock(&rwlock);
         Delete(val);
         pthread_rwlock_unlock(&rwlock);
         my_delete_count++;
      }
   }  /* for */

   pthread_mutex_lock(&count_mutex);
   member_count += my_member_count;
   insert_count += my_insert_count;
   delete_count += my_delete_count;
   pthread_mutex_unlock(&count_mutex);

   return NULL;
}  /* Thread_work */
