#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED
//
// isr.h -- Interface and structures for high level interrupt service routines.
// Part of this code is modified from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

// #include "common.h"

#include <stdint.h>

#ifdef __cplusplus
#define CONSTEXPR constexpr
extern "C" {
#else
#define CONSTEXPR const
#endif

typedef struct registers
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;


typedef struct registers64
{
   uint64_t ds;                  // Data segment selector
   uint64_t rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax; // Pushed by pusha.
   uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
   uint64_t int_no, err_code;    // Interrupt number and error code (if applicable)
   uint64_t rip, cs, rflags, userrsp, ss; // Pushed by the processor automatically.
} registers64_t;



// A few defines to make life a little easier
// for the remapped IRQ's
const uint8_t IRQ0 = 32u;
const uint8_t IRQ1 = 33u;
const uint8_t IRQ2 = 34u;
const uint8_t IRQ3 = 35u;
const uint8_t IRQ4 = 36u;
const uint8_t IRQ5 = 37u;
const uint8_t IRQ6 = 38u;
const uint8_t IRQ7 = 39u;
const uint8_t IRQ8 = 40u;
const uint8_t IRQ9 = 41u;
const uint8_t IRQ10 = 42u;
const uint8_t IRQ11 = 43u;
const uint8_t IRQ12 = 44u;
const uint8_t IRQ13 = 45u;
const uint8_t IRQ14 = 46u;
const uint8_t IRQ15 = 47u;


CONSTEXPR uint16_t PIC1 = 0x20u;		/* IO base address for master PIC */
CONSTEXPR uint16_t PIC2 = 0xA0u;		/* IO base address for slave PIC */
CONSTEXPR uint16_t MASTER_PIC_COMMAND = PIC1;
CONSTEXPR uint16_t MASTER_PIC_DATA = (PIC1+1);
CONSTEXPR uint16_t SLAVE_PIC_COMMAND = PIC2;
CONSTEXPR uint16_t SLAVE_PIC_DATA = (PIC2+1);

CONSTEXPR uint8_t PIC_READ_IRR = 0x0Au;    /* OCW3 irq ready next CMD read */
CONSTEXPR uint8_t PIC_READ_ISR = 0x0Bu;    /* OCW3 irq service next CMD read */

CONSTEXPR uint8_t PIC_RESET_COMMAND = 0x20u;

CONSTEXPR uint8_t PIC_EOI = 0x20u;		/* End-of-interrupt command code */


CONSTEXPR uint16_t ISR_DIV_BY_ZERO = 0x00u;
CONSTEXPR uint16_t ISR_DEBUG_TRAP = 0x01u;
CONSTEXPR uint16_t ISR_NMI_INTERRUPT = 0x02u;
CONSTEXPR uint16_t ISR_BREAKPOINT_TRAP = 0x03u;
CONSTEXPR uint16_t ISR_OVERFLOW_TRAP = 0x04u;
CONSTEXPR uint16_t ISR_BOUND_RANGE_FAULT = 0x05u;
CONSTEXPR uint16_t ISR_INVALID_OPCODE_FAULT = 0x06u;
CONSTEXPR uint16_t ISR_DEVICE_NOT_AVAILABLE_FAULT = 0x07u;
CONSTEXPR uint16_t ISR_DOUBLE_FAULT_ABORT = 0x08u;
CONSTEXPR uint16_t ISR_FPU_OVERRUN_FAULT = 0x09u;
CONSTEXPR uint16_t ISR_TSS_INVALID_FAULT = 0x0Au;
CONSTEXPR uint16_t ISR_SEGMENT_NOT_PRESENT_FAULT = 0x0Bu;
CONSTEXPR uint16_t ISR_STACK_SEGMENT_FAULT = 0x0Cu;
CONSTEXPR uint16_t ISR_GPF_FAULT = 0x0Du;
CONSTEXPR uint16_t ISR_PAGE_FAULT = 0x0Eu;
CONSTEXPR uint16_t ISR_RESERVED1 = 0x0Fu;
CONSTEXPR uint16_t ISR_FPU_EXCEPTION_FAULT = 0x10u;
CONSTEXPR uint16_t ISR_ALIGNMENT_CHECK_FAULT = 0x11u;
CONSTEXPR uint16_t ISR_MACHINE_CHECK_ABORT = 0x12u;
CONSTEXPR uint16_t ISR_SIMD_EXCEPTION_FAULT = 0x13u;
CONSTEXPR uint16_t ISR_VIRTUALIZATION_FAULT = 0x14u;
CONSTEXPR uint16_t ISR_SECURITY_EXCEPTION = 0x30u;
CONSTEXPR uint16_t ISR_FPU_ERROR_INTERRUPT = IRQ13;

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void (*isr_t)(registers_t);
typedef void (*isr64_t)(registers64_t);
void register_interrupt_handler(uint8_t n, isr_t handler);
void register_interrupt_handler64(uint8_t n, isr64_t handler);

extern isr_t interrupt_handlers[256] ;
extern isr64_t interrupt64_handlers[256] ;

void install_processor_handlers();

#ifdef __cplusplus
}
#endif

#endif // ISR_H_INCLUDED
