all:
	verilator  --cc --exe --build --trace --vpi -Wno-WIDTHEXPAND -Wno-WIDTHTRUNC axi_mem_demo.cpp axi4_demo.v --top-module axi4_demo  && ./obj_dir/Vaxi4_demo > axi_mem.log
	
display: all
	gtkwave wave.vcd &

clean:
	rm -rf obj_dir *.vcd *.log

.PHONY: clean