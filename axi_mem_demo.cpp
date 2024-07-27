#include <cstdint>
#include<stdint.h>
#include <verilated.h>
#include "verilated_vpi.h"
#include <iostream>
#include "vpi_user.h"
#include <stdio.h>
#include <unistd.h>
#include "verilated_vcd_c.h"
#include "Vaxi4_demo.h"

#define AXI_LOG // print log about axi4
#include "axi4_full_mem.hpp"

using namespace std;

Vaxi4_demo *top;
uint64_t main_time = 0;

int main(int argc, char** argv) {
	Verilated::commandArgs(argc, argv);
	top = new Vaxi4_demo; 

    uint8_t mem[4096] = {0};
    axi_mem<uint32_t, uint32_t> axi_ports;
    axi_ports.aclk = &top->clk;
    axi_ports.aresetn = &top->rstn;
    axi_ports.awvalid = &top->awvalid;
    axi_ports.awready = &top->awready;
    axi_ports.awaddr = &top->awaddr;
    axi_ports.wvalid = &top->wvalid;
    axi_ports.wready = &top->wready;
    axi_ports.awburst = &top->awburst;
    axi_ports.awsize = &top->awsize;
    axi_ports.wdata = &top->wdata;
    axi_ports.wlast = &top->wlast;
    axi_ports.bvalid = &top->bvalid;
    axi_ports.bready = &top->bready;
    axi_ports.arvalid = &top->arvalid;
    axi_ports.arready = &top->arready;
    axi_ports.araddr = &top->araddr;
    axi_ports.rvalid = &top->rvalid;
    axi_ports.rready = &top->rready;
    axi_ports.rdata = &top->rdata;
    axi_ports.arburst = &top->arburst;
    axi_ports.arsize = &top->arsize;
    axi_ports.arlen = &top->arlen;
    axi_ports.rlast = &top->rlast;
    axi_ports.init(3, 3, 3, 3, 3,  0x80000000, (uint8_t*)mem, NULL, NULL);

	Verilated::traceEverOn(true);            
    VerilatedVcdC* tfp = new VerilatedVcdC();
    top->trace(tfp, 0);                      
    tfp->open("wave.vcd");                   

	while (1) {
 		if (main_time > 10)
 			top->rstn = 1;
		else
			top->rstn = 0;

		if (main_time & 1)
 			top->clk = 1;
		else
			top->clk = 0;

  		top->eval();

		tfp->dump(main_time);                

		if (top->clk) {
			axi_ports.axi_signal_update();
		}

		main_time++;

        if (main_time > 400) 
            break;
  	}

    top->final();
    delete top;

	tfp->close();
}

