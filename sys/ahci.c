
#include <sys/interrupts.h>
#include <sys/ahci.h>
#include <sys/kprintf.h>
//#include<sys/defs.h>
#include<sys/memset_tmp.h>

//int _port;

uint32_t startl = 0;
uint32_t starth = 0;
int a,b;

hba_mem_t *abar;
hba_port_t *port;

void probe_port(hba_mem_t *abar);
static int check_type(hba_port_t *port);
int find_cmdslot(hba_port_t *port);
void port_rebase(hba_port_t *port, int portno);
int read2(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t*  buf);
int write2(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t* buf);
uint32_t update (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset);


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
    
 uint16_t pciConfigReadWord (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset){
   // unsigned long address;
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */

    sysOutLong (0xCF8, address);

    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the fisrt word of the 32 bits register */

    tmp = (uint16_t)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
 }


unsigned long ReadWord (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset){
    uint32_t address;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    unsigned long tmp = 0;


    address = (uint32_t)((lbus << 16) | (lslot << 11) |(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
    sysOutLong (0xCF8, address);
    tmp = (unsigned long)(sysInLong (0xCFC) /* & 0xffff*/);
    return (tmp);
 }

uint64_t checkAllBuses(void ){
	int bus;
	int slot;
	unsigned short vendor, sub_class,device ;
	uint16_t tmp1=0,tmp2=0;
//	kprintf("In CheckAllBuses\n");
	for(bus = 0; bus <256 ; bus++) 
	{
		for(slot = 0; slot < 32  ; slot++)
		{
			for(int function=0 ; function <8; function++){
				vendor = pciConfigReadWord(bus,slot,function,(0x0));
				if(vendor != 0xFFFF) 
				{
					device = pciConfigReadWord(bus,slot,function,(0x02));
					sub_class = pciConfigReadWord(bus,slot,function,(0x0A));
					if( sub_class == 0x106 )
					{	
						kprintf("Device found.%x	",device);
						kprintf("Vendor found.%x	",vendor);
						kprintf("Subclass found %x	",sub_class);
						int f = function;
						//uint32_t bar = update(bus,slot,0,0x24);
						tmp1 = pciConfigReadWord(bus,slot,function,(0x24));
						tmp2 = pciConfigReadWord(bus,slot,function,(0x26));
						//kprintf("%x",tmp1);
						//kprintf("\n%x",tmp2);
						uint32_t bar = tmp2 << 16| tmp1;
						kprintf("\nAHCI BAR5 value:%x",bar);
						uint32_t bar1 = update(bus,slot,f/*unction*/,0x24);
						hba_mem_t * abar = (hba_mem_t *)(uint64_t) bar1;
						probe_port(abar);

					}
				}
			}
		} 
	}  
	return 0;
}

uint32_t update (unsigned short bus, unsigned short slot,unsigned short func, unsigned short offset)
{
	uint32_t address;
	address = (uint32_t)((bus << 16) | (slot << 11)|  (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
	//kprintf("%x",address);
	sysOutLong (0xCF8, address);
	sysOutLong(0xCFC ,0xA6000);
	//sysOutLong(0xCFC ,0x3EBF1000);
	uint32_t tmp = (uint32_t)(sysInLong (0xCFC) /* & 0xffff*/);
	return tmp;
}

void probe_port(hba_mem_t *abar)
{
	uint32_t pi;
	pi = abar ->pi;
	int i = 0;
	while(i < 32)
	{
		if(pi & 1)
		{
			int dt = check_type(&abar->ports[i]);
			if(dt == AHCI_DEV_SATA){
				kprintf("\nSATA drive found at port %d",i);
				port_rebase(abar -> ports, 1);
				if(i == 0){
					uint32_t *buf = (uint32_t *)0x300000;
					uint32_t *newBuf = (uint32_t *)0x600000;
					uint8_t *p=(uint8_t*)0x300000;
					int sectors=0;
					for(int m=0; m<100;m++){
						p = (uint8_t *)0x300000;
						buf = (uint32_t *)0x300000;
						for(int j = 0;j< 512; j++){
							*p = m;
							p = p +1;
						}
						for(int n = 0; n < 8; n++){	
							b =  write2(&abar ->ports[i], sectors, 0, 1, (uint32_t *)buf);
							sectors++;
						}	
					}
					kprintf("Writing done");

					for(int j = 0; j<800;j++){
						a= read2(&abar ->ports[i], j, 0, 1, newBuf);
						uint8_t *z=(uint8_t*)newBuf;
						for(int x =0;x<512;x++)
						{
							kprintf(" %d ", *(z+x));
						}
					}
					kprintf("\nReading done");
				}
			}
			else if(dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
				//				kprintf("\nNo drive found at port %d\n", i);
			}

		} //end of if
		pi >>= 1;
		i ++;
	}//end of while
}//end of function


static int check_type(hba_port_t *port)
{
//uint64_t ssts = port->ssts;
kprintf("Port value: %x",port->sig);
switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	case SATA_SIG_ATA:
		return AHCI_DEV_SATA;
	default: 
		return 0;	
	}
}



int read2(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t* buf)
{       
//	kprintf("In Read function \n");
	int i = 0;
	port->is_rwc = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	//kprintf("\nRead slot : %d", slot);
	if(slot == -1){
	//kprintf("no slot found\n");
		return -1;
	}
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint32_t)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset_tmp(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +  (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 8K bytes (16 sectors) per PRDT
	for (int i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = 0x25; //ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 	cmdfis ->count = count;
	//cmdfis->countl = LOBYTE(count);
	//cmdfis->counth = HIBYTE(count);
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}

	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return -1;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return -1;
	}
 
//	kprintf("In Read function \n");
	return 1;
}


int write2(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t* buf)
{
//	kprintf("In Write function\n");
        int i = 0;
        port->is_rwc = (uint32_t)-1;            // Clear pending interrupt bits
        int spin = 0; // Spin lock timeout counter
        int slot = find_cmdslot(port);
	if (slot == -1) {
          return -1;
	}

        hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
        cmdheader += slot;
        cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);        // Command FIS size
        cmdheader->w = 1;               // Write to device
        cmdheader->prdtl = (uint32_t)((count-1)>>4) + 1;        // PRDT entries count

        hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
        memset_tmp(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +  (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

        // 8K bytes (16 sectors) per PRDT
        for (int i=0; i<cmdheader->prdtl-1; i++)
        {
                cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
                cmdtbl->prdt_entry[i].dbc = 8*1024;     // 8K bytes
                cmdtbl->prdt_entry[i].i = 1;
                buf += 4*1024;  // 4K words
                count -= 16;    // 16 sectors
        }
        // Last entry
        cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
        cmdtbl->prdt_entry[i].dbc = count<<9;   // 512 bytes per sector
        cmdtbl->prdt_entry[i].i = 1;

        // Setup command
        fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);

        cmdfis->fis_type = FIS_TYPE_REG_H2D;
        cmdfis->c = 1;  // Command
        cmdfis->command = 0x35;

        cmdfis->lba0 = (uint8_t)startl;
        cmdfis->lba1 = (uint8_t)(startl>>8);
        cmdfis->lba2 = (uint8_t)(startl>>16);
        cmdfis->device = 1<<6;  // LBA mode

        cmdfis->lba3 = (uint8_t)(startl>>24);
        cmdfis->lba4 = (uint8_t)starth;
        cmdfis->lba5 = (uint8_t)(starth>>8);
        cmdfis->count = count;

        // The below loop waits until the port is no longer busy before issuing a new command
        while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
        {
                spin++;
        }
        if (spin == 1000000)
        {
                kprintf("Port is hung\n");
                return 0;
        }

        port->ci = 1<<slot;     // Issue command

        // Wait for completion
        while (1)
        {
                // In some longer duration reads, it may be helpful to spin on the DPS bit
                // in the PxIS port field as well (1 << 5)
                if ((port->ci & (1<<slot)) == 0)
                        break;
                if (port->is_rwc & HBA_PxIS_TFES)       // Task file error
                {
                        kprintf("Read disk error\n");
                        return 0;
                }
        }

        // Check again
        if (port->is_rwc & HBA_PxIS_TFES)
        {
                kprintf("Read disk error\n");
                return 0;
        }
//	kprintf("Exiting write function \n");
        return 1;
}

int find_cmdslot(hba_port_t *port)
{
//sata active and command issue in port struct
uint32_t slots = (port->sact | port->ci);
//kprintf("\nPort siact:%x",port ->sact);
//kprintf("\nSlots:%d",slots);
//int no_of_slots = (abar->cap & 0x0f00)>>8 ;

//kprintf("\nnum: %d",no_of_slots);

for (int i=0; i<32; i++)
	{
		if ((slots&1) == 0){
			//kprintf("Slots: %d\n",i);
			return i;}
		slots >>= 1;
	}
kprintf("\nCannot find free command list entry\n");
return -1;

}

// Start command engine
void start_cmd(hba_port_t *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}

// Stop command engine
void stop_cmd(hba_port_t *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 	port->cmd &= ~HBA_PxCMD_FRE;
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



void port_rebase(hba_port_t *port, int portno)
{


abar -> ghc = 0x80000001;
hba_mem_t *abar = (hba_mem_t *)(uint64_t)0x3ebf1000;
abar->ghc=abar ->ghc|(uint32_t)0x2;


stop_cmd(port);	// Stop command engine

port->clb = AHCI_BASE + (portno<<10);
memset_tmp((void*)(port->clb), 0, 1024);

port->fb = AHCI_BASE + (32<<10) + (portno<<8);
memset_tmp((void*)(port->fb), 0, 256);

hba_cmd_header_t *cmdheader = (hba_cmd_header_t *)(port->clb);

for (int i=0; i<32; i++)
{
cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
// 256 bytes per command table, 64+16+48+16*8
// Command table offset: 40K + 8K*portno + cmdheader_index*256
cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
memset_tmp((void*)cmdheader[i].ctba, 0, 256);
}

start_cmd(port);	// Start command engine

}

/*
void perform_operation()
{
int a = write(port, startl, starth, 100, buf);
kprintf("a: %d\n",a);
if(a ==1) {
kprintf("Write successful!%s\n");
//int b = read(port, startl, starth, 100, buf2);
//if(b==1) {
//     kprintf("Here i am: %s\n",buf2);
// }
 }
}
*/
