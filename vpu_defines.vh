//`define FPGA
//`define PDP_LITE

/////////////////////Define The MAC Array
`define Tc 64					//Parallel factor of CH_in
`define log2Tc 6				//Depends

`define Tk 32					//Parallel factor of CH_out
`define log2Tk 5				//Depends
//`define TcEQTk  				//Depends

`define Tp 64					//Maxinum Stripe length
`define log2Tp 6				//Depends

////////////////////Define the CBUF
`define Logic_MEM_DEP 1024
`define log2Logic_MEM_DEP 10			//Depends
`define Logic_MEM_NUM 16
`define log2Logic_MEM_NUM 4			//Depends

`define CBUF_DAT_RD_LATENCY 2			//Do not Modify
`define CBUF_WT_RD_LATENCY `CBUF_DAT_RD_LATENCY
`define log2CBUF_DEP  (`log2Logic_MEM_DEP+`log2Logic_MEM_NUM)

///////////////////CMAC Pipeline DFF definition
`ifdef FPGA
    `define MAC_LATENCY ((`log2Tc+2)/3+1)  //Depends
`else
    `define BUF_MUL_OUT 1
    `define BUF_ADD_OUT "001001"		//from left to rigth, 1011 means the first, third, forth add out is buffered
    `define MAC_LATENCY 3 			//Depends
`endif

///////////////////CACC Pipeline Latency
`define CACC_ADDITION_DW 10
`define CACC_LATENCY 1

///////////////////Credit of the CSC->CACC backpressure
`define CSC_CREDIT_NUM	64			//The credit cnt reset value
`define log2CSC_CREDIT_NUM 6			//Depends

//////////////////SDP
`define SDP_A_RD_FIFO_DEP     128
`define SDP_B_RD_FIFO_DEP     128
`define LUT_ENTRIES           1024
`define log2LUT_ENTRIES       10                     //Depends
`define UPSAMPLE_BUF_DEP      32
`define log2UPSAMPLE_BUF_DEP   5                     //Depends

`define PDP_B_RD_FIFO_DEP 128
//The maxinum out bar width supported in PDP_core_2D_lite
`define MAX_OUT_BAR_WIDTH_PDP_LITE 16
//The size of the 2D shift chain array used in PDP_core_2D
`define PDP_CORE_2D_LINEBUFF_LEN 49
`define log2PDP_CORE_2D_LINEBUFF_LEN 6		//Depends
//The pipe length used in PDP_stream
`define PIPE_LEN_1D     64
`define log2PIPE_LEN_1D 6			//Depends
`define PIPE_LEN_2D     1024
`define log2PIPE_LEN_2D 10			//Depends

//CSR define
`define CSR_REG_NUM 512
`define log2CSR_REG_NUM 9			//Depends

//////////////////Define the MCIF
`define AXI_DATA_WIDTH (16*`Tk)			//Make sure that `AXI_DATA_WIDTH==16*`Tk
`define log2AXI_DATA_WIDTH (4+`log2Tk)		//Depends

`define AXI_MAX_BURST_LEN 64
`define log2AXI_MAX_BURST_LEN 6			//Depends

//`define ATOM_NUM_PER_ENTRY (`AXI_DATA_WIDTH/(16*`Tk))	
`define MAX_BURST_ATOM_CUBE `AXI_MAX_BURST_LEN 	//(`AXI_MAX_BURST_LEN*`ATOM_NUM_PER_ENTRY)
`define log2MAX_BURST_ATOM_CUBE `log2AXI_MAX_BURST_LEN	//(`log2AXI_MAX_BURST_LEN+`log2AXI_DATA_WIDTH-4-`log2Tk)

//MCIF Read Port Credit Number
`define MCIF_RD_PORT0_CREDIT_NUM 128
`define MCIF_RD_PORT1_CREDIT_NUM 128
`define MCIF_RD_PORT2_CREDIT_NUM `SDP_A_RD_FIFO_DEP
`define MCIF_RD_PORT3_CREDIT_NUM `SDP_B_RD_FIFO_DEP
`define MCIF_RD_PORT4_CREDIT_NUM `PDP_B_RD_FIFO_DEP
`define log2MCIF_CREDIT_NUM   7			//Depends
