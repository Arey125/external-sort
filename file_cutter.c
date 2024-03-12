#define _GNU_SOURCE 1

#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>



int file_cutter(int fs_fd, long long int freeRAM, long long int f_size, char** file_name){



    int fd = fs_fd;
    long long int offset = 0;
    long long int halfRAM = (freeRAM * 40) / 100;
    long long int last_char = halfRAM; //byte to byte 
    int file_count = (f_size + halfRAM - 1) / halfRAM; //Divide around 
    long long int alignment = 0;
    int iter = 0; //Just tick itertion
    char* string;

    while (offset <= f_size)
    {
        long long int map_size = halfRAM;
        if(map_size > f_size - offset){
            map_size = f_size - offset;
        }
        
        last_char = map_size - 1;

        char* buf = mmap(NULL, map_size, PROT_READ, MAP_SHARED, fd, offset);
        if (buf == MAP_FAILED) {
            perror("mmap(cutter)");
            }

        while ((offset + map_size) < f_size && buf[last_char] != '\n')
        {   
            last_char--;
            buf = mremap(buf, map_size, last_char, MREMAP_MAYMOVE); //truncate
            if (buf == MAP_FAILED) {
            perror("mremap");
            }
        }
        char* new_file_name = malloc(64);
        sprintf(new_file_name, "./tmp/split%d.txt", iter);
        FILE* newfile = fopen(new_file_name, "w");
        fwrite(buf + alignment, last_char, sizeof(char), newfile);

        fclose(newfile);
        munmap(buf, map_size);
        if (buf == MAP_FAILED) {
            perror("munmap (cutter)");
        }

        offset = offset + last_char + 1;
        if (offset >= f_size)
        {
            break;
        }
        
        alignment = offset % 4096;
        offset = offset - alignment;
        last_char = map_size;
        iter++;


        printf("file offset %lld\n", offset);
    }
}