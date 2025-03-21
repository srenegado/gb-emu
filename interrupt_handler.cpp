#include "interrupt_handler.h"

InterruptHandler::InterruptHandler(
    Registers &regs_, CpuContext &ctx_, MemoryBus &bus_
) : regs(regs_), ctx(ctx_), bus(bus_) {}
InterruptHandler::~InterruptHandler() {}

void InterruptHandler::service_interrupt(interrupt_type type) {
    bus.emulate_cycles(2);

    // Choose the right interrupt
    u16 int_handler_addr = 0x00;
    u8 disable = 0x00; 
    switch (type) {
        case (Int_Joypad):   int_handler_addr = 0x60; disable = ~0x10; break;
        case (Int_Serial):   int_handler_addr = 0x58; disable = ~0x08; break;
        case (Int_Timer):    int_handler_addr = 0x50; disable = ~0x04; break;
        case (Int_LCD_STAT): int_handler_addr = 0x48; disable = ~0x02; break;
        case (Int_VBlank):   int_handler_addr = 0x40; disable = ~0x01; break;
    }

    bus.set_IF(bus.get_IF() & disable);  // Acknowledge interrupt
    ctx.IME = 0;                         // Prevent any further interrupts
    ctx.halted = false;                  // CPU resumes after interrupt handling

    // Call interrupt handler
    bus.write(--regs.SP, (regs.PC >> 8) & 0xFF);
    bus.emulate_cycles(1);
    bus.write(--regs.SP, regs.PC & 0xFF);
    bus.emulate_cycles(1);

    regs.PC = int_handler_addr;
    bus.emulate_cycles(1);
} 

void InterruptHandler::handle_interrupts() {
    u8 IF = bus.get_IF();
    u8 IE = bus.get_IE();

    bool joypad_requested = IF & 0x10;
    bool serial_requested = IF & 0x08;
    bool timer_requested  = IF & 0x04;
    bool LCD_requested    = IF & 0x02;
    bool VBlank_requested = IF & 0x01;
    bool joypad_enabled = IE & 0x10;
    bool serial_enabled = IE & 0x08;
    bool timer_enabled  = IE & 0x04;
    bool LCD_enabled    = IE & 0x02;
    bool VBlank_enabled = IE & 0x01;

    // Highest priority is VBlank while lowest is Joypad
    if (VBlank_enabled && VBlank_requested) {
        // std::cout << "Servicing VBlank interrupt\n";
        service_interrupt(Int_VBlank);
    } 
    else if (LCD_enabled && LCD_requested) {
        // std::cout << "Servicing LCD_STAT interrupt\n";
        service_interrupt(Int_LCD_STAT);
    }
    else if (timer_enabled && timer_requested) {
        // std::cout << "Servicing Timer interrupt\n";
        service_interrupt(Int_Timer);
    }
    else if (serial_enabled && serial_requested) {
        // std::cout << "Servicing Serial interrupt\n";
        service_interrupt(Int_Serial);
    }
    else if (joypad_enabled && joypad_requested) {
        // std::cout << "Servicing Joypad interrupt\n";
        service_interrupt(Int_Joypad);
    }
}