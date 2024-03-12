
struct memory_manage
{
    long long int freeRAM;
    long long int max_mapped_file_size;
};

int get_freeRAM (struct sysinfo arg, struct memory_manage *s_mm);
