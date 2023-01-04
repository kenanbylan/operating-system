#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>


sem_t sem;
int *arr;
int cursor = 0;


//thread fonksiyonu burası.
struct thread_args {
    int k;
    char *filename;
};

void swap(int *arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

void bubble_sort(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr, j, j + 1);
            }
        }
    }
}

int findTopK(FILE *fp, int k){

    int *arr = malloc(k * sizeof(int));
    int temp;
    for (int i = 0; i < k; i++) {
        arr[i] = 0;
    }

    while (fscanf(fp, "%d", &temp) != EOF){
        if (temp > arr[0]){
            arr[0] = temp;
            for (int i = 0; i < k-1; i++){
                if (arr[i] < arr[i+1]){
                    break;
                }
                swap(arr, i, i+1);
            }
        }
    }
    temp = arr[0];
    free(arr);
    return temp;
}

//create thrad func it parameter is file name and k
void *thread_func(void *arg) { //sema
    struct thread_args *args = (struct thread_args *)arg;
    FILE *fp = fopen(args->filename, "r");
    int temp = findTopK(fp, args->k); //k sayısını bulur.
    sem_wait(&sem);
    arr[cursor] = temp;
    cursor++;
    sem_post(&sem); //indirmeyip , karışmasın diye.
    fclose(fp);
    return NULL;

}

int main(int argc, char *argv[]) {

    //time_t start_time = time(NULL);  //çalışma zamanı başlangıcı.
    
    clock_t start_t, end_t;
    start_t = clock();

    
    int k = atoi(argv[1]);
    int n = atoi(argv[2]);
    char **files = argv+3;
    char *output = argv[argc - 1];
    sem_open(&sem, 0, 1);
    arr = malloc(k * sizeof(int));
    for (int i = 0; i < k; i++) {
        arr[i] = 0;
    }
    pthread_t tid[n];  //thread id
    //create n threads and open text[n] files
    for (int i = 0; i < n; i++) {
        struct thread_args *args = malloc(sizeof(struct thread_args)); //thread argumanları için bellek ayırıyoruz.
        args->k = k; //k sayısını alır
        args->filename = files[i]; //dosya ismini alır
        pthread_create(&tid[i], NULL, thread_func, args); //thread oluşturur.
    }
    //wait for all threads to finish
    for (int i = 0; i < n; i++) {
        pthread_join(tid[i], NULL); //thread bitene kadar bekler.
    }


    bubble_sort(arr, n); //output dosyaları sıralanmış olması lazım.
    //write the result to output file
    FILE *fp = fopen(output, "w");
    for (int i = n-1; i >= 0; i--) {
        fprintf(fp, "%d\n", arr[i]);
    }
    fclose(fp);
    free(arr);

    end_t = clock();

    //double elapsed_time = difftime(end_time, start_time);
    printf("\nstart_t değeri: %ld\n", start_t);
    printf("end_t değeri: %ld\n", end_t);
    printf("CPU süreci (saniye): %f\n", (double)(end_t - start_t) / CLOCKS_PER_SEC);

    return 0;
}
