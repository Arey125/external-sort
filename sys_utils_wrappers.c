#include <stdlib.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/resource.h>

#include "sys_utils_wrappers.h"

int get_freeRAM (struct sysinfo s_sysinfo, struct memory_manage *s_mm) {
    
    struct rlimit s_rlim;
    long long max_mapped_file_size;
    long long int freeRAM;

    if (sysinfo(&s_sysinfo) != 0)
    {
        perror("sysinfo-call error");
        exit(EXIT_FAILURE);
    }
    
    freeRAM = s_sysinfo.freeram * s_sysinfo.mem_unit; //Get freeRAM value in current system
    printf("Free RAM allowed: %lld bytes [%lld MB]\n", freeRAM, freeRAM / (1024 * 1024));


    max_mapped_file_size = sysconf(_SC_MAPPED_FILES) * sysconf(_SC_PAGE_SIZE); //Get max mapped file size via mmap in current system

    printf("mmp = %lld \n", max_mapped_file_size);
    //Trying to bite off as much memory as possible from the system

    if (getrlimit(RLIMIT_AS, &s_rlim) == -1) { //Max addres space in process
        perror("(1)getrlimit() error");
        exit(EXIT_FAILURE);
    }

    if (s_rlim.rlim_cur < max_mapped_file_size) {
        max_mapped_file_size = s_rlim.rlim_cur;
    }

    // s_mm->freeRAM = freeRAM;
    s_mm->freeRAM = 300 * 1024 * 1024;
    // s_mm->max_mapped_file_size = max_mapped_file_size;
    s_mm->max_mapped_file_size = 100 * 1024*1024;
    
    return 0;
}
