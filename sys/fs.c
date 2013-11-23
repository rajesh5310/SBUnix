#include<fs.h>
#include<ports.h>

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

void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);

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
void port_rebase(HBA_PORT *port, int portno)
{
    
    print("\n!!!inside!!!"); 
    stop_cmd(port); // Stop command engine
 
    print("\n!!!inside!!!"); 
    // Command list offset: 1K*portno
    // Command list entry size = 32
    // Command list entry maxim count = 32
    // Command list maxim size = 32*32 = 1K per port
    port->clb = AHCI_BASE + (portno<<10);
    port->clbu = 0;
    memset((void*)((uint64_t)port->clb), 0, 1024);
 
    // FIS offset: 32K+256*portno
    // FIS entry size = 256 bytes per port
    port->fb = AHCI_BASE + (32<<10) + (portno<<8);
    port->fbu = 0;
    memset((void*)((uint64_t)port->fb), 0, 256);
    // Command table offset: 40K + 8K*portno
    // Command table size = 256*32 = 8K per port
    HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*) ( (uint64_t)port->clb);
    int i;
    for (i=0; i<32; i++)
    {
        cmdheader[i].prdtl = 8; // 8 prdt entries per command table
                    // 256 bytes per command table, 64+16+48+16*8
        // Command table offset: 40K + 8K*portno + cmdheader_index*256
        cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
        cmdheader[i].ctbau = 0;
        memset((void*)(uint64_t)cmdheader[i].ctba, 0, 256);
    }
 
   // start_cmd(port);    // Start command engine
}
 
// Start command engine
void start_cmd(HBA_PORT *port)
{
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR);
 
    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(HBA_PORT *port)
{
    print("\n st %d , cr %d , fr %d ", HBA_PxCMD_ST, HBA_PxCMD_CR, HBA_PxCMD_FR);
    // Clear ST (bit0)
    port->cmd &= ~HBA_PxCMD_ST;
 
    // Wait until FR (bit14), CR (bit15) are cleared
    while(1)
    {
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }
 
    // Clear FRE (bit4)
    port->cmd &= ~HBA_PxCMD_FRE;
} 

void probe_port(HBA_MEM *abar)
{
    // Search disk in impelemented ports
    DWORD pi = abar->pi;
    int i = 0;
    while (i<32)
    {
        if (pi & 1)
        {
            int dt = check_type((HBA_PORT *)&abar->ports[i]);
            if (dt == AHCI_DEV_SATA)
            {
                print("\nSATA drive found at port %d\n", i);
                port_rebase((HBA_PORT *)&abar->ports[i], i);
            }
            else if (dt == AHCI_DEV_SATAPI)
            {
                print("\nSATAPI drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_SEMB)
            {
                print("\nSEMB drive found at port %d\n", i);
            }
            else if (dt == AHCI_DEV_PM)
            {
                print("\nPM drive found at port %d\n", i);
            }
            else
            {
                print("\nNo drive found at port %d\n", i);
            }
        }
 
        pi >>= 1;
        i ++;
    }
}
 

