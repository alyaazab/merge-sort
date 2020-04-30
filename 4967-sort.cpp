#include <iostream>
#include <bits/stdc++.h>
#include <cstdlib>
#include <pthread.h>

using namespace std;

// struct containing data that is sent to each thread
struct thread_data
{
  int *array;
  int left;
  int right;
};

void *merge_sort(void *thread_arg);
void merge(int *array, int left, int right);
void print_array(int *array, int length);

int main()
{
  int length;
  // open the file for reading
  FILE *file = fopen("input.txt", "r");

  // read in length of array
  fscanf(file, "%d", &length);

  // allocate memory for array
  int *array = (int *)malloc(length * sizeof(int));

  // set elements of matrix A by reading them from file
  for (int i = 0; i < length; i++)
    fscanf(file, "%d", &array[i]);

  fclose(file);

  // allocate memory for a struct containing the first thread's data
  struct thread_data *thread_data = (struct thread_data *)malloc(sizeof(struct thread_data));
  thread_data->array = array;
  thread_data->left = 0;
  thread_data->right = length - 1;
  pthread_t thread;

  // create a thread
  if (pthread_create(&thread, NULL, merge_sort, (void *)(thread_data)))
  {
    printf("Unable to create a thread.");
    exit(-1);
  }

  // when all threads are done executing, print the sorted array
  pthread_join(thread, NULL);
  print_array(array, length);

  free(thread_data);
  free(array);

  return 0;
}

void *merge_sort(void *thread_arg)
{
  // extract thread data from parameter
  struct thread_data *thread_data = (struct thread_data *)thread_arg;
  int left = thread_data->left;
  int right = thread_data->right;
  int *array = thread_data->array;

  if (left < right)
  {
    int middle = left + (right - left) / 2;

    // create two threads (a "left" one and a "right" one)
    pthread_t threads[2];

    // allocate memory for a struct that will be sent to the thread on the "left" side
    struct thread_data *left_thread_data = (struct thread_data *)malloc(sizeof(struct thread_data));
    left_thread_data->left = left;
    left_thread_data->right = middle;
    left_thread_data->array = array;

    // create left thread
    if (pthread_create(&threads[0], NULL, merge_sort, (void *)(left_thread_data)))
    {
      printf("Unable to create a thread.");
      exit(-1);
    }

    // allocate memory for a struct that will be sent to the thread on the "right" side
    struct thread_data *right_thread_data = (struct thread_data *)malloc(sizeof(struct thread_data));
    right_thread_data->left = middle + 1;
    right_thread_data->right = right;
    right_thread_data->array = array;

    // create right thread
    if (pthread_create(&threads[1], NULL, merge_sort, (void *)(right_thread_data)))
    {
      printf("Unable to create a thread.");
      exit(-1);
    }

    // wait for both threads to finish
    void *status;
    for (int i = 0; i < 2; i++)
      pthread_join(threads[i], &status);

    merge(array, left, right);

    free(left_thread_data);
    free(right_thread_data);
  }
}

void merge(int *array, int left, int right)
{
  int middle = (left + right) / 2;
  int lengthA = middle - left + 1;
  int lengthB = right - middle;

  // create two temp arrays
  int tempA[lengthA], tempB[lengthB];

  // fill temp arrays with values from our main array
  for (int i = 0; i < lengthA; i++)
    tempA[i] = array[left + i];

  for (int i = 0; i < lengthA; i++)
    tempB[i] = array[middle + 1 + i];

  int i = 0, j = 0, k = left;

  // compare elements from tempA and tempB and merge them in an ascending manner
  while (i < lengthA && j < lengthB)
  {
    if (tempA[i] <= tempB[j])
    {
      array[k] = tempA[i];
      i++;
    }
    else
    {
      array[k] = tempB[j];
      j++;
    }

    k++;
  }

  // if any elements are left in tempA, add them to the end of our main array
  while (i < lengthA)
  {
    array[k] = tempA[i];
    i++;
    k++;
  }

  // if any elements are left in tempB, add them to the end of our main array
  while (j < lengthB)
  {
    array[k] = tempB[j];
    j++;
    k++;
  }
}

// prints our array :)
void print_array(int *array, int length)
{
  printf("\n");
  for (int i = 0; i < length; i++)
  {
    printf("%d ", array[i]);
  }
  printf("\n");
}