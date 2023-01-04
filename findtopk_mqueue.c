#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <mqueue.h>


sem_t sem;

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


//findtopk with using posix message queue
int main(int argc, char *argv[]) {

    clock_t start_t, end_t;
    start_t = clock();
    




    int n;
    printf("Enter a number: ");
    scanf("%d", &n);
    mqd_t mq;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 33;
    attr.mq_curmsgs = 0;
    mq = mq_open("/myqueue", O_CREAT | O_RDWR, 0666, &attr);
    if (mq == -1) {
        printf("Error opening message queue\n");
        exit(1);
    }
    pid_t pid[n];
    //create n threads and open text[n] files
    for (int i = 0; i < n; i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            char filename[]="filex.txt";
            filename[4] = i + 49;
            FILE *fp = fopen(filename, "r");
            if (fp == NULL) {
                printf("Error opening file\n");
                exit(1);
            }
            int result = findTopK(fp, 3);
            fclose(fp);
            char *msg = malloc(sizeof(char)*20);
            sprintf(msg, "%d", result);
            mq_send(mq, msg, sizeof(msg)+1, 0);
            printf("process %d has finished\n", getpid());
            exit(0);
        }
    }
    //wait for all child processes to finish, receive the numbers from message queue and sort them
    for (int i = 0; i < n; i++) {
        waitpid(pid[i], NULL, 0);
    }
    printf("all processes have finished\n");
    char msg[50];
    int *arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        mq_receive(mq, msg, sizeof(msg), 0);
        perror("mq_receive");
        printf("received :%s\n", msg);
        arr[i] = atoi(msg);
    }
    mq_close(mq);
    mq_unlink("/myqueue");
    //bubble sort
    bubble_sort(arr, n);
    //create result.txt and write the sorted numbers
    FILE *fp = fopen("result.txt", "w");
    for (int i = 0; i < n; i++) {
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
