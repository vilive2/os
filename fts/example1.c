#include <fts.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[])
{
    FTS *ftsp;
    FTSENT *entry;

    char *paths[] = {"../proj/", NULL};

    ftsp = fts_open (paths, FTS_NOCHDIR | FTS_PHYSICAL, NULL);
    if (!ftsp) {
        perror ("fts_open");
        return 1;
    }

    while ((entry = fts_read (ftsp)) != NULL) 
    {
        printf("%*s%s : %s\n", entry->fts_level * 2, "", entry->fts_name, entry->fts_accpath);
        if (strcmp(entry->fts_name, "findutils") == 0) 
        {
            fts_set(ftsp, entry, FTS_SKIP);
        }
    }

    fts_close (ftsp);

    return 0;
}