# AXI4_FULL_MEM
## 项目说明
1. 本项目是一个简单的、 AXI4 FULL 接口的虚拟存储器，通过与 AXI4 Master 接口相连接，该存储器能够被正确地访存；
2. 该项目中 AXI4 的五个通道均能够设置固定的响应延迟（以系统时钟周期为单位）；该虚拟存储器能够被指定基地址，是否采用该基地址指定的存储器由注册的 uncached_read / uncached_write 的返回值决定，（如未注册则访问基地址指定的存储器）。


## 使用方法
1. 首先确定在您的设备上已经安装了 Verilator，Git；
2. axi_mem_demo.cpp 和 axi4_demo.v 给出了一个简单的示例：
```cpp
    // step 0：包含头文件；
    #include "axi4_full_mem.hpp"

    // step 1：初始化 Verilog 模块的仿真模型；
    top = new Vaxi4_demo;

    // step 2：定义一片虚拟的存储器区域；
    uint8_t mem[4096] = {0};

    // step 3：初始化一个 axi_mem 类（其中第一个模板参数为地址宽度对应的数据类型；第二个模板参数为数据宽度对应的数据类型）；
    axi_mem<uint32_t, uint32_t> axi_ports;

    // step 4：将axi_mem 类中的成员变量（与AXI4 信号相关的指针）与仿真模型的端口相绑定；
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

    // step 5：调用类的 init 方法，指定 AXI4 接口各个通道的延迟、虚拟存储器的基地址、虚拟存储器对应的变量；基地址以下地址的读写函数指针；
    axi_ports.init(3, 3, 3, 3, 3,  0x80000000, (uint8_t*)mem, NULL, NULL);
	
    
    ...

    // step 6：在仿真过程中，每次 eval 执行以后（!），调用类的 axi_signal_update 方法以更新 AXI4 信号。
    top->eval();
	axi_ports.axi_signal_update();



```
3. 如果需要将 AXI4 读写事务相关的信息通过标准输出显示，请在包含的头文件之前定义宏 AXI_LOG：
```cpp
#define AXI_LOG // print log about axi4
#include "axi4_full_mem.hpp"
```
4. 本项目中包含了一个简单的 Makefile，其选项有：
```shell
# 编译并运行，运行结果的 log 将被保存到 axi_mem.log，仿真导出的波形文件名为wave.vcd；
make 

# 通过 gtkwave （确定设备中已经安装该软件）查看波形文件；
make display

# 清理编译生成的所有文件；
make clean
```
## 项目结构
![项目结构](./picture/基本结构.png "项目结构")

## TODO
1. 目前该项目并未实现 AXI4 FULL 的完整协议，如 wstrb、qos、xuser、xid 等，如有其他改进欢迎提交 PR 与 issue。
