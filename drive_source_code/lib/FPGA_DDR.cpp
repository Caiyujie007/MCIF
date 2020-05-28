#include "FPGA_DDR.h"

vector<mem_control_block> mcb(1);
unsigned int has_initialized=0;

void *FPGA_DDR_malloc(unsigned int numbytes)
{
	//make sure the address is aligned to MIN_BLOCK_SIZE
	numbytes=((numbytes+MIN_BLOCK_SIZE-1)/MIN_BLOCK_SIZE)*MIN_BLOCK_SIZE;
	
	if(!has_initialized)
	{
		mcb[0].available=1;
		mcb[0].blocksize=FPGA_DDR_SIZE;
		mcb[0].board_DDR_address=FPGA_DDR_BASE_ADDRESS;
		has_initialized=1;
	}
	//Debug_mcb();

	unsigned int current_mcb_num=0;
	void * memory_location = FPGA_NULL;
	while(current_mcb_num<mcb.size())
	{
		if(mcb[current_mcb_num].available)
		{
			if(mcb[current_mcb_num].blocksize>=numbytes)
			{
				//printf("Malloc Success:%d\n",NULL);
				memory_location=(void *)mcb[current_mcb_num].board_DDR_address;
				mcb[current_mcb_num].available=0;
				if(mcb[current_mcb_num].blocksize>numbytes)
				{
					mem_control_block new_mcb;
					new_mcb.available=1;
					new_mcb.blocksize=mcb[current_mcb_num].blocksize-numbytes;
					new_mcb.board_DDR_address=mcb[current_mcb_num].board_DDR_address+numbytes;
					mcb.insert(mcb.begin()+current_mcb_num+1,new_mcb);
				}
				mcb[current_mcb_num].blocksize=numbytes;
				break;
			}
		}
		current_mcb_num++;
	}

	return memory_location;
}

void FPGA_DDR_free(void *firstbyte) 
{
	unsigned int current_mcb_num=0;
	for(int i=0;i<mcb.size();i++)
	{
		if(mcb[i].board_DDR_address==(uint64_t)firstbyte)
		{
			current_mcb_num=i;
			break;
		}
	}
	mcb[current_mcb_num].available=1;
	if(current_mcb_num!=mcb.size()-1)
	{
		if(mcb[current_mcb_num+1].available)
		{
			mcb[current_mcb_num].blocksize+=mcb[current_mcb_num+1].blocksize;
			mcb.erase(mcb.begin()+current_mcb_num+1,mcb.begin()+current_mcb_num+2);
		}
	}
	if(current_mcb_num!=0)
	{
		if(mcb[current_mcb_num-1].available)
		{
			mcb[current_mcb_num-1].blocksize+=mcb[current_mcb_num].blocksize;
			mcb.erase(mcb.begin()+current_mcb_num,mcb.begin()+current_mcb_num+1);
		}
	}
}

void Debug_mcb()
{
	printf("\n===========================\n");
	for(int i=0;i<mcb.size();i++)
	{
		printf("%d:[%8lx,%8lx]\n",mcb[i].available,mcb[i].board_DDR_address,mcb[i].board_DDR_address+mcb[i].blocksize-1);
	}
	printf("===========================\n");
}
