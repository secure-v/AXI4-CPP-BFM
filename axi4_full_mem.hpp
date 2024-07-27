#ifndef AXI4_FULL_MEM
#define AXI4_FULL_MEM

#include <cstdint>
#define BURST_TYPE_FIXED 0
#define BURST_TYPE_INCR 1
#define BURST_TYPE_WRAP 2
#define BURST_TYPE_RESV 3 // reserve

#define RESP_TYPE_OKAY 0
#define RESP_TYPE_EXOKAY 1
#define RESP_TYPE_SLVERR 2
#define RESP_TYPE_DECERR 3


#include<stdint.h>
#include <stdio.h>

#include<stdint.h>
#include <stdio.h>

template <typename ADDRT, typename DATAT>
class axi_mem {
public:
    uint8_t* aclk     ;
    uint8_t* aresetn  ;

// write address channel
    uint8_t* awid     ;
    ADDRT* awaddr     ;
    uint8_t* awlen    ;
    uint8_t* awsize   ;
    uint8_t* awburst  ;
    uint64_t* awlock  ; // TODO
    uint64_t* awcache ; // TODO
    uint64_t* awprot  ; // TODO
    uint64_t* awqos   ; // TODO
    uint64_t* awregion; // TODO
    uint64_t* awuser  ; // TODO
    uint8_t* awvalid  ;
    uint8_t* awready  ;

// write data channel
    DATAT* wdata      ;
    uint32_t* wstrb   ; // TODO
    uint8_t* wlast    ;
    uint64_t* wuser   ; // TODO
    uint8_t* wvalid   ;
    uint8_t* wready   ;

// write response channel
    uint8_t* bid      ;
    uint8_t* bresp    ; // TODO
    uint64_t* buser   ; // TODO
    uint8_t* bvalid   ;
    uint8_t* bready   ;

// read address channel
    uint8_t* arid     ;
    ADDRT* araddr     ;
    uint8_t* arlen    ;
    uint8_t* arsize   ;
    uint8_t* arburst  ;
    uint64_t* arlock  ; // TODO
    uint64_t* arcache ; // TODO
    uint64_t* arprot  ; // TODO
    uint64_t* arqos   ; // TODO
    uint64_t* arregion; // TODO
    uint64_t* aruser  ; // TODO
    uint8_t* arvalid  ;
    uint8_t* arready  ;

// read data channel
    uint8_t* rid      ;
    DATAT* rdata      ;
    uint8_t* rresp    ; // TODO
    uint8_t* rlast    ;
    uint64_t* ruser   ; // TODO
    uint8_t* rvalid   ;
    uint8_t* rready   ;

// signal about the memory
    uint8_t* mem;
    size_t mem_base_addr;

// transaction information
    ADDRT waddr;
    ADDRT raddr;
    size_t rcount = 0;
    DATAT wdata_at_posedge;
    uint8_t bready_at_posedge;
    bool read_at_posedge = false;
    bool in_read_stage = false;
    bool b_delay_stage = false;
    bool read_stage = false;

    size_t aw_delay = 0;
    size_t w_delay = 0 ;
    size_t b_delay = 0 ;
    size_t ar_delay = 0;
    size_t r_delay = 0 ;

    size_t aw_delay_count = 0;
    size_t w_delay_count = 0;
    size_t b_delay_count = 0;
    size_t ar_delay_count = 0;
    size_t r_delay_count = 0;

    typedef bool (*UNCACHED_WRITE) (ADDRT waddr, DATAT wdata, uint8_t wlen);
    typedef bool (*UNCACHED_READ) (ADDRT raddr, DATAT& rdata);
    UNCACHED_WRITE uncached_write;
    UNCACHED_READ uncached_read;

    axi_mem(/* args */) {};
    ~axi_mem() {};

    void init(const size_t aw_delay, const size_t w_delay , const size_t b_delay, const size_t ar_delay, const size_t r_delay, const size_t mem_base_addr, uint8_t* const mem, UNCACHED_WRITE func_ptr0, UNCACHED_READ func_ptr1);
    void axi_signal_update();
};

template <typename ADDRT, typename DATAT>
void axi_mem<ADDRT, DATAT>::axi_signal_update()
{
    if (!this->aresetn)
        return ;
    
    if (!this->aclk)
        return ;
   
    if (*awvalid) {
        waddr = *awaddr;

        if (!(*awready)) {
            if (aw_delay_count == 0)
                *awready = 1;
            else
                aw_delay_count--;
        }
        else {
            aw_delay_count = aw_delay;
            *awready = 0;
        }

        b_delay_stage = false;
    }
    else {
        aw_delay_count = aw_delay;
        *awready = 0;
    }

    if (*wvalid) {
        if (!(*wready)) {
            if (w_delay_count == 0) {
                *wready = 1;

                #ifdef AXI_LOG
                    printf("[write] %x: %x\n", waddr, wdata_at_posedge);
                #endif

                if ((uncached_write == NULL) || (!uncached_write(waddr, wdata_at_posedge, 8))) // uncached mem write
                    for (size_t i = 0; i < sizeof(DATAT); i++) { // write to the memory
                        mem[i + waddr - mem_base_addr] = (uint8_t)(wdata_at_posedge & 0xff);
                        wdata_at_posedge >>= 8;
                    }
            }
            else
                w_delay_count--;
        }
        else {
            w_delay_count = w_delay;
            *wready = 0;
                 
            // update write address
            switch (*awburst) {
                case BURST_TYPE_FIXED: break;
                case BURST_TYPE_INCR : waddr += (1 << (*awsize));
                case BURST_TYPE_WRAP : break; // TODO
                case BURST_TYPE_RESV : ;
            }
        }

        wdata_at_posedge = *wdata;
    }
    else {
        w_delay_count = w_delay;
        *wready = 0;
    }

    if ((*wlast) && (*wvalid) && (*wready)) {
        b_delay_stage = true;
    }

    if (b_delay_stage) {
        if (b_delay_count == 0) {
            b_delay_stage = false;
            *bvalid = 1;
            b_delay_count = b_delay;
        }
        else
            b_delay_count--;
    }

    if (*bvalid) {
        if (bready_at_posedge) 
            *bvalid = 0;
        else
            *bvalid = 1;
    }

    bready_at_posedge = (*bvalid)? *bready : 0;

    if (*arvalid) {
        raddr = *araddr;
        rcount = (*arlen) + 1;

        if (!(*arready)) {
            if (ar_delay_count == 0)
                *arready = 1;
            else
                ar_delay_count--;
        }
        else {
            ar_delay_count = ar_delay;
            *arready = 0;
        }
    }
    else {
        ar_delay_count = ar_delay;
        *arready = 0;
    }

    if (in_read_stage)
        read_stage = true;

    if (read_stage) {
        DATAT read_data = 0x0;

        if ((uncached_read == NULL) || (!uncached_read(raddr, read_data)))
            for (size_t i = 0; i < sizeof(DATAT); i++) {
                read_data |= (((DATAT)mem[i + raddr - mem_base_addr]) << (i * 8));
            }

        *rdata = read_data;

        if (r_delay_count)
            r_delay_count--;
        else {
            r_delay_count = r_delay;
            *rvalid = 1;

            if (rcount == 1) // the last data
                *rlast = 1;
        }     
    }

    if (read_at_posedge) {
        #ifdef AXI_LOG
            printf("[read] %x: %x\n", raddr, *rdata);
        #endif
        
        rcount--;
        *rvalid = 0;
        *rlast = 0;
       
        switch (*arburst) {
            case BURST_TYPE_FIXED: break;
            case BURST_TYPE_INCR : raddr += (1 << (*arsize));
            case BURST_TYPE_WRAP : break; // TODO
            case BURST_TYPE_RESV : ;
        }
    }

    if (rcount == 0)
        read_stage = false;
    else
        read_stage = true;
    
    in_read_stage = ((*arvalid) && (*arready))? 1:0;
    read_at_posedge = (*rready) && (*rvalid);
    
    return ;
}

template <typename ADDRT, typename DATAT>
void axi_mem<ADDRT, DATAT>::init(const size_t aw_delay, const size_t w_delay , const size_t b_delay, const size_t ar_delay, const size_t r_delay, const size_t mem_base_addr, uint8_t* const mem, UNCACHED_WRITE func_ptr0, UNCACHED_READ func_ptr1)
{
    this->aw_delay = aw_delay;
    this->w_delay = w_delay;
    this->b_delay = b_delay;
    this->ar_delay = ar_delay;
    this->r_delay = r_delay;

    this->aw_delay_count = aw_delay;
    this->w_delay_count = w_delay;
    this->b_delay_count = b_delay;
    this->ar_delay_count = ar_delay;
    this->r_delay_count = r_delay;
    this->mem_base_addr = mem_base_addr;
    this->mem = mem;

    this->uncached_write = func_ptr0;
    this->uncached_read = func_ptr1;

    return ;
}

#endif