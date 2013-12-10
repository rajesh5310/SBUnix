#include<sys/ahci.h>
#include<sys/fs.h>
#include<string.h>
#include<sys/ports.h>
#include<sys/pmap.h>
#define SATA_SIG_ATA    0x00000101  // SATA drive
#define SATA_SIG_ATAPI  0xEB140101  // SATAPI drive
#define SATA_SIG_SEMB   0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM 0x96690101  // Port multiplier
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SATAPI 4
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE 1
#define AHCI_BASE   0x400000    // 4M
#define HBA_PxCMD_CR            (1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR            (1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE           (1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD           (1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST            (1 <<  0) /* ST - Start (command processing) */
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES   (1 << 30)       /* TFES - Task File Error Status */
#define ATA_CMD_READ_DMA_EX     0x25
#define ATA_CMD_WRITE_DMA_EX     0x35
char test[20] = "6317065029";

void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);
int find_cmdslot(HBA_PORT *port);

HBA_MEM *abar;
extern char fs_buf[1024];
//ReadSector

int read(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, QWORD buf)  
{
    //   buf = KERNBASE + buf;
        port->is = 0xffff;              // Clear pending interrupt bits
       // int spin = 0;           // Spin lock timeout counter
        int slot = find_cmdslot(port);
        if (slot == -1)
                return 0;
        uint64_t addr = 0;
//        print("\n clb %x clbu %x", port->clb, port->clbu);
        addr = (((addr | port->clbu) << 32) | port->clb);
        HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(KERNBASE + addr);
 
        //HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
        cmdheader += slot;
       cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(DWORD);     // Command FIS size
        cmdheader->w = 0;               // Read from device
        cmdheader->c = 1;               // Read from device
        cmdheader->p = 1;               // Read from device
        // 8K bytes (16 sectors) per PRDT
        cmdheader->prdtl = (WORD)((count-1)>>4) + 1;    // PRDT entries count
 
        addr=0;
        addr=(((addr | cmdheader->ctbau)<<32)|cmdheader->ctba);
        HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(KERNBASE + addr);
        
        //memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
        int i = 0; 
  //      print("[PRDTL][%d]", cmdheader->prdtl);
        // 8K bytes (16 sectors) per PRDT
        for (i=0; i<cmdheader->prdtl-1; i++)
        {
               cmdtbl->prdt_entry[i].dba = (DWORD)(buf & 0xFFFFFFFF);
                cmdtbl->prdt_entry[i].dbau = (DWORD)((buf << 32) & 0xFFFFFFFF);
                cmdtbl->prdt_entry[i].dbc = 8*1024-1;     // 8K bytes
                cmdtbl->prdt_entry[i].i = 0;
                buf += 4*1024;  // 4K words
                count -= 16;    // 16 sectors
       }
        /**If the final Data FIS transfer in a command is for an odd number of 16-bit words, the transmitter�s
Transport layer is responsible for padding the final Dword of a FIS with zeros. If the HBA receives one
more word than is indicated in the PRD table due to this padding requirement, the HBA shall not signal
this as an overflow condition. In addition, if the HBA inserts padding as required in a FIS it is transmitting,
an overflow error shall not be indicated. The PRD Byte Count field shall be updated based on the
number of words specified in the PRD table, ignoring any additional padding.**/
        
        // Last entry

        cmdtbl->prdt_entry[i].dba = (DWORD)(buf & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbau = (DWORD)((buf << 32) & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbc = count<<9;   // 512 bytes per sector
        cmdtbl->prdt_entry[i].i = 0;
        

        // Setup command
        FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
        cmdfis->fis_type = FIS_TYPE_REG_H2D;
        cmdfis->c = 1;  // Command
        cmdfis->command = ATA_CMD_READ_DMA_EX;
 
        cmdfis->lba0 = (BYTE)startl;
        cmdfis->lba1 = (BYTE)(startl>>8);
        cmdfis->lba2 = (BYTE)(startl>>16);
        cmdfis->device = 1<<6;  // LBA mode
 
        cmdfis->lba3 = (BYTE)(startl>>24);
        cmdfis->lba4 = (BYTE)starth;
        cmdfis->lba5 = (BYTE)(starth>>8);
 
        cmdfis->countl = count & 0xff;
        cmdfis->counth = count>>8;
 
    //    print("[slot]{%d}", slot);
        port->ci = 1;    // Issue command
       // Wait for completion
        while (1)
        {
                // In some longer duration reads, it may be helpful to spin on the DPS bit 
                // in the PxIS port field as well (1 << 5)
                if ((port->ci & (1<<slot)) == 0) 
                        break;
                if (port->is & HBA_PxIS_TFES)   // Task file error
                {
                        //print("Read disk error\n");
                        return 0;
                }
        }
     //   print("\n after while 1"); 
     //   print("\nafter issue : %d" , port->tfd);
        // Check again
        if (port->is & HBA_PxIS_TFES)
        {
                //print("Read disk error\n");
                return 0;
        }
        
    //    print("\n[Port ci ][%d]", port->ci);
        int k = 0;
        while(port->ci != 0)
        {
            print("[%d]", k++);
        }
        return 1;
}
 
int write(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, QWORD buf)  
{
       port->is = 0xffff;              // Clear pending interrupt bits
       // int spin = 0;           // Spin lock timeout counter
        int slot = find_cmdslot(port);
        if (slot == -1)
                return 0;
        uint64_t addr = 0;
     //   print("\n clb %x clbu %x", port->clb, port->clbu);
        addr = (((addr | port->clbu) << 32) | port->clb);
        HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(KERNBASE + addr);
 
        //HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
        cmdheader += slot;
       cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(DWORD);     // Command FIS size
        cmdheader->w = 1;               // Read from device
        cmdheader->c = 1;               // Read from device
        cmdheader->p = 1;               // Read from device
        // 8K bytes (16 sectors) per PRDT
        cmdheader->prdtl = (WORD)((count-1)>>4) + 1;    // PRDT entries count
 
        addr=0;
        addr=(((addr | cmdheader->ctbau)<<32)|cmdheader->ctba);
        HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(KERNBASE + addr);
        
        //memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
        int i = 0; 
    //    print("[PRDTL][%d]", cmdheader->prdtl);
        // 8K bytes (16 sectors) per PRDT
        for (i=0; i<cmdheader->prdtl-1; i++)
        {
               cmdtbl->prdt_entry[i].dba = (DWORD)(buf & 0xFFFFFFFF);
                cmdtbl->prdt_entry[i].dbau = (DWORD)((buf << 32) & 0xFFFFFFFF);
                cmdtbl->prdt_entry[i].dbc = 8*1024-1;     // 8K bytes
                cmdtbl->prdt_entry[i].i = 0;
                buf += 4*1024;  // 4K words
                count -= 16;    // 16 sectors
       }
        /**If the final Data FIS transfer in a command is for an odd number of 16-bit words, the transmitter�s
Transport layer is responsible for padding the final Dword of a FIS with zeros. If the HBA receives one
more word than is indicated in the PRD table due to this padding requirement, the HBA shall not signal
this as an overflow condition. In addition, if the HBA inserts padding as required in a FIS it is transmitting,
an overflow error shall not be indicated. The PRD Byte Count field shall be updated based on the
number of words specified in the PRD table, ignoring any additional padding.**/
        
        // Last entry

        cmdtbl->prdt_entry[i].dba = (DWORD)(buf & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbau = (DWORD)((buf << 32) & 0xFFFFFFFF);
        cmdtbl->prdt_entry[i].dbc = count << 9 ;   // 512 bytes per sector
        cmdtbl->prdt_entry[i].i = 0;
        

        // Setup command
        FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
 
        cmdfis->fis_type = FIS_TYPE_REG_H2D;
        cmdfis->c = 1;  // Command
        cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
        cmdfis->lba0 = (BYTE)startl;
        cmdfis->lba1 = (BYTE)(startl>>8);
        cmdfis->lba2 = (BYTE)(startl>>16);
        cmdfis->device = 1<<6;  // LBA mode
 
        cmdfis->lba3 = (BYTE)(startl>>24);
        cmdfis->lba4 = (BYTE)starth;
        cmdfis->lba5 = (BYTE)(starth>>8);
 
        cmdfis->countl = count & 0xff;
        cmdfis->counth = count>>8;
 
    //    print("[slot]{%d}", slot);
        port->ci = 1;    // Issue command
    //    print("\n[Port ci ][%d]", port->ci);
   //     print("\nafter issue : %d" , port->tfd);
    //    print("\nafter issue : %d" , port->tfd);

    //    print("\nbefore while 1--> %d", slot); 
        // Wait for completion
        while (1)
        {
                // In some longer duration reads, it may be helpful to spin on the DPS bit 
                // in the PxIS port field as well (1 << 5)
                if ((port->ci & (1<<slot)) == 0) 
                        break;
                if (port->is & HBA_PxIS_TFES)   // Task file error
                {
                        print("Read disk error\n");
                        return 0;
                }
        }
     //   print("\n after while 1"); 
     //   print("\nafter issue : %d" , port->tfd);
        // Check again
        if (port->is & HBA_PxIS_TFES)
        {
                print("Read disk error\n");
                return 0;
        }
        
     //   print("\n[Port ci ][%d]", port->ci);
       // int k = 0;
        while(port->ci != 0)
        {
       //     print("[%d]", k++);
        }
        return 1;
}
// To setup command fing a free command list slot
int find_cmdslot(HBA_PORT *port)
{
    // An empty command slot has its respective bit cleared to �0� in both the PxCI and PxSACT registers.
        // If not set in SACT and CI, the slot is free // Checked
        DWORD slots = (port->sact | port->ci);
        int num_of_slots= (abar->cap & 0x0f00)>>8 ; // Bit 8-12
        int i;
        for (i=0; i<num_of_slots; i++)
        {
                
                if ((slots&1) == 0)
                {
                       // print("\n[command slot is : %d]", i);
                        return i;
                        
                }       
                slots >>= 1;
        }
                print("Cannot find free command list entry\n");
        return -1;
}




// Check device type
static int check_type(HBA_PORT *port)
{
    DWORD ssts = port->ssts;
 
    BYTE ipm = (ssts >> 8) & 0x0F;
    BYTE det = ssts & 0x0F;
    

    print ("\n ipm %d det %d sig %d", ipm, det, port->sig); 
   if (det != HBA_PORT_DET_PRESENT)    // Check drive status
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;
 
    switch (port->sig)
    {
    case SATA_SIG_ATAPI:
        return AHCI_DEV_SATAPI ;
    case SATA_SIG_SEMB:
        return AHCI_DEV_SEMB;
    case SATA_SIG_PM:
        return AHCI_DEV_PM;
    default:
        return AHCI_DEV_SATA;
    }

    return 0;
}
void port_rebase(HBA_PORT *port, int portno){
    
    abar->ghc=(DWORD)(1<<31);
    abar->ghc=(DWORD)(1<<0);
    abar->ghc=(DWORD)(1<<31);
    abar->ghc=(DWORD)(1<<1);
    int i;
    //print("in");

    port->cmd &= ~HBA_PxCMD_ST;
 
    port->cmd &= ~HBA_PxCMD_FRE;
    // Wait until FR (bit14), CR (bit15) are cleared
        //print("\nshashank 4");
    while(1)
    {
        if (port->cmd & HBA_PxCMD_FR){
        print("\nFIS IS RUNNING");
            continue;}
        if (port->cmd & HBA_PxCMD_CR){
        print("\nCMD LIST IS RUNNING");
            continue;}
        break;
    }
    // Clear FRE (bit4)
    //print("\n %x pages_start : %x", pages_for_ahci_start); 
    port->cmd=port->cmd & 0xffff7fff; //Bit 15
    port->cmd=port->cmd & 0xffffbfff; //Bit 14
    port->cmd=port->cmd & 0xfffffffe; //Bit 0
    port->cmd=port->cmd & 0xfffffff7; //Bit 4
    //int num_slots= (abar->cap & 0x0f00)>>8 ; // Bit 8-12
    //print("\n NUK SLOTS [%d]",num_slots); 
    
     //   print("\naddress is ssad=%x", pages_for_ahci_start);
  // if( (abar->cap )>>31 )
  //  {
      // while(1); 
       // print("\naddress is ssad=%x",pages_for_ahci_start);
        port->clb = (((uint64_t)pages_for_ahci_start & 0xffffffff));
        
       // print("\naddress is ssad=%x\n",port->clb);
        port->clbu = 0;
       // print("\naddress is ssad=%x\n",port->clbu);
        port->fb =  (((uint64_t)pages_for_ahci_start + (uint64_t) ((32<<10)/8))& 0xffffffff);
       // print("\naddress is ssad=%x\n",port->fb);
        port->fbu =  ((((uint64_t)pages_for_ahci_start + (uint64_t) ((32<<10)/8))>>32)& 0xffffffff);
       /// print("\naddress is ssad=%x\n",port->fbu);
  // }

    port->serr =1;//For each implemented port, clear the PxSERR register, by writing 1 to each mplemented location
    port->is=0;//
    port->ie = 0;
   // while(1);
    uint64_t addres=0;
    addres=(((addres|port->clbu)<<32)|port->clb);
    addres =  addres + KERNBASE;
    //while(1);
    //while(1);
    memset((void *)addres, 0, 1024);
    //while(1);
    addres=0;
    addres=(((addres | port->fbu)<<32)|port->fb);
    addres = addres + KERNBASE;
    memset((void*)addres, 0, 256);
    /*
       int i;
       uint64_t addres=0;
       port->clb = (((uint64_t)pages_for_ahci_start & 0xffffffff)+ (portno<<10));
       port->clbu = ((uint64_t)pages_for_ahci_start >> 32);
       addres=0;
       addres=(((addres | port->clbu)<<32)|port->clb);
       addres =  addres+ kernmem1;
       memset((void *)addres, 0, 1024);

       port->fb =  (((uint64_t)pages_for_ahci_start + (uint64_t) 512)& 0xffffffff)+ (32<<10) + (portno<<8);
       port->fbu = ((uint64_t)pages_for_ahci_start + (uint64_t) 512) *((uint64_t)pages_for_ahci_start >> 32);
       addres=0;
       addres=(((addres | port->fbu)<<32)|port->fb);
       addres = ((uint64_t)pages_for_ahci_start + (uint64_t) 512) + kernmem1;
       memset((void*)addres, 0, 256);

     */
    addres=0;
    addres=(((addres | port->clbu)<<32)|port->clb);
    addres = (addres+ KERNBASE);
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)addres;
    i = 0;
    //print("\nbefore");
    //while(1);
    for (i=0; i<32; i++)
    {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
        // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba =  (((uint64_t)pages_for_ahci_start + (uint64_t) ((40<<10)/8) + (uint64_t)((i<<8)/8))& 0xffffffff);
        cmdheader[i].ctbau= ((((uint64_t)pages_for_ahci_start + (uint64_t) ((40<<10)/8) + (uint64_t)((i<<8)/8))>>32)& 0xffffffff);

        //  cmdheader[i].ctba = ((uint64_t)pages_for_ahci_start & 0xffffffff) + (40<<10) + (i<<8);
        //  cmdheader[i].ctbau = ((uint64_t)pages_for_ahci_start >> 32);
        addres=0;
        addres=(((addres | cmdheader[i].ctbau)<<32)|cmdheader[i].ctba);
        addres =  addres+ KERNBASE;
        //  memset((void*)addres, 0, 256);
        //      memset((void*)cmdheader[i].ctba, 0, 256);
    }
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR);
 
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST; 
    //start_cmd(port);*/    // Start command engine
    port->is = 0;   
    port->ie = 0xffffffff;
}

void probe_port(HBA_MEM *abar_temp)
{
    // Search disk in impelemented ports
    DWORD pi = abar_temp->pi;
    int i = 0;
    while (i<32)
    {
        if (pi & 1)
        {
            int dt = check_type((HBA_PORT *)&abar_temp->ports[i]);
            if (dt == AHCI_DEV_SATA)
            {
                print("\nSATA drive found at port %d\n", i);
                abar = abar_temp;
                port_rebase(abar_temp->ports, i);
                //read(&abar_temp->ports[0], 0, 0, 2, (uint64_t)pages_for_ahci_start + (20*4096)/8);
                //print("\nafter read %d",((HBA_PORT *)&abar_temp->ports[i])->ssts);
                return;
            }
            else if (dt == AHCI_DEV_SATAPI)
            {
                //print("\nSATAPI drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SEMB)
            {
                //print("\nSEMB drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_PM)
            {
                //print("\nPM drive found at port %d\n", i);
            }
            else
            {
                //print("\nNo drive found at port %d\n", i);
            }
        }
 
        pi >>= 1;
        i ++;
    }
}
static inline void sysOutLong( unsigned short port, uint32_t val ){
    __asm__ volatile( "outl %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline uint32_t sysInLong( unsigned short port ){
    uint32_t ret;
    __asm__ volatile( "inl %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

// byte -- 8 bits
// word -- 16 bits
// long -- 64 bits

unsigned short pciConfigReadWord (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset){
   // unsigned long address;
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    unsigned short tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */

    sysOutLong (0xCF8, address);

    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the fisrt word of the 32 bits register */

    tmp = (unsigned short)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
 }


unsigned long ReadWord (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset){
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    unsigned long tmp = 0;


    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    sysOutLong (0xCF8, address);
    tmp = (unsigned long)(sysInLong (0xCFC) /* & 0xffff*/);
    return (tmp);
 }
 
uint64_t checkAllBuses(void) {
    int bus;
    int slot;
    unsigned short vendor,device;
    for(bus = 0; bus < 256; bus++) {
        for(slot = 0; slot < 32; slot++) {
            vendor = pciConfigReadWord(bus,slot,0,(0x00|0x0)); 
            device = pciConfigReadWord(bus,slot,0,(0x00|0x02));
            if(vendor==0x8086 && device==0x2922){
                print("\nBUS[%d],DEVICE[%d],VENDOR[%d],DEVICE[%d]",bus,slot,vendor,device);
                print("\nRead_Address Bar 5=[%x]",ReadWord(bus,slot,0,(0x24|0x0))); 
                print("\nRead_Address=[%x]",ReadWord(bus,slot,0,(0x3c|0x0))& 0x000000000000ff00 ); 
                return ReadWord(bus,slot,0,(0x24|0x0)); 
                //mem_map_ahci((uint64_t)(&kernmem - &physbase + ((ReadWord(bus,slot,0,(0x24|0x0))) & 0xfffffffffffffff0)));
        //      mem_map_ahci(ReadWord(bus,slot,0,(0x24|0x0)));
            }
            //pciConfigReadWord (bus,device,0,0)
            // checkDevice(bus, device);
        }
    }
    return 0;
}
