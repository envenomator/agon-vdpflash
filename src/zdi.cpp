/*
 * Title:           Electron - HAL
 *                  a playful alternative to Quark
 *                  quarks and electrons combined are matter.
 * Author:          Mario Smit (S0urceror)
 *                  Modified for specific use by Jeroen Venema (envenomator)
 */

// ZDI library
#include "zdi.h"

byte debug_flags = 0x00;
bool zdi_mode_flag = false;
uint8_t upper_address = 0x00;
byte breakpoints[4][3];
bool enabled_breakpoints[4];

// low-level bit stream
///////////////////////
void zdi_start ()
{
    // TCK
    digitalWrite (ZDI_TCK,HIGH);
    usleep (ZDI_WAIT_MICRO);
    // TDI
    digitalWrite (ZDI_TDI,LOW);
    pinMode (ZDI_TDI, OUTPUT);
    usleep (ZDI_WAIT_MICRO);

    // TCK: xx^^^^^^^^^
    // TDI: x^^^\\*____
}
void zdi_write_bit (bool bit)
{
    // TCK: ^\\___*//^^
    // TDI: xxxxxBBBBBB
    digitalWrite (ZDI_TCK,LOW);
    usleep (ZDI_WAIT_MICRO);
    //
    digitalWrite (ZDI_TDI,bit);
    pinMode (ZDI_TDI, OUTPUT);
    usleep (ZDI_WAIT_MICRO);
    //
    digitalWrite (ZDI_TCK,HIGH);
    usleep (ZDI_WAIT_MICRO);
}
bool zdi_read_bit ()
{
    bool bit;

    // TCK: ^\\*___//^^
    // TDI: xxxxBBBBBBB
    digitalWrite (ZDI_TCK,LOW);
    usleep (ZDI_WAIT_MICRO);
    //
    pinMode (ZDI_TDI, INPUT);
    bit = digitalRead (ZDI_TDI);
    usleep (ZDI_WAIT_MICRO);
    //
    digitalWrite (ZDI_TCK,HIGH);
    usleep (ZDI_WAIT_MICRO);
    return bit;
}
void zdi_register (byte regnr,bool read)
{
    // write 7-bits, msb to lsb order
    for (int i=6;i>=0;i--)
    {
        zdi_write_bit (regnr & (0b1<<i));
    }
    // write read/write bit
    zdi_write_bit (read);
}
void zdi_separator (bool done_or_continue)
{
    zdi_write_bit (done_or_continue);
}
void zdi_write (byte value)
{
    // write 8-bits, msb to lsb order
    for (int i=7;i>=0;i--)
    {
        zdi_write_bit (value & (0b1<<i));
    }
}
byte zdi_read ()
{
    byte value = 0;
    // read 8-bits, msb to lsb order
    for (int i=7;i>=0;i--)
    {
        value = value << 1;
        value = value | zdi_read_bit ();
    }
    return value;
}

// medium-level register read and writes
///////////////////////

byte zdi_read_register (byte regnr)
{
    byte value;
    // TCK: xx\\*__//^^
    // TDI: xxxxxBBBBBB

    // TCK: xx^^^^^^^^ xxx___*//^^ ^^^___*//^^ ^^^___*//^^ ^^^___*//^^ ^^\\*__//^^
    // TDI: x^^^\\*___ xxxxxBBBBBB xxxxxBBBBBB xxxxxBBBBBB xxxxxBBBBBB xxxxxBBBBBB
    zdi_start ();
    zdi_register (regnr,ZDI_READ);
    zdi_separator (ZDI_CMD_CONTINUE);
    value  = zdi_read ();
    zdi_separator (ZDI_CMD_DONE);

    return value;
}
void zdi_write_register (byte regnr, byte value)
{
    zdi_start ();
    zdi_register (regnr,ZDI_WRITE);
    zdi_separator (ZDI_CMD_CONTINUE);
    zdi_write(value);
    zdi_separator (ZDI_CMD_DONE);
}

void zdi_read_registers (byte startregnr, byte count, byte* values)
{
    byte* ptr = values;

    zdi_start ();
    zdi_register (startregnr,ZDI_READ);
    while (count-- > 0)
    {
        zdi_separator (ZDI_CMD_CONTINUE);
        *(ptr++)  = zdi_read ();
    }
    zdi_separator (ZDI_CMD_DONE);
}

void zdi_write_registers (byte startregnr, byte count, byte* values)
{
    byte* ptr = values;

    zdi_start ();
    zdi_register (startregnr,ZDI_WRITE);
    while (count-- > 0)
    {
        zdi_separator (ZDI_CMD_CONTINUE);
        zdi_write (*(ptr++));
    }
    zdi_separator (ZDI_CMD_DONE);
}

// high-level debugging, register and memory read functions
///////////////////////
uint16_t zdi_get_productid ()
{
    byte pids[2];
    zdi_read_registers (ZDI_ID_L,2,pids);
    return (pids[1]<<8)+pids[0];
}
uint8_t zdi_get_revision ()
{
    return zdi_read_register (ZDI_ID_REV);
}
uint8_t zdi_get_cpu_status ()
{
    return zdi_read_register (ZDI_STAT);
}
uint8_t zdi_get_bus_status ()
{
    return zdi_read_register (ZDI_BUS_STAT);
}
uint32_t zdi_read_cpu (rw_control_t rw)
{
    zdi_write_register (ZDI_RW_CTL,rw);
    byte values[3];
    zdi_read_registers (ZDI_RD_L,3,values);
    return (values[2]<<16)+(values[1]<<8)+values[0]; // U+H+L
}
void zdi_write_cpu (rw_control_t rw,uint32_t value)
{
    byte values[3];
    values[0] = value & 0xff;       // L
    values[1] = (value>>8) & 0xff;  // H
    values[2] = (value>>16) & 0xff; // U
    zdi_write_registers (ZDI_WR_DATA_L,3,values);
    zdi_write_register (ZDI_RW_CTL,rw | 0b10000000); // set high bit to indicate write
}
void zdi_read_memory (uint32_t address,uint16_t count, byte* memory)
{
    byte* ptr = memory;
    // set start address
    zdi_write_cpu (REG_PC,address);
    // commence read from auto-increment read memory byte register
    zdi_start ();
    zdi_register (ZDI_RD_MEM,ZDI_READ);
    // one by one
    for (int i=0;i<count;i++)
    {
        zdi_separator (ZDI_CMD_CONTINUE);
        *(ptr++) = zdi_read ();
    }
    // done
    zdi_separator (ZDI_CMD_DONE);
}
void zdi_write_memory (uint32_t address,uint32_t count, byte* memory)
{
    byte* ptr = memory;

    zdi_write_cpu (REG_PC,address);
    // commence write from auto-increment read memory byte register
    zdi_start ();
    zdi_register (ZDI_WR_MEM,ZDI_WRITE);
    for (uint32_t i=0;i<count;i++)
    {
        zdi_separator (ZDI_CMD_CONTINUE);
        zdi_write (*(ptr++));
    }
    // done
    zdi_separator (ZDI_CMD_DONE);
}
void zdi_debug_break ()
{
    debug_flags |= 0b10000000;
    zdi_write_register (ZDI_BRK_CTL, debug_flags);
}
void zdi_debug_continue ()
{
    debug_flags &= 0b01111110;
    zdi_write_register (ZDI_BRK_CTL, debug_flags);
}
void zdi_debug_step ()
{
    // set single-step bit
    debug_flags |= 0b00000001;
    zdi_write_register (ZDI_BRK_CTL, debug_flags);
}
bool zdi_debug_breakpoint_reached ()
{
    byte status = zdi_get_cpu_status ();
    return (status & 0b10000000); // ZDI mode means breakpoint
}
void zdi_debug_breakpoint_enable (uint8_t index,uint32_t address)
{
    // set bp address
    byte bp[3] = {  (byte)(address & 0xff),
                    (byte)((address>>8) & 0xff),
                    (byte)((address>>16) & 0xff)
                 };
    
    memcpy(&breakpoints[index], bp, 3);
    enabled_breakpoints[index] = true;
    zdi_write_registers (ZDI_ADDR0_L+4*index,3,bp);
    // enable bp
    debug_flags|=(0b00001000 << index);
    zdi_write_register (ZDI_BRK_CTL, debug_flags);
}
void zdi_debug_breakpoint_disable (uint8_t index)
{
    byte mask = (0b00001000 << index);
    debug_flags &= (~mask);
    enabled_breakpoints[index] = false;
    zdi_write_register (ZDI_BRK_CTL, debug_flags);
}
uint8_t zdi_available_break_point()
{
    uint8_t i;
    byte mask = 0b00001000;
    for (i=0;i<4;i++)
    {
        if (!(debug_flags & (mask << i)))
            break;
    }
    if (i==4)
        return 0xff;

    return i;
}
void zdi_reset ()
{
    zdi_write_register (ZDI_MASTER_CTL, 0b10000000);
}
bool zdi_mode ()
{
    return zdi_mode_flag;
}
void zdi_enter ()
{
    digitalWrite (ZDI_TCK,HIGH);
    pinMode (ZDI_TCK, OUTPUT);

    // Clear breakpoints
    uint8_t bp,n;
    for(n = 0; n < 4; n++) {
        for(bp = 0; bp < 3; bp++) breakpoints[n][bp] = 0;
        enabled_breakpoints[n] = false;
    }
}
void zdi_exit ()
{
    // remove all breakpoints
    for (uint8_t i=0;i<4;i++)
        zdi_debug_breakpoint_disable (i);

    // back to running mode if we are at breakpoint
    if (zdi_debug_breakpoint_reached())
        zdi_debug_continue ();

    zdi_mode_flag = false;
}

void zdi_cpu_instruction_out (uint8_t regnr, uint8_t value) {   

    // ld a, value
    zdi_write_register(ZDI_IS1, value);
    zdi_write_register(ZDI_IS0, 0x3e);
    // out (regnr), a
    zdi_write_register(ZDI_IS1, regnr);
    zdi_write_register(ZDI_IS0, 0xd3);

/*
    // ld a, nn
    uint8_t instructions[3];
    instructions[0]=value;
    instructions[1]=0x3e;
    zdi_write_registers (ZDI_IS1,2,instructions);
    // out0 (nn),a
    instructions[0]=regnr;
    instructions[1]=0x39;
    instructions[2]=0xed;
    zdi_write_registers (ZDI_IS2,3,instructions);
*/
/*
    zdi_write_register(ZDI_IS1, value);
    zdi_write_register(ZDI_IS0, 0x3e);
    zdi_write_register(ZDI_IS2, regnr);
    zdi_write_register(ZDI_IS1, 0x39);
    zdi_write_register(ZDI_IS0, 0xed);
*/
}

void zdi_cpu_instruction_di ()
{
    // di
    zdi_write_register (ZDI_IS0,0xf3);
}

