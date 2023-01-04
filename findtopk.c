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

    int *arr = malloc(k * sizeof(int)); // bellekte k elemanlı dizi oluşturur.
    int temp;
    for (int i = 0; i < k; i++)
    {
        arr[i] = 0; // bellekte oluşturulan dizinin elemanlarınnı 0 yaptık.
    }

    while (fscanf(fp, "%d", &temp) != EOF) // end of file.
    {                                      // dosyadan okuma yapılır.dosyanın sonuna gelene kadar çalış.
        if (temp > arr[0])                 // okunan temp degeri dizinin en büyük elemanından büyükse
        {
            arr[0] = temp; // dizinin en büyük elemanı temp olur.

            for (int i = 0; i < k - 1; i++) // ardından dizi sıralarız.
            {
                if (arr[i] < arr[i + 1])
                {
                    break;
                }
                swap(arr, i, i + 1); // swap işlemiyle büyük olan eleman dizinin en başına gelir.
            }
        }
    }
    temp = arr[0]; // en kücük k sayısını döndürür
    free(arr);     // dizi silinir rami boşaltır.
    return temp;
}

int main(int argc, char *argv[])
{

    clock_t start_t, end_t; // cpu üzerinde kodun çalışma sürelerini hesaplamak için kullanılır.
    start_t = clock();

    int k = atoi(argv[1]);         // terminal üzerinden girilen k sayısını alır.
    int n = atoi(argv[2]);         // terminal üzerinden girilen dosya sayısını alır.
    char **files = argv + 3;       // terminal üzerinden girilen dosya isimlerini alır.
    char *output = argv[argc - 1]; // terminal üzerinden girilen output dosyasını alır. Bu output dosyasına en büyük k sayıları yazılır.

    sem_open(&sem, 0, 1); // oluşturulan child processlerin birbirlerini beklemesi için semaphore kullanılır.

    pid_t pid[n]; // child processlerin process idlerini  tutmak için
    for (int i = 0; i < n; i++)
    { // her elemanı forkluyoruz.

        pid[i] = fork(); // ana processin child processler oluşturması için fork kullanılır.
        if (pid[i] == 0) // eğer 0 ise child process'dir.
        {
            char *filename = files[i];       // terminalden girilen dosya isimlerini alır.
            FILE *fp = fopen(filename, "r"); // girilen dosyaları okuruz.
            if (fp == NULL)
            {
                printf("Error opening file\n");
                exit(1); // bir gelirse hata çıkışı
            }

            // ardından text dosyalarından alınan degerler fintopK fonksiyonuna yollarız.
            // ana sebebi ise en büyük k. sayısını bulmak için.
            int result = findTopK(fp, k); // en büyük k sayısını bulur.

            // result degeri , child processlerin bulduğu en büyük k sayısını tutar.
            fclose(fp);

            sem_wait(&sem); // beklemesi gerekirse bekletiyor.
            // semafor neden kullandık ? çünkü havalı.
            // child processlerin birbini beklemesi için semaphore kullanılırız böylelikle
            // segmentation fault hatasını almayı önleriz aynı zamanda raceContition hatasına benzer.

            fp = fopen("intermediate.txt", "a"); // bu dosyaya en büyük k. sayıları yazılır.
                                                 // bu bizim istediğimiz cevaptır.

            fprintf(fp, "%d\n", result); // dosyaya yazma işlemi.
            fclose(fp);
            sem_post(&sem); // sırada child process varsa bekleyen child processi çalıştırır.
            exit(0);
        }
    }

    // ana processin beklemesi için , child processlerin bitmesini bekliyoruz.
    for (int i = 0; i < n; i++)
    {
        waitpid(pid[i], NULL, 0); // waitpid , child processlerin bitmesini bekliyor.
    }

    FILE *fp = fopen("intermediate.txt", "r"); // dosyayı okuma modunda açıyoruz.
    // üstte append modunda açtığımız dosyaya child processlerin bulduğu en büyük k sayılarını yazdık.

    int *arr = malloc(n * sizeof(int)); // n elemanlı dizi oluşturuyoruz. n dosyası  sayısı.
    int temp;                           // dosyadan okunan sayıyı tutmak için.
    // bütün child processlerin sonucunu okuyoruz. ana processin içinde. child processlerin içinde değil.

    for (int i = 0; i < n; i++)
    {
        fscanf(fp, "%d", &temp); // dosyadan okuma işlemi.
        arr[i] = temp;
    }
    fclose(fp);
    remove("intermediate.txt"); // dosyayı silme işlemi. rami boşaltma işlemi.
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
    end_t = clock();

    // double elapsed_time = difftime(end_time, start_time);
    printf("\nstart_t değeri: %ld\n", start_t);
    printf("end_t değeri: %ld\n", end_t);
    printf("CPU süreci (saniye): %f\n", (double)(end_t - start_t) / CLOCKS_PER_SEC);

    return 0;
}