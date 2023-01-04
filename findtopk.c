#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

sem_t sem; // semaphore

void swap(int *arr, int i, int j)
{ // büyükse yer değiştiriyoruz.
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

void bubble_sort(int *arr, int n) // child processlerin degerleri bulunduktan sonra parent process,
// degerleri okuyarak result.txt dosyasına yazmadan önce sıralayarak yazması sağlanması için.
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr, j, j + 1);
            }
        }
    }
}

int findTopK(FILE *fp, int k)
{ // en büyük k sayısını bulur
    int *arr = malloc(k * sizeof(int));
    int temp;
    for (int i = 0; i < k; i++)
    {
        arr[i] = 0;
    }

    while (fscanf(fp, "%d", &temp) != EOF)
    { // dosyadan okuma yapılır.dosyanın sonuna gelene kadar çalış.
        if (temp > arr[0])
        { // gelen dosya en büyük k sayısından büyükse
            arr[0] = temp;
            for (int i = 0; i < k - 1; i++)
            {
                if (arr[i] < arr[i + 1])
                {
                    break;
                }
                swap(arr, i, i + 1); // en küçük sayıyı bulur.
            }
        }
    }
    temp = arr[0]; // en kücük k sayısını döndürür
    free(arr);     // dizi silinir rami boşaltır.
    return temp;
}

int main(int argc, char *argv[])
{

    // time_t start_time = time(NULL);  //çalışma zamanı başlangıcı.

    clock_t start_t, end_t;

    start_t = clock();

    int k = atoi(argv[1]);
    int n = atoi(argv[2]);
    char **files = argv + 3;
    char *output = argv[argc - 1];
    sem_open(&sem, 0, 1); // indirmeyip , karışmasın diye.

    pid_t pid[n]; // n elemanlı process arrayi .
    for (int i = 0; i < n; i++)
    { // her elemanı forkluyoruz.

        pid[i] = fork();
        if (pid[i] == 0)
        {
            char *filename = files[i];
            FILE *fp = fopen(filename, "r");
            if (fp == NULL)
            {
                printf("Error opening file\n");
                exit(1); // bir gelirse hata çıkışı
            }
            int result = findTopK(fp, k); // en büyük k sayısını bulur.
            fclose(fp);
            sem_wait(&sem);                      // beklemesi gerekirse bekletiyor.
            fp = fopen("intermediate.txt", "a"); // append modunda ekleme yapılıyor.
            fprintf(fp, "%d\n", result);         // dosyaya yazma işlemi.
            fclose(fp);
            sem_post(&sem); // post , eğer sırada bekleyen varsa onu çalıştırıyor. yoksa bekliyor.
            exit(0);
        }
    }
    // wait for all child processes to finish, open intermediate.txt, sort the numbers and delete intermediate.txt

    // ana processin beklemesi için , child processlerin bitmesini bekliyoruz.
    for (int i = 0; i < n; i++)
    {
        waitpid(pid[i], NULL, 0); // waitpid , child processlerin bitmesini bekliyor.
    }

    FILE *fp = fopen("intermediate.txt", "r"); // dosyayı okuma modunda açıyoruz.
    int *arr = malloc(n * sizeof(int));        // n elemanlı dizi oluşturuyoruz. n dosyas sayısı.
    int temp;                                  // dosyadan okunan sayıyı tutmak için.
    // bütün child processlerin sonucunu okuyoruz. ana processin içinde. child processlerin içinde değil.
    for (int i = 0; i < n; i++)
    {
        fscanf(fp, "%d", &temp); // dosyadan okuma işlemi.
        arr[i] = temp;
    }
    fclose(fp);
    remove("intermediate.txt"); // dosyayı silme işlemi.
    // bubble sort
    bubble_sort(arr, n); // diziyi sıralama işlemi.
    // write to output file
    fp = fopen(output, "w");
    for (int i = n - 1; i >= 0; i--)
    {
        fprintf(fp, "%d\n", arr[i]);
    }

    fclose(fp);
    free(arr);

    // çalışma zamanı bitişi.
    // time_t end_time = time(NULL);

    end_t = clock();

    // double elapsed_time = difftime(end_time, start_time);
    printf("\nstart_t değeri: %ld\n", start_t);
    printf("end_t değeri: %ld\n", end_t);
    printf("CPU süreci (saniye): %f\n", (double)(end_t - start_t) / CLOCKS_PER_SEC);

    return 0;
}