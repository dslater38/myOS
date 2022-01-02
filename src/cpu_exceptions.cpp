#include "isr.h"
#include "vesavga.h"

static void divide_by_zero_fault(registers64_t);
static void debug_trap(registers64_t);
static void NMI_interrupt(registers64_t);
static void breakpoint_trap(registers64_t);
static void overflow_trap(registers64_t);
static void bound_range_fault(registers64_t);
static void invalid_opcode_fault(registers64_t);
static void device_not_available_fault(registers64_t);
static void double_fault_abort(registers64_t);
static void fpu_overrun_fault(registers64_t);
static void TSS_invalid_fault(registers64_t);
static void segment_not_present_fault(registers64_t);
static void stack_segment_fault(registers64_t);
static void general_protection_fault(registers64_t);
static void page_fault(registers64_t);
static void reserved(registers64_t);
static void FPU_exception_fault(registers64_t);
static void alignment_check_fault(registers64_t);
static void machine_check_abort(registers64_t);
static void SIMD_exception_fault(registers64_t);
static void virtualization_fault(registers64_t);
static void security_exception(registers64_t);
static void FPU_error_interrupt(registers64_t);

struct IsrDesc
{
    uint16_t num;
    isr64_t  handler;
};

void install_processor_handlers()
{
    IsrDesc handlers[] = {
        { ISR_DIV_BY_ZERO, divide_by_zero_fault },
        { ISR_DEBUG_TRAP, debug_trap },
        { ISR_NMI_INTERRUPT, NMI_interrupt },
        { ISR_BREAKPOINT_TRAP, breakpoint_trap },
        { ISR_OVERFLOW_TRAP, overflow_trap },
        { ISR_BOUND_RANGE_FAULT, bound_range_fault },
        { ISR_INVALID_OPCODE_FAULT, invalid_opcode_fault },
        { ISR_DEVICE_NOT_AVAILABLE_FAULT, device_not_available_fault },
        { ISR_DOUBLE_FAULT_ABORT, double_fault_abort },
        { ISR_FPU_OVERRUN_FAULT, fpu_overrun_fault },
        { ISR_TSS_INVALID_FAULT, TSS_invalid_fault },
        { ISR_SEGMENT_NOT_PRESENT_FAULT, segment_not_present_fault },
        { ISR_STACK_SEGMENT_FAULT, stack_segment_fault },
        { ISR_GPF_FAULT, general_protection_fault },
        { ISR_PAGE_FAULT, page_fault },
        { ISR_FPU_EXCEPTION_FAULT, FPU_exception_fault },
        { ISR_ALIGNMENT_CHECK_FAULT, alignment_check_fault },
        { ISR_MACHINE_CHECK_ABORT, machine_check_abort },
        { ISR_SIMD_EXCEPTION_FAULT, SIMD_exception_fault },
        { ISR_VIRTUALIZATION_FAULT, virtualization_fault },
        { ISR_SECURITY_EXCEPTION, security_exception }
    };
    constexpr size_t num_entries = sizeof(handlers)/sizeof(handlers[0]);
    for( auto i=0u; i<num_entries; ++i)
    {
        const IsrDesc &entry = handlers[i];
        register_interrupt_handler64(entry.num, entry.handler);
    }
}


static void divide_by_zero_fault(registers64_t)
{
    PANIC("Divide By Zero.");
}

static void debug_trap(registers64_t)
{
    PANIC("Debug Trap.");
}

static void NMI_interrupt(registers64_t)
{
    PANIC("NMI Interrupt.");
}

static void breakpoint_trap(registers64_t)
{
    PANIC("Breakpoint Trap.");
}


static void overflow_trap(registers64_t)
{
    PANIC("Overflow Trap.");
}


static void bound_range_fault(registers64_t)
{
    PANIC("Bound Range Fault.");
}

static void invalid_opcode_fault(registers64_t regs)
{
    PANIC1("Invalid Opcode Fault, RIP 0x%016.16lx", regs.rip);
}

static void device_not_available_fault(registers64_t)
{
    PANIC("FPU Device Not Available Fault.");
}

static void double_fault_abort(registers64_t)
{
    PANIC("Double Fault.");
}

static void fpu_overrun_fault(registers64_t)
{
    PANIC("FPU Overrun Fault.");
}

static void TSS_invalid_fault(registers64_t)
{
    PANIC("Invalid TSS Fault.");
}

static void segment_not_present_fault(registers64_t)
{
    PANIC("Segment Not Presetn Fault.");
}

static void stack_segment_fault(registers64_t)
{
    PANIC("Stack Segment Fault.");
}

static void general_protection_fault(registers64_t regs)
{
    if( regs.err_code != 0)
    {
        printf("General Protection Fault");
        const auto code = regs.err_code;
        printf("\tSegment Error: code: 0x%lx\n", code);

        if( code & 0x01)
        {
            printf("\tExternally Generated Exception.\n");
        }
        const auto table = ((code>>1) & 0x03);
        const char *strTable = nullptr;
        switch(table)
        {
            case 0:
                strTable = "GDT ";
                break;
            case 1:
                strTable = "IDT ";
                break;
            case 2:
                strTable = "LDT ";
                break;
            case 3:
                strTable = "IDT ";
                break;
        }
        uint16_t selector_index = ((code >> 3) & 0x1FFF);
        printf("Accessing: %s, Selector Index: 0x%04.4x\n", strTable, selector_index);
    }

    PANIC1("General Protection Fault, RIP 0x%016.16lx", regs.rip);
}

static void page_fault(registers64_t regs)
{
    // Output an error message.
    //monitor_write("Page fault! ( ");
    char buf[64] = {0};
    // A page fault has occurred.
    // The faulting address is stored in the CR2 register.
    uint64_t faulting_address = get_fault_addr64();
    // uint32_t faulting_address;
    // asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    // The error code gives us details of what happened.
    int present = (regs.err_code & 0x1);    // Page not present
    int rw = regs.err_code & 0x2;           // Write operation?
    int us = regs.err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

    // Output an error message.
    printf("Page fault! at 0x%016.16lX executing: 0x%016.16lX\n\t( %s %s %s %s %s )\n", 
        faulting_address,
        regs.rip,
        present ? "present" : "absent",
        rw ? "write" : "read",
        us ? "user-mode" : "kernel-mode",
        reserved ? "reserved" : "not-reserved",
        id  ? "fetch" : "data"
       );

    PANIC("Page fault");

}

static void reserved(registers64_t)
{
    PANIC("Reserved 1.");
}

static void FPU_exception_fault(registers64_t)
{
    PANIC("FPU Exception Fault");
}

static void alignment_check_fault(registers64_t)
{
    PANIC("Alignment Check Fault.");
}

static void machine_check_abort(registers64_t)
{
    PANIC("Machine Check Abort.");
}

static void SIMD_exception_fault(registers64_t)
{
    PANIC("SIMD Exception Fault.");
}

static void virtualization_fault(registers64_t)
{
    PANIC("Virtualization Fault.");
}

static void security_exception(registers64_t)
{
    PANIC("Security Exception.");
}


static void FPU_error_interrupt(registers64_t)
{
    PANIC("FPU Error Interrupt.");
}

