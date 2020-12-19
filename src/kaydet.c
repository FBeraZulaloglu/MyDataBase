#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 


int main(int argc, char *argv[]) 
{  
    FILE *f;
    f = fopen("kaydet.txt","w");
    fprintf(f,"%s",argv[0]);
    return 0; 
} 