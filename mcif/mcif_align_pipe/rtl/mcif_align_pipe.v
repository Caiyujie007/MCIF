`include "vpu_defines.vh"

//Warning: this module is not implemented because `AXI_DATA_WIDTH=`Tk*16
module mcif_align_pipe
(
	//input clk,
	//input rst_n,

	input data_in_vld,
	input [`AXI_DATA_WIDTH-1:0]data_in,
	output data_in_rdy,

	output data_out_vld,
	output [`AXI_DATA_WIDTH-1:0]data_out,
	input data_out_rdy
);

assign data_out_vld=data_in_vld;
assign data_out=data_in;
assign data_in_rdy=data_out_rdy;

endmodule

