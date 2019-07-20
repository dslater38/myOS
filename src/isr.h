#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED
//
// isr.h -- Interface and structures for high level interrupt service routines.
// Part of this code is modified from Bran's kernel development tutorials.
// Rewritten for JamesM's kernel development tutorials.
//

#include "common.h"

#ifdef __cplusplus
extern "C" {
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
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47


#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define MASTER_PIC_COMMAND	PIC1
#define MASTER_PIC_DATA	(PIC1+1)
#define SLAVE_PIC_COMMAND	PIC2
#define SLAVE_PIC_DATA	(PIC2+1)

#define PIC_RESET_COMMAND 0x20

#define PIC_EOI		0x20		/* End-of-interrupt command code */


constexpr uint16_t ISR_DIV_BY_ZERO = 0x00;
constexpr uint16_t ISR_DEBUG_TRAP = 0x01;
constexpr uint16_t ISR_NMI_INTERRUPT = 0x02;
constexpr uint16_t ISR_BREAKPOINT_TRAP = 0x03;
constexpr uint16_t ISR_OVERFLOW_TRAP = 0x04;
constexpr uint16_t ISR_BOUND_RANGE_FAULT = 0x05;
constexpr uint16_t ISR_INVALID_OPCODE_FAULT = 0x06;
constexpr uint16_t ISR_DEVICE_NOT_AVAILABLE_FAULT = 0x07;
constexpr uint16_t ISR_DOUBLE_FAULT_ABORT = 0x08;
constexpr uint16_t ISR_FPU_OVERRUN_FAULT = 0x09;
constexpr uint16_t ISR_TSS_INVALID_FAULT = 0x0A;
constexpr uint16_t ISR_SEGMENT_NOT_PRESENT_FAULT = 0x0B;
constexpr uint16_t ISR_STACK_SEGMENT_FAULT = 0x0C;
constexpr uint16_t ISR_GPF_FAULT = 0x0D;
constexpr uint16_t ISR_PAGE_FAULT = 0x0E;
constexpr uint16_t ISR_RESERVED1 = 0x0F;
constexpr uint16_t ISR_FPU_EXCEPTION_FAULT = 0x10;
constexpr uint16_t ISR_ALIGNMENT_CHECK_FAULT = 0x11;
constexpr uint16_t ISR_MACHINE_CHECK_ABORT = 0x12;
constexpr uint16_t ISR_SIMD_EXCEPTION_FAULT = 0x13;
constexpr uint16_t ISR_VIRTUALIZATION_FAULT = 0x14;
constexpr uint16_t ISR_SECURITY_EXCEPTION = 0x30;
constexpr uint16_t ISR_FPU_ERROR_INTERRUPT = IRQ13;

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
