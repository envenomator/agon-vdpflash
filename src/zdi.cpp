#include "zdi.h"
#include <Arduino.h>

ZDI::ZDI(uint8_t tckpin, uint8_t tdipin) {
    this->tckpin = tckpin;
    this->tdipin = tdipin;

    pinMode(tckpin, OUTPUT);
    digitalWrite(tckpin,HIGH);

    set_debugflags(0);
}

void ZDI::signal_start(void) {
    pinMode(tdipin, OUTPUT);
    digitalWrite(tckpin,HIGH);
    digitalWrite(tdipin,LOW);
}

void ZDI::signal_done(void) {
    write_bit(true);
}

void ZDI::signal_continue(void) {
    write_bit(false);
}

void ZDI::write_bit(bool bit) {
    digitalWrite(tckpin,LOW);
    digitalWrite(tdipin,bit);
    digitalWrite(tckpin,HIGH);
}

bool ZDI::read_bit(void) {
    bool bit;
    digitalWrite(tckpin,LOW);
    pinMode(tdipin, INPUT);
    bit = digitalRead(tdipin);
    pinMode(tdipin, OUTPUT);
    digitalWrite(tckpin,HIGH);
    return bit;
}

void ZDI::address_register(uint8_t zdi_regnr, ZDI_RWBIT rw) {
    // write 7-bits, msb to lsb order
    for(int i=6;i>=0;i--) write_bit(zdi_regnr & (0b1<<i));
    write_bit(rw);
}

void ZDI::write_byte(uint8_t value) {
    // write 8-bits, msb to lsb order
    for(int i=7;i>=0;i--) write_bit(value & (0b1<<i));
}

uint8_t ZDI::read_byte(void) {
    uint8_t value = 0;
    //pinMode(tdipin, INPUT);
    // read 8-bits, msb to lsb order
    for(int i=7;i>=0;i--)
    {
        value = value << 1;
        value = value | read_bit();
    }
    //pinMode(tdipin, OUTPUT);
    return value;
}

uint8_t ZDI::read_register(ZDI_REG zdi_regnr) {
    uint8_t value;
    signal_start();
    address_register(zdi_regnr, READ);
    signal_continue();
    value  = read_byte();
    signal_done();

    return value;
}

void ZDI::write_register(ZDI_REG zdi_regnr, uint8_t value) {
    signal_start();
    address_register(zdi_regnr, WRITE);
    signal_continue();
    write_byte(value);
    signal_done();
}

uint32_t ZDI::read_cpu_register(ZDI_RWControl cpureg) {
    write_register(ZDI_RW_CTL, cpureg);
    uint8_t values[3];
    uint8_t* ptr = values;

    read_register(ZDI_RD_L, 3, values);
    return(values[2]<<16)+(values[1]<<8)+values[0];
}

void ZDI::write_cpu_register(ZDI_RWControl cpureg, uint32_t value) {
    uint8_t buffer[3];
    buffer[0] = value & 0xff;       // L
    buffer[1] = (value>>8) & 0xff;  // H
    buffer[2] = (value>>16) & 0xff; // U
    write_register(ZDI_WR_DATA_L, 3, buffer);
    write_register(ZDI_RW_CTL, cpureg | 0b10000000); // set high bit to indicate write
}

void ZDI::read_register(ZDI_REG zdi_regnr, uint8_t count, uint8_t* values) {
    signal_start();
    address_register(zdi_regnr, READ);
    while(count--) {
        signal_continue();
        *(values++) = read_byte();
    }
    signal_done();
}

void ZDI::write_register(ZDI_REG zdi_regnr, uint8_t count, uint8_t* values) {
    signal_start();
    address_register(zdi_regnr, WRITE);
    while(count--) {
        signal_continue();
        write_byte(*(values++));
    }
    signal_done();
}

uint16_t ZDI::get_productid(void) {
    uint8_t id[2];
    read_register(ZDI_ID_L, 2, id);
    return (id[1]<<8)+id[0];
}

uint8_t ZDI::get_revision(void) {
    return read_register(ZDI_ID_REV);
}

void ZDI::read_memory (uint32_t address, uint32_t count, uint8_t* buffer) {
    uint32_t oldpc;
    uint32_t i;

    oldpc = read_cpu_register(ZDI_RWControl::pc);
    write_cpu_register(ZDI_RWControl::pc, address);

    signal_start();
    address_register(ZDI_RD_MEM, READ);
    for(i=0;i<count;i++) {
        signal_continue();
        *(buffer++) = read_byte();
    }
    signal_done();

    write_cpu_register(ZDI_RWControl::pc, oldpc);
}

void ZDI::write_memory(uint32_t address, uint32_t count, uint8_t* buffer) {
    uint32_t oldpc;
    uint32_t i;

    write_cpu_register(ZDI_RWControl::pc, address);

    signal_start();
    address_register(ZDI_WR_MEM, WRITE);
    for (i=0;i<count;i++) {
        signal_continue();
        write_byte(*(buffer++));
    }
    signal_done();
}

void ZDI::set_debugflags(uint8_t flags) {
    write_register(ZDI_BRK_CTL, flags);
}
