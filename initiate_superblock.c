#include<stdlib.h>
#include<stdio.h>

typedef struct {
    char fs_type[10];
    int size;
    int magic_no;
    int inode_start_sector;
    int data_start_sector;
    int free_inode_block[4];
    int free_data_block[1272];
} superblock;

int main()
{
    superblock f;
    f.fs_type[0] = 'H'; 
    f.fs_type[1] = 'i'; 
    f.fs_type[2] = 'p'; 
    f.fs_type[3] = 'H'; 
    f.fs_type[4] = 'o'; 
    f.fs_type[5] = 'p'; 
    f.fs_type[6] = 'F'; 
    f.fs_type[7] = 'F'; 
    f.fs_type[8] = '\0'; 
    f.size = 20971520;
    f.magic_no = 2014;
    f.inode_start_sector = 23;
    f.data_start_sector = 191;
    f.free_inode_block[0] = 0; 
    f.free_inode_block[1] = 0; 
    f.free_inode_block[2] = 0; 
    f.free_inode_block[3] = 0; 
    int i ;
    for(i = 0 ; i < 1272 ; i++)
    {
        f.free_data_block[i] = 0;
    }

   FILE * fp;
   fp = fopen ("superblock", "w+");
   fwrite(&f, sizeof(superblock), 1, fp);
 
   fclose(fp);
 
   /*superblock rs;
   fp = fopen ("superblock", "r");
   fread(&rs, sizeof(superblock), 1, fp);
 
   fclose(fp);
   printf("\n%d", rs.size);*/

}
