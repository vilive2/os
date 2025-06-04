#include <stdio.h>
#include <dirent.h>

void print(char *dir, char *d_name, char d_type) {
    printf("%s/%s\t%s\n", dir, d_name, (d_type == DT_REG) ? "regular" :
                                                          (d_type == DT_DIR) ? "directory" :
                                                          (d_type == DT_FIFO) ? "FIFO" :
                                                          (d_type == DT_SOCK) ? "socket" :
                                                          (d_type == DT_LNK) ? "symlink" :
                                                          (d_type == DT_BLK) ? "block dev" :
                                                          (d_type == DT_CHR) ? "char dev" : "???");
}