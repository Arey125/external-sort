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
    struct stat s_fstat; //sys ctruct for file info
    struct offsets s_offset = {0, {0}, 0}; // struct for multithreads sorts, manual wrote
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
    if(fstat(sf_fd, &s_fstat) !=0){
        perror("fstat() error");
        return -1;
    }
    f_size = s_fstat.st_size;
    printf("File size: %lld bytes, [%lld MB]\n", f_size, f_size / (1024 * 1024)); 


    if(get_freeRAM(s_sysinfo, &s_mm) != 0){ //Get max mapped file size via mmap in current system
        perror("Get memory info error");
        exit(EXIT_FAILURE);
    }


    //If the file fits entirely at one time, do it in one thread
    if((f_size) < s_mm.max_mapped_file_size){  
    
        char* buf = mmap(NULL, f_size, PROT_READ | PROT_WRITE, MAP_SHARED, sf_fd, s_offset.old_offset);
        if (buf == MAP_FAILED)
        {
            perror("(1)mmap() error");
            return -1;
        }

        char* buf_copy; //= malloc(f_size);
        

        buf_copy = singlethread_sort(buf, f_size);
        memcpy(buf, buf_copy, f_size) == NULL; //WARNING!!! Unsave function! At-ta-ta!!


        if(msync(buf, f_size, MS_ASYNC) != 0){
            perror("(1)msync() error");
            return -1;
        }

        munmap(buf, f_size);
        free(buf_copy);
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