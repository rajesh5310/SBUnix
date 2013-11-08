#include <defs.h>
#include <print.h>
#include <sys/tarfs.h>
#include <string.h>
#include <utils.h>
uint64_t is_file_exists(char* filename)
{
		//print("\n tarfs %x", &_binary_tarfs_start);

	    //print("\n tarfs %x", &_begin_tarfs_begin);
	   struct posix_header_ustar *test_tarfs = (struct posix_header_ustar *)&_binary_tarfs_start;
	   int i = 1, temp = 512;
	   uint64_t size;
	   while(strlen(test_tarfs->name) != 0)
	   {
		   test_tarfs = (struct posix_header_ustar *)(&_binary_tarfs_start + temp);
		   size = octalToDecimal(stoi(test_tarfs->size));
		   //print("\n");puts(test_tarfs->name);print("    size %d", size);
		   if(strlen(test_tarfs->name) == 0)
			   return 999;
		   if(strcmp(test_tarfs->name, filename) >= 0)
			   return &_binary_tarfs_start + temp + 512;
		   if(size == 0)
			   temp = temp + 512;
		   else
			   temp +=  (size%512==0) ? size + 512 : size + (512 - size%512) + 512;
		   //print("    %d", temp);
		   i += 1;
	   }
	   return 0;
}
void get_file_sections(char* filename)
{
	char *file_start= (char *)is_file_exists(filename);
	print("\n%d", *file_start);
}
