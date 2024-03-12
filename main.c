#define _GNU_SOURCE 1

#include <stdio.h>
#include <sys/sysinfo.h> //For get RAM info
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "sort_engine.h"
#include "sys_utils_wrappers.h"

#define MAX_THREADS 10

struct offsets
{
    long long int filesize;
    long long int offsets[MAX_THREADS];
    long long int old_offset;
};

//struct memory_manage
//{
//    long long int freeRAM;
//    long long int max_mapped_file_size;
//};



int main(int argc, char const *argv[])
{
    if(argc < 2){
        printf("Use %s [FILENAME]\n", argv[0]);
        return -1;
    }
 
    FILE* sf_flow; // Source file flow
    int sf_fd;      //File descriptor
    struct sysinfo s_sysinfo; //sys stuct for current machine info (RAM & etc)
    struct memory_manage s_mm = {0, 0};
    long long int f_size;
    //long long int freeRAM;
    



    sf_flow = fopen(argv[1], "r+w");
    if (sf_flow == NULL)
    {
        perror("FILE*-flow open error");
        return -1;
    }
    sf_fd = fileno(sf_flow); //int-type file descriptor for lowlewel syscalls


    //Get filesize info  

    if(get_freeRAM(s_sysinfo, &s_mm) != 0){ //Get max mapped file size via mmap in current system
        perror("Get memory info error");
        exit(EXIT_FAILURE);
    }


    //If the file fits entirely at one time, do it in one thread
    if((f_size) < s_mm.max_mapped_file_size){  
        sort_file(sf_fd);

        return 0;
    }
    
    
    //Multithread sort
    //Now I try mapded file whitch largest max_mapped_file_size
    long long int alignment;
    long long int offset = 0;
    long long int halfRAM = (s_mm.freeRAM * 40) / 100;
    long long int last_character = halfRAM;
 

    while (offset < f_size)
    {
        char* buf = mmap(NULL, halfRAM, PROT_READ | PROT_WRITE, MAP_SHARED, sf_fd, offset);
        if (buf == MAP_FAILED) {
            perror("mmap(cutter)");
            }

        while (buf[last_character] != '\n')
        {
            last_character--;
            buf = mremap(buf, halfRAM, last_character, MREMAP_MAYMOVE);
            if (buf == MAP_FAILED) {
            perror("mremap");
            }
        }
        FILE* newfile = fopen("split.txt", "w");
        fwrite(buf, last_character, sizeof(char), newfile);
        fclose(newfile);
        munmap(buf, halfRAM);
        if (buf == MAP_FAILED) {
            perror("munmap (cutter)");
        }
        offset = last_character + 1;
        offset = offset - (offset % 4096);


        last_character = halfRAM;
        
        printf("file offset %lld\n", offset);

    }
    
    





    return 0;
}
