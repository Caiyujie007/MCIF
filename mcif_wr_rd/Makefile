ROOT_DIR=../../../
DESIGN_DIR=$(ROOT_DIR)design

RTL_FILES= $(DESIGN_DIR)/vpu_defines.vh \
	${DESIGN_DIR}/mcif/mcif_align_pipe/rtl/mcif_align_pipe.v \
	${DESIGN_DIR}/mcif/mcif_rr_arb/mcif_arb_comb/rtl/mcif_arb_comb5.v \
	${DESIGN_DIR}/mcif/mcif_rr_arb/rtl/mcif_rr_arb5.v \
	${DESIGN_DIR}/mcif/mcif_rr_arb/mcif_arb_comb/rtl/mcif_arb_comb4.v \
	${DESIGN_DIR}/mcif/mcif_rr_arb/rtl/mcif_rr_arb4.v \
	${DESIGN_DIR}/mcif/rtl/mcif_5r_4w.v \
	${DESIGN_DIR}/mcif/mcif_rd/rtl/mcif_rd_5r.v \
	${DESIGN_DIR}/mcif/mcif_wr/rtl/mcif_wr_4w.v \
	${DESIGN_DIR}/vlibs/hs_pipe.v \
	${DESIGN_DIR}/vlibs/fifo_dff.v

TB_FILES= 	./AXI_HP_Slave.sv \
		./testbench.sv

build:
	vcs -full64 -sverilog -debug_all +incdir+${DESIGN_DIR} ${RTL_FILES} ${TB_FILES}

run:
	@./simv

clean:
	@rm -rf csrc DVEfiles simv simv.daidir ucli.key *.vpd
