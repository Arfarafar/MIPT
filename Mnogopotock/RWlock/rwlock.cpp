#include <atomic>
#include <thread>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/types.h>
#include <climits>
#include <unistd.h>

#define TREADNUM 10

unsigned int readers_active = 0;
unsigned int waiting_writers = 0;
unsigned int active_writer = 0;
std::atomic<unsigned int> Lock = {0};


unsigned int cmpxchg(std::atomic<unsigned int>* atom, unsigned int expected, unsigned int desired) {
  	atom -> compare_exchange_weak(expected, desired, std::memory_order_acquire);
  	return expected;
}


void lock(){
	int lock = cmpxchg(&Lock, 0, 1);
    if (lock != 0) {
      do {
        if (lock == 2 || cmpxchg(&Lock, 1, 2) != 0) 
          syscall(SYS_futex, &Lock, FUTEX_WAIT_PRIVATE, 2, 0, 0, 0);

      } while ((lock = cmpxchg(&Lock, 0, 2)) != 0);
    }
}

void unlock(){
	if(Lock.fetch_sub(1, std::memory_order_release) == 2){
		Lock.store(0, std::memory_order_release);
		syscall(SYS_futex, &Lock, FUTEX_WAKE_PRIVATE, 1, 0, 0, 0);
	}
}



void begin_read(){
	lock();
	
	int wait = 1;
	while(waiting_writers > 0 || active_writer){
		wait = waiting_writers > 0 ? waiting_writers : 1;
		unlock();	
		syscall(SYS_futex, &waiting_writers, FUTEX_WAIT_PRIVATE, wait, NULL, NULL, 0);
		lock();
	}

	readers_active++;
	unlock();
}

void end_read(){
	lock();
	readers_active--;
	if (readers_active == 0){
		syscall(SYS_futex, &readers_active, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, NULL, 0);
	}
	unlock();
}

void begin_write(){
	lock();
	waiting_writers++;
	int read = 1;
	while (readers_active > 0 || active_writer){
		read = readers_active > 0 ? readers_active : 1;
		unlock();
		syscall(SYS_futex, &active_writer, FUTEX_WAIT_PRIVATE, 1, NULL, NULL, 0);	
		syscall(SYS_futex, &readers_active, FUTEX_WAIT_PRIVATE, read, NULL, NULL, 0);	 
		lock();
	}

	active_writer = 1;
	waiting_writers--;
	
	unlock();
}

void end_write(){
	lock();
	active_writer = 0;
	if (waiting_writers == 0)
		syscall(SYS_futex, &waiting_writers, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, NULL, 0);
	syscall(SYS_futex, &active_writer, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
	unlock();
}



int shmem[4] = {};



void readers(int threadnum){

	for (int i = 0; i < 20; i++) {
		begin_read();
			int value = shmem[0];
			if (value != shmem[1] || value != shmem[2] || value != shmem[3]){
				printf("Wrong answer\n");
				exit(1);
			}
			printf("tread %d read %d active_writer %d, readers_active %d waiting_writers %d\n",threadnum, *shmem, active_writer, readers_active, waiting_writers);
			fflush(0);
		end_read();
	}
}

void writers(int threadnum){

	for (int i = 0; i < 20; i++) {

		begin_write();
			shmem[0]++;
			shmem[1]++;
			shmem[2]++;
			shmem[3]++;
			printf("tread %d write %d active_writer %d, readers_active %d waiting_writers %d\n", threadnum, *shmem, active_writer, readers_active, waiting_writers);
			fflush(0);
		end_write();
	}

}

int main(int argc, char * argv[]) {

	std::thread* threads[2*TREADNUM] = {};
	
	
	
    for (int i = 0; i < TREADNUM; i++) {
    	threads[2*i+1] = new std::thread(readers, 2*i+1);
    	threads[2*i] = new std::thread(writers, 2*i);
    }

    for (int i = 0; i < TREADNUM*2; i++) {
        threads[i] -> join();
        delete threads[i];
    }
	
    printf("%d %d %d %d \n", shmem[0], shmem[1], shmem[2], shmem[3]);

	return 0;
}
 
