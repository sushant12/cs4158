#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 1000000 // Define a large array size for better performance visibility

// This function uses a simple loop to iterate through the array and sum the elements one by one
void sum_regular(int arr[], int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    printf("Regular Loop: Sum = %d\n", sum);
}

// This function uses a loop unrolling technique to sum the array
// It processes 4 elements in each iteration, reducing the loop overhead
void sum_unrolled(int arr[], int size) {
    int sum = 0;
    int i = 0;
    
    // Process 4 elements at a time
    for (i = 0; i <= size - 4; i += 4) {
        sum += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
    }
    
    // Handle any remaining elements
    for (; i < size; i++) {
        sum += arr[i];
    }
    
    printf("unrolled loop: Sum = %d\n", sum);
}

int main() {
    int *arr = (int*)malloc(SIZE * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    // Fill the array with 1's
    for (int i = 0; i < SIZE; i++) {
        arr[i] = 1;
    }
    
    // Measure time for regular loop
    clock_t start, end;
    double cpu_time_used;
    
    start = clock();
    sum_regular(arr, SIZE);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken by regular loop: %.10f\n", cpu_time_used);
    
    // Measure time for unrolled loop
    start = clock();
    sum_unrolled(arr, SIZE);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("time taken by loop unrolling: %.10f\n", cpu_time_used);
    
    free(arr);
    return 0;
}
