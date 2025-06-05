#include <stdio.h>
#include <dirent.h>

void print(const char *dir, const char *d_name, char d_type) {
    printf("%-5s",(d_type == DT_REG) ? "REG" :
                   (d_type == DT_DIR) ? "DIR" :
                   (d_type == DT_FIFO) ? "FIFO" :
                   (d_type == DT_SOCK) ? "SOC" :
                   (d_type == DT_LNK) ? "LNK" :
                   (d_type == DT_BLK) ? "BLK" :
                   (d_type == DT_CHR) ? "CHR" : "???");
    printf("%s/%s\n", dir, d_name);
}