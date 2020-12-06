#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include <fcntl.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define clear() printf("\033[H\033[J") 
#define MAXPARAM 100
#define MAXSQL 10
#define MAXRESULT 1000

struct Students {
   char  name[50];
   char  number[5];
};


void init_database(void);
char* getResult(char *);
char * getName(char * );
char * getNo(char * );
char * getAll(char *);
char * executeSQL(char *,char *,char*);
char *executeCondition(char* ,char *);

int main(int argc, char *argv[]) 
{  
    int fd1; 
  
    // FIFO file path 
    char * myfifo = "/tmp/myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>,<permission>) 
    mkfifo(myfifo, 0666); 
  
    char sql[MAXPARAM], message[MAXPARAM]; 
    while (1) 
    { 
        // First open in read only and read 
        fd1 = open(myfifo,O_RDONLY); 
        read(fd1, sql, MAXPARAM); 
  
        // Print the read string and close 
        printf("SQL KOMUTU: %s\n", sql); 
        close(fd1); 

        // Now open in write mode and write 
        // string taken from user. 
        fd1 = open(myfifo,O_WRONLY);
        fflush(stdin);
        char *result = getResult(sql);
        if(result != NULL){
            printf("----- SQL RESULT ------\n");
            printf("%s\n",result);
            free(result);
            strcpy(message,"ok");
        }
        else{
            strcpy(message,"null");
        }
        //printf("MESSAGE : %s\n",message);
        write(fd1, message, MAXPARAM); 
        close(fd1); 
    } 
    return 0; 
} 

void init_database(){
    clear(); 
    printf("\n\n\n\n******************"
        "************************"); 
    printf("\n\n\t-DATABASE SYSTEM HAS STARTED-"); 
    printf("\n\n\n\n*******************"
        "***********************"); 
    char* username = getenv("USER"); 
    printf("\n\n\n USER is: @%s", username); 
    printf("\n"); 
    sleep(1); 
    clear();
}

char* getResult(char *sql){
    //printf("SQL in IS SQL : %s",sql);
    if(strlen(sql)==0) return NULL;

    char **sqlCommands = (char **)malloc(MAXSQL*sizeof(char *));

    for (int i = 0; i < MAXSQL; i++)
    {
        sqlCommands[i] = (char *)malloc(MAXSQL* sizeof(char));
    }
    
    int i = 0;
    sqlCommands[i] = strsep(&sql," ");
    while(sqlCommands[i] != NULL)
    {
        i++;
        sqlCommands[i] = strsep(&sql," ");
    }

    if(strcmp(sqlCommands[0],"select")||strcmp(sqlCommands[2],"from")) return NULL;
    
    //directorydeki filelları bulmak için bknz:  https://keramida.wordpress.com/2009/07/05/fts3-or-avoiding-to-reinvent-the-wheel/
    
    // get the result from txt file
    char fileName[MAXRESULT];
    //printf("FILE NAME : %s",sqlCommands[3]);
    strcpy(fileName,sqlCommands[3]);
    // * or column name : sqlCommands[1]
    // veri1.txt or veri2.txt : sqlCommand[3]
    // where == sqlCommand[4]
    // seperate by "=" = sqlCommand[5]
    
    if(sqlCommands[4] == NULL){
        return executeSQL(fileName,sqlCommands[1],NULL);
    }
    else{
        if(strcmp(sqlCommands[4],"where")) return NULL;
        return executeSQL(fileName,sqlCommands[1],sqlCommands[5]);
    }
}

char * executeSQL(char *fileName,char *column,char*condition){
    // durum bir where yok :
    // * queryi geri döndür
    // ad ise sadece ad arrayini döndür (arraylere böl)
    // numara ise sadece numara arrayini dödnür
    //printf("condition %s",condition);

    // if there is no where go here
    if(condition == NULL){
        if(!strcmp(column,"*")){
            //printf("get ALL has worked : %s",getAll(fileName));
            printf("get all\n");
            return getAll(fileName);
        }
        else if(!strcmp(column,"ad")){
            return getName(fileName);
        }
        else if(!strcmp(column,"no")){
            return getNo(fileName);
        }
        else{
            return NULL;
        }
    }else{
        char *response[2]; // returns the condition response
        response[0] = strsep(&condition,"=");
        response[1] = strsep(&condition,"=");
        //printf("response 0 : %s response 1 %s \n",response[0],response[1]);
        if(!strcmp(column,"*")){
            //printf("get ALL has worked : %s",getAll(fileName));
            printf("get all\n");
            return getAll(fileName);
        }
        else if(!strcmp(column,"ad")){
            return getName(fileName);
        }
        else if(!strcmp(column,"no")){
            return getNo(fileName);
        }
        else{
            return NULL;
        }
        if(!strcmp(response[0],"ad")){
            return(executeCondition(getName(fileName),response[1]));
        }
        else if(!strcmp(response[0],"no")){
            return(executeCondition(getNo(fileName),response[1]));
        }
        else{
            return NULL;
        }
    }
}

char * getName(char * fileName){
    char* nameQuery = (char*) malloc(1000*sizeof(char));
    FILE *fp;
    fp = fopen(fileName,"r");
    if(fp == NULL){
        return NULL;
    }
    int c;
    int count = 0;
    int space = 0;
    while(!feof(fp)) {
        c = fgetc(fp);
        // loop until you see the space
        while(c != ' '){
            *(nameQuery+count) = c;
            count++;
            c = fgetc(fp);
        }
        c = fgetc(fp);
        while(c != '\n'){
            if(feof(fp)){
                fclose(fp);
                return nameQuery;
            }
            c = fgetc(fp);
        }
        // add /n to adQuery
        *(nameQuery+count) = '\n';
        count++;
        }

    fclose(fp);
    return NULL;
}

char * getNo(char * fileName){
    char* noQuery = (char*) malloc(1000*sizeof(char));
    FILE *fp;
    fp = fopen(fileName,"r");
    if(fp == NULL){
        return NULL;
    }
    int c;
    int count = 0;
    int space = 0;
    while(!feof(fp)) {
        c = fgetc(fp);
        // loop until you see the space
        while(c != ' '){
            c = fgetc(fp);
        }
        c = fgetc(fp);
        while(c != '\n'){
            *(noQuery+count) = c;
            count++;
            c = fgetc(fp);

            if(feof(fp)){
                fclose(fp);
                return noQuery;
            }
        }
        // add /n to adQuery
        *(noQuery+count) = '\n';
        count++;
        }

    fclose(fp);
    return NULL;
}

char * getAll(char * fileName){
    FILE *fp;
    char* query = (char*) malloc(MAXRESULT*sizeof(char));
    fp = fopen(fileName,"r");
    if(fp == NULL){
        return NULL;
    }
    int c;
    int count = 0;
    c = fgetc(fp);
    while(!feof(fp)) {
        *(query+count) = c;
        c = fgetc(fp);
        count++;
    }
    fclose(fp);
    return query;
}

char *executeCondition(char* data,char *condition){
    char *matched = (char *)malloc(MAXSQL*sizeof(char));// all the values that matches and will return to upper function

    char **temp = (char **)malloc(MAXSQL*sizeof(char *)); //  to store the row values

    for (int i = 0; i < MAXSQL; i++)
    {
        temp[i] = (char *)malloc(MAXSQL* sizeof(char));
    }
    int i = 0;
    int matchedCount = 0;
    temp[i] = strsep(&data,"\n");
    while(temp[i] != NULL)
    {
        if(!strcmp(temp[i],condition)){
            //printf("----MATCHED----\n");
            //printf("%s has added.\n",temp[i]);
            if(matchedCount == 0){
                strcpy(matched,temp[i]);
            }
            else{
                strcat(matched,"\n");
                strcat(matched,temp[i]);
            }
            matchedCount++;
            
        }
        i++;
        temp[i] = strsep(&data,"\n");
    }
    if(matchedCount != 0){
        return matched;
    }
    return NULL;
}