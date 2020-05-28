#include "vpu_pool.h"

void RunPool(unsigned int kernel_width,unsigned int kernel_height,unsigned int x_stride,unsigned int y_stride,unsigned int pooling_method,
		unsigned int pad_left,unsigned int pad_right,unsigned int pad_up,unsigned int pad_down,
		struct Mapped_Feature *feature_in,struct Mapped_Feature *feature_out)
{
	unsigned int out_width;
	unsigned int out_height;

	unsigned int split_num;
	unsigned int max_outbar_width;

	unsigned int fwidth_in;
	unsigned int lwidth_in;
	unsigned int fwidth_out;
	unsigned int lwidth_out;
	unsigned int overlap;

	out_width=((feature_in->width-kernel_width)/x_stride+1);
	out_height=((feature_in->height-kernel_height)/y_stride+1);
	overlap=kernel_width-x_stride;
	//printf("out_width=%d,out_height=%d\n",out_width,out_height);

	if(kernel_height==1)
		max_outbar_width=99999999;
	else
		max_outbar_width=PDP_CORE_2D_LINEBUFF_LEN/(kernel_height-1);

	split_num=out_width/max_outbar_width;//ceil(out_width/max_outbar_width)-1

	if(split_num==0)
		{fwidth_out=out_width-1;fwidth_in=feature_in->width-1;}
	else
		{fwidth_out=max_outbar_width-1;fwidth_in=max_outbar_width*x_stride+overlap-1;}

	lwidth_out=out_width%max_outbar_width-1;
	lwidth_in=overlap+(feature_in->width-fwidth_in-1)%(max_outbar_width*x_stride)-1;

	//printf("in_width=%0d,out_width=%0d\r\n",feature_in->width,out_width);
	//printf("max_outbar_width=%0d,split_num=%0d\r\n",max_outbar_width,split_num);
	//printf("fwidth_in=%0d,lwidth_in=%0d\r\n",fwidth_in,lwidth_in);
	//printf("fwidth_out=%0d,lwidth_out=%0d\r\n",fwidth_out,lwidth_out);
	//printf("in_line_stride=%d,in_surface_stride=%d\n",feature_in->line_stride,feature_in->surface_stride);
	//printf("out_line_stride=%d,out_surface_stride=%d\n",feature_out->line_stride,feature_out->surface_stride);

	#ifdef GET_PERFORMANCE
		CSB_Write(PDP_REG_BIAS+25,0);
	#endif

	CSB_Write(PDP_REG_BIAS+2,pooling_method);//pooling_method
	CSB_Write(PDP_REG_BIAS+3,(uint64_t)(feature_in->payload));//feature_in_base_addr
	CSB_Write(PDP_REG_BIAS+4,feature_in->surface_stride);//feature_in_surface_stride_in
	CSB_Write(PDP_REG_BIAS+5,feature_in->line_stride);//feature_in_line_stride_in
	CSB_Write(PDP_REG_BIAS+6,(uint64_t)(feature_out->payload));//feature_out_base_addr
	CSB_Write(PDP_REG_BIAS+7,feature_out->surface_stride);//feature_out_surface_stride_in
	CSB_Write(PDP_REG_BIAS+8,feature_out->line_stride);//feature_out_line_stride_in
	CSB_Write(PDP_REG_BIAS+9,(feature_in->channel+Tk-1)/Tk);//ch_in_div_Tk
	CSB_Write(PDP_REG_BIAS+10,feature_in->height);//h_in
	CSB_Write(PDP_REG_BIAS+11,out_height);//h_out
	CSB_Write(PDP_REG_BIAS+12,split_num);//split_num
	CSB_Write(PDP_REG_BIAS+13,fwidth_in);//fwidth_in
	CSB_Write(PDP_REG_BIAS+14,max_outbar_width*x_stride+overlap-1);//mwidth_in
	CSB_Write(PDP_REG_BIAS+15,lwidth_in);//lwidth_in
	CSB_Write(PDP_REG_BIAS+16,fwidth_out);//fwidth_out
	CSB_Write(PDP_REG_BIAS+17,max_outbar_width-1);//mwidth_out
	CSB_Write(PDP_REG_BIAS+18,lwidth_out);//lwidth_out
	CSB_Write(PDP_REG_BIAS+19,kernel_width);//Kx
	CSB_Write(PDP_REG_BIAS+20,kernel_height);//Ky
	CSB_Write(PDP_REG_BIAS+21,x_stride);//x_stride
	CSB_Write(PDP_REG_BIAS+22,y_stride);//y_stride

	if(pooling_method==2)
	{
		CSB_Write(PDP_REG_BIAS+23,0x10000/kernel_width);//recip_Kx
		CSB_Write(PDP_REG_BIAS+24,0x10000/kernel_height);//recip_Ky
	}

	//Kick of the run
	CSB_Write(PDP_REG_BIAS+0,1);//pdp_start

	//Waiting for pdp_wdma done
	while(CSB_Read(PDP_REG_BIAS+1)!=1)
	{
		usleep(10);
//		xil_printf("Wait for PDP_wdma done\r\n");
	}

	#ifdef GET_PERFORMANCE
		printf("\npdp_performance_cnt_r: %d\n\n",CSB_Read(PDP_REG_BIAS+25));
	#endif
}

void RunPool_soft(unsigned int kernel_width,unsigned int kernel_height,unsigned int x_stride,unsigned int y_stride,unsigned int pooling_method,
		unsigned int pad_left,unsigned int pad_right,unsigned int pad_up,unsigned int pad_down,
		struct Mapped_Feature *feature_in,struct Mapped_Feature *feature_out)
{
	for(int i=0;i<feature_out->channel;i++)
		for(int j=0;j<feature_out->height;j++)
			for(int k=0;k<feature_out->width;k++)
			
			{
				int tp;
				int axis_h=y_stride*j-pad_up;
				int axis_w=x_stride*k-pad_left;

				if(pooling_method==0)//mean
					tp=0;
				else
					if(pooling_method==1)//max
						tp=-32768;
					else//min
						tp=32768;

				for(int ky=0;ky<kernel_height;ky++)
					for(int kx=0;kx<kernel_width;kx++)
					{
						int ptr_h=axis_h+ky;
						int ptr_w=axis_w+kx;

						short dat;
						if(ptr_h>=0 && ptr_w>=0 && ptr_h<feature_in->height && ptr_w<feature_in->width)
							dat=*Get_Element(feature_in,ptr_h,ptr_w,i);
						else
							dat=0;

						if(pooling_method==0)//mean
							tp=tp+dat;
						else
							if(pooling_method==1)//max
							{
								if(dat>tp)
									tp=dat;
							}
							else//min
							{
								if(dat<tp)
									tp=dat;
							}
					}

				if(pooling_method==0)//mean
					*Get_Element(feature_out,j,k,i)=tp/((int)(kernel_height*kernel_width));
				else
					*Get_Element(feature_out,j,k,i)=tp;
			}
}
