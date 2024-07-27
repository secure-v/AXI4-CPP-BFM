module axi4_demo #(parameter  base_addr	= 32'h80000000) (
	input wire             clk    ,
	input wire             rstn   ,
	output wire [31:0]     araddr ,
    output wire [1:0]      arburst,
    output wire [3:0]      arcache,
    output wire [1:0]      arid   ,
	output wire [7:0]      arlen  ,
    output wire            arlock ,
    output wire [2:0]      arprot ,
    output wire [3:0]      arqos  ,
    input wire             arready,
	output wire [2:0]      arsize ,
	output wire [1:0]      aruser ,
	output wire            arvalid,
	output wire [31:0]     awaddr ,
	output wire [1:0]      awburst,
    output wire [3:0]      awcache,
    output wire [1:0]      awid   ,
	output wire            awlock ,
    output wire [7:0]      awlen  ,
    output wire [2:0]      awprot ,
    output wire [3:0]      awqos  ,
    input wire             awready,
	output wire [2:0]      awsize ,
	output wire [1:0]      awuser ,
	output wire            awvalid,
	input wire [1:0]       bid    ,
    output wire            bready ,
	input wire [1:0]       bresp  ,
	input wire [1:0]       buser  ,
	input wire             bvalid ,
	input wire [1:0]       rid    ,
	input wire [31:0]      rdata  ,
    input wire             rlast  ,
	input wire [1:0]       rresp  ,
	output wire            rready ,
	input wire [1:0]       ruser  ,
	input wire             rvalid ,
    output wire [31:0]     wdata  ,
    output wire [1:0]      wid    ,
    output wire            wlast  ,
    input wire             wready ,
	output wire [3:0]      wstrb  ,
	output wire [1:0]      wuser  ,
	output wire            wvalid 
);

    parameter IDLE = 3'b000;
    parameter AW   = 3'b001;
    parameter WD   = 3'b010;
    parameter RESP = 3'b011;
    parameter AR   = 3'b100;
    parameter RD   = 3'b101;
    parameter DONE = 3'b110;
    
    reg [2:0] state;

    always @ (posedge clk) begin
        if (!rstn)
            state <= IDLE;
        else if (state == IDLE) 
            state <= AW;
        else if ((state == AW) && (awready) && (awvalid))
            state <= WD;
        else if ((state == WD) && (count == 16))
            state <= RESP;
        else if ((state == RESP) && (bready) && (bvalid))
            state <= AR;
        else if ((state == AR) && (arready) && (arvalid))
            state <= RD;
        else if ((state == RD) && (count == 16))
            state <= DONE;
        else 
            state <= state;
    end

    reg [4:0] count;

    always @ (posedge clk) begin
        if (!rstn)
            count <= 5'b0;
        else if (state == AW)
            count <= 5'b0;
        else if ((state == WD) && (wready) && (wvalid)) 
            count <= count + 5'b1;
        else if (state == AR)
            count <= 5'b0;
        else if ((state == RD) && (rready) && (rvalid)) 
            count <= count + 5'b1;
        else
            count <= count;
    end

    // address read channel
    assign araddr = base_addr;
    assign arburst = 1;
    assign arcache = 2;
    assign arid = 0;
    assign arlen = 15;
    assign arlock = 0;
    assign arprot = 0;
    assign arqos = 0;
    // arready
    assign arsize = 2;
    assign aruser = 0;
    assign arvalid = (state == AR);


    assign awaddr = base_addr;
    assign awburst = 1;
    assign awcache = 2;
    assign awid = 0;
    assign awlock = 0;
    assign awlen = 15;
    assign awprot = 0;
    assign awqos = 0;
    // awready
    assign awsize = 2;
    assign awuser = 0;
    assign awvalid = (state == AW);

    // bid
    assign bready = bvalid;
    // bresp
    // buser
    // bvalid

    // rid
    // rdata
    // rlast
    // rresp
    assign rready = rvalid;
    // ruser
    // rvalid

    assign wdata = (count << 2) + base_addr;
    assign wlast = (count == 15) && (state == WD);
    // wready
    assign wstrb = 4'b1111;
    assign wuser = 0;
    assign wvalid = (state == WD);
    assign awid = 0;
endmodule
