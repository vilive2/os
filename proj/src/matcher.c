#include<string.h>
#include<ctype.h>

void To_Lowercase(char *string){
    while (*string)
    {
        if (isupper(*string) != 0)
        {
            *string = tolower(*string);
        }
        string++;
    }
    
}

int match(const char *name, const char *pattern, unsigned int opts){
    if (opts & (1 << 4)) {
        // Ignore case
        char new_name[32];
        char new_pattern[32];
        strncpy(new_name, name, 31);
        new_name[31] = '\0';
        strncpy(new_pattern, pattern, 31);
        new_pattern[31] = '\0';
        To_Lowercase(new_name);
        To_Lowercase(new_pattern);
        return match(new_name, new_pattern, opts-(1<<4));
    }
    
    if (*pattern == '\0' )
    {
        if (*name == '\0')
        {
            return 1;
        }else
        {
            return 0;
        }
    }
    
    if (*pattern == '*')
    {
        while (*pattern == '*')
        {
            pattern++;
        }
        if (*pattern == '\0')
        {
            return 1;
        }
        while (*name)
        {
            if (match(name, pattern, opts))
            {
                return 1;
            }
            name++;
        }
        return 0;
            
    }

    if (*pattern == '?')
    {
        if (*name == '\0')
        {
            return 0;
        }
        return match(name+1, pattern+1,opts);
    }
    
    if (*pattern == '[')
    {
        pattern++;
        int match_found = 0;

        while (*pattern != ']' && *pattern != '\0')
        {
            if (*pattern == *name)
            {
                match_found =1;
            }
            pattern++;
        }
        if (*pattern != ']')
        {
            return 0;
        }
        
        if (match_found == 1)
        {
            return match(name +1, pattern +1,opts);
        }
        return 0;
        
    }
    
    
    
    if (*pattern == *name)
    {
        return match(name+1,pattern+1,opts);
    }
    
    return 0;

}