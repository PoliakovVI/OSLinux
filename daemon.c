#include <stdio.h> 
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <resolv.h>
#include <errno.h>
#include <syslog.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>

	int flag = 0, flagDeath = 0;
	sem_t semaphore;

	void handler(){
		flag = 1;
	
	}

	void handler2(){
		flagDeath = 1;
	
	}


        int Daemon(char* str, char* argv[]) {
		int fd, count, child_counter;
		int child_status;
		char buf[50] = "";
		char *ch;
		pid_t parpid, parpid2;
		signal(SIGALRM, handler);
		signal(SIGTERM, handler2);
		
		
		/*fd = open(str, O_CREAT|O_RDWR, S_IRWXU);
		count = read(fd, buf, sizeof(buf));*/

		//buf[count - 1] = buf[count];
		//write(1, buf, sizeof(buf));
		//printf("!!!!\n");

		char buffer[20] = "log.txt";		

		while(1){

			pause();

			if(flag == 1){	
		
				FILE *fp;
				if ((fp = fopen(str, "r")) == NULL){
		  		
		    			printf("Не удалось открыть файл");
		    			getchar();
		    			return 0;
		  		}
				
				sem_init(&semaphore, 0, 1);
				int fd2 = open(buffer, O_CREAT|O_RDWR, S_IRWXU);
				lseek(fd2, 0, SEEK_END);
				close(1);
				dup2(fd2, 1); 
				child_counter = 0;

				while(fgets(buf,49,fp)){
					ch = strchr(buf, '\n');
					*ch = '\0';
					child_counter++;

				if(parpid=fork()==0){

					sem_wait(&semaphore);
					
					if(parpid2=fork()==0){
						execve(buf, argv + 1, NULL);
					}
					wait(&child_status);
					if(WIFEXITED(child_status)){
						printf("\n\nthe process exited normally, with exit code %d\n\n", WEXITSTATUS(child_status));
					}
					else{
						printf("\n\nthe process exited abnormally\n\n");
					}
					
					sem_post(&semaphore);
				
				exit(0);
				}
				
			
				}
			while(child_counter > 0){
				wait(&child_status);
				child_counter--;
			}
				
			close(fd2);
			fclose(fp);
			flag = 0;
			}

		if(flagDeath == 1){
			
 			sem_destroy(&semaphore);

			close(1);
			exit(0);
		}

		}
		
		
	}

        int main(int argc, char* argv[]) //--это наша главная процедура с которой начинается программа
        {
            
          pid_t parpid;
          if((parpid=fork())<0) //--здесь мы пытаемся создать дочерний процесс главного процесса (масло масляное в прямом смысле) 
            {                   //--точную копию исполняемой программы
             printf("\ncan't fork"); //--если нам по какойлибо причине это сделать не удается выходим с ошибкой.
             exit(1);                //--здесь, кто не совсем понял нужно обратится к man fork
            }
          else if (parpid!=0) //--если дочерний процесс уже существует
          exit(0);            //--генерируем немедленный выход из программы(зачем нам еще одна копия программы)
          setsid();           //--перевод нашего дочернего процесса в новую сесию
          Daemon(argv[1], argv);           //--ну а это вызов нашего демона с нужным нам кодом (код будет приведен далее)
          return 0;
        }





