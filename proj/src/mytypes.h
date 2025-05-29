#ifndef MYTYPES
#define MYTYPES 1

#define BUF_SIZE 1024

struct linux_dirent {
    unsigned long d_ino;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
};

#endif // MYTYPES