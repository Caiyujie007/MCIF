#include "vpu_basic.h"

void *vpu_map_base;
void *mem_map_base;

void VPU_Init()
{
	int fd;
	fd=open("/dev/mem",O_RDWR|O_SYNC);
	if(fd==-1)
		printf("Error: Can't open /dev/mem\n");
	
	vpu_map_base=mmap(0,0x1000,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0xA0000000);
	if(vpu_map_base==NULL)
		printf("Error: vpu_base mmap fail\n");

	mem_map_base=mmap(0,0x100000000,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x400000000);
	if(mem_map_base==NULL)
		printf("Error: mem_base mmap fail\n");

        //for(int i=0;i<1000000;i++)
	//	*(int *)(((uint64_t)mem_map_base)+i*4)=i;
	//for(int i=0;i<1000000;i++)
	//{
	//	if(*(int *)(((uint64_t)mem_map_base)+i*4)!=i)
	//		printf("ddr4[%d]=%d\n",i,*(int *)(((uint64_t)mem_map_base)+i*4));
	//}

	printf("VPU Init Done\n");
}

__attribute__((optimize("-O0"))) void CSB_Write(unsigned int addr,unsigned int data)
{
	*(uint32_t *)(((unsigned long int)vpu_map_base)+(addr<<2))=data;
	//printf("CSB_Write(%d,%d)\r\n",addr,data);
	//CSB_Read(addr);
}

__attribute__((optimize("-O0"))) unsigned int CSB_Read(unsigned int addr)
{
	unsigned int rt=*(uint32_t *)(((unsigned long int)vpu_map_base)+(addr<<2));
	//printf("CSB_Read(%0d)=%0d\r\n",addr,(unsigned int)rt);
	return rt;
}

void Map_Feature(short *in,struct Mapped_Feature *feature)
{
	for(int i=0;i<feature->height;i++)
		for(int j=0;j<feature->width;j++)
			for(int k=0;k<feature->channel;k=k+Tk)
			{
				unsigned int dst_base=(k/Tk)*feature->surface_stride/2+i*feature->line_stride/2+j*Tk;
				unsigned int src_base=i*feature->width*feature->channel+j*feature->channel;
				for(int kk=k;kk<k+Tk;kk++)
				{
					if(kk<feature->channel)
						*(short *)(((uint64_t)mem_map_base)+((uint64_t)(feature->payload+dst_base+(kk-k))))=in[src_base+kk];
					else
						*(short *)(((uint64_t)mem_map_base)+((uint64_t)(feature->payload+dst_base+(kk-k))))=0;
				}
			}
}

void DeMap_Feature(struct Mapped_Feature *feature,short *out)
{
	for(int i=0;i<feature->height;i++)
		for(int j=0;j<feature->width;j++)
			for(int k=0;k<feature->channel;k=k+Tk)
			{
				unsigned int dst_base=i*feature->width*feature->channel+j*feature->channel;
				unsigned int src_base=(k/Tk)*feature->surface_stride/2+i*feature->line_stride/2+j*Tk;
				for(int kk=k;kk<k+Tk;kk++)
				{
					if(kk<feature->channel)
						out[dst_base+kk]=*(short *)(((uint64_t)mem_map_base)+((uint64_t)(feature->payload+src_base+(kk-k))));
				}
			}
}

void Map_Weight(short *kernel,struct Mapped_Weight *weight)
{
	int addr;
	addr=0;
	for(int k=0;k<weight->out_ch;k+=Tk)
		for(int l=0;l<weight->in_ch;l+=Tc)
			for(int i=0;i<weight->Ky;i++)
				for(int j=0;j<weight->Kx;j++)
					for(int kk=k;kk<k+Tk;kk++)
					{
						if(kk<weight->out_ch)
						{
							for(int ll=l;ll<l+Tc;ll+=Tk)
							{
								short tp[Tk];
								unsigned int src_base=i*weight->Kx*weight->in_ch*weight->out_ch+j*weight->in_ch*weight->out_ch+kk;
								for(int lll=ll;lll<ll+Tk;lll++)
								{
									if(lll<weight->in_ch)
										tp[lll-ll]=kernel[src_base+lll*weight->out_ch];
									else
										tp[lll-ll]=0;
								}
								for(int cp=0;cp<Tk;cp++)
								{
									*(short *)(((uint64_t)mem_map_base)+(uint64_t)(&weight->payload[addr+cp]))=tp[cp];
								}
								addr=addr+Tk;
							}
						}
					}
}

struct Mapped_Feature *Malloc_Feature(unsigned int height,unsigned int width,unsigned int ch,unsigned int precision,unsigned int precision_for_conv_out_sft,int line_stride,int surface_stride)
{
	struct Mapped_Feature *ret=(struct Mapped_Feature *)malloc(sizeof(struct Mapped_Feature));
	if(ret==NULL)
	{
		return NULL;
	}

	ret->precision=precision;
	ret->precision_for_conv_out_sft=precision_for_conv_out_sft;
	ret->height=height;
	ret->width=width;
	ret->channel=ch;

	if(line_stride==-1)
		ret->line_stride=width*2*Tk;
	else
		ret->line_stride=line_stride;

	if(surface_stride==-1)
		ret->surface_stride=ret->line_stride*height;
	else
		ret->surface_stride=surface_stride;

	unsigned int require_bytes=ret->surface_stride*((ch+Tk-1)/Tk);
	ret->payload_size=require_bytes;

	ret->payload=(short *)FPGA_DDR_malloc(require_bytes);
	if(ret->payload==FPGA_NULL)
	{
		free(ret);
		return NULL;
	}

	return ret;
}

void Free_Feature(struct Mapped_Feature *feature)
{
	if(feature->payload!=FPGA_NULL) FPGA_DDR_free(feature->payload);
	free(feature);
}

void Fill_Feature_Soft(short value,struct Mapped_Feature *feature)
{
	for (int i = 0; i < ((feature->channel + Tk - 1) / Tk); i++)
	{
		for (int j = 0; j<feature->height; j++)
		{
			memset( (void *)((uint64_t)feature->payload + (feature->surface_stride * i) + (feature->line_stride*j)), 0, feature->width * Tk * 2);
		}
	}
}

void Fill_Feature(short value,struct Mapped_Feature *feature)//Call Hardware to fill DDR on FPGA board
{
	//cfg sdp_rdma_A
	CSB_Write(SDP_REG_BIAS+2,feature->height);//sdp_rdma_A_h_in
	CSB_Write(SDP_REG_BIAS+3,feature->width);//sdp_rdma_A_w_in
	CSB_Write(SDP_REG_BIAS+4,(feature->channel+Tk-1)/Tk);//sdp_rdma_A_ch_in_div_Tk
	CSB_Write(SDP_REG_BIAS+5,((uint64_t)feature->payload));//sdp_rdma_A_feature_in_base_addr
	CSB_Write(SDP_REG_BIAS+6,feature->surface_stride);//sdp_rdma_A_feature_in_surface_stride
	CSB_Write(SDP_REG_BIAS+7,feature->line_stride);//sdp_rdma_A_feature_in_line_stride

	//cfg sdp_core_per_ch
	CSB_Write(SDP_REG_BIAS+32,0b000000001);

	//cfg sdp_core_element_wise
	CSB_Write(SDP_REG_BIAS+64,0b00);//cfg_sel1_ew, cfg_sel0_ew_r

	//cfg sdp_core_nonlinear
	CSB_Write(SDP_REG_BIAS+96,3);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear
	for(int i=0;i<LUT_ENTRIES;i++)
	{
		CSB_Write(SDP_REG_BIAS+97,i);//lut_addr
		CSB_Write(SDP_REG_BIAS+98,value);//lut_value
	}

	//cfg sdp_wdma
	CSB_Write(SDP_REG_BIAS+130,feature->width);
	CSB_Write(SDP_REG_BIAS+131,feature->height);
	CSB_Write(SDP_REG_BIAS+132,(feature->channel+Tk-1)/Tk);
	CSB_Write(SDP_REG_BIAS+133,((uint64_t)feature->payload));
	CSB_Write(SDP_REG_BIAS+134,feature->surface_stride);
	CSB_Write(SDP_REG_BIAS+135,feature->line_stride);

	//Kick off the run
	CSB_Write(SDP_REG_BIAS+128,1);//sdp_wdma_start
	CSB_Write(SDP_REG_BIAS,1);//sdp_rdma_A_start

	//Wait for SDP_WDMA Done
	while(CSB_Read(SDP_REG_BIAS+129)!=1)
	{
		//usleep(10);
		//printf("Wait for SPD_wdma done\r\n");
	}
}

void Load_Feature_From_File(struct Mapped_Feature *feature,const char *filename)
{
	FILE *fp=fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("Can't open file: %s\n",filename);
		return;
	}
	size_t rd_size=fread((void *)(((uint64_t)mem_map_base)+((uint64_t)feature->payload)),1,feature->payload_size,fp);
	if(rd_size!=feature->payload_size)
		printf("Load Feature From File Error\n");

	fclose(fp);
}

void Load_Weight_From_File(struct Mapped_Weight *weight,const char *filename)
{
	FILE *fp=fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("Can't open file: %s\n",filename);
		return;
	}
	size_t rd_size=fread((void *)(((uint64_t)mem_map_base)+(uint64_t)weight->payload),1,weight->payload_size,fp);
	if(rd_size!=weight->payload_size)
		printf("Load Weight From File Error\n");
	
	fclose(fp);
}

struct Mapped_Weight *Malloc_Weight(unsigned int Ky,unsigned int Kx,unsigned int in_ch,unsigned int out_ch,unsigned int precision)
{
	struct Mapped_Weight *ret=(struct Mapped_Weight *)malloc(sizeof(struct Mapped_Weight));
	if(ret==NULL)
		return NULL;

	ret->precision=precision;
	ret->Ky=Ky;
	ret->Kx=Kx;
	ret->in_ch=in_ch;
	ret->out_ch=out_ch;

	unsigned int require_bytes=(Tc*2*Kx*Ky*out_ch*((in_ch+Tc-1)/Tc));
	ret->payload_size=require_bytes;

	ret->payload=(short *)FPGA_DDR_malloc(require_bytes);
	if(ret->payload==FPGA_NULL)
	{
		free(ret);
		return NULL;
	}

	return ret;
}

void Free_Weight(struct Mapped_Weight *weight)
{
	if(weight->payload!=FPGA_NULL) FPGA_DDR_free(weight->payload);
	free(weight);
}

short* Get_Element(struct Mapped_Feature *feature,unsigned int row,unsigned int col,unsigned int ch)
{
	return (short *)(((uint64_t)mem_map_base) + ((uint64_t)feature->payload) + (ch/Tk)*feature->surface_stride + row*feature->line_stride + col*Tk*2 + (ch%Tk)*2);
}

short* Get_Weight(struct Mapped_Weight *weight,unsigned int n_h,unsigned int n_w,unsigned int n_cin,unsigned int n_cout)
{
	unsigned int ch_group_now=((n_cout/Tk)==((weight->out_ch-1)/Tk))?(((weight->out_ch-1)%Tk)+1):Tk;
		
	uint64_t addr_bias=((n_cout/Tk)*Tk*((weight->in_ch+Tc-1)/Tc)*weight->Kx*weight->Ky+(n_cin/Tc)*ch_group_now*weight->Kx*weight->Ky+(n_h*weight->Kx+n_w)*ch_group_now+(n_cout%Tk))*Tc*2+(n_cin%Tc)*2;
	//printf("bias:%d\n",addr_bias);
	return (short *)((uint64_t)mem_map_base+((uint64_t)weight->payload) + addr_bias);
}

void Get_LUT(unsigned int precision_in,unsigned int precision_out,double (*func)(double),short lut[LUT_ENTRIES])
{
	for(int i=0;i<LUT_ENTRIES;i++)
	{
		short tp=(i*2+1)<<(16-log2LUT_ENTRIES-1);
		double tp_double=(tp*1.0)/pow(2.0,(int)precision_in);
		double Y_double=(*func)(tp_double);
		short Y=Y_double*pow(2.0,(int)precision_out);

		lut[i]=Y;
	}
}

void Nonlinear_OP(unsigned int relu_en,short lut[LUT_ENTRIES],struct Mapped_Feature *feature_in,struct Mapped_Feature *feature_out)
{
	//cfg sdp_rdma_A
	CSB_Write(SDP_REG_BIAS+2,feature_in->height);//sdp_rdma_A_h_in
	CSB_Write(SDP_REG_BIAS+3,feature_in->width);//sdp_rdma_A_w_in
	CSB_Write(SDP_REG_BIAS+4,(feature_in->channel+Tk-1)/Tk);//sdp_rdma_A_ch_in_div_Tk
	CSB_Write(SDP_REG_BIAS+5,((uint64_t)feature_in->payload));//sdp_rdma_A_feature_in_base_addr
	CSB_Write(SDP_REG_BIAS+6,feature_in->surface_stride);//sdp_rdma_A_feature_in_surface_stride
	CSB_Write(SDP_REG_BIAS+7,feature_in->line_stride);//sdp_rdma_A_feature_in_line_stride

	//cfg sdp_core_per_ch
	CSB_Write(SDP_REG_BIAS+32,0b000000001);

	//cfg sdp_core_element_wise
	CSB_Write(SDP_REG_BIAS+64,0b00);//cfg_sel1_ew, cfg_sel0_ew_r

	//cfg sdp_core_nonlinear
	if(lut!=NULL)
	{
		CSB_Write(SDP_REG_BIAS+96,(relu_en<<2)+3);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear
		for(int i=0;i<LUT_ENTRIES;i++)
		{
			CSB_Write(SDP_REG_BIAS+97,i);//lut_addr
			CSB_Write(SDP_REG_BIAS+98,lut[i]);//lut_value
		}
	}
	else
	{
		CSB_Write(SDP_REG_BIAS+96,(relu_en<<2)+3);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear
		for(int i=0;i<LUT_ENTRIES;i++)
		{
			short tp=(i*2+1)<<(16-log2LUT_ENTRIES-1);
			double tp_double=(tp*1.0)/pow(2.0,feature_in->precision);
			short Y=tp_double*pow(2.0,feature_out->precision);

			//printf("%f\n",tp_double);
			CSB_Write(SDP_REG_BIAS+97,i);//lut_addr
			CSB_Write(SDP_REG_BIAS+98,Y);//lut_value
		}
	}

	//cfg sdp_wdma
	CSB_Write(SDP_REG_BIAS+130,feature_out->width);
	CSB_Write(SDP_REG_BIAS+131,feature_out->height);
	CSB_Write(SDP_REG_BIAS+132,(feature_out->channel+Tk-1)/Tk);
	CSB_Write(SDP_REG_BIAS+133,((uint64_t)feature_out->payload));
	CSB_Write(SDP_REG_BIAS+134,feature_out->surface_stride);
	CSB_Write(SDP_REG_BIAS+135,feature_out->line_stride);

	//Kick off the run
	CSB_Write(SDP_REG_BIAS+128,1);//sdp_wdma_start
	CSB_Write(SDP_REG_BIAS,1);//sdp_rdma_A_start

	//Wait for SDP_WDMA Done
	while(CSB_Read(SDP_REG_BIAS+129)!=1)
	{
		//usleep(10);
		//printf("Wait for SPD_wdma done\r\n");
	}
}

void Element_Wise(unsigned int relu_en,short lut[LUT_ENTRIES],unsigned int element_wise_op,//0: add, 1: mul, 2: minus, 3: not defined
		struct Mapped_Feature *feature_1,struct Mapped_Feature *feature_2,struct Mapped_Feature *feature_out)
{
	//cfg sdp_rdma_A
	CSB_Write(SDP_REG_BIAS+2,feature_1->height);//sdp_rdma_A_h_in
	CSB_Write(SDP_REG_BIAS+3,feature_1->width);//sdp_rdma_A_w_in
	CSB_Write(SDP_REG_BIAS+4,(feature_1->channel+Tk-1)/Tk);//sdp_rdma_A_ch_in_div_Tk
	CSB_Write(SDP_REG_BIAS+5,((uint64_t)feature_1->payload));//sdp_rdma_A_feature_in_base_addr
	CSB_Write(SDP_REG_BIAS+6,feature_1->surface_stride);//sdp_rdma_A_feature_in_surface_stride
	CSB_Write(SDP_REG_BIAS+7,feature_1->line_stride);//sdp_rdma_A_feature_in_line_stride

	//cfg sdp_rdma_B
	CSB_Write(SDP_REG_BIAS+18,feature_2->height);//sdp_rdma_B_h_in
	CSB_Write(SDP_REG_BIAS+19,feature_2->width);//sdp_rdma_B_w_in
	CSB_Write(SDP_REG_BIAS+20,(feature_2->channel+Tk-1)/Tk);//sdp_rdma_B_ch_in_div_Tk
	CSB_Write(SDP_REG_BIAS+21,((uint64_t)feature_2->payload));//sdp_rdma_B_feature_in_base_addr
	CSB_Write(SDP_REG_BIAS+22,feature_2->surface_stride);//sdp_rdma_B_feature_in_surface_stride
	CSB_Write(SDP_REG_BIAS+23,feature_2->line_stride);//sdp_rdma_B_feature_in_line_stride

	//cfg sdp_core_per_ch
	CSB_Write(SDP_REG_BIAS+32,0b011000001);

	//cfg sdp_core_element_wise
	CSB_Write(SDP_REG_BIAS+64,0b11);//cfg_sel1_ew, cfg_sel0_ew_r
	CSB_Write(SDP_REG_BIAS+65,(element_wise_op==1)?0:feature_2->precision);//cfg_sft_s1_ew
	CSB_Write(SDP_REG_BIAS+66,(element_wise_op==1)?0:feature_1->precision);//cfg_sft_s2_ew
	CSB_Write(SDP_REG_BIAS+67,feature_1->precision+feature_2->precision-feature_out->precision);//cfg_sft_d_ew
	CSB_Write(SDP_REG_BIAS+68,element_wise_op);//0: add, 1: mul, 2: minus, 3: not defined

	if(lut!=NULL)
	{
		CSB_Write(SDP_REG_BIAS+96,(relu_en<<2)+3);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear
		for(int i=0;i<LUT_ENTRIES;i++)
		{
			CSB_Write(SDP_REG_BIAS+97,i);//lut_addr
			CSB_Write(SDP_REG_BIAS+98,lut[i]);//lut_value
		}
	}
	else
		CSB_Write(SDP_REG_BIAS+96,relu_en<<2);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear

	//cfg sdp_wdma
	CSB_Write(SDP_REG_BIAS+130,feature_out->width);
	CSB_Write(SDP_REG_BIAS+131,feature_out->height);
	CSB_Write(SDP_REG_BIAS+132,(feature_out->channel+Tk-1)/Tk);
	CSB_Write(SDP_REG_BIAS+133,((uint64_t)feature_out->payload));
	CSB_Write(SDP_REG_BIAS+134,feature_out->surface_stride);
	CSB_Write(SDP_REG_BIAS+135,feature_out->line_stride);
		
	//Kick off the run
	CSB_Write(SDP_REG_BIAS+128,1);//sdp_wdma_start
	CSB_Write(SDP_REG_BIAS,1);//sdp_rdma_A_start
	CSB_Write(SDP_REG_BIAS+16,1);//sdp_rdma_B_start

	//Wait for SDP_WDMA Done
	while(CSB_Read(SDP_REG_BIAS+129)!=1)
	{
		//usleep(10);
		//printf("Wait for SPD_wdma done\r\n");
	}
}
