#3 farklı dosyanın build almak için gereken komutlar makefile olarak yazılmıştır.
#Build aldıktan sonra ise uygulamaları : ./findtopk  1 1 text0.txt result.txt diyerek çalıştırabilirsiniz.
all: findtopk.c findtopk_thread.c findtopk_mqueue.c
	gcc -o findtopk findtopk.c
	gcc -o findtopk_thread findtopk_thread.c
	gcc -o findtopk_mqueue findtopk_mqueue.c

clean:  rm -f findtopk rm -f findtopk_thread rm -f findtopk_mqueue

