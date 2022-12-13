#include <atomic>
#include <thread>
#include <stdio.h>
#include <unistd.h>
#include <queue>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <linux/limits.h>
       #include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>



#define THREADNUM 8
enum {Ready, Processing};
#define PIPE1 "pipe1"
#define PIPE2 "pipe2"
#define PIPE3 "pipe3"
#define PIPE4 "pipe4"
#define PIPE5 "pipe5"
#define FIFOSNUM 5

typedef struct{
	char bytes[PATH_MAX];
	int size;
} Job;


typedef struct{
	std::queue<Job*> jobs;
	std::atomic<int> count = {0};
	std::atomic<int> status = {Ready};
} Queue;


Queue que[THREADNUM] = {};


void back_off(unsigned int times){
    for (unsigned int k = 0; k < times; ++k)
      __asm__ volatile("nop\n\t");
}



int QueuePush(Queue* qu, Job* job){
	
    qu -> jobs.push(job);        
    qu -> count.fetch_add(1, std::memory_order_relaxed);
    return 0;
	    
}

Job* QueuePopTop(Queue* qu, int stealing){
	int attempts = 0;
	do{
		int expected = Ready;
	    if(qu -> status.compare_exchange_weak(expected, Processing, std::memory_order_release, std::memory_order_relaxed)){
	    	int cur_size = qu -> count.load(std::memory_order_acquire);
	        if(cur_size == 0 || (stealing && cur_size == 1)){
	        	qu -> status.store(Ready, std::memory_order_relaxed);
	        	return NULL;
	        }
	        Job* tmp = qu -> jobs.front();   
	        qu -> jobs.pop();
	        qu -> count.fetch_sub(1, std::memory_order_relaxed);
	        qu -> status.store(Ready, std::memory_order_release);
	        return tmp;
	    }
	    back_off(2);
	    attempts++;
	    if (attempts > 7){
	    	return NULL;
	    }
	} while(1);
}


int checksrecentjobsnum(int threadnum){
	return que[threadnum].count.load(std::memory_order_relaxed);
}


int mostandleast(int* most){ //вернет наименее загруженный процесс, а в most запишет наиболее
	
	int mintr = 0;
	int maxtr = 0;

	int size = checksrecentjobsnum(0);
	int min = size, max = size;

	for (int i = 1; i < THREADNUM; ++i)
	{
		int size = checksrecentjobsnum(i);
		if (size > max){
			max = size;
			maxtr = i;
		}
		else if (size <= min){
			min = size;
			mintr = i;
		}
	}

	if(max <= 1)
		maxtr = mintr;

	if(most != NULL){
		*most = maxtr;
	}

	return mintr;
}



int dojob(Job* job, int fd){
	if (job -> size == 0) {
		return -1;
	}
	write(fd, job -> bytes, job -> size);
	return 0;
}



void routine(int threadnum){
    Job* current;
    int most = threadnum;
    int res = 0; //если нам сообщили что это последняя работа, то завершаем процесс
    int fd = open("result.txt", O_APPEND|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) {
    	perror("не открыли файл результата");
        exit(1);
    }

    while(1){
		while ((current = QueuePopTop(que + threadnum, 0)) != NULL){
			res = dojob(current, fd); 
			printf("поток %d выполнил свою работу\n", threadnum);
			free(current);
		}
		if (res)
			break;

		std::this_thread::yield();
		int least = mostandleast(&most); //последнюю задачу воровать не будем, вдруг она должна завершить поток
		if (least == most){
			back_off(100);
			continue;
		}
		if( (current = QueuePopTop(que + most, 1)) != NULL){
			dojob(current, fd);
			printf("поток %d выполнил работу потока %d \n", threadnum, most);
			free(current);
		}
	}

	close(fd);
	printf("поток %d закончил и вышел\n", threadnum);
	fflush(0);
}


void Closefds(int* fds){

	for (int i = 0; i < FIFOSNUM; ++i){
		if (close(fds[i]) == -1 && errno != EBADF){
			printf("что-то не так при закрытии трубы %d", i);
			exit(1);
		}
	}
	
	free(fds);
}


int Openfds(int** fds, int flags){

	(*fds) = (int*)calloc(sizeof(int), FIFOSNUM);

	(*fds)[0] = open(PIPE1, flags);
	(*fds)[1] = open(PIPE2, flags);
	(*fds)[2] = open(PIPE3, flags);
	(*fds)[3] = open(PIPE4, flags);
	(*fds)[4] = open(PIPE5, flags);
	if ((*fds)[0] == -1 || (*fds)[1] == -1 || (*fds)[2] == -1 || (*fds)[3] == -1 || (*fds)[4] == -1){
	    exit(1);
	}

	return FIFOSNUM;
}

void Make_fifos(){
	umask(0);                          
	if (mkfifo(PIPE1, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        printf("open FIFO %s", PIPE1);
    	exit(1);
	}
	if (mkfifo(PIPE2, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        printf("open FIFO %s", PIPE2);
    	exit(1);
	}
	if (mkfifo(PIPE3, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        printf("open FIFO %s", PIPE3);
    	exit(1);
	}
	if (mkfifo(PIPE4, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        printf("open FIFO %s", PIPE4);
    	exit(1);
	}
	if (mkfifo(PIPE5, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
        printf("open FIFO %s", PIPE5);
    	exit(1);
	}
}



void Masterthread(){
	struct timeval waiting_time = {
        .tv_sec = 2,
        .tv_usec = 0 /*2 sec*/
    };

    fd_set read_fds;
    int readsize;
    int* fds = NULL;
    int size = Openfds(&fds, O_RDONLY);
    for(;;){
        FD_ZERO(&read_fds);
        for (int i = 0; i < size; ++i)
        {
        	FD_SET(fds[i], &read_fds);
        }
         
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, &waiting_time) <= 0){
        	Closefds(fds);
            break;
        }

		for (int i = 0; i < size; ++i)
        {
        	if(FD_ISSET(fds[i], &read_fds)){           		
        		ioctl(fds[i], FIONREAD, &readsize);
        		if (readsize == 0){
        			fds[i] = fds[size-1];
        			size--;
        			continue;
           		}

           		Job* job = (Job*)calloc(sizeof(Job), 1);
        		if ((job -> size = read(fds[i], job -> bytes, PATH_MAX)) < 0){
        			free(job);
        			printf("Job size < 0, fdsize = %d\n", size);
        			exit(1);
        		}        		
        		QueuePush(que + mostandleast(NULL), job);
            }
        }

        waiting_time.tv_sec = 2;
    } 

    for (int i = 0; i < THREADNUM; ++i)
    {
    	Job* job = (Job*)calloc(sizeof(Job), 1);
    	QueuePush(que + i, job);
    }

    printf("Masterthread закончил и вышел\n");
    fflush(0);
}

void netemulator(){
	int* fds = NULL;
    int size = Openfds(&fds, O_WRONLY);
    FILE* fin = fopen("text.txt", "rb");
    int readsize = 0;
    int counter = 0;
    char buf[PATH_MAX] = "";
    while ((readsize = fread(buf, sizeof(char), PATH_MAX, fin)) > 0){
    	write(fds[counter], buf, PATH_MAX);
    	counter = (counter+1) % size;
    }
    fclose(fin);
    Closefds(fds);
    printf("netemulator закончил и вышел\n");
}


int main(int argc, char * argv[]) {

	std::thread* threads[THREADNUM+1] = {};   

	Make_fifos();

	pid_t pid = fork();
	if (pid < 0) 
       exit(1);

	if (pid == 0){
		netemulator();
		printf("Нетэмулятор выходит\n");
		exit(0);
	}

	threads[THREADNUM] = new std::thread(Masterthread);

    for (int i = 0; i < THREADNUM; i++) 
      threads[i] = new std::thread(routine, i);

    for (int i = 0; i < THREADNUM + 1; i++) {
        threads[i] -> join();
        delete threads[i];
    }
    printf("все соединились\n");
	return 0;
}