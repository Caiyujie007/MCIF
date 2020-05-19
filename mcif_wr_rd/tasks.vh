task CH0_wr(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,input bit [`AXI_DATA_WIDTH-1:0] data[],input bit nonposted);
begin
	@(posedge clk);
	wr_req_vld0<=1'b1;
	wr_req_pd0<={1'b1,{(`AXI_DATA_WIDTH-1-32-`log2MAX_BURST_ATOM_CUBE){1'b0}},nonposted,length,addr};
	@(posedge clk);
	while(wr_req_rdy0!=1'b1) @(posedge clk);
	wr_req_vld0<=1'b0;
	@(posedge clk);
	fork
		begin
			for(int i=0;i<=length;i++)
			begin
				wr_req_vld0<=1'b1;
				wr_req_pd0<={1'b0,data[i]};
				@(posedge clk);
				while(wr_req_rdy0!=1'b1) @(posedge clk);
			end
			wr_req_vld0<=1'b0;
		end
		if(nonposted)
		begin
			while(wr_rsp_complete0!=1'b1) @(posedge clk);
		end
	join
end
endtask

task CH1_wr(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,input bit [`AXI_DATA_WIDTH-1:0] data[],input bit nonposted);
begin
	@(posedge clk);
	wr_req_vld1<=1'b1;
	wr_req_pd1<={1'b1,{(`AXI_DATA_WIDTH-1-32-`log2MAX_BURST_ATOM_CUBE){1'b0}},nonposted,length,addr};
	@(posedge clk);
	while(wr_req_rdy1!=1'b1) @(posedge clk);
	wr_req_vld1<=1'b0;
	@(posedge clk);
	fork
		begin
			for(int i=0;i<=length;i++)
			begin
				wr_req_vld1<=1'b1;
				wr_req_pd1<={1'b0,data[i]};
				@(posedge clk);
				while(wr_req_rdy1!=1'b1) @(posedge clk);
			end
			wr_req_vld1<=1'b0;
		end
		if(nonposted)
		begin
			while(wr_rsp_complete1!=1'b1) @(posedge clk);
		end
	join
end
endtask

task CH2_wr(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,input bit [`AXI_DATA_WIDTH-1:0] data[],input bit nonposted);
begin
	@(posedge clk);
	wr_req_vld2<=1'b1;
	wr_req_pd2<={1'b1,{(`AXI_DATA_WIDTH-1-32-`log2MAX_BURST_ATOM_CUBE){1'b0}},nonposted,length,addr};
	@(posedge clk);
	while(wr_req_rdy2!=1'b1) @(posedge clk);
	wr_req_vld2<=1'b0;
	@(posedge clk);
	fork
		begin
			for(int i=0;i<=length;i++)
			begin
				wr_req_vld2<=1'b1;
				wr_req_pd2<={1'b0,data[i]};
				@(posedge clk);
				while(wr_req_rdy2!=1'b1) @(posedge clk);
			end
			wr_req_vld2<=1'b0;
		end
		if(nonposted)
		begin
			while(wr_rsp_complete2!=1'b1) @(posedge clk);
		end
	join
end
endtask

task CH3_wr(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,input bit [`AXI_DATA_WIDTH-1:0] data[],input bit nonposted);
begin
	@(posedge clk);
	wr_req_vld3<=1'b1;
	wr_req_pd3<={1'b1,{(`AXI_DATA_WIDTH-1-32-`log2MAX_BURST_ATOM_CUBE){1'b0}},nonposted,length,addr};
	@(posedge clk);
	while(wr_req_rdy3!=1'b1) @(posedge clk);
	wr_req_vld3<=1'b0;
	@(posedge clk);
	fork
		begin
			for(int i=0;i<=length;i++)
			begin
				wr_req_vld3<=1'b1;
				wr_req_pd3<={1'b0,data[i]};
				@(posedge clk);
				while(wr_req_rdy3!=1'b1) @(posedge clk);
			end
			wr_req_vld3<=1'b0;
		end
		if(nonposted)
		begin
			while(wr_rsp_complete3!=1'b1) @(posedge clk);
		end
	join
end
endtask

task CH0_read(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,ref bit [`AXI_DATA_WIDTH-1:0] data[]);
begin
	@(posedge clk);
	rd_req_vld0<=1'b1;
	rd_req_pd0<={length,addr};
	@(posedge clk);
	while(rd_req_rdy0!=1'b1) @(posedge clk);
	rd_req_vld0<=1'b0;
	@(posedge clk);
	rd_resp_rdy0<=1'b1;
	for(int i=0;i<=length;i++)
	begin
		@(posedge clk);
		while( (rd_resp_vld0&rd_resp_rdy0)!=1'b1 ) @(posedge clk);
		data[i]=rd_resp_pd0;
	end
	rd_resp_rdy0<=1'b0;
end
endtask

task CH1_read(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,ref bit [`AXI_DATA_WIDTH-1:0] data[]);
begin
	@(posedge clk);
	rd_req_vld1<=1'b1;
	rd_req_pd1<={length,addr};
	@(posedge clk);
	while(rd_req_rdy1!=1'b1) @(posedge clk);
	rd_req_vld1<=1'b0;
	@(posedge clk);
	rd_resp_rdy1<=1'b1;
	for(int i=0;i<=length;i++)
	begin
		@(posedge clk);
		while( (rd_resp_vld1&rd_resp_rdy1)!=1'b1 ) @(posedge clk);
		data[i]=rd_resp_pd1;
	end
	rd_resp_rdy1<=1'b0;
end
endtask

task CH2_read(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,ref bit [`AXI_DATA_WIDTH-1:0] data[]);
begin
	@(posedge clk);
	rd_req_vld2<=1'b1;
	rd_req_pd2<={length,addr};
	@(posedge clk);
	while(rd_req_rdy2!=1'b1) @(posedge clk);
	rd_req_vld2<=1'b0;
	@(posedge clk);
	rd_resp_rdy2<=1'b1;
	for(int i=0;i<=length;i++)
	begin
		@(posedge clk);
		while( (rd_resp_vld2&rd_resp_rdy2)!=1'b1 ) @(posedge clk);
		data[i]=rd_resp_pd2;
	end
	rd_resp_rdy2<=1'b0;
end
endtask

task CH3_read(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,ref bit [`AXI_DATA_WIDTH-1:0] data[]);
begin                      
	@(posedge clk);
	rd_req_vld3<=1'b1;
	rd_req_pd3<={length,addr};
	@(posedge clk);
	while(rd_req_rdy3!=1'b1) @(posedge clk);
	rd_req_vld3<=1'b0;
	@(posedge clk);   
	rd_resp_rdy3<=1'b1;
	for(int i=0;i<=length;i++)
	begin
		@(posedge clk);
		while( (rd_resp_vld3&rd_resp_rdy3)!=1'b1 ) @(posedge clk);
		data[i]=rd_resp_pd3;
	end
	rd_resp_rdy3<=1'b0;
end
endtask

task CH4_read(input bit [`log2MAX_BURST_ATOM_CUBE-1:0] length,input bit[31:0] addr,ref bit [`AXI_DATA_WIDTH-1:0] data[]);
begin                      
	@(posedge clk);
	rd_req_vld4<=1'b1;
	rd_req_pd4<={length,addr};
	@(posedge clk);
	while(rd_req_rdy4!=1'b1) @(posedge clk);
	rd_req_vld4<=1'b0;
	@(posedge clk);   
	rd_resp_rdy4<=1'b1;
	for(int i=0;i<=length;i++)
	begin
		@(posedge clk);
		while( (rd_resp_vld4&rd_resp_rdy4)!=1'b1 ) @(posedge clk);
		data[i]=rd_resp_pd4;
	end
	rd_resp_rdy4<=1'b0;
end
endtask
