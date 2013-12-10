/*
Author : Rajesh Golani
*/
#include <sys/process.h>
#include <defs.h>
#include <sys/print.h>
#include <string.h>
#include <sys/pmap.h>
#include <sys/gdt.h>
#include <sys/timer.h>
#include <sys/tarfs.h>

static int j=0;
//static int i=0;
static int c=0;
struct task_struct *prev;
struct task_struct *next; 
static uint64_t PID=1;
static int first_switch = 1;
//static bool cont=false;
extern void isr32();
uint64_t reg[14]; // set of registers  
uint64_t child_ret_addr;
uint64_t child_stack_addr; 
uint64_t child_pid=0; 
char pathname[100]; 

void * get_kva(struct Page *pp)
{
    physaddr_t ppn_addr= page2ppn(pp) << PGSHIFT;
    //uint32_t __m_ppn = ppn_addr >> PTXSHIFT ;
    uint64_t kva_addr = ppn_addr + KERNBASE; 
    return (void *)kva_addr;
}

extern pml4e_t *boot_pml4e;
extern physaddr_t boot_cr3;

//struct task_struct* malloc_pcb(struct task_struct *pcb,void *fun_ptr)
struct task_struct* malloc_pcb(void *fun_ptr)
{

    struct Page   *pp1=NULL;
//    struct Page   *pp2=NULL;

    struct task_struct *pcb=(struct task_struct *)kmalloc(sizeof(struct task_struct));

   // print("fun_ptr=%x",fun_ptr);

    ready_queue[++process_count] = (uint64_t )pcb;

    pcb->mm = (struct mm_struct *)((char *)(pcb + 1));
    pcb->mm->count = 0;
    pcb->mm->mmap = NULL;

    pcb->pid = PID++; 
    pcb->ppid = 0; //I'm kernel's child  
    //no_of_process = PID; // number of process currenly ready
    pcb->zombie = 0; // this is not zombie   
    pcb->sleep_time = 0; // this is not sleeping 
    pcb->is_waiting = 0; //no waitpid wait 
  //  print("Inside malloc_pcb -- > PID=%d,pcb->pid=%d, process_count=%d",PID,pcb->pid,process_count);

    memcpy(pcb->pname,fun_ptr,strlen(fun_ptr)); 
 
       pp1=page_alloc(0);
  //  pp2=page_alloc(0);

    uint64_t *pml4a=(uint64_t *)get_kva(pp1);
    
    int i;
    for(i=0;i<512;i++) 
        pml4a[i] = 0;

    pml4a[511] = boot_pml4e[511];
    pcb->pml4e = (uint64_t)pml4a;
    pcb->cr3 = PADDR(pml4a);


//    print("before");
    pcb->stack=kmalloc_user((pml4e_t *)pcb->pml4e,512);

  //  print("before");
    //uint64_t *pte=NULL;                                                                                                                        
    //pte = pml4e_walk((pml4e_t *)pcb->pml4e, (char*)pcb->stack, 1); 
 //   print("Page adress=%x-%x",pte,*pte); 

    asm volatile("movq %0, %%cr3":: "b"(pcb->cr3));
    print("pcb->cr3=%x",pcb->cr3);

    //pcb->stack=(uint64_t *)get_kva(pp2);

   // pcb->kstack[507] = (uint64_t)fun_ptr;  


   pcb->kstack[506] = 1; pcb->kstack[505] = 2;  pcb->kstack[504] = 3;  pcb->kstack[503] = 4;
   pcb->kstack[502] = 5; pcb->kstack[501] = 6;  pcb->kstack[500] = 7;  pcb->kstack[499] = 8;
   pcb->kstack[498] = 9; pcb->kstack[497] = 10; pcb->kstack[496] = 11; pcb->kstack[495] = 12; 
   pcb->kstack[494] = 13; pcb->kstack[493] = 14; pcb->kstack[492] = 15; 
   
   pcb->kstack[491] = (uint64_t)(&isr32+34); 

    //pcb->rsp = (&pcb->kstack[493]);
    pcb->rsp = &(pcb->kstack[490]);

    pcb->kstack[511] = 0x23 ;                               
    pcb->kstack[510]=(uint64_t)(&pcb->stack[511]);

 //   print("stack=%x",pcb->kstack[510]);
    pcb->kstack[509] = 0x246;                                            
    pcb->kstack[508] = 0x1b ;                            


    load_icode(pcb,fun_ptr,0);
    pcb->kstack[507] = (uint64_t)pcb->entry; 
    lcr3(boot_cr3);
   asm volatile("movq %0, %%cr3":: "b"(boot_cr3));
    return pcb;

}

void schedule() {
    c++;
    if(c==100) {
        
        int i , k;                                                                                                                                     
        struct task_struct *pcb; 
        struct task_struct *pcb1;  
        for(i=1;i<=process_count;i++) {    
            pcb = (struct task_struct *)ready_queue[i]; 
            if(pcb->sleep_time > 0) 
                pcb->sleep_time--;    

        }
        for(i=1;i<=process_count;i++)
        {
            pcb = (struct task_struct *)ready_queue[i];
           if(pcb->is_waiting)  // find the pid  
            {
                for(k=1;k<=process_count;k++)
                {
                    pcb1 = (struct task_struct *)ready_queue[k];
                    if((pcb1->pid==pcb->is_waiting) && (pcb1->ppid ==pcb->pid))                                                                                
                        break;
                }
                if(k>process_count)   //there is no child in ready_queue
                    pcb->is_waiting = 0;
         }
        }

   if(first_switch) {
    //    print("inside first switch");
        prev = (struct task_struct *)ready_queue[0];
        int k=1;                                                                                                                                  while(k<=process_count)  // find the first ready process wHo is not sleeping and not in wait 
        {
            pcb =(struct task_struct *) ready_queue[k];
            if((pcb->sleep_time == 0)  )
                break;
                k++;
       } 

  next = (struct task_struct *)ready_queue[k];   

  //     print("<<PCB->PID=%d>>",next->pid); 
        __asm__ __volatile__(
                "movq %%rsp, %0;"
                :"=m"(prev->rsp)
                :   
                :"memory"
        );  


        asm volatile("movq %0, %%cr3":: "a"(next->cr3));
        
     //   print("I am in process virtual address space \n");
        
        __asm__ __volatile__ (
                "movq %0, %%rsp;"
                :   
                :"m"(next->rsp)
                :"memory"
        );  
        
     first_switch = 0;  
//        print("%d", k);
        if(process_count)
            j=k;
        else
            j = 0; 
        //tss.rsp0 =(uint64_t)(&(next->kstack[511]));    
        tss.rsp0 =(uint64_t)(&(next->kstack[511]));    
        c=0;


    }

    else {
     prev = (struct task_struct*) ready_queue[j];
 
     if (process_count)
     {
         j = (j + 1) % (process_count+1);                                                                                                         

         if(j == 0 )   // we don't do switch with kernel now 
             j = 1; 
         for(i=0;i<=process_count;i++)    // find the first awaken process in the round robin queue 
         {
             pcb = (struct task_struct *)ready_queue[j];
             if((pcb->sleep_time == 0) )
                 break;
             j = (j+1)%(process_count+1);
             if(j == 0)
                 j=1;   
         } 
     }
     else
         j = 0;
      next =(struct task_struct *)ready_queue[j];
     __asm__ __volatile__(
                "movq %%rsp, %0;"
                :"=m"(prev->rsp)
                :   
                :"memory"
        );  


        asm volatile("movq %0, %%cr3":: "a"(next->cr3));
        
        __asm__ __volatile__ (
                "movq %0, %%rsp;"
                :   
                :"m"(next->rsp)
                :"memory"
        );  

    tss.rsp0 =(uint64_t)(&(next->kstack[511]));    
    c=0;
    __asm__ __volatile__("sti");
  /*   __asm__ __volatile__("iretq");

   */ }
    }

}

void function1() {
    print("\nHello");              
    while(1) {
        print("\nHello inside while\n");
    }   
}

void function2() {
    print("\nWorld..!!");
    while(1) {
        print("\nWorld..!! inside while\n");
    }   
}


void region_alloc(struct task_struct *t, void *va, size_t len) {
    void *va_start = ROUNDDOWN(va, PGSIZE);
    void *va_end = ROUNDUP((char*)va+len, PGSIZE);
    struct Page *p;
    char *v;
    print("%x %x %x", t->pml4e, va , len);

    for (v = (char*) va_start; v < (char*) va_end; v += PGSIZE) {
        // print("virtuladdr=%x\n",(uint64_t)v);
        if (!(p = page_alloc(ALLOC_ZERO)))
            panic("Running out of memory\n");
        else {
            if (page_insert((pml4e_t *) t->pml4e, p, v, PTE_P | PTE_U | PTE_W)
                    != 0)
                panic("Running out of memory\n");
        }
    }
}


// Set up the initial program binary, stack, and processor flags
// for a user process.
// This function is ONLY called during kernel initialization,
// before running the first user-mode environment.
//
// This function loads all loadable segments from the ELF binary image
// into the environment's user memory, starting at the appropriate
// virtual addresses indicated in the ELF program header.
// At the same time it clears to zero any portions of these segments
// that are marked in the program header as being mapped
// but not actually present in the ELF file - i.e., the program's bss section.
//
// All this is very similar to what our boot loader does, except the boot
// loader also needs to read the code from disk. Take a look at
// boot/main.c to get ideas.
//
// Finally, this function maps one page for the program's initial stack.
//
// load_icode panics if it encounters problems.
// - How might load_icode fail? What might be wrong with the given input?
//
void load_icode(struct task_struct *pcb, char *filename, size_t size) {
    // Hints:
    // Load each program segment into virtual memory
    // at the address specified in the ELF section header.
    // You should only load segments with ph->p_type == ELF_PROG_LOAD.
    // Each segment's virtual address can be found in ph->p_va
    // and its size in memory can be found in ph->p_memsz.
    // The ph->p_filesz bytes from the ELF binary, starting at
    // 'binary + ph->p_offset', should be copied to virtual address
    // ph->p_va. Any remaining memory bytes should be cleared to zero.
    // (The ELF header should have ph->p_filesz <= ph->p_memsz.)
    // Use functions from the previous lab to allocate and map pages.
    //
    // All page protection bits should be user read/write for now.
    // ELF segments are not necessarily page-aligned, but you can
    // assume for this function that no two segments will touch
    // the same virtual page.
    //
    // You may find a function like region_alloc useful.
    //
    // Loading the segments is much simpler if you can move data
    // directly into the virtual addresses stored in the ELF binary.
    // So which page directory should be in force during
    // this function?
    //
    // You must also do something with the program's entry point,
    // to make sure that the environment starts executing there.
    // What? (See env_run() and env_pop_tf() below.)

    // LAB 3: Your code here.

   // print("\n%s", filename);
    uint64_t offset = is_file_exists(filename);

    // uint64_t i=0;

    if (offset == 0 || offset == 999) {
        //        print("\n Error. File not found in tarfs.");
        offset = 0;
    } else {
        Elf_hdr *elf = (Elf_hdr *) (&_binary_tarfs_start + offset);
        print("in");
        Elf64_Phdr *ph, *eph;
        // struct Page *p = NULL;
        ph = (Elf64_Phdr *) ((uint8_t *) elf + elf->e_phoff);

        eph = ph + elf->e_phnum;

        for (; ph < eph; ph++) {
            if (ph->p_type == ELF_PROG_LOAD) {
                //int64_t va = ph->p_vaddr;
                // uint64_t size = ph->p_memsz;
                //uint64_t offset = ph->p_offset;
                //uint64_t i = 0;
                if (ph->p_filesz > ph->p_memsz)
                    print("Wrong size in elf binary\n");
                //                print("\n vaddr : %x size %x", ph->p_vaddr, ph->p_memsz);

     //           print("[%x %x ]", pcb->cr3 , pcb->pml4e);

                //    t->vma[i++]=ph->p_vaddr;
                //    print("vaadr=%x\n",t->vma[i]);
                // Switch to env's address space so that we can use memcpy
               print("\n teesttt\n"); 
                lcr3(pcb->cr3);
       //         puts(filename);
         //       print("BF[%x %x %x]", pcb->cr3 , pcb->pml4e, pcb->stack);
                region_alloc(pcb, (void*) ph->p_vaddr, ph->p_memsz);
                memcpy((char*) ph->p_vaddr, (void *) elf + ph->p_offset,
                        ph->p_filesz);

                if (ph->p_filesz < ph->p_memsz)
                    memset((char*) ph->p_vaddr + ph->p_filesz, 0, ph->p_memsz
                            - ph->p_filesz);

                // Switch back to kernel's address space
   //             lcr3(boot_cr3);
               
           //     print("in");
               struct vm_area_struct *vm;
                vm = malloc_vma(pcb->mm);
                vm->vm_start = ph->p_vaddr;
                vm->vm_end = vm->vm_start + ph->p_memsz;
                vm->vm_mmsz = ph->p_memsz;
                vm->vm_next = NULL;
                vm->vm_file =(uint64_t)elf;
                vm->vm_flags = ph->p_flags;
                vm->vm_offset = ph->p_offset;  
            }
        }               

        // Now map one page for the program's initial stack
        // at virtual address USTACKTOP - PGSIZE.
        //region_alloc(e, (void*) (USTACKTOP - PGSIZE), PGSIZE);

        // Magic to start executing environment at its address.
        pcb->entry = elf->e_entry;
        //print("entry=%d",t->entry);
        //t->vma[i++]='\0';
        pcb->heap_vma = (struct vm_area_struct *)kmalloc(1); // allocate a separate page for heap vma, however this is waste
        //printf("heap_vma %p %p %p %d\n",pcb->heap_vma,pcb->heap_vma->vm_start,pcb->heap_vma->vm_end,pcb->heap_vma->vm_mmsz);  
        //asm volatile("hlt");  
        struct vm_area_struct *tmp = pcb->mm->mmap;
        while(tmp->vm_next != NULL)  tmp = tmp->vm_next;  // go to the last vma

        pcb->heap_vma->vm_start = pcb->heap_vma->vm_end = ALIGN_DOWN((uint64_t)(tmp->vm_end + 0x1000));  // start from next page (keep the distance)  
        pcb->heap_vma->vm_mmsz = 0x1000;

        region_alloc(pcb, (void *)pcb->heap_vma->vm_start, pcb->heap_vma->vm_mmsz);    

    }

}
int get_pid()
{
   return next->pid; 
}

int get_ppid()
{
  return next->ppid;
}

//assumption one page will be sufficient for each VMA
struct vm_area_struct *malloc_vma(struct mm_struct *mm)
{
  struct vm_area_struct *vm_tail;
  char *tmp;
  if(mm->mmap == NULL)  // first vma allocate one page for it
   {
     tmp=(char *)kmalloc(1); // this will allocate one page so just pass size>0
     vm_tail = (struct vm_area_struct *)tmp;
     mm->mmap = vm_tail;
     mm->count += 1;
     return (struct vm_area_struct *)tmp;
   }
  else
   {
     vm_tail = mm->mmap;
     while(vm_tail->vm_next != NULL) 
         vm_tail = vm_tail->vm_next;
     
     tmp = (char *)vm_tail + sizeof(struct vm_area_struct);  // go to the next vma in the same page (base +size)
     vm_tail->vm_next = (struct vm_area_struct *)tmp;
     mm->count += 1;
     return (struct vm_area_struct *)tmp;
   }
}

struct task_struct *get_current_process()
{
 return next;
}

void fork_process()
{
  /* int f; 
    
    for(f=0;f<14;f++) //reg[0] %rbx,reg[1] %rcx ....reg[14] %r15 
        print("   %d %x", f, reg[f]);
    print("[rip child %x %x",child_ret_addr,child_stack_addr); 
   asm volatile("hlt");   */
    struct task_struct *parent_pcb = get_current_process();
    struct task_struct *child_pcb = (struct task_struct *)kmalloc(sizeof(struct task_struct));//Review
    if(!child_pcb)
        return ;

    struct Page   *pp1=NULL;                                                                                                               
  //  struct Page   *pp2=NULL;
    struct Page   *pp3=NULL;
    pp1=page_alloc(0);                                                                                                                     
    //pp2=page_alloc(0);
    pp3=page_alloc(0);

    ready_queue[++process_count] =(uint64_t)child_pcb;
    
    child_pcb->mm = (struct mm_struct *)((char *)(child_pcb + 1));
    child_pcb->mm->count = 0;
    child_pcb->mm->mmap = NULL;

    //fill the entry of pcb
    child_pcb->pid =PID++;
    child_pid = child_pcb->pid;
    child_pcb->ppid = parent_pcb->pid; //I'm parent's child
    //no_of_process = PID; // number of process currenly ready
    child_pcb->zombie = 0; // this is not zombie
    child_pcb->sleep_time = 0; // this is not sleeping
    child_pcb->is_waiting = 0; // this is not sleeping
    memcpy(child_pcb->pname,parent_pcb->pname,strlen(parent_pcb->pname));        
    
    uint64_t *pml4e =  (uint64_t *)get_kva(pp1);
    print("\nIn Fork - pcb->pid=%d, process_count=-%d,parent_pcb->pid=%d\n",child_pcb->pid,process_count,parent_pcb->pid);
    //print("\n<<<<child_pid ---- > %d>>>>>>>>\n",child_pcb->pid);
    //initialize it
    int j;
    for(j=0;j<512;j++)
        pml4e[j] = 0;
    pml4e[511] = boot_pml4e[511]; //point to pdpe of kerne
    child_pcb->pml4e = (uint64_t)pml4e;
    child_pcb->cr3 = PADDR(pml4e);
//    print("[CR3 %x]", child_pcb->cr3);
    //my process stack grows downwards
    //child_pcb->bmpPtr = (uint64_t *)BUMP_PTR_USER_SPACE;
    //child->stack=(uint64_t *)get_kva(pp2); 

    // lets copy the content of parent stack
    //int *tmp_stack=(int *)kmalloc(512);//review
    uint64_t *tmp_stack=(uint64_t *)get_kva(pp3); 

    for(j=0;j<512;j++)
    {
        tmp_stack[j] = parent_pcb->stack[j];
        //   print(" %x %x ",tmp_stack[j],parent_pcb->stack[j]);
    }
    //  asm volatile("hlt");  
    /* we need to switch child process address space in order to allocate its stack */

    // asm volatile( "cli");
    asm volatile("movq %0, %%cr3":: "b"(child_pcb->cr3));
    //  asm volatile( "sti");

    // printf("bump ptr %p\n",pcb->bmpPtr);   //currently stack is being allocated static
    //child_pcb->stack = (uint64_t *)vmmngr_bump_alloc_process(&child_pcb->bmpPtr,PAGE_SIZE);  //lets allocate two page
    // printf("%p ..child\n",&child_pcb->stack[511]); 
    //child_pcb->stack=(uint64_t *)get_kva(pp2); 
    child_pcb->stack=parent_pcb->stack; 
    //asm volatile("hlt"); 

    boot_map_segment( (pml4e_t *)child_pcb->pml4e,(uintptr_t)child_pcb->stack, 4096, (uintptr_t)PADDR(tmp_stack),PTE_W|PTE_P|PTE_U); 


    //copy the stack
 /*   for(j=0;j<512;j++) {
        child_pcb->stack[j] = tmp_stack[j];
      //     print(" %x %x ",tmp_stack[j],child_pcb->stack[j]);
    }*/
    //asm("hlt");
   
   //Stack values...to be modified by udit
    child_pcb->kstack[511] = 0x23; //ss
    child_pcb->kstack[510] = parent_pcb->kstack[508]; //rsp
    //clear_screen();

    print("[child rsp %x]",child_pcb->kstack[510]);
    
    child_pcb->kstack[509] = 0x200286; //rflags
    child_pcb->kstack[508] = 0x1b; //cs
    child_pcb->kstack[507] = parent_pcb->kstack[505];  //rip entry point scroll down to see the value
    
    print("[child rip %x]",child_pcb->kstack[507]);

    //print("[RIP %x]", child_ret_addr);
   /* child_pcb->kstack[506] = 0; //rax return to child           
    child_pcb->kstack[505] = reg[0];  child_pcb->kstack[504] = reg[1];  child_pcb->kstack[503] = reg[2];
    child_pcb->kstack[502] = reg[3]; child_pcb->kstack[501] = reg[4];  child_pcb->kstack[500] = reg[5];  child_pcb->kstack[499] = reg[6];
    child_pcb->kstack[498] = reg[7]; child_pcb->kstack[497] = reg[8]; child_pcb->kstack[496] = reg[9]; child_pcb->kstack[495] = reg[10];
    child_pcb->kstack[494] = reg[11];child_pcb->kstack[493] = reg[12];
    child_pcb->kstack[492] = reg[13];*/
    child_pcb->kstack[491] = (uint64_t)(&isr32 + 34);
    child_pcb->kstack[490] = 16; //for extra rbx
   
   
    child_pcb->kstack[506] = 0; //rax return to child           

    child_pcb->kstack[505] = parent_pcb->kstack[503]; 
    child_pcb->kstack[504] = parent_pcb->kstack[502]; 
    child_pcb->kstack[503] = parent_pcb->kstack[501];  
    child_pcb->kstack[502] = parent_pcb->kstack[500];  
    child_pcb->kstack[501] = parent_pcb->kstack[499];   
    child_pcb->kstack[500] = parent_pcb->kstack[498];   
    child_pcb->kstack[499] = parent_pcb->kstack[497];  
    child_pcb->kstack[498] = parent_pcb->kstack[496];  
    child_pcb->kstack[497] = parent_pcb->kstack[495];  
    child_pcb->kstack[496] = parent_pcb->kstack[494];  
    child_pcb->kstack[495] = parent_pcb->kstack[493];  
    child_pcb->kstack[494] = parent_pcb->kstack[492];  
    child_pcb->kstack[493] = parent_pcb->kstack[491];  
    child_pcb->kstack[492] = parent_pcb->kstack[490];  
   
   // int i;
   /* for(i = 490; i<=511; i++)
        print("[%d %x]", i, child_pcb->kstack[i]);*/
    child_pcb->rsp = &(child_pcb->kstack[490]);
    //End
    struct vm_area_struct* parent_vm = parent_pcb->mm->mmap;
    struct vm_area_struct* child_vm;
    while(parent_vm!=NULL)
    {
        child_vm = malloc_vma(child_pcb->mm);
        child_vm->vm_start = parent_vm->vm_start;
        child_vm->vm_end = parent_vm->vm_end;
        child_vm->vm_mmsz = parent_vm->vm_mmsz;
        child_vm->vm_next = NULL;
        child_vm->vm_file = parent_vm->vm_file;
        child_vm->vm_flags = parent_vm->vm_flags;
        child_vm->vm_offset = parent_vm->vm_offset;
        /******************* I AM TRYING TO LOAD HERE IT SELF ************************/
        region_alloc(child_pcb, (void*) parent_vm->vm_start, parent_vm->vm_mmsz);        
        /*uint64_t* pte = getPTE((uint64_t *)parent_vm->vm_start);
        if((*pte & PRESENT) == 0) // if frame is not allocated in pm
        {
            uint64_t* frame = pmmngr_alloc_block();
            if(frame == 0)
            {
                printf("I can't allocate here good bye\n");
                return ;   // frame allocation failed
            }
            *pte =(uint64_t)frame | PRESENT | WRITE | USER;
        }*/
        int j;

        /*memcpy((char*) child_vm->vm_start, (void *) elf + ph->p_offset, 
                        ph->p_filesz);*/
        for(j=0; j<parent_vm->vm_mmsz; j++)
        {
            *((uint64_t *)(parent_vm->vm_start + j)) = *((uint64_t *)((uint64_t)parent_vm->vm_file + parent_vm->vm_offset + j));
           // print("\n%x",  *((uint64_t *)(parent_vm->vm_start + j)) );
        }
        parent_vm = parent_vm->vm_next;
    }

    child_pcb->heap_vma = (struct vm_area_struct *)kmalloc(1); // Review  allocate a separate page for heap vma, however this is waste
    struct vm_area_struct *tmp = child_pcb->mm->mmap;
    while(tmp->vm_next != NULL)  tmp = tmp->vm_next;  // go to the last vma
    child_pcb->heap_vma->vm_start = child_pcb->heap_vma->vm_end = ALIGN_DOWN((uint64_t)(tmp->vm_end + 0x1000));  // start from next page (keep the distance)
    child_pcb->heap_vma->vm_mmsz = 0x1000;
    region_alloc(child_pcb, (void *)child_pcb->heap_vma->vm_start, child_pcb->heap_vma->vm_mmsz); 
    /*uint64_t* pte = getPTE((uint64_t *)child_pcb->heap_vma->vm_start);
    if((*pte & PRESENT) == 0) // if frame is not allocated in pm
    {
        uint64_t* frame = pmmngr_alloc_block();
        if(frame == 0)
        {
            printf("I can't allocate here good bye\n");
            exit_process(-1);   // frame allocation failed
        }
        *pte =(uint64_t)frame | PRESENT | WRITE | USER;
    }*/
    /* switch back to caller address space before we return */
    asm volatile("movq %0, %%cr3":: "b"(parent_pcb->cr3));
    //printf("%d Process count\n",process_count);   
    //asm volatile("hlt"); 
}

int fork()
{
   fork_process();
   return child_pid;
}
void exit_process(int status)  
{
 
// Load kernel's rsp
       __asm__(
               "movq %0, %%rsp;"
               :   
               :"r"(((struct task_struct *)ready_queue[0])->rsp)
             );  
/* you are in process page table so if you have allocated any memoryfree it   
   we need to free all vma's related stuff we will implement free later so now there is memory leak :(  
*/


/* load kernel's cr3 register */  
  asm volatile("movq %0, %%cr3":: "b"(boot_cr3));

// now we need to free the pcb's allocations, process kstack, process pml4e but we are not doing this memory leak :(  
  next->zombie = 1;  // mark this process as zombie;  
  zombie_queue[next->pid] = status;   // so you can get the exit status by using process pid 

  // now update the ready_queue and process_count
  // we are sure that current running process is "next" 

   int i = 1;         
   while(i <= process_count)  //find the pcb in ready_queue 
    {   
      if(ready_queue[i] == (uint64_t)next) 
         break;
      i++; 
    }           
    if(i < process_count)  // update the ready_queue 
    {
      while((i+1)<= process_count)
       {
         ready_queue[i] =  ready_queue[i+1];   
         i++;
       }  
    }
   process_count = process_count - 1; 
   // now you are in kernel mode so set it accorodingly 
   first_switch = 1;

//  for(i=0;i<100;i++)
//   printf("I am kernel\n");
   
//   printf("%d %p %p %p\n",process_count, ready_queue[1],ready_queue[2],ready_queue[3]);   
//   while(1);
   asm volatile("pop %rbx");  // extra rbx pop 
   asm volatile("pop %rax");  // get the isr addr
   asm volatile("jmp *%rax"); 
} 
void sleep_process()                                                                                                                           
{
  struct task_struct *pcb = get_current_process();   
  // save current rsp  
  __asm__(
          "movq %%rsp, %0;"
          :"=g"(pcb->rsp)
         );
  
   print("\n %x ", (((struct task_struct *)ready_queue[0])->cr3));
 // asm("hlt");
  // Load kernel's rsp
   __asm__(
           "movq %0, %%rsp;"
            :
            :"r"(((struct task_struct *)ready_queue[0])->rsp)
          );
   print("\n %x ", (((struct task_struct *)ready_queue[0])->cr3));
   //load kernels cr3
   asm volatile("movq %0, %%cr3":: "b"(((struct task_struct *)ready_queue[0])->cr3));

   first_switch = 1;
  // __asm__("hlt");
   asm volatile("pop %rbx");  // extra reg schedule;
   asm volatile("pop %rax");  // isr next instruction
   asm volatile("jmp *%rax");
}

int sleep(int t)
{
    print("\ntime %d", t);
  struct task_struct *pcb = get_current_process();
  pcb->sleep_time = t; 
//   printf("sleep time %d \n",t);  
//   asm volatile("hlt"); 
  // save current process rsp
  sleep_process();   

  return t;  
}

void*  malloc(uint64_t  size)
{
    struct task_struct *pcb = get_current_process();
    uint64_t old = pcb->heap_vma->vm_end;
    pcb->heap_vma->vm_end = pcb->heap_vma->vm_end + size;   
    // printf("Malloc %p\n",old);
    if(pcb->heap_vma->vm_end - pcb->heap_vma->vm_start > 0x1000)
    {
        print("Can't allocate memory\n");
        return NULL;
    }
    return (void *)old;   
}



uint64_t execvpe(char *arg1,char *arg2[], char* arg3[])
{
 
    /*************** use kmalloc to access it further when u switch the stack  *******/
    char *tmp_pathname = (char *)arg1;
    char **tmp_argument = (char **)arg2;
    char **tmp_env = (char **)arg3;
    int i=0;
    int j=0;
    char pathname[1024];
    strcpy(pathname,tmp_pathname);
 
    char **temp1=tmp_argument;
    while(i<3){
        print("arg%d is %s\n",i,temp1[i]);
        i++;
    }
 
    char **temp2=tmp_env;
    while(j<3){
        print("env%d is %s\n",j,temp2[j]);
        j++;
    }
    print("done");
    struct task_struct *pcb = malloc_pcb(pathname);
    print("done");
   
    struct task_struct *current_pcb =  get_current_process();
   
    pcb->pid =  current_pcb->pid;
    pcb->ppid = current_pcb->ppid;
    exit_process(0);
   
    return 0;
}

int waitpid(int pid, int status)
{
    struct task_struct *pcb = get_current_process();
    struct task_struct *child_pcb;
    uint64_t ppid = pcb->pid;
    int i;
    if(pid<0) //find any child and check for the status
    {
        for(i=1;i<=process_count;i++)
        {
            child_pcb = (struct task_struct *)ready_queue[i];
            if(child_pcb->ppid == ppid )  //
            {
                pcb->is_waiting=child_pcb->pid;
                break; 
            }
        }
        if(i>process_count) //no child process found go and resume execution
        {
            status = 0;
            pcb->is_waiting = 0;   // remove the wait
            return 0;
        }
        else if(child_pcb->zombie) // child is already dead
        {
            status = zombie_queue[child_pcb->pid];
            pcb->is_waiting = 0; // remove the wait ;
            return status;
        }
        else
        {
            pcb->is_waiting = 1;
            asm("int $0x81;"); //yield
            return 0;  //just return it won't schedule
        }
    }
    else // you have the pid find the child /
    {
        for(i=1;i<=process_count;i++)
        {
            child_pcb = (struct task_struct *)ready_queue[i];
            if(child_pcb->pid == pid)  // found the child
            {
                pcb->is_waiting = child_pcb->pid;
                break; 
            }
        }
        if(i>process_count) //no child process found go and resume execution
        {
            status = 0;
            pcb->is_waiting = 0;   // remove the wait
            return 0;
        }
        else if(child_pcb->zombie) // child is already dead
        {
            status = zombie_queue[child_pcb->pid];
            pcb->is_waiting = 0; // remove the wait ;
            return status;
        }
        else
        {
            pcb->is_waiting = 1;
            asm("int $0x81;"); //yield
            return 0;  //just return it won't schedule
        }
    }
}
void dp()
{
  struct task_struct *pcb;
  int i;
  for(i=1;i<=process_count;i++)
  {
    pcb = (struct task_struct *)ready_queue[i];
    print("%s pid:%d ppid:%d\n", pcb->pname,pcb->pid,pcb->ppid); 
  } 
}

void kshutdown() {

    // Load kernel's rsp
    __asm__(
            "movq %0, %%rsp;"
            :
            :"r"(((struct task_struct *)ready_queue[0])->rsp)
           );
                                                                                                                                               
    asm volatile("movq %0, %%cr3":: "b"(boot_cr3));

    int i = 1;

    uint64_t num=process_count;

    while(num--) {
        if(i < process_count)  {
            while((i+1)<= process_count) {
                ready_queue[i] =  ready_queue[i+1];
                i++;
            }
        }

        process_count = 0;

    }

}
