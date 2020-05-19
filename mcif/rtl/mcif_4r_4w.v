`include "vpu_defines.vh"

module mcif_4r_4w #
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
	input wr_req_vld0,
	output wr_req_rdy0,
	input [`AXI_DATA_WIDTH+1-1:0]wr_req_pd0,
	output wr_rsp_complete0,
	//////////////////////////////////////////////////
	input wr_req_vld1,
	output wr_req_rdy1,
	input [`AXI_DATA_WIDTH+1-1:0]wr_req_pd1,
	output wr_rsp_complete1,
	//////////////////////////////////////////////////
	input wr_req_vld2,
	output wr_req_rdy2,
	input [`AXI_DATA_WIDTH+1-1:0]wr_req_pd2,
	output wr_rsp_complete2,
	//////////////////////////////////////////////////
	input wr_req_vld3,
	output wr_req_rdy3,
	input [`AXI_DATA_WIDTH+1-1:0]wr_req_pd3,
	output wr_rsp_complete3,
	//////////////////////////////////////////////////
	//AW channel
	output [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_AWID,
	output [32-1 : 0] M_AXI_AWADDR,
	output [`log2AXI_MAX_BURST_LEN-1 : 0] M_AXI_AWLEN,
	output [2 : 0] M_AXI_AWSIZE,//=clogb2((`AXI_DATA_WIDTH/8)-1);
	output [1 : 0] M_AXI_AWBURST,//=2'b01;
	output  M_AXI_AWLOCK,//1'b0;
	output [3 : 0] M_AXI_AWCACHE,//=4'b0010
	output [2 : 0] M_AXI_AWPROT,//=3'h0;
	output [3 : 0] M_AXI_AWQOS,//=4'h0;
	output  M_AXI_AWVALID,
	input  M_AXI_AWREADY,
	
	//Wr channel
	output [`AXI_DATA_WIDTH-1 : 0] M_AXI_WDATA,
	output [`AXI_DATA_WIDTH/8-1 : 0] M_AXI_WSTRB,
	output  M_AXI_WLAST,
	output  M_AXI_WVALID,
	input  M_AXI_WREADY,
	input [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_BID,//ignore
	input [1 : 0] M_AXI_BRESP,//ignore
	input  M_AXI_BVALID,//Bvalid and Bread means a a write response.
	output  M_AXI_BREADY,//Bvalid and Bread means a a write response.
	
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

mcif_rd_4r #
(
	.C_M_AXI_ID_WIDTH(C_M_AXI_ID_WIDTH)
)u_mcif_rd
(
	clk,
	rst_n,

	//////////////////////////////////////////////////
	rd_req_vld0,
	rd_req_rdy0,
	rd_req_pd0,

	rd_resp_vld0,
	rd_resp_rdy0,
	rd_resp_pd0,
	rd_fifo_pop0,
	//////////////////////////////////////////////////
	rd_req_vld1,
	rd_req_rdy1,
	rd_req_pd1,

	rd_resp_vld1,
	rd_resp_rdy1,
	rd_resp_pd1,
	rd_fifo_pop1,
	//////////////////////////////////////////////////
	rd_req_vld2,
	rd_req_rdy2,
	rd_req_pd2,

	rd_resp_vld2,
	rd_resp_rdy2,
	rd_resp_pd2,
	rd_fifo_pop2,
	//////////////////////////////////////////////////
	rd_req_vld3,
	rd_req_rdy3,
	rd_req_pd3,

	rd_resp_vld3,
	rd_resp_rdy3,
	rd_resp_pd3,
	rd_fifo_pop3,
	//////////////////////////////////////////////////
	//AR channel
	M_AXI_ARID,
	M_AXI_ARADDR,
	M_AXI_ARLEN,
	M_AXI_ARSIZE,//=clogb2((`AXI_DATA_WIDTH/8)-1);
	M_AXI_ARBURST,//=2'b01;
	M_AXI_ARLOCK,//=1'b0;
	M_AXI_ARCACHE,//=4'b0010;
	M_AXI_ARPROT,//=3'h0;
	M_AXI_ARQOS,//=4'h0;
	M_AXI_ARVALID,
	M_AXI_ARREADY,
	
	//Rd channel
	M_AXI_RID,
	M_AXI_RDATA,
	M_AXI_RRESP,//ignore
	M_AXI_RLAST,
	M_AXI_RVALID,
	M_AXI_RREADY
);

mcif_wr_4w #
(
	.C_M_AXI_ID_WIDTH(C_M_AXI_ID_WIDTH)
)u_mcif_wr
(
	clk,
	rst_n,

	//////////////////////////////////////////////////
	wr_req_vld0,
	wr_req_rdy0,
	wr_req_pd0,
	wr_rsp_complete0,
	//////////////////////////////////////////////////
	wr_req_vld1,
	wr_req_rdy1,
	wr_req_pd1,
	wr_rsp_complete1,
	//////////////////////////////////////////////////
	wr_req_vld2,
	wr_req_rdy2,
	wr_req_pd2,
	wr_rsp_complete2,
	//////////////////////////////////////////////////
	wr_req_vld3,
	wr_req_rdy3,
	wr_req_pd3,
	wr_rsp_complete3,
	//////////////////////////////////////////////////
	//AW channel
	M_AXI_AWID,
	M_AXI_AWADDR,
	M_AXI_AWLEN,
	M_AXI_AWSIZE,//=clogb2((`AXI_DATA_WIDTH/8)-1);
	M_AXI_AWBURST,//=2'b01;
	M_AXI_AWLOCK,//1'b0;
	M_AXI_AWCACHE,//=4'b0010
	M_AXI_AWPROT,//=3'h0;
	M_AXI_AWQOS,//=4'h0;
	M_AXI_AWVALID,
	M_AXI_AWREADY,
	
	//Wr channel
	M_AXI_WDATA,
	M_AXI_WSTRB,
	M_AXI_WLAST,
	M_AXI_WVALID,
	M_AXI_WREADY,
	M_AXI_BID,//ignore
	M_AXI_BRESP,//ignore
	M_AXI_BVALID,//Bvalid and Bread means a a write response.
	M_AXI_BREADY//Bvalid and Bread means a a write response.
);

endmodule

