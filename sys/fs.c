#include<sys/fs.h>
#include<string.h>
#include<sys/ahci.h>

//void initialize_superblock(){}
uint64_t * pages_for_ahci_start;
HBA_MEM *abar;
int sata_fs_count = 0;
uint64_t superblock_start_addr_p ;
uint64_t superblock_start_addr;
superblock* superblock_fs ;
uint64_t inode_mgmt_buffer_p;
uint64_t inode_mgmt_buffer;
uint64_t data_mgmt_buffer_p;
uint64_t data_mgmt_buffer;
satafs_entry sata_fs[100];
file fd;
char buf[512];
void add_inode_to_table(inode *inode_i)
{
    satafs_entry table_entry;
    table_entry.inode_num = inode_i->inode_num;
    //table_entry.name = inode_i->filename;
    strcpy(table_entry.name, inode_i->filename);
    table_entry.size = inode_i->size;
    table_entry.typeflag = inode_i->type;
    table_entry.size = inode_i->size;
    sata_fs[sata_fs_count++] = table_entry; 

}
int get_free_inode_block(){
    int *bitmap_start = (int *)(superblock_fs->free_inode_block);
    int *bitmap_end = (int *)((int *)superblock_fs->free_inode_block + 4);
    int *temp = bitmap_start;
    int inode_num = 0;
    while(temp < bitmap_end){
        int temp1 = *temp;
        int j;
        for(j=0;j<32;j++){
            int temp2;
            temp2 = temp1;
            temp2 &=(1<<j);
            if(temp2 == 0){
                temp1 |=(1<<j);
                *temp = temp1;
                return ++inode_num;
            }
            inode_num++;
        }
            temp++;
    }
    return -1;
}

void set_inode_block_free( int inode_num){
    int inode_array_index = inode_num/32;
    int bit_index = inode_num%32;
    superblock_fs->free_inode_block[inode_array_index] &= ~(1 << bit_index);
}

int get_free_data_block(){
    int *bitmap_start = (int *)superblock_fs->free_data_block;
    int *bitmap_end = (int *)(superblock_fs->free_data_block + 1272);
    int *temp = bitmap_start;
    int data_num = 0;
    while(temp < bitmap_end){
        int temp1 = *temp;
        int j;
        for(j=0;j<32;j++){
            int temp2;
            temp2 = temp1;
            temp2 &=(1<<j);
            if(temp2 == 0){
                temp1 |=(1<<j);
                *temp = temp1;
                return ++data_num;
            }
            data_num++;
        }
            temp++;
            

    }
    return -1;
}
void set_data_block_free( int data_num ){
    int inode_array_index = data_num/32;
    int bit_index = data_num%32;
    superblock_fs->free_data_block[inode_array_index] &= ~(1 << bit_index);

}

int get_inode_sector(int inode_num){
    if(inode_num%INODES_PER_SECTOR  != 0 )
        return INODE_S + inode_num/INODES_PER_SECTOR ;
    else
        return INODE_S + inode_num/INODES_PER_SECTOR  - 1;
}
int get_data_sector(int inode_num){
    return inode_num + DATA_S - 1; 
}

void write_inode(inode *i)
{
    int inode_sector = get_inode_sector(i->inode_num); 
    //print("[[%d]]", i->sector_loc[0]);  
    write(&abar->ports[0], inode_sector, 0, 1, (uint64_t)inode_mgmt_buffer_p );
    read(&abar->ports[0], inode_sector, 0, 1, (uint64_t)inode_mgmt_buffer_p );
    inode *write_inode = (inode *)inode_mgmt_buffer;
    write_inode += (i->inode_num-1)%3;
    write_inode = i; 
//    print("[[%d]]",write_inode->sector_loc[0]);  
    write(&abar->ports[0], inode_sector, 0, 1, (uint64_t)inode_mgmt_buffer_p );
}

inode* read_inode(int inode_num)
{
    int inode_sector = get_inode_sector(inode_num); 
    //print("%d", inode_sector);
    read(&abar->ports[0], inode_sector, 0, 1, (uint64_t)inode_mgmt_buffer_p );
    inode *inode_i = (inode *)inode_mgmt_buffer;
    inode_i += (inode_num-1)%3;
    return inode_i;
}

void write_superblock()
{
    write(&abar->ports[0], SUPERBLOCK_S, 0, SUPERBLOCK_SIZE, (uint64_t)superblock_start_addr_p );
    /*int i = 0;
    for(i=1; i<=20; i++)
    {
        strncpy(buf, (((char *)superblock_fs) + (512*(i-1))), 512);
        write(&abar->ports[0], i, 0, 1, (uint64_t) buf);
    }*/
}

void write_data(int sector_num, uint64_t buf)
{
    write(&abar->ports[0], sector_num, 0, 1, buf );
}

void write_file(file *fd,inode *i){
    // if file exist
    //read inode num from table
    //get inode from disk
    //else
    // make changes in inode accordingly
    //  
    // 
}

int lookup_fs(char *name, int flag)
{
    satafs_entry table_entry;
    int i = 0;
    while(true )
    {
        table_entry = sata_fs[i];
        i++;
        if(strlen(table_entry.name) == 0)
            break;
        
        if(strcmp(table_entry.name, name) >= 0 && table_entry.typeflag == flag)
            return table_entry.inode_num;
    }
    return 0;
}

file* file_open(char *filename)
{
    int inode_num = lookup_fs(filename, FILE_TYPE);
    inode * inode_i ;
    if(inode_num != 0 ) 
    {     
        inode_i = read_inode(inode_num);
//        print("\nsize %d\n", inode_i->size);
    }
    else
    {
        int new_inode_number =  get_free_inode_block();
        int new_inode_sector =  get_inode_sector(new_inode_number);
        //strcpy(file_i->filename, filename);
//        print("\n %d %d", new_inode_number, new_inode_sector);

        read(&abar->ports[0], new_inode_sector, 0, 1, (uint64_t) inode_mgmt_buffer_p);
 //       print("\n %x %x", inode_mgmt_buffer, inode_mgmt_buffer_p);
        inode_i = (inode *)inode_mgmt_buffer;
        inode_i += (new_inode_number-1)%3;

        inode_i->inode_num = new_inode_number;
        strcpy(inode_i->filename, filename);
        inode_i->size = 0 ;
        inode_i->type = FILE_TYPE;
        inode_i->perm = 0;
        write_superblock();
        write(&abar->ports[0], new_inode_sector, 0, 1, (uint64_t) inode_mgmt_buffer_p);
    }
    fd.is_fs = 1;
    fd.inode_num = inode_i->inode_num;
    strcpy(fd.filename, inode_i->filename);
    fd.size = inode_i->size;
    fd.type = FILE_TYPE;

    return &fd;
}

int file_write(file *fd, char * buf, int size)
{
    inode * inode_i;
    if(fd->inode_num != 0 ) 
    {     
        inode_i = read_inode(fd->inode_num);
    //    puts(inode_i->filename);
        int no_of_sectors = size/512 + 1;
        int i ; 
        int free_data_block_no;
        int free_data_block_sector;
        int page_no = 0;
        for(i=0; i<no_of_sectors; i++)
        {
            strncpy((char *)data_mgmt_buffer, buf + 512*i, 512);
            free_data_block_no =  get_free_data_block();
            free_data_block_sector =  get_data_sector(free_data_block_no);
//            print("\n[data block] %d [sector] %d", free_data_block_no, free_data_block_sector);
            write(&abar->ports[0], free_data_block_sector, 0, 1, (uint64_t) data_mgmt_buffer_p);
            inode_i->sector_loc[page_no++] = free_data_block_sector; 
        }
        inode_i->size = size;
//        print("[%d]", inode_i->sector_loc[0]);
        write_inode(inode_i);
        write_superblock();
        add_inode_to_table(inode_i);
    }
    else
    {
        print("Error !!!");
        return 0;
    }
    return size;
}
void hdd_ls()
{
    satafs_entry table_entry;
    int i = 0;
    while(true )
    {
        table_entry = sata_fs[i];
        i++;
        if(strlen(table_entry.name) == 0)
            break;

        print("%s    ", table_entry.name);
    }
}


int file_seek(file *fd, int offset, int whence)
{
    if(fd == NULL)
        return 0;
    //int new_pos = offset + whence;
    /*if(new_pos < 0 || fd->size < new_pos)
        return 0;*/
    inode* inode_i = read_inode(fd->inode_num);
//    print("\n size %d", inode_i->size);
    int old_off = fd->offset;
    if(whence == SEEK_SET)
        fd->offset = offset;
    else if (whence == SEEK_END)
        fd->offset = inode_i->size - offset;
    else
        fd->offset = fd->offset + offset;
    
    if(fd->offset < 0 || fd->size < fd->offset)
    { 
        fd->offset = old_off;
        return 0;
    }
    return 1;
}

int file_read(void *ptr, size_t nitems, file *fd)
{
  //  print("\n[%s ----  %d]", fd->filename, fd->size);
    if(fd == NULL)
        return 0;
    inode * inode_i;
    if(fd->inode_num != 0 ) 
    {     
        inode_i = read_inode(fd->inode_num);
        //puts(inode_i->filename);
        int no_of_sectors = (nitems + fd->offset)/512 + 1;
        int i ; 
        int no_of_sectors_to_skip = fd->offset/512;
    //    print("[STS] %d", no_of_sectors_to_skip);
        int bytes_skip_in_fst_sec = fd->offset%512;
        int start_sector_index = no_of_sectors_to_skip;
        int bytes_read_in_lst_sec = (fd->offset + nitems)%512;
      //  print("[STS] %d", bytes_skip_in_fst_sec);
      //  print("[STS] %d", bytes_read_in_lst_sec);
        int last_sector_index = start_sector_index + no_of_sectors - 1;
        int buffer_size_increament = 0;
        if(last_sector_index > 10)
            return 0; 
        for(i=start_sector_index; i<= last_sector_index; i++)
        {
            if(inode_i->sector_loc[i] == 0)
                return 0;
            //strncpy((char *)data_mgmt_buffer, buf + 512*i, 512);
            //free_data_block_no =  get_free_data_block();
            //free_data_block_sector =  get_data_sector(free_data_block_no);
//            print("\n[read sector] %d", inode_i->sector_loc[i]);
            read(&abar->ports[0], inode_i->sector_loc[i], 0, 1, (uint64_t) data_mgmt_buffer_p);
            if(i == start_sector_index && i != last_sector_index)
            {
                strncpy( ptr +  buffer_size_increament , (char *)data_mgmt_buffer + bytes_skip_in_fst_sec, 512 - bytes_skip_in_fst_sec);
                buffer_size_increament += 512 - bytes_skip_in_fst_sec;
            }
            else if(i == start_sector_index && i == last_sector_index)
            {
                strncpy( ptr +  buffer_size_increament , (char *)data_mgmt_buffer + bytes_skip_in_fst_sec, nitems);
                buffer_size_increament += nitems;
            }   
            else if(i == last_sector_index)
            {
                strncpy( ptr +  buffer_size_increament , (char *)data_mgmt_buffer, bytes_read_in_lst_sec);
                buffer_size_increament += bytes_read_in_lst_sec;
            }
            else
            {
                strncpy( ptr + buffer_size_increament , (char *)data_mgmt_buffer, 512);
                buffer_size_increament += 512;
            }
            //print("\n buffer_size_increament %d %x", buffer_size_increament, ptr);
            //print("\n");
            //puts((char *)ptr);
        }
        //inode_i.size = size;
        //write_inode(inode_i);
        //write_superblock();
    }
    else
    {
        print("Error !!! Invalid file descriptor.");
        return 0;
    }
    return nitems;
}




void inialize_sata_table(){

    superblock_start_addr_p = ((uint64_t)pages_for_ahci_start + (20*4096)/8);
    superblock_start_addr = KERNBASE + superblock_start_addr_p;

    inode_mgmt_buffer_p = ((uint64_t)pages_for_ahci_start + (23*4096)/8);
    inode_mgmt_buffer = KERNBASE + ((uint64_t)pages_for_ahci_start + (23*4096)/8);

    data_mgmt_buffer_p = ((uint64_t)pages_for_ahci_start + (24*4096)/8);
    data_mgmt_buffer = KERNBASE + ((uint64_t)pages_for_ahci_start + (24*4096)/8);
    
    read(&abar->ports[0], 1, 0, 20, (uint64_t) superblock_start_addr_p );
    
    superblock_fs = (superblock *)superblock_start_addr;
    //print("\n[superblock] [%x]", (uint64_t)superblock_start_addr_p);
   // print("\n[superblock] [%x]", (uint64_t)superblock_start_addr);
    //print("\n[magic] [%d]", superblock_fs->magic_no);
    //print("\n[name] [%s]", superblock_fs->fs_type);
    //print("\n[inode_free] [%x]", superblock_fs->free_inode_block[0]);

    int i, j;
    inode* inode_i;
    satafs_entry table_entry;
    for(i = INODE_S; i< INODE_E ; i++)
    {
        read(&abar->ports[0], i, 0, 1, (uint64_t) inode_mgmt_buffer_p );
        inode_i = (inode *)inode_mgmt_buffer;
        for(j=0; j<3; j++)
        {
            if((*((char *)inode_i->filename)) == '\0')
                continue;
            table_entry.inode_num = inode_i->inode_num;
            //table_entry.name = inode_i->filename;
            strcpy(table_entry.name, inode_i->filename);
            table_entry.size = inode_i->size;
            table_entry.typeflag = inode_i->type;
            table_entry.size = inode_i->size;
            sata_fs[sata_fs_count++] = table_entry; 
     //       print("\n %d", inode_i->inode_num);
     //       puts(inode_i->filename);
            inode_i++;
        }
    }
    return; 
}
int make_dir(char * name)
{
    inode * inode_i ;
    int new_inode_number =  get_free_inode_block();
    int new_inode_sector =  get_inode_sector(new_inode_number);
    //strcpy(file_i->filename, filename);
    print("\n %d %d", new_inode_number, new_inode_sector);

    read(&abar->ports[0], new_inode_sector, 0, 1, (uint64_t) inode_mgmt_buffer_p);
    print("\n %x %x", inode_mgmt_buffer, inode_mgmt_buffer_p);
    inode_i = (inode *)inode_mgmt_buffer;
    inode_i += (new_inode_number-1)%3;

    inode_i->inode_num = new_inode_number;
    strcpy(inode_i->filename, name);
    inode_i->size = 0 ;
    inode_i->type = DIRECTORY;
    inode_i->perm = 0;
    int i = 0;
    for(i=0; i<10; i++)
        inode_i->sector_loc[i] = 0;
    write_superblock();
    write(&abar->ports[0], new_inode_sector, 0, 1, (uint64_t) inode_mgmt_buffer_p);
    add_inode_to_table(inode_i);
    return 1;
}

