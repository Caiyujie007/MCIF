#include "vpu_conv.h"

struct Conv_Cfg Get_Conv_Cfg(unsigned int Hin,unsigned int Win,unsigned int CHin,unsigned int CHout,
		unsigned int Kx,unsigned int Ky,unsigned int Sx,unsigned int Sy,
		unsigned int pad_left,unsigned int pad_right,unsigned int pad_up,unsigned int pad_down)
{
	struct Conv_Cfg ret;

	int overlap;
	int entries_per_line;
	int out_width,out_height;
	int dat_banks_restrict;//mininum required dat banks
	int wt_banks_restrict;//mininum required wt banks

	int dat_buf_num;
	int best_dat_banks=0;
	int mininum_bw;
	int best_method=0;

	mininum_bw=0;

	out_width=((Win+pad_left+pad_right-Kx)/Sx+1);
	out_height=((Hin+pad_up+pad_down-Ky)/Sy+1);

	overlap=Ky-Sy;
	entries_per_line=Win*((CHin+Tc-1)/Tc);

	dat_banks_restrict=(entries_per_line*Ky+Logic_MEM_DEP-1)/Logic_MEM_DEP;
	wt_banks_restrict=(Kx*Ky*Tk*((CHin+Tc-1)/Tc)+Logic_MEM_DEP-1)/Logic_MEM_DEP;

	if((dat_banks_restrict+wt_banks_restrict)>Logic_MEM_NUM)
	{
		printf("Error: CBUF entries not enough!!!\n");
		return ret;
	}

	for(dat_buf_num=dat_banks_restrict;dat_buf_num<Logic_MEM_NUM-wt_banks_restrict;dat_buf_num=dat_buf_num+1)
	{
		int K;
		int in_height_first,in_height_middle,in_height_last;
		int out_height_first,out_height_middle,out_height_last;

		int N;int wt_banks;
		int out_ch_slice,out_ch_slice_last;

		int total_bw_K_to_N;
		int total_bw_N_to_K;

		wt_banks=Logic_MEM_NUM-dat_buf_num;
		out_ch_slice=( (Logic_MEM_DEP*wt_banks)/(Kx*Ky*Tk*((CHin+Tc-1)/Tc)) ) *Tk;

		if(out_ch_slice>=CHout)
		{
			out_ch_slice=CHout;
			N=1;
		}
		else
		{
			N=(CHout+out_ch_slice-1)/out_ch_slice;
		}

		if(CHout%out_ch_slice==0)
			out_ch_slice_last=out_ch_slice;
		else
			out_ch_slice_last=CHout%out_ch_slice;

		out_height_first=((Logic_MEM_DEP*dat_buf_num)/entries_per_line+pad_up-Ky)/Sy+1;
		in_height_first=(out_height_first-1)*Sy+Ky-pad_up;

		out_height_middle=((Logic_MEM_DEP*dat_buf_num)/entries_per_line-Ky)/Sy+1;
		in_height_middle=(out_height_middle-1)*Sy+Ky;

		if(out_height_first>=out_height)
		{
			out_height_first=out_height;
			in_height_first=Hin;
		}

		if((out_height-out_height_first)%out_height_middle == 0)
		{
			K=(out_height-out_height_first)/out_height_middle+1;
			out_height_last=out_height_middle;
		}
		else
		{
			K=(out_height-out_height_first)/out_height_middle+2;
			out_height_last=(out_height-out_height_first)%out_height_middle;
		}
		in_height_last=Hin-in_height_first+overlap-(K-2)*(in_height_first-overlap);

		total_bw_K_to_N=(entries_per_line*Hin+entries_per_line*overlap*(K-1))*N+Kx*Ky*CHout*((CHin+Tc-1)/Tc);
		total_bw_N_to_K=K*Kx*Ky*CHout*((CHin+Tc-1)/Tc)+entries_per_line*Hin+entries_per_line*overlap*(K-1);

		if((mininum_bw==0)||(total_bw_K_to_N<mininum_bw))
		{
			best_dat_banks=dat_buf_num;
			mininum_bw=total_bw_K_to_N;
			best_method=0;
		}

		if((mininum_bw==0)||(total_bw_N_to_K<mininum_bw))
		{
			best_dat_banks=dat_buf_num;
			mininum_bw=total_bw_N_to_K;
			best_method=1;
		}
	}

	dat_buf_num=best_dat_banks;
	{
		int K;
		int in_height_first,in_height_middle,in_height_last;
		int out_height_first,out_height_middle,out_height_last;

		int N;int wt_banks;
		int out_ch_slice,out_ch_slice_last;

		wt_banks=Logic_MEM_NUM-dat_buf_num;
		out_ch_slice=( (Logic_MEM_DEP*wt_banks)/(Kx*Ky*Tk*((CHin+Tc-1)/Tc)) ) *Tk;

		if(out_ch_slice>=CHout)
		{
			out_ch_slice=CHout;
			N=1;
		}
		else
		{
			N=(CHout+out_ch_slice-1)/out_ch_slice;
		}

		if(CHout%out_ch_slice==0)
			out_ch_slice_last=out_ch_slice;
		else
			out_ch_slice_last=CHout%out_ch_slice;

		out_height_first=((Logic_MEM_DEP*dat_buf_num)/entries_per_line+pad_up-Ky)/Sy+1;
		in_height_first=(out_height_first-1)*Sy+Ky-pad_up;

		out_height_middle=((Logic_MEM_DEP*dat_buf_num)/entries_per_line-Ky)/Sy+1;
		in_height_middle=(out_height_middle-1)*Sy+Ky;

		if(out_height_first>=out_height)
		{
			out_height_first=out_height;
			in_height_first=Hin;
		}

		if((out_height-out_height_first)%out_height_middle == 0)
		{
			K=(out_height-out_height_first)/out_height_middle+1;
			out_height_last=out_height_middle;
		}
		else
		{
			K=(out_height-out_height_first)/out_height_middle+2;
			out_height_last=(out_height-out_height_first)%out_height_middle;
		}
		in_height_last=Hin-in_height_first+overlap-(K-2)*(in_height_first-overlap);

		ret.CHin=CHin;ret.Win=Win;ret.CHout=CHout;
		ret.overlap=overlap;ret.wt_size=(Tc*2*Kx*Ky*CHout*((CHin+Tc-1)/Tc));
		ret.Kx=Kx;ret.Ky=Ky;ret.Sx=Sx;ret.Sy=Sy;ret.pad_x=pad_left;ret.pad_y=pad_up;
		ret.K=K;ret.N=N;ret.dat_banks=best_dat_banks;ret.method=best_method;
		ret.entries_per_line=entries_per_line;ret.out_width=out_width;ret.out_height=out_height;
		ret.in_height_first=in_height_first;ret.in_height_middle=in_height_middle;ret.in_height_last=in_height_last;
		ret.out_height_first=out_height_first;ret.out_height_middle=out_height_middle;ret.out_height_last=out_height_last;
		ret.out_ch_slice=out_ch_slice;ret.out_ch_slice_last=out_ch_slice_last;
	}
	return ret;
}

void RunConv_Simplest(struct Conv_Cfg conv_cfg,unsigned int relu_en,short lut[LUT_ENTRIES],
		struct Mapped_Feature *feature_in,
		struct Mapped_Weight *wt,
		struct Mapped_Feature *feature_out)
{
	int use_lut;

	#ifdef GET_PERFORMANCE
		CSB_Write(27,0);
		CSB_Write(28,0);
		CSB_Write(29,0);
		CSB_Write(30,0);
	#endif

	if(lut==NULL)
	{
		use_lut=0;
	}
	else
	{
		use_lut=1;
		for(int i=0;i<LUT_ENTRIES;i++)
		{
			CSB_Write(SDP_REG_BIAS+97,i);//lut_addr
			CSB_Write(SDP_REG_BIAS+98,lut[i]);//lut_value
		}
	}

	if(conv_cfg.method==0)//K->N
	{
		for(int n=0;n<conv_cfg.N;n=n+1)
			for(int k=0;k<conv_cfg.K;k=k+1)
			{
				int in_height_single,out_height_single;
				int line_offset_in,line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_wt_reuse_single;//cdma_dat_reuse_single=0

				if(n!=conv_cfg.N-1)
					ch_out_single=conv_cfg.out_ch_slice;
				else
					ch_out_single=conv_cfg.out_ch_slice_last;

				if(k==0)
					{line_offset_in=0;line_offset_out=0;pad_y_single=conv_cfg.pad_y;cdma_wt_reuse_single=0;}
				else
					{line_offset_in=(conv_cfg.in_height_first-conv_cfg.overlap)+(k-1)*(conv_cfg.in_height_middle-conv_cfg.overlap);line_offset_out=conv_cfg.out_height_first+(k-1)*conv_cfg.out_height_middle;pad_y_single=0;cdma_wt_reuse_single=1;}

				if(k==0)
					{in_height_single=conv_cfg.in_height_first;out_height_single=conv_cfg.out_height_first;}
				else
					if(k==conv_cfg.K-1)
						{in_height_single=conv_cfg.in_height_last;out_height_single=conv_cfg.out_height_last;}
					else
						{in_height_single=conv_cfg.in_height_middle;out_height_single=conv_cfg.out_height_middle;}

				RunConv_Simplest_single_time(conv_cfg.CHin,in_height_single,conv_cfg.Win,ch_out_single,
							conv_cfg.Kx,conv_cfg.Ky,conv_cfg.Sx,conv_cfg.Sy,
							conv_cfg.pad_x,pad_y_single,
							relu_en,use_lut,
							((uint64_t)feature_in->payload)+feature_in->line_stride*line_offset_in,feature_in->surface_stride,feature_in->line_stride,feature_in->precision,
							((uint64_t)wt->payload)+conv_cfg.wt_size/conv_cfg.CHout*conv_cfg.out_ch_slice*n,conv_cfg.wt_size/conv_cfg.CHout*ch_out_single,wt->precision,
							((uint64_t)feature_out->payload)+feature_out->line_stride*line_offset_out+feature_out->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_out->surface_stride,feature_out->line_stride,feature_out->precision_for_conv_out_sft,
							conv_cfg.out_width,out_height_single,conv_cfg.dat_banks,0,cdma_wt_reuse_single);

			}
	}
	else//N->K
	{
		for(int k=0;k<conv_cfg.K;k=k+1)
			for(int n=0;n<conv_cfg.N;n=n+1)
			{
				int in_height_single,out_height_single;
				int line_offset_in,line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_dat_reuse_single;//cdma_wt_reuse_single=0

				if(n!=conv_cfg.N-1)
					ch_out_single=conv_cfg.out_ch_slice;
				else
					ch_out_single=conv_cfg.out_ch_slice_last;

				if(n==0)
					cdma_dat_reuse_single=0;
				else
					cdma_dat_reuse_single=1;

				if(k==0)
					{line_offset_in=0;line_offset_out=0;pad_y_single=conv_cfg.pad_y;}
				else
					{line_offset_in=(conv_cfg.in_height_first-conv_cfg.overlap)+(k-1)*(conv_cfg.in_height_middle-conv_cfg.overlap);line_offset_out=conv_cfg.out_height_first+(k-1)*conv_cfg.out_height_middle;pad_y_single=0;}


				if(k==0)
					{in_height_single=conv_cfg.in_height_first;out_height_single=conv_cfg.out_height_first;}
				else
					if(k==conv_cfg.K-1)
						{in_height_single=conv_cfg.in_height_last;out_height_single=conv_cfg.out_height_last;}
					else
						{in_height_single=conv_cfg.in_height_middle;out_height_single=conv_cfg.out_height_middle;}

				RunConv_Simplest_single_time(conv_cfg.CHin,in_height_single,conv_cfg.Win,ch_out_single,
							conv_cfg.Kx,conv_cfg.Ky,conv_cfg.Sx,conv_cfg.Sy,
							conv_cfg.pad_x,pad_y_single,
							relu_en,use_lut,
							(uint64_t)feature_in->payload+feature_in->line_stride*line_offset_in,feature_in->surface_stride,feature_in->line_stride,feature_in->precision,
							((uint64_t)wt->payload)+conv_cfg.wt_size/conv_cfg.CHout*conv_cfg.out_ch_slice*n,conv_cfg.wt_size/conv_cfg.CHout*ch_out_single,wt->precision,
							((uint64_t)feature_out->payload)+feature_out->line_stride*line_offset_out+feature_out->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_out->surface_stride,feature_out->line_stride,feature_out->precision_for_conv_out_sft,
							conv_cfg.out_width,out_height_single,conv_cfg.dat_banks,cdma_dat_reuse_single,0);
			}
	}

	#ifdef GET_PERFORMANCE
		printf("\ncdma_dat_performance_cnt_r: %d\n",CSB_Read(27));
		printf("cdma_wt_performance_cnt_r: %d\n",CSB_Read(28));
		printf("csc_dat_performance_cnt_r: %d\n",CSB_Read(29));
		printf("csc_wt_performance_cnt_r: %d\n\n",CSB_Read(30));
	#endif
}

void RunConv_With_Bias(struct Conv_Cfg conv_cfg,unsigned int relu_en,short lut[LUT_ENTRIES],
		struct Mapped_Feature *feature_in,
		struct Mapped_Weight *wt,
		struct Mapped_Feature *bias,
		struct Mapped_Feature *feature_out)
{
	int use_lut;
	#ifdef GET_PERFORMANCE
		CSB_Write(27,0);
		CSB_Write(28,0);
		CSB_Write(29,0);
		CSB_Write(30,0);
	#endif

	if(lut==NULL)
	{
		use_lut=0;
	}
	else
	{
		use_lut=1;
		for(int i=0;i<LUT_ENTRIES;i++)
		{
			CSB_Write(SDP_REG_BIAS+97,i);//lut_addr
			CSB_Write(SDP_REG_BIAS+98,lut[i]);//lut_value
		}
	}

	if(conv_cfg.method==0)//K->N
	{
		for(int n=0;n<conv_cfg.N;n=n+1)
			for(int k=0;k<conv_cfg.K;k=k+1)
			{
				int in_height_single,out_height_single;
				int line_offset_in,line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_wt_reuse_single;//cdma_dat_reuse_single=0

				if(n!=conv_cfg.N-1)
					ch_out_single=conv_cfg.out_ch_slice;
				else
					ch_out_single=conv_cfg.out_ch_slice_last;

				if(k==0)
					{line_offset_in=0;line_offset_out=0;pad_y_single=conv_cfg.pad_y;cdma_wt_reuse_single=0;}
				else
					{line_offset_in=(conv_cfg.in_height_first-conv_cfg.overlap)+(k-1)*(conv_cfg.in_height_middle-conv_cfg.overlap);line_offset_out=conv_cfg.out_height_first+(k-1)*conv_cfg.out_height_middle;pad_y_single=0;cdma_wt_reuse_single=1;}

				if(k==0)
					{in_height_single=conv_cfg.in_height_first;out_height_single=conv_cfg.out_height_first;}
				else
					if(k==conv_cfg.K-1)
						{in_height_single=conv_cfg.in_height_last;out_height_single=conv_cfg.out_height_last;}
					else
						{in_height_single=conv_cfg.in_height_middle;out_height_single=conv_cfg.out_height_middle;}

				RunConv_With_Bias_single_time(conv_cfg.CHin,in_height_single,conv_cfg.Win,ch_out_single,
							conv_cfg.Kx,conv_cfg.Ky,conv_cfg.Sx,conv_cfg.Sy,
							conv_cfg.pad_x,pad_y_single,
							relu_en,use_lut,
							((uint64_t)feature_in->payload)+feature_in->line_stride*line_offset_in,feature_in->surface_stride,feature_in->line_stride,feature_in->precision,
							((uint64_t)wt->payload)+conv_cfg.wt_size/conv_cfg.CHout*conv_cfg.out_ch_slice*n,conv_cfg.wt_size/conv_cfg.CHout*ch_out_single,wt->precision,
							((uint64_t)bias->payload)+bias->surface_stride*n*(conv_cfg.out_ch_slice/Tk),bias->precision,
							((uint64_t)feature_out->payload)+feature_out->line_stride*line_offset_out+feature_out->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_out->surface_stride,feature_out->line_stride,feature_out->precision_for_conv_out_sft,
							conv_cfg.out_width,out_height_single,conv_cfg.dat_banks,0,cdma_wt_reuse_single);

			}
	}
	else//N->K
	{
		for(int k=0;k<conv_cfg.K;k=k+1)
			for(int n=0;n<conv_cfg.N;n=n+1)
			{
				int in_height_single,out_height_single;
				int line_offset_in,line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_dat_reuse_single;//cdma_wt_reuse_single=0

				if(n!=conv_cfg.N-1)
					ch_out_single=conv_cfg.out_ch_slice;
				else
					ch_out_single=conv_cfg.out_ch_slice_last;

				if(n==0)
					cdma_dat_reuse_single=0;
				else
					cdma_dat_reuse_single=1;

				if(k==0)
					{line_offset_in=0;line_offset_out=0;pad_y_single=conv_cfg.pad_y;}
				else
					{line_offset_in=(conv_cfg.in_height_first-conv_cfg.overlap)+(k-1)*(conv_cfg.in_height_middle-conv_cfg.overlap);line_offset_out=conv_cfg.out_height_first+(k-1)*conv_cfg.out_height_middle;pad_y_single=0;}


				if(k==0)
					{in_height_single=conv_cfg.in_height_first;out_height_single=conv_cfg.out_height_first;}
				else
					if(k==conv_cfg.K-1)
						{in_height_single=conv_cfg.in_height_last;out_height_single=conv_cfg.out_height_last;}
					else
						{in_height_single=conv_cfg.in_height_middle;out_height_single=conv_cfg.out_height_middle;}

				RunConv_With_Bias_single_time(conv_cfg.CHin,in_height_single,conv_cfg.Win,ch_out_single,
							conv_cfg.Kx,conv_cfg.Ky,conv_cfg.Sx,conv_cfg.Sy,
							conv_cfg.pad_x,pad_y_single,
							relu_en,use_lut,
							((uint64_t)feature_in->payload)+feature_in->line_stride*line_offset_in,feature_in->surface_stride,feature_in->line_stride,feature_in->precision,
							((uint64_t)wt->payload)+conv_cfg.wt_size/conv_cfg.CHout*conv_cfg.out_ch_slice*n,conv_cfg.wt_size/conv_cfg.CHout*ch_out_single,wt->precision,
							((uint64_t)bias->payload)+bias->surface_stride*n*(conv_cfg.out_ch_slice/Tk),bias->precision,
							((uint64_t)feature_out->payload)+feature_out->line_stride*line_offset_out+feature_out->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_out->surface_stride,feature_out->line_stride,feature_out->precision_for_conv_out_sft,
							conv_cfg.out_width,out_height_single,conv_cfg.dat_banks,cdma_dat_reuse_single,0);
			}
	}

	#ifdef GET_PERFORMANCE
		printf("\ncdma_dat_performance_cnt_r: %d\n",CSB_Read(27));
		printf("cdma_wt_performance_cnt_r: %d\n",CSB_Read(28));
		printf("csc_dat_performance_cnt_r: %d\n",CSB_Read(29));
		printf("csc_wt_performance_cnt_r: %d\n\n",CSB_Read(30));
	#endif
}

void RunConv_With_Element_Wise(struct Conv_Cfg conv_cfg,unsigned int relu_en,short lut[LUT_ENTRIES],unsigned int element_wise_op,//0: add, 1: mul, 2: minus, 3: not defined
		struct Mapped_Feature *feature_in,
		struct Mapped_Weight *wt,
		struct Mapped_Feature *feature_second,
		struct Mapped_Feature *feature_out)
{
	int use_lut;
	#ifdef GET_PERFORMANCE
		CSB_Write(27,0);
		CSB_Write(28,0);
		CSB_Write(29,0);
		CSB_Write(30,0);
	#endif

	if(lut==NULL)
	{
		use_lut=0;
	}
	else
	{
		use_lut=1;
		for(int i=0;i<LUT_ENTRIES;i++)
		{
			CSB_Write(SDP_REG_BIAS+97,i);//lut_addr
			CSB_Write(SDP_REG_BIAS+98,lut[i]);//lut_value
		}
	}

	if(conv_cfg.method==0)//K->N
	{
		for(int n=0;n<conv_cfg.N;n=n+1)
			for(int k=0;k<conv_cfg.K;k=k+1)
			{
				int in_height_single,out_height_single;
				int line_offset_in,line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_wt_reuse_single;//cdma_dat_reuse_single=0

				if(n!=conv_cfg.N-1)
					ch_out_single=conv_cfg.out_ch_slice;
				else
					ch_out_single=conv_cfg.out_ch_slice_last;

				if(k==0)
					{line_offset_in=0;line_offset_out=0;pad_y_single=conv_cfg.pad_y;cdma_wt_reuse_single=0;}
				else
					{line_offset_in=(conv_cfg.in_height_first-conv_cfg.overlap)+(k-1)*(conv_cfg.in_height_middle-conv_cfg.overlap);line_offset_out=conv_cfg.out_height_first+(k-1)*conv_cfg.out_height_middle;pad_y_single=0;cdma_wt_reuse_single=1;}

				if(k==0)
					{in_height_single=conv_cfg.in_height_first;out_height_single=conv_cfg.out_height_first;}
				else
					if(k==conv_cfg.K-1)
						{in_height_single=conv_cfg.in_height_last;out_height_single=conv_cfg.out_height_last;}
					else
						{in_height_single=conv_cfg.in_height_middle;out_height_single=conv_cfg.out_height_middle;}

				RunConv_With_Element_Wise_single_time(conv_cfg.CHin,in_height_single,conv_cfg.Win,ch_out_single,
							conv_cfg.Kx,conv_cfg.Ky,conv_cfg.Sx,conv_cfg.Sy,
							conv_cfg.pad_x,pad_y_single,
							relu_en,use_lut,element_wise_op,
							((uint64_t)feature_in->payload)+feature_in->line_stride*line_offset_in,feature_in->surface_stride,feature_in->line_stride,feature_in->precision,
							((uint64_t)wt->payload)+conv_cfg.wt_size/conv_cfg.CHout*conv_cfg.out_ch_slice*n,conv_cfg.wt_size/conv_cfg.CHout*ch_out_single,wt->precision,
							((uint64_t)feature_second->payload)+feature_second->line_stride*line_offset_out+feature_second->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_second->surface_stride,feature_second->line_stride,feature_second->precision,
							((uint64_t)feature_out->payload)+feature_out->line_stride*line_offset_out+feature_out->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_out->surface_stride,feature_out->line_stride,feature_out->precision_for_conv_out_sft,
							conv_cfg.out_width,out_height_single,conv_cfg.dat_banks,0,cdma_wt_reuse_single);

			}
	}
	else//N->K
	{
		for(int k=0;k<conv_cfg.K;k=k+1)
			for(int n=0;n<conv_cfg.N;n=n+1)
			{
				int in_height_single,out_height_single;
				int line_offset_in,line_offset_out;
				int ch_out_single;
				int pad_y_single;
				int cdma_dat_reuse_single;//cdma_wt_reuse_single=0

				if(n!=conv_cfg.N-1)
					ch_out_single=conv_cfg.out_ch_slice;
				else
					ch_out_single=conv_cfg.out_ch_slice_last;

				if(n==0)
					cdma_dat_reuse_single=0;
				else
					cdma_dat_reuse_single=1;

				if(k==0)
					{line_offset_in=0;line_offset_out=0;pad_y_single=conv_cfg.pad_y;}
				else
					{line_offset_in=(conv_cfg.in_height_first-conv_cfg.overlap)+(k-1)*(conv_cfg.in_height_middle-conv_cfg.overlap);line_offset_out=conv_cfg.out_height_first+(k-1)*conv_cfg.out_height_middle;pad_y_single=0;}


				if(k==0)
					{in_height_single=conv_cfg.in_height_first;out_height_single=conv_cfg.out_height_first;}
				else
					if(k==conv_cfg.K-1)
						{in_height_single=conv_cfg.in_height_last;out_height_single=conv_cfg.out_height_last;}
					else
						{in_height_single=conv_cfg.in_height_middle;out_height_single=conv_cfg.out_height_middle;}

				RunConv_With_Element_Wise_single_time(conv_cfg.CHin,in_height_single,conv_cfg.Win,ch_out_single,
							conv_cfg.Kx,conv_cfg.Ky,conv_cfg.Sx,conv_cfg.Sy,
							conv_cfg.pad_x,pad_y_single,
							relu_en,use_lut,element_wise_op,
							((uint64_t)feature_in->payload)+feature_in->line_stride*line_offset_in,feature_in->surface_stride,feature_in->line_stride,feature_in->precision,
							((uint64_t)wt->payload)+conv_cfg.wt_size/conv_cfg.CHout*conv_cfg.out_ch_slice*n,conv_cfg.wt_size/conv_cfg.CHout*ch_out_single,wt->precision,
							((uint64_t)feature_second->payload)+feature_second->line_stride*line_offset_out+feature_second->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_second->surface_stride,feature_second->line_stride,feature_second->precision,
							((uint64_t)feature_out->payload)+feature_out->line_stride*line_offset_out+feature_out->surface_stride*n*(conv_cfg.out_ch_slice/Tk),feature_out->surface_stride,feature_out->line_stride,feature_out->precision_for_conv_out_sft,
							conv_cfg.out_width,out_height_single,conv_cfg.dat_banks,cdma_dat_reuse_single,0);
			}
	}
	#ifdef GET_PERFORMANCE
		printf("\ncdma_dat_performance_cnt_r: %d\n",CSB_Read(27));
		printf("cdma_wt_performance_cnt_r: %d\n",CSB_Read(28));
		printf("csc_dat_performance_cnt_r: %d\n",CSB_Read(29));
		printf("csc_wt_performance_cnt_r: %d\n\n",CSB_Read(30));
	#endif
}

void Config_Conv_Path(unsigned int CHin,unsigned int Hin,unsigned int Win,unsigned int CHout,
		unsigned int Kx,unsigned int Ky,unsigned int Sx,unsigned int Sy,
		unsigned int pad_x,unsigned int pad_y,
		unsigned int feature_in_base,unsigned int feature_in_surface_stride,unsigned int feature_in_line_stride,
		unsigned int wt_base_addr,unsigned int wt_size,
		unsigned int feature_out_base,unsigned int feature_out_surface_stride,unsigned int feature_out_line_stride,
		unsigned int out_width,unsigned int out_height,unsigned int dat_buf_num,unsigned int cdma_dat_reuse,unsigned int cdma_wt_reuse)
{
	//printf("Start Run Conv Single\n");
	//printf("h_in=%0d,h_out=%0d,ch_out=%0d\n",Hin,out_height,CHout);
	//printf("feature_in_base=%0x,feature_out_base=%0x\n",feature_in_base,feature_out_base);
	//printf("wt_base_addr=%0x,wt_size=%0d\n",wt_base_addr,wt_size);

	CSB_Write(3,dat_buf_num);
	CSB_Write(4,Win);
	CSB_Write(5,Hin);
	CSB_Write(6,Win*Hin);
	CSB_Write(7,(CHin+Tk-1)/Tk);
	CSB_Write(8,(CHin+Tc-1)/Tc);
	CSB_Write(9,pad_y);
	CSB_Write(10,pad_x);
	CSB_Write(11,Sx);
	CSB_Write(12,Sy);
	CSB_Write(13,Kx);
	CSB_Write(14,Ky);
	CSB_Write(15,out_width);
	CSB_Write(16,out_width*out_height);
	CSB_Write(17,((out_width*out_height+Tp-1)/Tp));
	CSB_Write(18,CHout);
	CSB_Write(19,((CHout+Tk-1)/Tk));
	CSB_Write(20,feature_in_base);
	CSB_Write(21,feature_in_surface_stride);
	CSB_Write(22,feature_in_line_stride);
	CSB_Write(23,wt_size);
	CSB_Write(24,wt_base_addr);
	CSB_Write(25,(cdma_wt_reuse<<1)|cdma_dat_reuse);
}

void RunConv_Simplest_single_time(unsigned int CHin,unsigned int Hin,unsigned int Win,unsigned int CHout,
		unsigned int Kx,unsigned int Ky,unsigned int Sx,unsigned int Sy,
		unsigned int pad_x,unsigned int pad_y,
		unsigned int relu_en,unsigned int use_lut,
		unsigned int feature_in_base,unsigned int feature_in_surface_stride,unsigned int feature_in_line_stride,unsigned int feature_in_precision,
		unsigned int wt_base_addr,unsigned int wt_size,unsigned int wt_precision,
		unsigned int feature_out_base,unsigned int feature_out_surface_stride,unsigned int feature_out_line_stride,unsigned int feature_out_precision,
		unsigned int out_width,unsigned int out_height,unsigned int dat_buf_num,unsigned int cdma_dat_reuse,unsigned int cdma_wt_reuse)
{
	Config_Conv_Path(CHin,Hin,Win,CHout,Kx,Ky,Sx,Sy,pad_x,pad_y,
			feature_in_base,feature_in_surface_stride,feature_in_line_stride,
			wt_base_addr,wt_size,
			feature_out_base,feature_out_surface_stride,feature_out_line_stride,
			out_width,out_height,dat_buf_num,cdma_dat_reuse,cdma_wt_reuse);

	CSB_Write(26,feature_in_precision+wt_precision-feature_out_precision);//cfg_sft_cacc_out

	//cfg sdp_core_per_ch
	CSB_Write(SDP_REG_BIAS+32,0b000000010);
	//CSB_Write(SDP_REG_BIAS+33,feature_in_precision+wt_precision-feature_out_precision);

	//cfg sdp_core_element_wise
	CSB_Write(SDP_REG_BIAS+64,0b00);//cfg_sel1_ew, cfg_sel0_ew_r

	//cfg sdp_core_nonlinear
	if(use_lut)
		CSB_Write(SDP_REG_BIAS+96,(relu_en<<2)+3);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear
	else
		CSB_Write(SDP_REG_BIAS+96,relu_en<<2);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear

	//cfg sdp_wdma
	CSB_Write(SDP_REG_BIAS+130,out_width);
	CSB_Write(SDP_REG_BIAS+131,out_height);
	CSB_Write(SDP_REG_BIAS+132,(CHout+Tk-1)/Tk);
	CSB_Write(SDP_REG_BIAS+133,feature_out_base);
	CSB_Write(SDP_REG_BIAS+134,feature_out_surface_stride);
	CSB_Write(SDP_REG_BIAS+135,feature_out_line_stride);

	//time=Timer_GetCnt();
	//printf("Config_Reg time=%f us\n",time/100.0);
	//Timer_ResetCnt();

	//Kick off the run
	CSB_Write(SDP_REG_BIAS+128,1);//sdp_wdma_start
	CSB_Write(0,0xF);//Conv Path

	//Wait for SDP_WDMA Done
	while(CSB_Read(SDP_REG_BIAS+129)!=1)
	{
		//usleep(10);
		//xil_printf("Wait for SPD_wdma done\r\n");
	}
}

void RunConv_With_Bias_single_time(unsigned int CHin,unsigned int Hin,unsigned int Win,unsigned int CHout,
		unsigned int Kx,unsigned int Ky,unsigned int Sx,unsigned int Sy,
		unsigned int pad_x,unsigned int pad_y,
		unsigned int relu_en,unsigned int use_lut,
		unsigned int feature_in_base,unsigned int feature_in_surface_stride,unsigned int feature_in_line_stride,unsigned int feature_in_precision,
		unsigned int wt_base_addr,unsigned int wt_size,unsigned int wt_precision,
		unsigned int bias_base,unsigned int bias_precision,
		unsigned int feature_out_base,unsigned int feature_out_surface_stride,unsigned int feature_out_line_stride,unsigned int feature_out_precision,
		unsigned int out_width,unsigned int out_height,unsigned int dat_buf_num,unsigned int cdma_dat_reuse,unsigned int cdma_wt_reuse)
{

	Config_Conv_Path(CHin,Hin,Win,CHout,Kx,Ky,Sx,Sy,pad_x,pad_y,
			feature_in_base,feature_in_surface_stride,feature_in_line_stride,
			wt_base_addr,wt_size,
			feature_out_base,feature_out_surface_stride,feature_out_line_stride,
			out_width,out_height,dat_buf_num,cdma_dat_reuse,cdma_wt_reuse);

	CSB_Write(26,feature_in_precision+wt_precision-feature_out_precision);//cfg_sft_cacc_out

	//cfg sdp_rdma_B
	CSB_Write(SDP_REG_BIAS+18,1);//sdp_rdma_B_h_in
	CSB_Write(SDP_REG_BIAS+19,1);//sdp_rdma_B_w_in
	CSB_Write(SDP_REG_BIAS+20,(CHout+Tk-1)/Tk);//sdp_rdma_B_ch_in_div_Tk
	CSB_Write(SDP_REG_BIAS+21,bias_base);//sdp_rdma_B_feature_in_base_addr
	CSB_Write(SDP_REG_BIAS+22,Tk*2);//sdp_rdma_B_feature_in_surface_stride
	CSB_Write(SDP_REG_BIAS+23,Tk*2);//sdp_rdma_B_feature_in_line_stride

	//cfg sdp_core_per_ch
	CSB_Write(SDP_REG_BIAS+32,0b010001110);
	//CSB_Write(SDP_REG_BIAS+33,feature_in_precision+wt_precision-feature_out_precision);//cfg_sft_0_ch_op
	CSB_Write(SDP_REG_BIAS+34,bias_precision);//cfg_sft_1_ch_op
	CSB_Write(SDP_REG_BIAS+35,feature_out_precision);//cfg_sft_2_ch_op
	CSB_Write(SDP_REG_BIAS+36,bias_precision);//cfg_sft_3_ch_op
	CSB_Write(SDP_REG_BIAS+38,out_width*out_height);//pixel_ch_op

	//cfg sdp_core_element_wise
	CSB_Write(SDP_REG_BIAS+64,0b00);//cfg_sel1_ew, cfg_sel0_ew_r

	//cfg sdp_core_nonlinear
	if(use_lut)
		CSB_Write(SDP_REG_BIAS+96,(relu_en<<2)+3);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear
	else
		CSB_Write(SDP_REG_BIAS+96,relu_en<<2);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear

	//cfg sdp_wdma
	CSB_Write(SDP_REG_BIAS+130,out_width);
	CSB_Write(SDP_REG_BIAS+131,out_height);
	CSB_Write(SDP_REG_BIAS+132,(CHout+Tk-1)/Tk);
	CSB_Write(SDP_REG_BIAS+133,feature_out_base);
	CSB_Write(SDP_REG_BIAS+134,feature_out_surface_stride);
	CSB_Write(SDP_REG_BIAS+135,feature_out_line_stride);

	//Kick off the run
	CSB_Write(SDP_REG_BIAS+128,1);//sdp_wdma_start
	CSB_Write(SDP_REG_BIAS+16,1);//sdp_rdma_B_start
	CSB_Write(0,0xF);//Conv Path

	//Wait for SDP_WDMA Done
	while(CSB_Read(SDP_REG_BIAS+129)!=1)
	{
		//usleep(10);
		//xil_printf("Wait for SPD_wdma done\r\n");
	}
}

void RunConv_With_Element_Wise_single_time(unsigned int CHin,unsigned int Hin,unsigned int Win,unsigned int CHout,
		unsigned int Kx,unsigned int Ky,unsigned int Sx,unsigned int Sy,
		unsigned int pad_x,unsigned int pad_y,
		unsigned int relu_en,unsigned int use_lut,unsigned int element_wise_op,
		unsigned int feature_in_base,unsigned int feature_in_surface_stride,unsigned int feature_in_line_stride,unsigned int feature_in_precision,
		unsigned int wt_base_addr,unsigned int wt_size,unsigned int wt_precision,
		unsigned int second_tensor_base,unsigned int second_tensor_surface_stride,unsigned int second_tensor_line_stride,unsigned int second_tensor_precision,
		unsigned int feature_out_base,unsigned int feature_out_surface_stride,unsigned int feature_out_line_stride,unsigned int feature_out_precision,
		unsigned int out_width,unsigned int out_height,unsigned int dat_buf_num,unsigned int cdma_dat_reuse,unsigned int cdma_wt_reuse)
{

	Config_Conv_Path(CHin,Hin,Win,CHout,Kx,Ky,Sx,Sy,pad_x,pad_y,
			feature_in_base,feature_in_surface_stride,feature_in_line_stride,
			wt_base_addr,wt_size,
			feature_out_base,feature_out_surface_stride,feature_out_line_stride,
			out_width,out_height,dat_buf_num,cdma_dat_reuse,cdma_wt_reuse);

	CSB_Write(26,feature_in_precision+wt_precision-feature_out_precision);//cfg_sft_cacc_out

	//cfg sdp_rdma_B
	CSB_Write(SDP_REG_BIAS+18,out_height);//sdp_rdma_B_h_in
	CSB_Write(SDP_REG_BIAS+19,out_width);//sdp_rdma_B_w_in
	CSB_Write(SDP_REG_BIAS+20,(CHout+Tk-1)/Tk);//sdp_rdma_B_ch_in_div_Tk
	CSB_Write(SDP_REG_BIAS+21,second_tensor_base);//sdp_rdma_B_feature_in_base_addr
	CSB_Write(SDP_REG_BIAS+22,second_tensor_surface_stride);//sdp_rdma_B_feature_in_surface_stride
	CSB_Write(SDP_REG_BIAS+23,second_tensor_line_stride);//sdp_rdma_B_feature_in_line_stride

	//cfg sdp_core_per_ch
	CSB_Write(SDP_REG_BIAS+32,0b011000010);
	//CSB_Write(SDP_REG_BIAS+33,feature_in_precision+wt_precision-feature_out_precision);//cfg_sft_0_ch_op

	//cfg sdp_core_element_wise
	CSB_Write(SDP_REG_BIAS+64,0b11);//cfg_sel1_ew, cfg_sel0_ew_r
	CSB_Write(SDP_REG_BIAS+65,(element_wise_op==1)?0:second_tensor_precision);//cfg_sft_s1_ew
	CSB_Write(SDP_REG_BIAS+66,(element_wise_op==1)?0:feature_out_precision);//cfg_sft_s2_ew
	CSB_Write(SDP_REG_BIAS+67,second_tensor_precision);//cfg_sft_d_ew
	CSB_Write(SDP_REG_BIAS+68,element_wise_op);//0: add, 1: mul, 2: minus, 3: not defined

	//cfg sdp_core_nonlinear
	if(use_lut)
		CSB_Write(SDP_REG_BIAS+96,(relu_en<<2)+3);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear
	else
		CSB_Write(SDP_REG_BIAS+96,relu_en<<2);//cfg_relu_en, cfg_sel1_nonlinear, cfg_sel0_nonlinear

	//cfg sdp_wdma
	CSB_Write(SDP_REG_BIAS+130,out_width);
	CSB_Write(SDP_REG_BIAS+131,out_height);
	CSB_Write(SDP_REG_BIAS+132,(CHout+Tk-1)/Tk);
	CSB_Write(SDP_REG_BIAS+133,feature_out_base);
	CSB_Write(SDP_REG_BIAS+134,feature_out_surface_stride);
	CSB_Write(SDP_REG_BIAS+135,feature_out_line_stride);

	//Kick off the run
	CSB_Write(SDP_REG_BIAS+128,1);//sdp_wdma_start
	CSB_Write(SDP_REG_BIAS+16,1);//sdp_rdma_B_start
	CSB_Write(0,0xF);//Conv Path

	//Wait for SDP_WDMA Done
	while(CSB_Read(SDP_REG_BIAS+129)!=1)
	{
		//usleep(10);
		//xil_printf("Wait for SPD_wdma done\r\n");
	}
}

void RunConv_Simplest_Soft(
		struct Conv_Cfg conv_cfg,unsigned int relu_en,short lut[LUT_ENTRIES],
		struct Mapped_Feature *feature_in,
		struct Mapped_Weight *wt,
		struct Mapped_Feature *feature_out)
{
	unsigned int out_width;
	unsigned int out_height;

	unsigned int out_truncate;
	if(feature_in->precision+wt->precision>=feature_out->precision_for_conv_out_sft)
		out_truncate=feature_in->precision+wt->precision-feature_out->precision_for_conv_out_sft;
	else
	{
		printf("Error: feature_in_precision+W_precision should >= feature_out_precision!!!");
		return;
	}

	for(int i=0;i<feature_out->channel;i++)
	{
		for(int j=0;j<feature_out->height;j++)//
		{
			for(int k=0;k<feature_out->width;k++)//
			{
				int64_t tp1;
				int64_t tp2;
				short tp_sat;
				tp1=0;
				for(int kj=0;kj<conv_cfg.Ky;kj++)
					for(int ki=0;ki<conv_cfg.Kx;ki++)
					{
						for(int chi=0;chi<feature_in->channel;chi++)//
						{
							short data;
							int axis_h;
							int axis_w;
							axis_h=conv_cfg.Sy*j+kj-conv_cfg.pad_y;axis_w=conv_cfg.Sx*k+ki-conv_cfg.pad_x;
							if( (axis_h<0) || (axis_h>=feature_in->height) || (axis_w<0) || (axis_w>=feature_in->width) )//padding 0
								data=0;
							else
								data=*Get_Element(feature_in,axis_h,axis_w,chi);

							tp1=tp1 + data * (*Get_Weight(wt,kj,ki,chi,i));
						}
					}

				tp2=tp1;
				for(int sh_i=0;sh_i<out_truncate;sh_i++)
					tp2=tp2/2;

				if((out_truncate!=0) && (tp2!=0x7fffffff))
					tp2 = tp2 + ( (tp1&(1<<(out_truncate-1))) !=0);

				if(tp2<-32768)
					tp_sat=-32768;
				else
					if(tp2>32767)
						tp_sat=32767;
					else
						tp_sat=tp2;

				if(lut!=NULL)
					tp_sat=lut[((unsigned short)tp_sat)>>(16-log2LUT_ENTRIES)];

				if(relu_en && tp_sat<0)
					*Get_Element(feature_out,j,k,i)=0;
				else
					*Get_Element(feature_out,j,k,i)=tp_sat;
			}
		}
	}
}

void RunConv_With_Bias_Soft(
		struct Conv_Cfg conv_cfg,unsigned int relu_en,short lut[LUT_ENTRIES],
		struct Mapped_Feature *feature_in,
		struct Mapped_Weight *wt,struct Mapped_Feature *bias,
		struct Mapped_Feature *feature_out)
{
	unsigned int out_width;
	unsigned int out_height;

	unsigned int out_truncate;
	if(feature_in->precision+wt->precision>=feature_out->precision_for_conv_out_sft)
		out_truncate=feature_in->precision+wt->precision-feature_out->precision_for_conv_out_sft;
	else
	{
		printf("Error: feature_in_precision+W_precision should >= feature_out_precision!!!");
		return;
	}

	for(int i=0;i<feature_out->channel;i++)
	{
		for(int j=0;j<feature_out->height;j++)//feature_out->height
		{
			for(int k=0;k<feature_out->width;k++)//feature_out->width
			{
				int64_t tp1;
				int64_t tp2;
				short tp_sat;
				tp1=0;
				for(int kj=0;kj<conv_cfg.Ky;kj++)
					for(int ki=0;ki<conv_cfg.Kx;ki++)
					{
						for(int chi=0;chi<feature_in->channel;chi++)//
						{
							short data;
							int axis_h;
							int axis_w;
							axis_h=conv_cfg.Sy*j+kj-conv_cfg.pad_y;axis_w=conv_cfg.Sx*k+ki-conv_cfg.pad_x;
							if( (axis_h<0) || (axis_h>=feature_in->height) || (axis_w<0) || (axis_w>=feature_in->width) )//padding 0
								data=0;
							else
								data=*Get_Element(feature_in,axis_h,axis_w,chi);

							tp1=tp1 + data * (*Get_Weight(wt,kj,ki,chi,i));
						}
					}

				tp2=tp1;
				for(int sh_i=0;sh_i<out_truncate;sh_i++)
					tp2=tp2/2;

				if((out_truncate!=0) && (tp2!=0x7fffffff))
					tp2 = tp2 + ( (tp1&(1<<(out_truncate-1))) !=0);

				if(tp2<-32768)
					tp_sat=-32768;
				else
					if(tp2>32767)
						tp_sat=32767;
					else
						tp_sat=tp2;

				int32_t main_dat;
				int32_t aux_dat;
				int32_t result_32;
				main_dat=tp_sat<<bias->precision;
				aux_dat=bias->payload[i]<<feature_out->precision_for_conv_out_sft;

				result_32=main_dat+aux_dat;

				for(int sh_i=0;sh_i<bias->precision;sh_i++)
					result_32=result_32/2;

				short tp_sat2;
				if(result_32<-32768)
					tp_sat2=-32768;
				else
					if(result_32>32767)
						tp_sat2=32767;
					else
						tp_sat2=result_32;

				if(lut!=NULL)
					tp_sat2=lut[((unsigned short)tp_sat2)>>(16-log2LUT_ENTRIES)];

				if(relu_en && tp_sat2<0)
					*Get_Element(feature_out,j,k,i)=0;
				else
					*Get_Element(feature_out,j,k,i)=tp_sat2;
			}
		}
	}
}

