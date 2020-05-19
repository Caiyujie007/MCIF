`include "vpu_defines.vh"

module mcif_rd_4r #
(
	parameter C_M_AXI_ID_WIDTH=2
)
(
	input clk,
	input rst_n,

	//////////////////////////////////////////////////
	input rd_req_vld0,
	output rd_req_rdy0,
	input [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd0,

	output rd_resp_vld0,
	input rd_resp_rdy0,
	output [`AXI_DATA_WIDTH-1:0]rd_resp_pd0,
	input rd_fifo_pop0,
	//////////////////////////////////////////////////
	input rd_req_vld1,
	output rd_req_rdy1,
	input [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd1,

	output rd_resp_vld1,
	input rd_resp_rdy1,
	output [`AXI_DATA_WIDTH-1:0]rd_resp_pd1,
	input rd_fifo_pop1,
	//////////////////////////////////////////////////
	input rd_req_vld2,
	output rd_req_rdy2,
	input [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd2,

	output rd_resp_vld2,
	input rd_resp_rdy2,
	output [`AXI_DATA_WIDTH-1:0]rd_resp_pd2,
	input rd_fifo_pop2,
	//////////////////////////////////////////////////
	input rd_req_vld3,
	output rd_req_rdy3,
	input [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd3,

	output rd_resp_vld3,
	input rd_resp_rdy3,
	output [`AXI_DATA_WIDTH-1:0]rd_resp_pd3,
	input rd_fifo_pop3,
	//////////////////////////////////////////////////
	//AR channel
	output [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_ARID,
	output [32-1 : 0] M_AXI_ARADDR,
	output [`log2AXI_MAX_BURST_LEN-1 : 0] M_AXI_ARLEN,
	output [2 : 0] M_AXI_ARSIZE,//=clogb2((`AXI_DATA_WIDTH/8)-1);
	output [1 : 0] M_AXI_ARBURST,//=2'b01;
	output  M_AXI_ARLOCK,//=1'b0;
	output [3 : 0] M_AXI_ARCACHE,//=4'b0010;
	output [2 : 0] M_AXI_ARPROT,//=3'h0;
	output [3 : 0] M_AXI_ARQOS,//=4'h0;
	output  M_AXI_ARVALID,
	input  M_AXI_ARREADY,
	
	//Rd channel
	input [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_RID,
	input [`AXI_DATA_WIDTH-1 : 0] M_AXI_RDATA,
	input [1 : 0] M_AXI_RRESP,//ignore
	input  M_AXI_RLAST,
	input  M_AXI_RVALID,
	output  M_AXI_RREADY
);

function integer clogb2 (input integer bit_depth);              
begin                                                           
for(clogb2=0; bit_depth>0; clogb2=clogb2+1)                   
  bit_depth = bit_depth >> 1;                                 
end                                                           
endfunction

reg [`log2MCIF_CREDIT_NUM:0]port0_rd_cdt_cnt;
reg [`log2MCIF_CREDIT_NUM:0]port1_rd_cdt_cnt;
reg [`log2MCIF_CREDIT_NUM:0]port2_rd_cdt_cnt;
reg [`log2MCIF_CREDIT_NUM:0]port3_rd_cdt_cnt;

wire rd_cmd_fifo_0_dout_vld;
wire [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_cmd_fifo_0_dout;
wire rd_cmd_fifo_0_dout_rdy;

wire rd_cmd_fifo_1_dout_vld;
wire [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_cmd_fifo_1_dout;
wire rd_cmd_fifo_1_dout_rdy;

wire rd_cmd_fifo_2_dout_vld;
wire [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_cmd_fifo_2_dout;
wire rd_cmd_fifo_2_dout_rdy;

wire rd_cmd_fifo_3_dout_vld;
wire [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_cmd_fifo_3_dout;
wire rd_cmd_fifo_3_dout_rdy;

wire rd_arb_req_0=rd_cmd_fifo_0_dout_vld & (rd_cmd_fifo_0_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]<port0_rd_cdt_cnt);
wire rd_arb_req_1=rd_cmd_fifo_1_dout_vld & (rd_cmd_fifo_1_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]<port1_rd_cdt_cnt);
wire rd_arb_req_2=rd_cmd_fifo_2_dout_vld & (rd_cmd_fifo_2_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]<port2_rd_cdt_cnt);
wire rd_arb_req_3=rd_cmd_fifo_3_dout_vld & (rd_cmd_fifo_3_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]<port3_rd_cdt_cnt);

wire rd_arb_gnt_0;
wire rd_arb_gnt_1;
wire rd_arb_gnt_2;
wire rd_arb_gnt_3;

mcif_rr_arb4 u_rd_rr_arb4
(
	.clk(clk),
	.rst_n(rst_n),

	.gnt_valid(M_AXI_ARVALID),
	.gnt_ready(M_AXI_ARREADY),

	.arb_req({rd_arb_req_3,rd_arb_req_2,rd_arb_req_1,rd_arb_req_0}),
	.arb_gnt({rd_arb_gnt_3,rd_arb_gnt_2,rd_arb_gnt_1,rd_arb_gnt_0})
);

hs_pipe #
(
	.DATA_WIDTH(`log2MAX_BURST_ATOM_CUBE+32),
	.PIPE_DEPTH(4)
)u_rd_cmd_fifo_0
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(rd_req_vld0),
	.data_in(rd_req_pd0),
	.data_in_rdy(rd_req_rdy0),

	.data_out_vld(rd_cmd_fifo_0_dout_vld),
	.data_out(rd_cmd_fifo_0_dout),
	.data_out_rdy(rd_cmd_fifo_0_dout_rdy)
);

hs_pipe #
(
	.DATA_WIDTH(`log2MAX_BURST_ATOM_CUBE+32),
	.PIPE_DEPTH(4)
)u_rd_cmd_fifo_1
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(rd_req_vld1),
	.data_in(rd_req_pd1),
	.data_in_rdy(rd_req_rdy1),

	.data_out_vld(rd_cmd_fifo_1_dout_vld),
	.data_out(rd_cmd_fifo_1_dout),
	.data_out_rdy(rd_cmd_fifo_1_dout_rdy)
);

hs_pipe #
(
	.DATA_WIDTH(`log2MAX_BURST_ATOM_CUBE+32),
	.PIPE_DEPTH(4)
)u_rd_cmd_fifo_2
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(rd_req_vld2),
	.data_in(rd_req_pd2),
	.data_in_rdy(rd_req_rdy2),

	.data_out_vld(rd_cmd_fifo_2_dout_vld),
	.data_out(rd_cmd_fifo_2_dout),
	.data_out_rdy(rd_cmd_fifo_2_dout_rdy)
);

hs_pipe #
(
	.DATA_WIDTH(`log2MAX_BURST_ATOM_CUBE+32),
	.PIPE_DEPTH(4)
)u_rd_cmd_fifo_3
(
	.clk(clk),
	.rst_n(rst_n),

	.data_in_vld(rd_req_vld3),
	.data_in(rd_req_pd3),
	.data_in_rdy(rd_req_rdy3),

	.data_out_vld(rd_cmd_fifo_3_dout_vld),
	.data_out(rd_cmd_fifo_3_dout),
	.data_out_rdy(rd_cmd_fifo_3_dout_rdy)
);

always @(posedge clk or negedge rst_n)
if(~rst_n)
	port0_rd_cdt_cnt<=`MCIF_RD_PORT0_CREDIT_NUM;
else
	begin
		case({rd_fifo_pop0,(rd_cmd_fifo_0_dout_vld&rd_cmd_fifo_0_dout_rdy)})
			2'b01:port0_rd_cdt_cnt<=port0_rd_cdt_cnt-rd_cmd_fifo_0_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]-1;
			2'b10:if(port0_rd_cdt_cnt!=`MCIF_RD_PORT0_CREDIT_NUM) port0_rd_cdt_cnt<=port0_rd_cdt_cnt+1;
			2'b11:port0_rd_cdt_cnt<=port0_rd_cdt_cnt-rd_cmd_fifo_0_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32];
		endcase
	end

always @(posedge clk or negedge rst_n)
if(~rst_n)
	port1_rd_cdt_cnt<=`MCIF_RD_PORT1_CREDIT_NUM;
else
	begin
		case({rd_fifo_pop1,(rd_cmd_fifo_1_dout_vld&rd_cmd_fifo_1_dout_rdy)})
			2'b01:port1_rd_cdt_cnt<=port1_rd_cdt_cnt-rd_cmd_fifo_1_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]-1;
			2'b10:if(port1_rd_cdt_cnt!=`MCIF_RD_PORT1_CREDIT_NUM) port1_rd_cdt_cnt<=port1_rd_cdt_cnt+1;
			2'b11:port1_rd_cdt_cnt<=port1_rd_cdt_cnt-rd_cmd_fifo_1_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32];
		endcase
	end

always @(posedge clk or negedge rst_n)
if(~rst_n)
	port2_rd_cdt_cnt<=`MCIF_RD_PORT2_CREDIT_NUM;
else
	begin
		case({rd_fifo_pop2,(rd_cmd_fifo_2_dout_vld&rd_cmd_fifo_2_dout_rdy)})
			2'b01:port2_rd_cdt_cnt<=port2_rd_cdt_cnt-rd_cmd_fifo_2_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]-1;
			2'b10:if(port2_rd_cdt_cnt!=`MCIF_RD_PORT2_CREDIT_NUM) port2_rd_cdt_cnt<=port2_rd_cdt_cnt+1;
			2'b11:port2_rd_cdt_cnt<=port2_rd_cdt_cnt-rd_cmd_fifo_2_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32];
		endcase
	end

always @(posedge clk or negedge rst_n)
if(~rst_n)
	port3_rd_cdt_cnt<=`MCIF_RD_PORT3_CREDIT_NUM;
else
	begin
		case({rd_fifo_pop3,(rd_cmd_fifo_3_dout_vld&rd_cmd_fifo_3_dout_rdy)})
			2'b01:port3_rd_cdt_cnt<=port3_rd_cdt_cnt-rd_cmd_fifo_3_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]-1;
			2'b10:if(port3_rd_cdt_cnt!=`MCIF_RD_PORT3_CREDIT_NUM) port3_rd_cdt_cnt<=port3_rd_cdt_cnt+1;
			2'b11:port3_rd_cdt_cnt<=port3_rd_cdt_cnt-rd_cmd_fifo_3_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32];
		endcase
	end

wire [C_M_AXI_ID_WIDTH-1:0]rid_0=rd_arb_gnt_0?0:0;
wire [C_M_AXI_ID_WIDTH-1:0]rid_1=rd_arb_gnt_1?1:0;
wire [C_M_AXI_ID_WIDTH-1:0]rid_2=rd_arb_gnt_2?2:0;
wire [C_M_AXI_ID_WIDTH-1:0]rid_3=rd_arb_gnt_3?3:0;

wire [`log2AXI_MAX_BURST_LEN-1:0]rlen_0=rd_arb_gnt_0?rd_cmd_fifo_0_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]:0;
wire [`log2AXI_MAX_BURST_LEN-1:0]rlen_1=rd_arb_gnt_1?rd_cmd_fifo_1_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]:0;
wire [`log2AXI_MAX_BURST_LEN-1:0]rlen_2=rd_arb_gnt_2?rd_cmd_fifo_2_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]:0;
wire [`log2AXI_MAX_BURST_LEN-1:0]rlen_3=rd_arb_gnt_3?rd_cmd_fifo_3_dout[`log2MAX_BURST_ATOM_CUBE+32-1:32]:0;

wire [31:0]araddr_0=rd_arb_gnt_0?rd_cmd_fifo_0_dout[31:0]:32'd0;
wire [31:0]araddr_1=rd_arb_gnt_1?rd_cmd_fifo_1_dout[31:0]:32'd0;
wire [31:0]araddr_2=rd_arb_gnt_2?rd_cmd_fifo_2_dout[31:0]:32'd0;
wire [31:0]araddr_3=rd_arb_gnt_3?rd_cmd_fifo_3_dout[31:0]:32'd0;

assign rd_cmd_fifo_0_dout_rdy=rd_arb_gnt_0&M_AXI_ARREADY;
assign rd_cmd_fifo_1_dout_rdy=rd_arb_gnt_1&M_AXI_ARREADY;
assign rd_cmd_fifo_2_dout_rdy=rd_arb_gnt_2&M_AXI_ARREADY;
assign rd_cmd_fifo_3_dout_rdy=rd_arb_gnt_3&M_AXI_ARREADY;

assign rd_resp_vld0=(M_AXI_RID==0)&M_AXI_RVALID;
assign rd_resp_vld1=(M_AXI_RID==1)&M_AXI_RVALID;
assign rd_resp_vld2=(M_AXI_RID==2)&M_AXI_RVALID;
assign rd_resp_vld3=(M_AXI_RID==3)&M_AXI_RVALID;

assign rd_resp_pd0=M_AXI_RDATA;
assign rd_resp_pd1=M_AXI_RDATA;
assign rd_resp_pd2=M_AXI_RDATA;
assign rd_resp_pd3=M_AXI_RDATA;

assign M_AXI_ARVALID=rd_arb_gnt_0|rd_arb_gnt_1|rd_arb_gnt_2|rd_arb_gnt_3;
assign M_AXI_ARID=rid_0|rid_1|rid_2|rid_3;
assign M_AXI_ARLEN=rlen_0|rlen_1|rlen_2|rlen_3;
assign M_AXI_ARADDR=araddr_0|araddr_1|araddr_2|araddr_3;

assign M_AXI_RREADY = 	((M_AXI_RID==0)?rd_resp_rdy0:1'b0) |
			((M_AXI_RID==1)?rd_resp_rdy1:1'b0) |
			((M_AXI_RID==2)?rd_resp_rdy2:1'b0) |
			((M_AXI_RID==3)?rd_resp_rdy3:1'b0);

assign M_AXI_ARSIZE=clogb2((`AXI_DATA_WIDTH/8)-1);
assign M_AXI_ARBURST=2'b01; 
assign M_AXI_ARLOCK=1'b0;
assign M_AXI_ARCACHE=4'b0010;
assign M_AXI_ARPROT=3'h0;
assign M_AXI_ARQOS=4'h0;

endmodule

