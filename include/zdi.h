#ifndef __ZDI_H_
#define __ZDI_H_
#include <stdint.h>

#define MAXBREAKPOINTS 4
#define MAXINSTRUCTIONBYTES 3

class ZDI {
public:
    enum ZDI_REG:uint8_t { ZDI_ADDR0_L = 0x00, ZDI_ADDR0_H = 0x01, ZDI_ADDR0_U = 0x02, ZDI_ADDR1_L = 0x04, ZDI_ADDR1_H = 0x05, ZDI_ADDR1_U = 0x06, ZDI_ADDR2_L = 0x08, ZDI_ADDR2_H = 0x09, ZDI_ADDR2_U = 0x0A, ZDI_ADDR3_L = 0x0c, ZDI_ADDR3_H = 0x0d, ZDI_ADDR3_U = 0x0e, ZDI_BRK_CTL = 0x10, ZDI_MASTER_CTL = 0x11, ZDI_WR_DATA_L = 0x13, ZDI_WR_DATA_H = 0x14, ZDI_WR_DATA_U = 0x15, ZDI_RW_CTL = 0x16, ZDI_BUS_CTL = 0x17, ZDI_IS4 = 0x21, ZDI_IS3 = 0x22, ZDI_IS2 = 0x23, ZDI_IS1 = 0x24, ZDI_IS0 = 0x25, ZDI_WR_MEM = 0x30, ZDI_ID_L = 0x00, ZDI_ID_H = 0x01, ZDI_ID_REV = 0x02, ZDI_STAT = 0x03, ZDI_RD_L = 0x10, ZDI_RD_H = 0x11, ZDI_RD_U = 0x12, ZDI_BUS_STAT = 0x17, ZDI_RD_MEM = 0x20 };
    enum ZDI_RWControl:uint8_t { afmb, bc, de, hl, ix, iy, sp, pc, set_adl, reset_adl, exx, mem };
    enum ZDI_RWBIT { WRITE = 0, READ = 1};

    ZDI(uint8_t tckpin, uint8_t tdipin);

    // ZDI register access
     uint8_t read_register (ZDI_REG zdi_regnr);
        void read_register (ZDI_REG zdi_regnr, uint8_t count, uint8_t* values);
        void write_register(ZDI_REG zdi_regnr, uint8_t value);
        void write_register(ZDI_REG zdi_regnr, uint8_t count, uint8_t* values);

    // ZDI interface functions
    uint16_t get_productid(void);
     uint8_t get_revision (void);

        void set_debugflags(uint8_t flags);
    
    // Higher layer functions
    uint32_t read_cpu_register (ZDI_RWControl cpureg);
        void write_cpu_register(ZDI_RWControl cpureg, uint32_t value);
        void read_memory (uint32_t address, uint32_t count, uint8_t* buffer);
        void write_memory(uint32_t address, uint32_t count, uint8_t* buffer);

private:
    // low-level bitstream functions
        void signal_start(void);
        void signal_continue(void);
        void signal_done(void);
        bool read_bit(void);
        void write_bit(bool bit);

    // Intermediate helpers
        void address_register(uint8_t regnr, ZDI_RWBIT rw);
        void write_byte(uint8_t value);
     uint8_t read_byte(void);

    const int waitmicroseconds = 1;
    uint8_t tckpin,tdipin;
};

#endif