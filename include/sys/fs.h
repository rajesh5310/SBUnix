#include<sys/pmap.h>
#define DIRECTORY 5
#define FILE_TYPE 0

typedef struct {
    char fs_type[10];
    int size;
    int magic_no;
    int inode_start_sector;
    int data_start_sector;
    int free_inode_block[4];
    int free_data_block[1272];
} superblock;

typedef struct {
    int inode_num;
    char filename[100];
    int perm;
    int size;
    char type;
    int sector_loc[10];
    int written;
} inode;

typedef struct {
    char filename[100];
    int inode_num;
} dentry;

typedef struct {
    char inode_num;
    char name[100];
    int size;
    int typeflag;
    int par_ind;
} satafs_entry;

extern satafs_entry sata_fs[100];
#define SUPERBLOCK_SIZE 20
#define SUPERBLOCK_S 1
#define SUPERBLOCK_E 20
#define INODE_S 23
#define INODE_E 151
#define RESERVE_I1_S 152
#define RESERVE_I1_E 160
#define SUPERBLOCK_COPY_S 161
#define SUPERBLOCK_COPY_E 180
#define RESERVE_I2_S 181
#define RESERVE_I2_E 190
#define DATA_S 191
#define DATA_E 40704
#define SECTOR_SIZE 512
#define INODES_PER_SECTOR 3

void initialize_superblock();
void inialize_sata_table();
file* file_open(char *file_name);
int file_read(void *ptr, size_t nitems, file *fd);
int file_seek(file *fd, int offset, int whence);
int dir_read(char * name);
int file_write(file *fd, char * buf, int size);
int dir_write(char * name);
int dir_open(char * name);
int make_dir(char * name);
int file_close(file *fd);
void hdd_ls();
extern uint64_t superblock_start_addr_p ;
extern uint64_t superblock_start_addr;
extern superblock* superblock_fs ;
extern uint64_t inode_mgmt_buffer_p;
extern uint64_t inode_mgmt_buffer;
extern uint64_t data_mgmt_buffer_p;
extern uint64_t data_mgmt_buffer;
