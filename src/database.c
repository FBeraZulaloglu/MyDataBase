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
   char  number[50];
};

void init_database(void);
char* getResult(char *);
char * getName(struct Students *students);
char * getNo(struct Students *students);
char * getAll(struct Students *students);
char * executeSQL(char *,char * ,char *,char *); // select / filename/ column/value
struct Students* executeCondition(struct Students *students,char *column,char *value);

int main(int argc, char *argv[]) 
{  
    int fd1; 
  
    // FIFO file path 
    const char * myfifo = "/tmp/myfifo"; 
  
    // Creating the named file(FIFO) 
    // mkfifo(<pathname>,<permission>) 
    mode_t mode = 0666;
    mkfifo(myfifo, mode); 
  
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
            strcpy(message,result);
        }
        else{
            strcpy(message,"null");
        }
        free(result);
        //printf("MESSAGE : %s\n",message);
        write(fd1, message, strlen(message)+1); 
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
        return executeSQL(sqlCommands[1],fileName,NULL,NULL);
    }
    else{
        if(strcmp(sqlCommands[4],"where")) return NULL;
        char *column = strsep(&sqlCommands[5],"=");
        if(!(strcmp(column,"name") == 0 || strcmp(column,"number") == 0)) return NULL;
        char *value = strsep(&sqlCommands[5],"=");
        return executeSQL(sqlCommands[1],fileName,column,value);
    }
}

char * executeSQL(char *select,char *fileName,char *column,char*value){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    struct Students *student_array = (struct Students*) malloc(1000*sizeof(struct Students));

    fp = fopen(fileName, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("line : %d %s",read, line);
        
        strcpy(student_array[i].name,strsep(&line," "));
        strcpy(student_array[i].number,strsep(&line," "));
        i++;
    }
    if (line)
        free(line);
    fclose(fp);

    if(value != NULL){
        if(!strcmp(select,"*")){
            return getAll(executeCondition(student_array,column,value));
        }
        else if(!strcmp(select,"name")){
            return getName(executeCondition(student_array,column,value));
        }
        else if(!strcmp(select,"number")){
            return getNo(executeCondition(student_array,column,value));
        }
        return NULL;
    }
    if(!strcmp(select,"*")){
        return getAll(student_array);
    }
    else if(!strcmp(select,"name")){
        return getName(student_array);
    }
    else if(!strcmp(select,"number")){
        return getNo(student_array);
    }
    return NULL;
}

struct Students *executeCondition(struct Students *students,char *column,char *value){
    struct Students *temp = (struct Students*) malloc(sizeof(struct Students)*100);
    int lineNo = 0;
    int i = 0;
    printf("VALUE %s COLUMN %s\n",value,column);
    
    while(strcmp(students[i].name,"") != 0)
    {
        if(!strcmp(column,"number")){
            //printf(" %s%s\n",students[i].number,value);
            if(atoi(students[i].number)==atoi(value)){
                
                strcpy(temp[lineNo].name,students[i].name);
                strcpy(temp[lineNo].number,students[i].number);
                
                lineNo++;
            }
        }
        else if(!strcmp(column,"name")){
            if(!strcmp(students[i].name,value)){
                strcpy(temp[lineNo].name,students[i].name);
                strcpy(temp[lineNo].number,students[i].number);
                lineNo++;
            }
        }
        //printf("STUDENT %d name : %s number %s\n",i,students[i].name,students[i].number);
        i++;
    }
    //printf("LINES\n%s",lines);
    if(lineNo!=0)
        printf("condition has worked properly\n");
        return temp;
    return NULL;
}

char * getName(struct Students *students){
    char *lines = (char*) malloc(sizeof(char)*100);
    int lineNo = 0;
    int i = 0;
          
    while(1){
        if(!strcmp(students[i].name,"")) break;

        if(lineNo == 0){
            strcpy(lines,students[i].name);
        }
        else{
            strcat(lines,students[i].name);
        }
        //strcat(lines,"\n");
        i++;
        lineNo++;
    }
    //printf("LINES\n%s",lines);
    free(students);
    if(lineNo!=0)
        return lines;
    return NULL;
}

char * getNo(struct Students *students){
    char *lines = (char*) malloc(sizeof(char)*100);
    int lineNo = 0;
    int i = 0;
           
    while(strcmp(students[i].number,"")){
        if(lineNo == 0){
            strcpy(lines,students[i].number);
        }
        else{
            strcat(lines,students[i].number);
        }
        //strcat(lines,"\n");
        i++;
        lineNo++;
    };
    //printf("LINES\n%s",lines);

    free(students);
    if(lineNo!=0)
        return lines;
    return NULL;
}

char * getAll(struct Students *students){
    char *lines = (char*) malloc(sizeof(char)*100);
    int lineNo = 0;
    int i = 0;

    while(strcmp(students[i].name,"")){
        if(lineNo == 0){
            strcpy(lines,students[i].name);
        }
        else{
            strcat(lines,students[i].name);
        }
        strcat(lines," ");
        strcat(lines,students[i].number);
        //strcat(lines,"\n");
        i++;
        lineNo++;
    }
    //printf("LINES\n%s",lines);
    free(students);
    if(lineNo!=0)
        return lines;
    return NULL;
}

