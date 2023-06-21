#include "cpu.h"

CPU::CPU(ZDI* zdi) {
    int n;

    this->zdi = zdi;

    debug_flags = 0;
    for(n = 0; n < 4; n++) {
        setBreakpoint(n, 0);
        disableBreakpoint(n);
    }
}

void CPU::setBreak(void) {
    debug_flags |= 0b10000000;
    zdi->set_debugflags(debug_flags);
}

void CPU::SingleStep(void) {
    debug_flags |= 0b10000001;
    zdi->set_debugflags(debug_flags);
}

void CPU::setContinue(void) {
    debug_flags &= 0b01111110;
    zdi->set_debugflags(debug_flags);
}

void CPU::setBreakpoint(uint8_t index, uint32_t address) {
    uint8_t buffer[3];

    if(index > 3) return;

    breakpoints[index] = address;
    buffer[0] = (uint8_t)(address & 0xff);
    buffer[1] = (uint8_t)((address>>8) & 0xff);
    buffer[2] = (uint8_t)((address>>16) & 0xff);

    // send breakpoint address to ZDI at correct index in memory
    zdi->write_register(ZDI::ZDI_REG((ZDI::ZDI_ADDR0_L)+(4*index)), 3, &buffer[0]);
}

uint32_t CPU::getBreakpoint(uint8_t index) {
    if(index > 3) return 0;
    return breakpoints[index];
}

void CPU::enableBreakpoint(uint8_t index) {
    if(index > 3) return;
    enabled_breakpoints[index] = true;

    debug_flags |= (0b00001000 << index);
    zdi->set_debugflags(debug_flags);
}

void CPU::disableBreakpoint(uint8_t index) {
    if(index > 3) return;
    enabled_breakpoints[index] = false;

    debug_flags &= (~(0b00001000 << index));
    zdi->set_debugflags(debug_flags);
}

bool CPU::isBreakpointEnabled(uint8_t index) {
    if(index > 3) return false;

    return enabled_breakpoints[index];
}

bool CPU::isRunning(void) {
    uint8_t status = zdi->read_register(ZDI::ZDI_STAT);
    return !(status & 0b10000000); // bit 7 ZDI mode == breakpoint reached
}

int8_t CPU::onBreakpoint(void) {
    uint8_t i;
    uint32_t pc = zdi->read_cpu_register(ZDI::pc);

    for(i = 0; i < MAXBREAKPOINTS; i++) {
        if(enabled_breakpoints[i] && (pc == breakpoints[i])) return i;
    }
    return -1;
}

bool CPU::onHaltSleep(void) {
    uint8_t status = zdi->read_register(ZDI::ZDI_STAT);
    return (status & 0b00100000); // bit 5 halt or sleep mode
}

bool CPU::getADLmode(void) {
    uint8_t status = zdi->read_register(ZDI::ZDI_STAT);
    return (status & 0b00010000); // bit 4 ADL mode
}

void CPU::setADLmode(bool adlmode) {
    if(adlmode) zdi->read_cpu_register(ZDI::set_adl);
    else zdi->read_cpu_register(ZDI::reset_adl);
}

bool CPU::getMADLmode(void) {
    uint8_t status = zdi->read_register(ZDI::ZDI_STAT);
    return (status & 0b00001000); // bit 3 MADL mode
}

bool CPU::getIEF1flag(void) {
    uint8_t status = zdi->read_register(ZDI::ZDI_STAT);
    return (status & 0b00000100); // bit 2 IEF1 flag
}

uint8_t CPU::a(void) {
    return zdi->read_cpu_register(ZDI::afmb) & 0xff;
}
void CPU::a(uint8_t value) {
    uint32_t afmb = zdi->read_cpu_register(ZDI::afmb);
    afmb = (afmb & 0xffff00) | value;
    zdi->write_cpu_register(ZDI::afmb, afmb);
}
uint8_t CPU::f(void) {
    return (zdi->read_cpu_register(ZDI::afmb) >> 8) & 0xff;
}
void CPU::f(uint8_t value) {
    uint32_t afmb = zdi->read_cpu_register(ZDI::afmb);
    afmb = (afmb & 0xff00ff) | (value << 8);
    zdi->write_cpu_register(ZDI::afmb, afmb);
}
uint8_t CPU::mbase(void) {
    return (zdi->read_cpu_register(ZDI::afmb) >> 16) & 0xff;
}
uint8_t CPU::b(void) {
    return (zdi->read_cpu_register(ZDI::bc) >> 8) & 0xff;
}
void CPU::b(uint8_t value) {
    uint32_t bc = zdi->read_cpu_register(ZDI::bc);
    bc = (bc & 0xff00ff) | (value << 8);
    zdi->write_cpu_register(ZDI::bc, bc);
}
uint8_t CPU::c(void) {
    return zdi->read_cpu_register(ZDI::bc) & 0xff;
}
void CPU::c(uint8_t value) {
    uint32_t bc = zdi->read_cpu_register(ZDI::bc);
    bc = (bc & 0xffff00) | value;
    zdi->write_cpu_register(ZDI::bc, bc);
}
uint8_t CPU::bcu(void) {
    return (zdi->read_cpu_register(ZDI::bc) >> 16) & 0xff;
}
void CPU::bcu(uint8_t value) {
    uint32_t bc = zdi->read_cpu_register(ZDI::bc);
    bc = (bc & 0x00ffff) | (value << 16);
    zdi->write_cpu_register(ZDI::bc, bc);
}
uint8_t CPU::d(void) {
    return (zdi->read_cpu_register(ZDI::de) >> 8) & 0xff;
}
void CPU::d(uint8_t value) {
    uint32_t de = zdi->read_cpu_register(ZDI::de);
    de = (de & 0xff00ff) | (value << 8);
    zdi->write_cpu_register(ZDI::de, de);
}
uint8_t CPU::e(void) {
    return zdi->read_cpu_register(ZDI::de) & 0xff;
}
void CPU::e(uint8_t value) {
    uint32_t de = zdi->read_cpu_register(ZDI::de);
    de = (de & 0xffff00) | value;
    zdi->write_cpu_register(ZDI::de, de);
}
uint8_t CPU::deu(void) {
    return (zdi->read_cpu_register(ZDI::de) >> 16) & 0xff;
}
void CPU::deu(uint8_t value) {
    uint32_t de = zdi->read_cpu_register(ZDI::de);
    de = (de & 0x00ffff) | (value << 16);
    zdi->write_cpu_register(ZDI::de, de);
}
uint8_t CPU::h(void) {
    return (zdi->read_cpu_register(ZDI::hl) >> 8) & 0xff;
}
void CPU::h(uint8_t value) {
    uint32_t hl = zdi->read_cpu_register(ZDI::hl);
    hl = (hl & 0xff00ff) | (value << 8);
    zdi->write_cpu_register(ZDI::hl, hl);
}
uint8_t CPU::l(void) {
    return zdi->read_cpu_register(ZDI::hl) & 0xff;
}
void CPU::l(uint8_t value) {
    uint32_t hl = zdi->read_cpu_register(ZDI::hl);
    hl = (hl & 0xffff00) | value;
    zdi->write_cpu_register(ZDI::hl, hl);
}
uint8_t CPU::hlu(void) {
    return (zdi->read_cpu_register(ZDI::hl) >> 16) & 0xff;
}
void CPU::hlu(uint8_t value) {
    uint32_t hl = zdi->read_cpu_register(ZDI::hl);
    hl = (hl & 0x00ffff) | (value << 16);
    zdi->write_cpu_register(ZDI::hl, hl);
}
uint32_t CPU::bc(void) {
    return zdi->read_cpu_register(ZDI::bc);
}
void CPU::bc(uint32_t value) {
    zdi->write_cpu_register(ZDI::bc, value);
}
uint32_t CPU::de(void) {
    return zdi->read_cpu_register(ZDI::de);
}
void CPU::de(uint32_t value) {
    zdi->write_cpu_register(ZDI::de, value);
}
uint32_t CPU::hl(void) {
    return zdi->read_cpu_register(ZDI::hl);
}
void CPU::hl(uint32_t value) {
    zdi->write_cpu_register(ZDI::hl, value);
}
uint8_t CPU::ixu(void) {
    return (zdi->read_cpu_register(ZDI::ix) >> 16) & 0xff;
}
void CPU::ixu(uint8_t value) {
    uint32_t ix = zdi->read_cpu_register(ZDI::ix);
    ix = (ix & 0x00ffff) | (value << 16);
    zdi->write_cpu_register(ZDI::ix, ix);
}
uint8_t CPU::ixh(void) {
    return (zdi->read_cpu_register(ZDI::ix) >> 8) & 0xff;
}
void CPU::ixh(uint8_t value) {
    uint32_t ix = zdi->read_cpu_register(ZDI::ix);
    ix = (ix & 0xff00ff) | (value << 8);
    zdi->write_cpu_register(ZDI::ix, ix);
}
uint8_t CPU::ixl(void) {
    return zdi->read_cpu_register(ZDI::ix) & 0xff;
}
void CPU::ixl(uint8_t value) {
    uint32_t ix = zdi->read_cpu_register(ZDI::ix);
    ix = (ix & 0xffff00) | value;
    zdi->write_cpu_register(ZDI::ix, ix);
}
uint32_t CPU::ix(void) {
    return zdi->read_cpu_register(ZDI::ix);
}
void CPU::ix(uint32_t value) {
    zdi->write_cpu_register(ZDI::ix, value);
}
uint8_t CPU::iyu(void) {
    return (zdi->read_cpu_register(ZDI::iy) >> 16) & 0xff;
}
void CPU::iyu(uint8_t value) {
    uint32_t iy = zdi->read_cpu_register(ZDI::iy);
    iy = (iy & 0x00ffff) | (value << 16);
    zdi->write_cpu_register(ZDI::iy, iy);
}
uint8_t CPU::iyh(void) {
    return (zdi->read_cpu_register(ZDI::iy) >> 8) & 0xff;
}
void CPU::iyh(uint8_t value) {
    uint32_t iy = zdi->read_cpu_register(ZDI::iy);
    iy = (iy & 0xff00ff) | (value << 8);
    zdi->write_cpu_register(ZDI::iy, iy);
}
uint8_t CPU::iyl(void) {
    return zdi->read_cpu_register(ZDI::iy) & 0xff;
}
void CPU::iyl(uint8_t value) {
    uint32_t iy = zdi->read_cpu_register(ZDI::iy);
    iy = (iy & 0xffff00) | value;
    zdi->write_cpu_register(ZDI::iy, iy);
}
uint32_t CPU::iy(void) {
    return zdi->read_cpu_register(ZDI::iy);
}
void CPU::iy(uint32_t value) {
    zdi->write_cpu_register(ZDI::iy, value);
}
uint32_t CPU::sp(void) {
    return zdi->read_cpu_register(ZDI::sp);
}
void CPU::sp(uint32_t value) {
    zdi->write_cpu_register(ZDI::sp, value);
}
uint32_t CPU::pc(void) {
    return zdi->read_cpu_register(ZDI::pc);
}
void CPU::pc(uint32_t value) {
    zdi->write_cpu_register(ZDI::pc, value);
}

void CPU::instruction_out(uint8_t port_address, uint8_t value) {
    // ld a, value
    zdi->write_register(ZDI::ZDI_IS1, value);
    zdi->write_register(ZDI::ZDI_IS0, 0x3e);
    // out (port_address), a
    zdi->write_register(ZDI::ZDI_IS1, port_address);
    zdi->write_register(ZDI::ZDI_IS0, 0xd3);
}

void CPU::instruction_di(void) {
    zdi->write_register(ZDI::ZDI_IS0, 0xf3);
}

void CPU::reset(void) {
    zdi->write_register(ZDI::ZDI_MASTER_CTL, 0x80);
}
void CPU::exx(void) {
    zdi->write_register(ZDI::ZDI_RW_CTL, ZDI::exx);
}