#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<fcntl.h>
#include<sys/stat.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define clear() printf("\033[H\033[J") 
#define MSGSIZE 1000

void init_program(void);

int main(int argc, char *argv[]) 
{  
   int fd; 
  
    // FIFO file path 
    const char * myfifo = "/tmp/myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mode_t mode = 0666;
    mkfifo(myfifo, mode); 
  
    char answer[MSGSIZE]; 
    while (1) 
    { 
        // WRITE SQL TO DATABASE PROGRAM
        fd = open(myfifo, O_WRONLY);  
        char *sql;
        sql = readline("\nSQL>> "); 
        add_history(sql);
        write(fd, sql, strlen(sql)+1); 
        close(fd);
  
        // READ THE MESSAGE THAT RETURNS FROM DATABASE
        fd = open(myfifo, O_RDONLY); 
        read(fd, answer, sizeof(answer)); 
        close(fd);
        // Print the read message 
        if(!strcmp(answer,"null")){
            // answer is  equal null
            printf("SQL couldn't has answered!!\n");
        }
        else{
            // ASK QUESTION TO WRITE THE ANSWER A FILE
            char save;
            printf("Will you save the result (Y/N)");
            if(scanf("%c",&save)){
                    if(save == 'Y' || save == 'y'){
                        int filePipe[2];
                        
                        if(pipe(filePipe) == -1){
                            perror("PIPE HAS FAILED ! \n");
                            break;
                        }
                        int i;
                        pid_t p = fork();
                        if(p < 0){
                            fprintf(stderr,"FORK FAILED");
                            break;
                        }
                        if(p > 0){
                            close(filePipe[0]); // reading has closed to write
                            write(filePipe[1],answer,strlen(answer)+1);
                            //printf("ANSWER WILL BE WRITTEN\n%s",answer);
                            close(filePipe[1]);
                            wait(&i);
                            int result = WEXITSTATUS(i);
                            if(result==0){
                                printf("The result has saved to txt file.\n");
                            }
                            else if(result == 1){
                                printf("The result couldn't saved to txt file.Please try again.\n");
                            }
                        }
                        else{
                            close(filePipe[1]); // close writing to read
                            char finalResult[MSGSIZE];
                            read(filePipe[0],finalResult,strlen(answer)+1);
                            char **newargv = (char **)malloc(2*sizeof(char *));

                            for (int i = 0; i < 2; i++)
                            {
                                newargv[i] = (char *)malloc(2* sizeof(char));
                            }
                            //printf("RESULT : %s\n",finalResult);
                            strcpy(newargv[0],finalResult);
                            
                            i = execv("./kaydet",newargv);
                            // unnamed pipe çalıştır.
                        }
                    }
            }
        }
    } 
    return 0;
} 

void init_program(){
    clear(); 
    printf("\n\n\n\n******************"
        "************************"); 
    printf("\n\n\t-PROGRAM HAS STARTED-"); 
    printf("\n\n\n\n*******************"
        "***********************"); 
    char* username = getenv("USER"); 
    printf("\n\n\n USER is: @%s", username); 
    printf("\n"); 
    sleep(1); 
    clear();
}

