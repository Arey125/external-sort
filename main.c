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
#include "file_cutter.h"

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
    struct stat s_fstat; //sys struct for file info
    long long int f_size;
    
    



    sf_flow = fopen(argv[1], "r+w");
    if (sf_flow == NULL)
    {
        perror("FILE*-flow open error");
        return -1;
    }
    sf_fd = fileno(sf_flow); //int-type file descriptor for lowlewel syscalls


    //Get filesize info
    if (fstat(sf_fd, &s_fstat) != 0) {
        perror("fstat() error");
        exit(1);
    }
    f_size = s_fstat.st_size;  

    if(get_freeRAM(s_sysinfo, &s_mm) != 0){ //Get max mapped file size via mmap in current system and get RAM-size
        perror("Get memory info error");
        exit(EXIT_FAILURE);
    }
    printf("File size: %lld bytes [%lld] MB\n", f_size, f_size / (1024 * 1024));
    printf("Maximum file size that can be mapped with mmap: %lld bytes [%lld MB]\n", s_mm.max_mapped_file_size, s_mm.max_mapped_file_size / (1024 * 1024));

    //If the file fits entirely at one time, do it in one thread
    if(f_size < s_mm.max_mapped_file_size){  
        sort_file(sf_fd);
        return 0;
    } else
    {   
        char** file_names;
        int files_count = file_cutter(sf_fd, s_mm.freeRAM, f_size, file_names);
        return 0;
    }
    
    return 0;
}
