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

int match(char *name, char *expr, char *opts){
    To_Lowercase(name);
    To_Lowercase(expr);
    //if it necessary to convert "opts" in lowercase, same thing do .
    if (*expr == '\0' )
    {
        if (*name == '\0')
        {
            return 1;
        }else
        {
            return 0;
        }
        
        
    }
    
    if (*expr == '*')
    {
        while (*expr == '*')
        {
            expr++;
        }
        if (*expr == '\0')
        {
            return 1;
        }
        while (*name)
        {
            if (match(name, expr, opts))
            {
                return 1;
            }
            name++;
        }
        return 0;
            
    }

    if (*expr == '?')
    {
        if (*name == '\0')
        {
            return 0;
        }
        return match(name+1, expr+1,opts);
    }
    
    
    if (*expr == *name)
    {
        return match(name+1,expr+1,opts);
    }
    
    
    return 0;

}