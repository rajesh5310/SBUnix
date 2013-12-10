#include <stdio.h>
#include <defs.h>
#include <stdlib.h>
#define ASM 1
char path[100] = "bin/";
char pwd[100] = "~";


int strlen1(const char *s);
int strnlen(const char *s, size_t size);
char *  strcpy(char *dst, const char *src);
char *  strncpy(char *dst, const char *src, size_t size);
size_t  strlcpy(char *dst, const char *src, size_t size);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t size);
char *  strchr(const char *s, char c);
char *  strfind(const char *s, char c);
void *  memset(void *dst, uint64_t c, size_t len);
/* no memcpy - use memmove instead */
void *  memmove(void *dst, const void *src, size_t len);
void *  memcpy(void *dst, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t len);
void *  memfind(const void *s, int c, size_t len);
long    strtol(const char *s, char **endptr, int base);
char * strtrim_start(char *dst, int trim);
char * trimwhitespace(char *str);
uint64_t stoi1(char *s); // the message and then the line #



void execute(char * command);
void addToPWD(char *name)
{
    int len = strlen1(pwd);
    while(*name != '\0')
    {
        pwd[len++] = *name;
        name++;
    }
    --name;
   if(*name!='/')
        pwd[len++] = '/';
    pwd[len] = '\0';
    
}
void cdBack(char *name)
{
    int len = strlen1(pwd);
    len -= 2;
    char ch;
    while((ch = pwd[--len]) != '~' ) ;
    pwd[++len] = '\0';
}

void cat(char *name)
{
    char temp[100];
    strcpy(temp, pwd);
    int len = strlen1(temp);
    while(*name != '\0')
    {
        temp[len++] = *name;
        name++;
    }
    temp[len] = '\0';
    printf("\n"); 
    //printf("\n %s", fd->filename);
    file* fd = open(temp + 1);
//    printf("\n %s", fd->filename);
    char buff[256];
    int j = 0;
    if (true)
    {
        int bytes= read(fd, 256, buff);
      //  printf("\n%d bytes read.\n", bytes);
        while(bytes-- != 0)
            printf("%c", buff[j++]);
    }
    printf("\n");
}
void vi(char *name)
{
    if(strncmp(pwd+1,"hdd/", 4) != 0)
    {
        printf("\nCommand not supported in tarfs. Go to hdd/ directory to create files.");
        return;
    }
    char temp[100];
    printf("\n");
    strcpy(temp, pwd);
    int len = strlen1(temp);
    while(*name != '\0')
    {
        temp[len++] = *name;
        name++;
    }
    temp[len] = '\0';
    //scanf("%s", temp);
    file* fd = open(temp + 1);
    /*char* buf = (char *)malloc(1000*sizeof(char));
    scanf("%s", buf);
    printf("----->%s", buf);
*/
    char buf[30] = "asdas ssd sd qwdsad--->";
    scanf("%s", buf);
    write(fd, buf, strlen1(buf));
    //printf("\n%d bytes written.", strlen1(temp));
    printf("\n");
}


int get_file(char *name, char * buf)
{
    char temp[100];
    strcpy(temp, pwd);
    int len = strlen1(temp);
    while(*name != '\0')
    {
        temp[len++] = *name;
        name++;
    }
    temp[len] = '\0';
    //print("\n filename %s", temp);
    file* fd = open(temp + 1);
    if (fd != 0)
    {
        read(fd, 512, buf);
        
        return 1;
        //printf("%d bytes read.\n", bytes);
        //while(bytes-- != 0)
           // printf("%c", buff[j++]);
    }
    return 0;
}



int main(int argc, char* argv[], char* envp[]) {
//    printf("test");
//    while(1);
    char input[100];
//   print("\n"); 
    //scanf(input);
    while((strcmp(input, "exit")) != 0)
    {
        printf_color("\n$MFerdman@SBUnix %s $ ", pwd);
        scanf("%s", input); 
        //printf("%s\n", input);
        if((strncmp(input, "sh ", 3)) == 0)
        {
                printf("\n");
                char *str = trimwhitespace(input + 2);
                //printf("%s", file);
                char buf[512];
                int is_file_exist = get_file(str, buf);
                if(is_file_exist)
                {
                    int i;
                    char temp[100];
                    int k =0;
                    int first = 0;
                    for(i = 0; i<512; i++)
                    {
                        if(buf[i] == '\0')
                        {
                            break;
                        }
                        else if(buf[i] == '\n')
                        {
                            temp[k] = '\0';
                            k = 0;
                            if(first == 0)
                            {
                                if(strcmp(trimwhitespace(temp), "#!/bin/sh") != 0)
                                {
                                    printf("Not valid script file.\n");
                                    break;
                                }
                                else
                                    first = 1;
                            }
                            else
                            {
                                execute(trimwhitespace(temp) );
                            }
                        }
                        else
                            temp[k++] = buf[i];
                    }
                }
        }
        else
        {
            execute(input);    
        }
 
   }
    printf("\nExiting...");
    exit(-1);
    return 0;

}
void execute(char * input)
{
         if((strncmp(input, "cd", 2)) == 0 && (strncmp(input, "cd ..", 5)) != 0)
        { 
            char *directory = trimwhitespace(input + 2);
            //print("You entered %s \n", directory);
            if(*directory == '\0' || (strcmp(directory, "/") == 0) || (strcmp(directory, "/") == 0) )
            {
                pwd[0] =  '~';
                pwd[1] =  '\0';
            }
            else
            { 
                addToPWD(directory);
                //printf("\n%s pwd", pwd);
                if(( strncmp(directory, "hdd", 3)==0 && strlen1(directory)==3 ) || ( strncmp(directory, "hdd/", 4)==0 && strlen1(directory)==4 )  )
                {
                    
                }
                else
                { 
                    if(opendir(pwd + 1) == 0)
                        cdBack(pwd + 1); 
                 }                
            }
        } 
        else if((strncmp(input, "ls", 2)) == 0)
        {
            //print("You entered %s \n", pwd + 1);
            
            //execvpe("bin/test", "" , 0);
            /*int j = fork();
            if(j == 0)
            {
                char *a[]={"-a","-b","-c"};
                char *b[]={"dummy","value","pass"};
                execvpe("bin/ls",a,b);
            }
            shell_sleep(10); */
            printf("\n");
            if(strncmp(pwd, "~", 1) == 0 && strlen1(pwd)==1)
            {
                printf("hdd/    ");

            }
            if(strncmp(pwd, "~hdd/", 5) == 0 )
            {
                readdir(99999);
                return;
            }

            if(strlen1(pwd) == 1)
                readdir(999);       
            else
            {
                uint64_t dir_add = opendir(pwd + 1);
                readdir(dir_add);       
            }
        }
        else if((strncmp(input, "cd ..", 5)) == 0)
        {
            if(!(strlen1(pwd) == 1))
                 cdBack(pwd + 1);       
        }
        else if((strncmp(input, "cat", 3)) == 0)
        {
            char *file = trimwhitespace(input + 3);
            cat(file);
        }   
        else if((strncmp(input, "vi", 2)) == 0)
        {
            char *file = trimwhitespace(input + 3);
            vi(file);
        } 
        else if((strncmp(input, "ps", 2)) == 0)
        {
                printf("\n");
                ps();
        }  
        else if((strncmp(input, "sleep", 5)) == 0)
        {
                printf("\n");
                char *str = trimwhitespace(input + 5);
                //printf("%s", ticks_str);
                 if(*str != '\0')
                {
                    int ticks = stoi1(str);
                    printf("\n I am going to sleep...");
                    shell_sleep(ticks);
                }
            
        }
        else if((strncmp(input, "echo", 4)) == 0)
        {
                printf("\n");
                char *str = trimwhitespace(input + 4);
                //printf("%s", ticks_str);
                if(*str != '\0' && strncmp(str, "PATH", 5) == 0)
                {
                    printf("%s", path);
                }
                else if(*str != '\0')
                {
                    printf("%s", str);
                }
            
        }
        else if((strncmp(input, "pwd", 4)) == 0)
        {
            printf("\n%s", pwd);
        }
        else if((strncmp(input, "man", 4)) == 0)
        {
            printf("\nSupported commands are : cd, cd .., ls, cat, shutdown, echo, sleep, ps, sh, man, pwd, vi.");
        }
       else if((strncmp(input, "shutdown", 4)) == 0)
        {
            printf("\nSave Me. I am dying....");
            shutdown();
        }
        else
        {
                printf("\nCommand not found.");
        }
           
}

uint64_t stoi1(char *s) // the message and then the line #
{
    uint64_t i;
    i = 0;
    while(*s >= '0' && *s <= '9')
    {
        i = i * 10 + (*s - '0');
        s++;
    }
    return i;
}

int strlen1(const char *s) {
    int n;

    for (n = 0; *s != '\0'; s++)
        n++;
    return n;
}

int strnlen(const char *s, size_t size) {
    int n;

    for (n = 0; size > 0 && *s != '\0'; s++, size--)
        n++;
    return n;
}

char *
strcpy(char *dst, const char *src) {
    char *ret;

    ret = dst;
    while ((*dst++ = *src++) != '\0')
        /* do nothing */;
    return ret;
}

char *
strcat(char *dst, const char *src) {
    int len = strlen1(dst);
    strcpy(dst + len, src);
    return dst;
}

char * 
strtrim_start(char *dst, int trim) {
    return dst + trim;
}

char *
strncpy(char *dst, const char *src, size_t size) {
    size_t i;
    char *ret;

    ret = dst;
    for (i = 0; i < size; i++) {
        *dst++ = *src;
        // If strlen(src) < size, null-pad 'dst' out to 'size' chars
        if (*src != '\0')
            src++;
    }
    return ret;
}

size_t strlcpy(char *dst, const char *src, size_t size) {
    char *dst_in;

    dst_in = dst;
    if (size > 0) {
        while (--size > 0 && *src != '\0')
            *dst++ = *src++;
        *dst = '\0';
    }
    return dst - dst_in;
}

int strcmp(const char *p, const char *q) {
    while (*p || *q) {
        if (*p != *q)
            return -1;
        p++, q++;
    }
    return 0;
}

int strncmp(const char *p, const char *q, size_t n) {
    while (n > 0 && *p && *p == *q)
        n--, p++, q++;
    if (n == 0)
        return 0;
    else
        return (int) ((unsigned char) *p - (unsigned char) *q);
}

// Return a pointer to the first occurrence of 'c' in 's',
// or a null pointer if the string has no 'c'.
char *
strchr(const char *s, char c) {
    for (; *s; s++)
        if (*s == c)
            return (char *) s;
    return 0;
}

// Return a pointer to the first occurrence of 'c' in 's',
// or a pointer to the string-ending null character if the string has no 'c'.
char *
strfind(const char *s, char c) {
    for (; *s; s++)
        if (*s == c)
            break;
    return (char *) s;
}

int memcmp(const void *v1, const void *v2, size_t n) {
    const uint8_t *s1 = (const uint8_t *) v1;
    const uint8_t *s2 = (const uint8_t *) v2;

    while (n-- > 0) {
        if (*s1 != *s2)
            return (int) *s1 - (int) *s2;
        s1++, s2++;
    }

    return 0;
}

void* memmove(void *dst, const void *src, size_t n) {
    const char *s;
    char *d;

    s = src;
    d = dst;
    if (s < d && s + n > d) {
        s += n;
        d += n;
        while (n-- > 0)
            *--d = *--s;
    } else
        while (n-- > 0)
            *d++ = *s++;

    return dst;
}

void *
memcpy(void *dst, const void *src, size_t n) {
    return memmove(dst, src, n);
}

void *
memfind(const void *s, int c, size_t n) {
    const void *ends = (const char *) s + n;
    for (; s < ends; s++)
        if (*(const unsigned char *) s == (unsigned char) c)
            break;
    return (void *) s;
}

long strtol(const char *s, char **endptr, int base) {
    int neg = 0;
    long val = 0;

    // gobble initial whitespace
    while (*s == ' ' || *s == '\t')
        s++;

    // plus/minus sign
    if (*s == '+')
        s++;
    else if (*s == '-')
        s++, neg = 1;

    // hex or octal base prefix
    if ((base == 0 || base == 16) && (s[0] == '0' && s[1] == 'x'))
        s += 2, base = 16;
    else if (base == 0 && s[0] == '0')
        s++, base = 8;
    else if (base == 0)
        base = 10;

    // digits
    while (1) {
        int dig;

        if (*s >= '0' && *s <= '9')
            dig = *s - '0';
        else if (*s >= 'a' && *s <= 'z')
            dig = *s - 'a' + 10;
        else if (*s >= 'A' && *s <= 'Z')
            dig = *s - 'A' + 10;
        else
            break;
        if (dig >= base)
            break;
        s++, val = (val * base) + dig;
        // we don't properly detect overflow!
    }

    if (endptr)
        *endptr = (char *) s;
    return (neg ? -val : val);
}
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(*str == ' ') str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen1(str) - 1;
  while(end > str && *end == ' ') end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}
