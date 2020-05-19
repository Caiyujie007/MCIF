`include "vpu_defines.vh"

`include "vpu_defines.vh"

module mcif_arb_comb5
(
	input [2:0]cur_arb_id,
	input [4:0]arb_req,
	input arb_en,
	output reg [2:0]nxt_arb_id
);


always @(*)
begin
	if(arb_en)
	begin
		case(cur_arb_id)
			3'd0:
				case(1'b1)
					arb_req[1]:nxt_arb_id=3'd1;
					arb_req[2]:nxt_arb_id=3'd2;
					arb_req[3]:nxt_arb_id=3'd3;
					arb_req[4]:nxt_arb_id=3'd4;
					arb_req[0]:nxt_arb_id=3'd0;
					default:nxt_arb_id=3'd0;
				endcase
			3'd1:
				case(1'b1)
					arb_req[2]:nxt_arb_id=3'd2;
					arb_req[3]:nxt_arb_id=3'd3;
					arb_req[4]:nxt_arb_id=3'd4;
					arb_req[0]:nxt_arb_id=3'd0;
					arb_req[1]:nxt_arb_id=3'd1;
					default:nxt_arb_id=3'd1;
				endcase
			3'd2:
				case(1'b1)
					arb_req[3]:nxt_arb_id=3'd3;
					arb_req[4]:nxt_arb_id=3'd4;
					arb_req[0]:nxt_arb_id=3'd0;
					arb_req[1]:nxt_arb_id=3'd1;
					arb_req[2]:nxt_arb_id=3'd2;
					default:nxt_arb_id=3'd2;
				endcase
			3'd3:
				case(1'b1)
					arb_req[4]:nxt_arb_id=3'd4;
					arb_req[0]:nxt_arb_id=3'd0;
					arb_req[1]:nxt_arb_id=3'd1;
					arb_req[2]:nxt_arb_id=3'd2;
					arb_req[3]:nxt_arb_id=3'd3;
					default:nxt_arb_id=3'd3;
				endcase
			3'd4:
				case(1'b1)
					arb_req[0]:nxt_arb_id=3'd0;
					arb_req[1]:nxt_arb_id=3'd1;
					arb_req[2]:nxt_arb_id=3'd2;
					arb_req[3]:nxt_arb_id=3'd3;
					arb_req[4]:nxt_arb_id=3'd4;
					default:nxt_arb_id=3'd4;
				endcase
			default:nxt_arb_id=3'd0;
		endcase
	end
	else
		nxt_arb_id=cur_arb_id;
end

endmodule

