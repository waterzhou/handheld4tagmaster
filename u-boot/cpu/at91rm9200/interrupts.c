/*
 * (C) Copyright 2002
 * Lineo, Inc. <www.lineo.com>
 * Bernhard Kuhn <bkuhn@lineo.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/proc/ptrace.h>

extern void reset_cpu(ulong addr);

/* we always count down the max. */
#define TIMER_LOAD_VAL 0xffff

/* macro to read the 16 bit timer */
#define READ_TIMER (tmr->TC_CV)
AT91PS_TC tmr;



void enable_interrupts (void)
{
    return;
}
int disable_interrupts (void)
{
    return 0;
}


void bad_mode(void)
{
    panic("Resetting CPU ...\n");
    reset_cpu(0);
}

void show_regs(struct pt_regs * regs)
{
    unsigned long flags;
const char *processor_modes[]=
{ "USER_26", "FIQ_26" , "IRQ_26" , "SVC_26" , "UK4_26" , "UK5_26" , "UK6_26" , "UK7_26" ,
  "UK8_26" , "UK9_26" , "UK10_26", "UK11_26", "UK12_26", "UK13_26", "UK14_26", "UK15_26",
  "USER_32", "FIQ_32" , "IRQ_32" , "SVC_32" , "UK4_32" , "UK5_32" , "UK6_32" , "ABT_32" ,
  "UK8_32" , "UK9_32" , "UK10_32", "UND_32" , "UK12_32", "UK13_32", "UK14_32", "SYS_32"
};

    flags = condition_codes(regs);

    printf("pc : [<%08lx>]    lr : [<%08lx>]\n"
	   "sp : %08lx  ip : %08lx  fp : %08lx\n",
	   instruction_pointer(regs),
	   regs->ARM_lr, regs->ARM_sp,
	   regs->ARM_ip, regs->ARM_fp);
    printf("r10: %08lx  r9 : %08lx  r8 : %08lx\n",
	   regs->ARM_r10, regs->ARM_r9,
	   regs->ARM_r8);
    printf("r7 : %08lx  r6 : %08lx  r5 : %08lx  r4 : %08lx\n",
	   regs->ARM_r7, regs->ARM_r6,
	   regs->ARM_r5, regs->ARM_r4);
    printf("r3 : %08lx  r2 : %08lx  r1 : %08lx  r0 : %08lx\n",
	   regs->ARM_r3, regs->ARM_r2,
	   regs->ARM_r1, regs->ARM_r0);
    printf("Flags: %c%c%c%c",
	   flags & CC_N_BIT ? 'N' : 'n',
	   flags & CC_Z_BIT ? 'Z' : 'z',
	   flags & CC_C_BIT ? 'C' : 'c',
	   flags & CC_V_BIT ? 'V' : 'v');
    printf("  IRQs %s  FIQs %s  Mode %s%s\n",
	   interrupts_enabled(regs) ? "on" : "off",
	   fast_interrupts_enabled(regs) ? "on" : "off",
	   processor_modes[processor_mode(regs)],
	   thumb_mode(regs) ? " (T)" : "");
}

void do_undefined_instruction(struct pt_regs *pt_regs)
{
    printf("undefined instruction\n");
    show_regs(pt_regs);
    bad_mode();
}

void do_software_interrupt(struct pt_regs *pt_regs)
{
    printf("software interrupt\n");
    show_regs(pt_regs);
    bad_mode();
}

void do_prefetch_abort(struct pt_regs *pt_regs)
{
    printf("prefetch abort\n");
    show_regs(pt_regs);
    bad_mode();
}

void do_data_abort(struct pt_regs *pt_regs)
{
    printf("data abort\n");
    show_regs(pt_regs);
    bad_mode();
}

void do_not_used(struct pt_regs *pt_regs)
{
    printf("not used\n");
    show_regs(pt_regs);
    bad_mode();
}

void do_fiq(struct pt_regs *pt_regs)
{
    printf("fast interrupt request\n");
    show_regs(pt_regs);
    bad_mode();
}

void do_irq(struct pt_regs *pt_regs)
{
    printf("interrupt request\n");
    show_regs(pt_regs);
    bad_mode();
}

static ulong timestamp;
static ulong lastinc;

int interrupt_init (void)
{

    tmr = AT91C_BASE_TC0;

    /* enables TC1.0 clock */
    *AT91C_PMC_PCER = 1 << AT91C_ID_TC0;  /* enable clock */

    *AT91C_TCB0_BCR = 0;
    *AT91C_TCB0_BMR = AT91C_TCB_TC0XC0S_NONE | AT91C_TCB_TC1XC1S_NONE | AT91C_TCB_TC2XC2S_NONE;
    tmr->TC_CCR = AT91C_TC_CLKDIS;
    tmr->TC_CMR = AT91C_TC_TIMER_DIV1_CLOCK;  /* set to MCLK/2 */

    tmr->TC_IDR = ~0ul;
    tmr->TC_RC = TIMER_LOAD_VAL;
    lastinc = TIMER_LOAD_VAL;
    tmr->TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN;
    timestamp = 0;
    return (0);
}

/*
 * timer without interrupts
 */

void reset_timer(void)
{
    reset_timer_masked();
}

ulong get_timer (ulong base)
{
    return get_timer_masked() - base;
}

void set_timer (ulong t)
{
    timestamp = t;
}

void udelay(unsigned long usec)
{
    udelay_masked(usec);
}

void reset_timer_masked(void)
{
    /* reset time */
    lastinc = READ_TIMER;
    timestamp = 0;
}

ulong get_timer_masked(void)
{
    ulong now = READ_TIMER;
    if (now >= lastinc)
    {
        /* normal mode */
        timestamp += now - lastinc;
    } else {
        /* we have an overflow ... */
        timestamp += now + TIMER_LOAD_VAL - lastinc;
    }
    lastinc = now;

    return timestamp;
}

void udelay_masked(unsigned long usec)
{
    ulong tmo;

    tmo = usec / 1000;
    tmo *= CFG_HZ;
    tmo /= 1000;

    reset_timer_masked();

    while(get_timer_masked() < tmo);
      /*NOP*/;
}


