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
		   print("\n");puts(test_tarfs->name);print("    size %d", size);
		   if(strlen(test_tarfs->name) == 0)
			   return 999;
		   if(strcmp(test_tarfs->name, filename) >= 0)
			   return temp + 512;
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
	uint64_t offset = is_file_exists(filename);
	//char *file_start= (char *)(&_binary_tarfs_start + offset);
	struct posix_header_ustar *test_tarfs = (struct posix_header_ustar *)(&_binary_tarfs_start + offset - 512);
	print("\n %d file_size\n", octalToDecimal(stoi(test_tarfs->size)));
	puts(test_tarfs->magic);
	/*Elf64_Ehdr *elfhdr = (Elf64_Ehdr *) (&_binary_tarfs_start + offset);
	print("\nentry %x", elfhdr->e_entry);
	print("\nphysoffset %x num %d", elfhdr->e_phoff, elfhdr->e_phnum);
	print("\nssoffset %x num %d", elfhdr->e_shoff, elfhdr->e_shnum);*/
	 Elf_hdr *elfhdr1 = ( Elf_hdr *) (&_binary_tarfs_start + offset);
	print("\n %x", elfhdr1->e_entry);
	print("\nphysoffset %x num %d", elfhdr1->e_phoff, elfhdr1->e_phnum);
	print("\nssoffset %x num %d", elfhdr1->e_shoff, elfhdr1->e_shnum);

	Elf64_Phdr *elfhdr_p = (Elf64_Phdr *)((uint64_t)elfhdr1 + (uint64_t)elfhdr1->e_phoff);
	print("\nphysoffset %x ", elfhdr_p->p_paddr);
	Elf64_Shdr *elfhdr_s = (Elf64_Shdr *)((uint64_t)elfhdr1 + (uint64_t)elfhdr1->e_shoff);
	int i = 0;
	for(i=0; i<16; i++)
	{
		print("\noffset %x ", elfhdr_s->sh_offset);
		print("\naddress %x ", elfhdr_s->sh_addr);
		print("  type %x ", elfhdr_s->sh_type);
		print("  size %x ", elfhdr_s->sh_size);
		print("  name %c ", elfhdr_s->sh_name);
		elfhdr_s++;
	}
	//puts((char *)*elfhdr_s->sh_name);
	/*struct Proghdr *ph, *eph;
	if (elfhdr->e_magic != ELF_MAGIC)
		print("elf header's magic is not correct\n");
	print("\n %x", elfhdr);
	print("\n %x", elfhdr->e_phoff);
	print("\n %x", elfhdr->e_phnum);
	print("\n %x", elfhdr->e_shnum);
	ph = (struct Proghdr *) ((uint8_t *) elfhdr + elfhdr->e_phoff);
	print("\n %x", ph->p_filesz);
	print("\n %x", ph->p_offset);
	print("\n %x", elfhdr->e_shnum);

	eph = ph + elfhdr->e_phnum;
	print("\n %x", eph->p_filesz);
	print("\n %x", ph->p_offset);
	struct Secthdr *sectHdr = (struct Secthdr *)elfhdr + elfhdr->e_shoff;
	print("\n %x", elfhdr->e_shoff);
	print("\n %x", sectHdr->sh_offset);*/
	//puts((char *)(sectHdr->sh_name));
	/*lcr3(PADDR(e->env_pgdir));*/

	/*int i;
	for(i=0; i< 1000; i++)
	{
		print("%c", *file_start++);
	}*/
}
