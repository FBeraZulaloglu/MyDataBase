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
#define MSGSIZE 100
#define numberOfCommands 4

void init_program(void);

int main(int argc, char *argv[]) 
{  
   int fd; 
  
    // FIFO file path 
    char * myfifo = "/tmp/myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>, <permission>) 
    mkfifo(myfifo, 0666); 
  
    char answer[MSGSIZE]; 
    while (1) 
    { 
        // Open FIFO for write only 
        fd = open(myfifo, O_WRONLY); 
  
        // Take an input arr2ing from user. 
        // 80 is maximum length 
        char *sql;
        sql = readline("\nSQL>> "); 
        add_history(sql);
        // Write the input arr2ing on FIFO 
        // and close it 
        write(fd, sql, MSGSIZE); 
        close(fd);
  
        // Open FIFO for Read only 
        fd = open(myfifo, O_RDONLY); 
  
        // Read from FIFO 
        read(fd, answer, MSGSIZE); 
  
        // Print the read message 
        if(strcmp(answer,"null")){
            // answer is not equal null
            printf("CEVAP : %s\n",answer);
        }
        else{
            // answer is null
            printf("SQL couldn't has answered!!\n");
        } 
        close(fd); 
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

