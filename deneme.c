#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<fcntl.h>
#include<sys/stat.h> 
#include<sys/types.h> 
#include<sys/wait.h> 

char* getResult(char *);
char * getName(char * );
char * getNo(char * );
char * getAll(char *);
char * executeSQL(char *,char *,char*);
char *executeCondition(char* ,char *);
char *getLast();

int main(){
    char *name = getLast();
    printf("\n%s\r\n", name);
    free(name); 
}

char * getAd(char * fileName){
    char* adQuery = (char*) malloc(1000*sizeof(char));
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
            *(adQuery+count) = c;
            count++;
            c = fgetc(fp);
        }
        c = fgetc(fp);
        while(c != '\n'){
            if(feof(fp)){
                fclose(fp);
                return adQuery;
            }
            c = fgetc(fp);
        }
        // add /n to adQuery
        *(adQuery+count) = '\n';
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

char *executeCondition(char* data,char *condition){
    
    char *matched = (char *)malloc(1000*sizeof(char)); // all the values that mathces and will return to upper function
    char **temp = (char **)malloc(1000*sizeof(char *)); //  to store the row values

    for (int i = 0; i < 1000; i++)
    {
        temp[i] = (char *)malloc(1000* sizeof(char));
    }
    int i = 0;
    int matchedCount = 0;
    temp[i] = strsep(&data,"\n");
    while(temp[i] != NULL)
    {
        if(strcmp(temp[i],condition)==0){
            printf("eşit\n");
            strcat(matched,temp[i]);
            strcat(matched,"\n");
            matchedCount++;
        }
        i++;
        temp[i] = strsep(&data,"\n");
    }
    if(matchedCount != 0){
        printf("\n---MATCHED---\n");
        printf(matched);
        return matched;
    }
    return NULL;
}

char *getLast(){
    return executeCondition(getAd("veri2.txt"),"fatma");
}