`include "vpu_defines.vh"

module testbench;

parameter integer C_M_AXI_ID_WIDTH	= 3;

bit clk;
bit rst_n;

//////////////////////////////////////////////////
bit rd_req_vld0;
wire rd_req_rdy0;
bit [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd0;

wire rd_resp_vld0;
bit rd_resp_rdy0;
wire [`AXI_DATA_WIDTH-1:0]rd_resp_pd0;
bit rd_fifo_pop0;
//////////////////////////////////////////////////
bit rd_req_vld1;
wire rd_req_rdy1;
bit [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd1;

wire rd_resp_vld1;
bit rd_resp_rdy1;
wire [`AXI_DATA_WIDTH-1:0]rd_resp_pd1;
bit rd_fifo_pop1;
//////////////////////////////////////////////////
bit rd_req_vld2;
wire rd_req_rdy2;
bit [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd2;

wire rd_resp_vld2;
bit rd_resp_rdy2;
wire [`AXI_DATA_WIDTH-1:0]rd_resp_pd2;
bit rd_fifo_pop2;
//////////////////////////////////////////////////
bit rd_req_vld3;
wire rd_req_rdy3;
bit [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd3;

wire rd_resp_vld3;
bit rd_resp_rdy3;
wire [`AXI_DATA_WIDTH-1:0]rd_resp_pd3;
bit rd_fifo_pop3;
//////////////////////////////////////////////////
bit rd_req_vld4;
wire rd_req_rdy4;
bit [`log2MAX_BURST_ATOM_CUBE+32-1:0]rd_req_pd4;

wire rd_resp_vld4;
bit rd_resp_rdy4;
wire [`AXI_DATA_WIDTH-1:0]rd_resp_pd4;
bit rd_fifo_pop4;
//////////////////////////////////////////////////
bit wr_req_vld0;
wire wr_req_rdy0;
bit [`AXI_DATA_WIDTH+1-1:0]wr_req_pd0;
wire wr_rsp_complete0;
//////////////////////////////////////////////////
bit wr_req_vld1;
wire wr_req_rdy1;
bit [`AXI_DATA_WIDTH+1-1:0]wr_req_pd1;
wire wr_rsp_complete1;
//////////////////////////////////////////////////
bit wr_req_vld2;
wire wr_req_rdy2;
bit [`AXI_DATA_WIDTH+1-1:0]wr_req_pd2;
wire wr_rsp_complete2;
//////////////////////////////////////////////////
bit wr_req_vld3;
wire wr_req_rdy3;
bit [`AXI_DATA_WIDTH+1-1:0]wr_req_pd3;
wire wr_rsp_complete3;
//////////////////////////////////////////////////
wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_AWID;
wire [32-1 : 0] M_AXI_AWADDR;
wire [`log2AXI_MAX_BURST_LEN : 0] M_AXI_AWLEN;
wire [2 : 0] M_AXI_AWSIZE;
wire [1 : 0] M_AXI_AWBURST;
wire  M_AXI_AWLOCK;
wire [3 : 0] M_AXI_AWCACHE;
wire [2 : 0] M_AXI_AWPROT;
wire [3 : 0] M_AXI_AWQOS;
wire M_AXI_AWVALID;
wire  M_AXI_AWREADY;
wire [`AXI_DATA_WIDTH-1 : 0] M_AXI_WDATA;
wire [`AXI_DATA_WIDTH/8-1 : 0] M_AXI_WSTRB;
wire  M_AXI_WLAST;
wire  M_AXI_WVALID;
wire  M_AXI_WREADY;
wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_BID;
wire [1 : 0] M_AXI_BRESP;
wire  M_AXI_BVALID;
wire  M_AXI_BREADY;
wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_ARID;
wire [32-1 : 0] M_AXI_ARADDR;
wire [`log2AXI_MAX_BURST_LEN : 0] M_AXI_ARLEN;
wire [2 : 0] M_AXI_ARSIZE;
wire [1 : 0] M_AXI_ARBURST;
wire  M_AXI_ARLOCK;
wire [3 : 0] M_AXI_ARCACHE;
wire [2 : 0] M_AXI_ARPROT;
wire [3 : 0] M_AXI_ARQOS;
wire  M_AXI_ARVALID;
wire  M_AXI_ARREADY;
wire [C_M_AXI_ID_WIDTH-1 : 0] M_AXI_RID;
wire [`AXI_DATA_WIDTH-1 : 0] M_AXI_RDATA;
wire [1 : 0] M_AXI_RRESP;
wire  M_AXI_RLAST;
wire  M_AXI_RVALID;
wire  M_AXI_RREADY;
//////////////////////////////////////////////////

`include "tasks.vh"

always #5 clk=~clk;

initial
begin
	automatic bit [`AXI_DATA_WIDTH-1:0]data_wr0[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	automatic bit [`AXI_DATA_WIDTH-1:0]data_wr1[]={16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
	automatic bit [`AXI_DATA_WIDTH-1:0]data_wr2[]={32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};
	automatic bit [`AXI_DATA_WIDTH-1:0]data_wr3[]={48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63};
	automatic bit [`AXI_DATA_WIDTH-1:0]data_rd0[]=new[16];
	automatic bit [`AXI_DATA_WIDTH-1:0]data_rd1[]=new[16];
	automatic bit [`AXI_DATA_WIDTH-1:0]data_rd2[]=new[16];
	automatic bit [`AXI_DATA_WIDTH-1:0]data_rd3[]=new[16];
	$vcdpluson(0,testbench);
	rst_n=1;
	#20 rst_n=0;
	#20 rst_n=1;
	repeat(10) @(posedge clk);
	
	fork
		CH0_wr(16-1,0,data_wr0,0);
		CH1_wr(16-1,32*100,data_wr1,0);
		CH2_wr(16-1,32*200,data_wr2,0);
		CH3_wr(16-1,32*300,data_wr3,0);
	join

	fork
		CH0_wr(16-1,32'h10000+0,data_wr0,1);
		CH1_wr(16-1,32'h10000+32*100,data_wr1,1);
		CH2_wr(16-1,32'h10000+32*200,data_wr2,1);
		CH3_wr(16-1,32'h10000+32*300,data_wr3,1);
	join

	fork
		CH0_wr(16-1,32'h20000+0,data_wr0,0);
		CH1_wr(16-1,32'h20000+32*100,data_wr1,0);
		CH2_wr(16-1,32'h20000+32*200,data_wr2,0);
		CH3_wr(16-1,32'h20000+32*300,data_wr3,0);
	join

	$display("////////////////////////////////////////////////");
	fork
		CH0_read(16-1,0,data_rd0);
		CH1_read(16-1,32*100,data_rd1);
		CH2_read(16-1,32*200,data_rd2);
		CH4_read(16-1,32*300,data_rd3);
	join
	for(int i=0;i<16;i++)
	begin
		$display("data_rd0[%0d]=%0d",i,data_rd0[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd1[%0d]=%0d",i,data_rd1[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd2[%0d]=%0d",i,data_rd2[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd3[%0d]=%0d",i,data_rd3[i]);
	end

	$display("////////////////////////////////////////////////");
	fork
		CH0_read(16-1,32'h10000+0,data_rd0);
		CH1_read(16-1,32'h10000+32*100,data_rd1);
		CH2_read(16-1,32'h10000+32*200,data_rd2);
		CH4_read(16-1,32'h10000+32*300,data_rd3);
	join

	for(int i=0;i<16;i++)
	begin
		$display("data_rd0[%0d]=%0d",i,data_rd0[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd1[%0d]=%0d",i,data_rd1[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd2[%0d]=%0d",i,data_rd2[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd3[%0d]=%0d",i,data_rd3[i]);
	end

	$display("////////////////////////////////////////////////");
	fork
		CH0_read(16-1,32'h20000+0,data_rd0);
		CH1_read(16-1,32'h20000+32*100,data_rd1);
		CH2_read(16-1,32'h20000+32*200,data_rd2);
		CH4_read(16-1,32'h20000+32*300,data_rd3);
	join
	for(int i=0;i<16;i++)
	begin
		$display("data_rd0[%0d]=%0d",i,data_rd0[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd1[%0d]=%0d",i,data_rd1[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd2[%0d]=%0d",i,data_rd2[i]);
	end
	$display("");
	for(int i=0;i<16;i++)
	begin
		$display("data_rd3[%0d]=%0d",i,data_rd3[i]);
	end
	$display("////////////////////////////////////////////////");

	#1000 $finish;
end

initial
begin
#10000000 $finish;
end

mcif_5r_4w #
(
	.C_M_AXI_ID_WIDTH(C_M_AXI_ID_WIDTH)
)
u_mcif
(
	.clk(clk),
	.rst_n(rst_n),

	//////////////////////////////////////////////////
	.rd_req_vld0(rd_req_vld0),
	.rd_req_rdy0(rd_req_rdy0),
	.rd_req_pd0(rd_req_pd0),

	.rd_resp_vld0(rd_resp_vld0),
	.rd_resp_rdy0(rd_resp_rdy0),
	.rd_resp_pd0(rd_resp_pd0),
	.rd_fifo_pop0(rd_fifo_pop0),
	//////////////////////////////////////////////////
	.rd_req_vld1(rd_req_vld1),
	.rd_req_rdy1(rd_req_rdy1),
	.rd_req_pd1(rd_req_pd1),

	.rd_resp_vld1(rd_resp_vld1),
	.rd_resp_rdy1(rd_resp_rdy1),
	.rd_resp_pd1(rd_resp_pd1),
	.rd_fifo_pop1(rd_fifo_pop1),
	//////////////////////////////////////////////////
	.rd_req_vld2(rd_req_vld2),
	.rd_req_rdy2(rd_req_rdy2),
	.rd_req_pd2(rd_req_pd2),

	.rd_resp_vld2(rd_resp_vld2),
	.rd_resp_rdy2(rd_resp_rdy2),
	.rd_resp_pd2(rd_resp_pd2),
	.rd_fifo_pop2(rd_fifo_pop2),
	//////////////////////////////////////////////////
	.rd_req_vld3(rd_req_vld3),
	.rd_req_rdy3(rd_req_rdy3),
	.rd_req_pd3(rd_req_pd3),

	.rd_resp_vld3(rd_resp_vld3),
	.rd_resp_rdy3(rd_resp_rdy3),
	.rd_resp_pd3(rd_resp_pd3),
	.rd_fifo_pop3(rd_fifo_pop3),
	//////////////////////////////////////////////////
	.rd_req_vld4(rd_req_vld4),
	.rd_req_rdy4(rd_req_rdy4),
	.rd_req_pd4(rd_req_pd4),

	.rd_resp_vld4(rd_resp_vld4),
	.rd_resp_rdy4(rd_resp_rdy4),
	.rd_resp_pd4(rd_resp_pd4),
	.rd_fifo_pop4(rd_fifo_pop4),
	//////////////////////////////////////////////////
	.wr_req_vld0(wr_req_vld0),
	.wr_req_rdy0(wr_req_rdy0),
	.wr_req_pd0(wr_req_pd0),
	.wr_rsp_complete0(wr_rsp_complete0),
	//////////////////////////////////////////////////
	.wr_req_vld1(wr_req_vld1),
	.wr_req_rdy1(wr_req_rdy1),
	.wr_req_pd1(wr_req_pd1),
	.wr_rsp_complete1(wr_rsp_complete1),
	//////////////////////////////////////////////////
	.wr_req_vld2(wr_req_vld2),
	.wr_req_rdy2(wr_req_rdy2),
	.wr_req_pd2(wr_req_pd2),
	.wr_rsp_complete2(wr_rsp_complete2),
	//////////////////////////////////////////////////
	.wr_req_vld3(wr_req_vld3),
	.wr_req_rdy3(wr_req_rdy3),
	.wr_req_pd3(wr_req_pd3),
	.wr_rsp_complete3(wr_rsp_complete3),
	//////////////////////////////////////////////////
	//AW channel
	.M_AXI_AWID(M_AXI_AWID),//='b0;
	.M_AXI_AWADDR(M_AXI_AWADDR),
	.M_AXI_AWLEN(M_AXI_AWLEN),
	.M_AXI_AWSIZE(M_AXI_AWSIZE),//=clogb2((`AXI_DATA_WIDTH/8)-1);
	.M_AXI_AWBURST(M_AXI_AWBURST),//=2'b01;
	.M_AXI_AWLOCK(M_AXI_AWLOCK),//1'b0;
	.M_AXI_AWCACHE(M_AXI_AWCACHE),//=4'b0010
	.M_AXI_AWPROT(M_AXI_AWPROT),//=3'h0;
	.M_AXI_AWQOS(M_AXI_AWQOS),//=4'h0;
	.M_AXI_AWVALID(M_AXI_AWVALID),
	.M_AXI_AWREADY(M_AXI_AWREADY),
	
	//Wr channel
	.M_AXI_WDATA(M_AXI_WDATA),
	.M_AXI_WSTRB(M_AXI_WSTRB),//={(`AXI_DATA_WIDTH/8){1'b1}};
	.M_AXI_WLAST(M_AXI_WLAST),
	.M_AXI_WVALID(M_AXI_WVALID),
	.M_AXI_WREADY(M_AXI_WREADY),
	.M_AXI_BID(M_AXI_BID),//ignore
	.M_AXI_BRESP(M_AXI_BRESP),//ignore
	.M_AXI_BVALID(M_AXI_BVALID),//Bvalid and Bread means a a write response.
	.M_AXI_BREADY(M_AXI_BREADY),//Bvalid and Bread means a a write response.
	
	//AR channel
	.M_AXI_ARID(M_AXI_ARID),
	.M_AXI_ARADDR(M_AXI_ARADDR),
	.M_AXI_ARLEN(M_AXI_ARLEN),
	.M_AXI_ARSIZE(M_AXI_ARSIZE),//=clogb2((`AXI_DATA_WIDTH/8)-1);
	.M_AXI_ARBURST(M_AXI_ARBURST),//=2'b01;
	.M_AXI_ARLOCK(M_AXI_ARLOCK),//=1'b0;
	.M_AXI_ARCACHE(M_AXI_ARCACHE),//=4'b0010;
	.M_AXI_ARPROT(M_AXI_ARPROT),//=3'h0;
	.M_AXI_ARQOS(M_AXI_ARQOS),//=4'h0;
	.M_AXI_ARVALID(M_AXI_ARVALID),
	.M_AXI_ARREADY(M_AXI_ARREADY),
	
	//Rd channel
	.M_AXI_RID(M_AXI_RID),
	.M_AXI_RDATA(M_AXI_RDATA),
	.M_AXI_RRESP(M_AXI_RRESP),//ignore
	.M_AXI_RLAST(M_AXI_RLAST),
	.M_AXI_RVALID(M_AXI_RVALID),
	.M_AXI_RREADY(M_AXI_RREADY)
);

AXI_HP_Slave #
(
	.C_S_AXI_ID_WIDTH(C_M_AXI_ID_WIDTH),
	.C_S_AXI_DATA_WIDTH(`AXI_DATA_WIDTH),
	.C_S_AXI_ADDR_WIDTH(32)
)u_AXI_HP_Slave
(
	.S_AXI_ACLK(clk),
	.S_AXI_ARESETN(rst_n),
	.S_AXI_AWID(M_AXI_AWID),
	.S_AXI_AWADDR(M_AXI_AWADDR),
	.S_AXI_AWLEN(M_AXI_AWLEN),
	.S_AXI_AWSIZE(M_AXI_AWSIZE),
	.S_AXI_AWBURST(M_AXI_AWBURST),
	.S_AXI_AWLOCK(M_AXI_AWLOCK),
	.S_AXI_AWCACHE(M_AXI_AWCACHE),
	.S_AXI_AWPROT(M_AXI_AWPROT),
	.S_AXI_AWQOS(M_AXI_AWQOS),
	.S_AXI_AWVALID(M_AXI_AWVALID),
	.S_AXI_AWREADY(M_AXI_AWREADY),
	.S_AXI_WDATA(M_AXI_WDATA),
	.S_AXI_WSTRB(M_AXI_WSTRB),
	.S_AXI_WLAST(M_AXI_WLAST),
	.S_AXI_WVALID(M_AXI_WVALID),
	.S_AXI_WREADY(M_AXI_WREADY),
	.S_AXI_BID(M_AXI_BID),
	.S_AXI_BRESP(M_AXI_BRESP),
	.S_AXI_BVALID(M_AXI_BVALID),
	.S_AXI_BREADY(M_AXI_BREADY),
	.S_AXI_ARID(M_AXI_ARID),
	.S_AXI_ARADDR(M_AXI_ARADDR),
	.S_AXI_ARLEN(M_AXI_ARLEN),
	.S_AXI_ARSIZE(M_AXI_ARSIZE),
	.S_AXI_ARBURST(M_AXI_ARBURST),
	.S_AXI_ARLOCK(M_AXI_ARLOCK),
	.S_AXI_ARCACHE(M_AXI_ARCACHE),
	.S_AXI_ARPROT(M_AXI_ARPROT),
	.S_AXI_ARQOS(M_AXI_ARQOS),
	.S_AXI_ARVALID(M_AXI_ARVALID),
	.S_AXI_ARREADY(M_AXI_ARREADY),
	.S_AXI_RID(M_AXI_RID),
	.S_AXI_RDATA(M_AXI_RDATA),
	.S_AXI_RRESP(M_AXI_RRESP),
	.S_AXI_RLAST(M_AXI_RLAST),
	.S_AXI_RVALID(M_AXI_RVALID),
	.S_AXI_RREADY(M_AXI_RREADY)
);

endmodule

