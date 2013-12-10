#ifndef _STDLIB_H
#define _STDLIB_H
#include<defs.h>

// Main and Exit
int main(int argc, char* argv[], char* envp[]);
void exit(int status);

//Shutdown
void shutdown();


// Process Identification
typedef uint64_t pid_t; 
pid_t getpid (void); //The getpid function returns the process ID of the current process. 
pid_t getppid (void); //The getppid function returns the process ID of the parent of the current process. 
pid_t fork (void); // The fork function creates a new process. 

int execvpe (const char *filename, char *const argv[], char *const env[]); //This is similar to execv, but permits you to specify the environment for the new program explicitly as the env argument. This should be an array of strings in the same format as for the environ variable;see section Environment Access. 

// Memory Allocation functions
void *malloc(uint64_t size);

// Process Completion
int waitpid(int pid,int status);

//Process List
void  ps();
// Sleep Function
uint64_t sleep(uint64_t time);

//File System
uint64_t opendir(char* dir);
uint64_t readdir(uint64_t dir);
file* open(char*  file);
int read(file *, int size, char * buf);
int seek(file *fd, int offset, int whence);
int write(file *fd, char * buf, int size);
void shell_sleep(uint64_t ticks);


#endif
