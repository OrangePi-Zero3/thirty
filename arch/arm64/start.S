/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration settings for the Allwinner A64 (sun50i) CPU
 */
.section .start, "ax"
.global _start
.global return_to_fel
.global fel_stash
_start:

/*
 * Switch into AArch64 if needed.
 * Refer to arch/arm/mach-sunxi/rmr_switch.S for the original source.
 */
	tst     x0, x0                  // this is "b #0x84" in ARM
	b       reset
	.space  0x78
	.word	fel_stash - .

	.word	0xe24f000c	// sub     r0, pc, #12  // @(fel_stash - .)
	.word	0xe51f1010	// ldr     r1, [pc, #-16] // fel_stash - .
	.word	0xe0800001	// add     r0, r0, r1
	.word	0xe580d000	// str     sp, [r0]
	.word	0xe580e004	// str     lr, [r0, #4]
	.word	0xe10fe000	// mrs     lr, CPSR
	.word	0xe580e008	// str     lr, [r0, #8]
	.word	0xe101e300	// mrs     lr, SP_irq
	.word	0xe580e014	// str     lr, [r0, #20]
	.word	0xee11ef10	// mrc     15, 0, lr, cr1, cr0, {0}
	.word	0xe580e00c	// str     lr, [r0, #12]
	.word	0xee1cef10	// mrc     15, 0, lr, cr12, cr0, {0}
	.word	0xe580e010	// str     lr, [r0, #16]
	.word	0xe59f1034	// ldr     r1, [pc, #52] ; RVBAR_ADDRESS
	.word	0xe59f0034	// ldr     r0, [pc, #52] ; SUNXI_SRAMC_BASE
	.word	0xe5900024	// ldr     r0, [r0, #36] ; SRAM_VER_REG
	.word	0xe21000ff	// ands    r0, r0, #255    ; 0xff
	.word	0x159f102c	// ldrne   r1, [pc, #44] ; RVBAR_ALTERNATIVE
	.word	0xe59f002c	// ldr     r0, [pc, #44] ; CONFIG_*TEXT_BASE
	.word	0xe5810000	// str     r0, [r1]
	.word	0xf57ff04f	// dsb     sy
	.word	0xf57ff06f	// isb     sy
	.word	0xee1c0f50	// mrc     15, 0, r0, cr12, cr0, {2} ; RMR
	.word	0xe3800003	// orr     r0, r0, #3
	.word	0xee0c0f50	// mcr     15, 0, r0, cr12, cr0, {2} ; RMR
	.word	0xf57ff06f	// isb     sy
	.word	0xe320f003	// wfi
	.word	0xeafffffd	// b       @wfi

	.word	0x09010040
	.word	0x03000000
	.word	0x08100040
	.word	0x00020060

	.balign 4
fel_stash:
	.skip	24

reset:
	ldr	x0, =0x00050000
	mov	sp, x0

	bl	main

hang:
	wfi
	b	hang

/*
 * The FEL routines in BROM run in AArch32.
 * Reset back into 32-bit mode here and restore the saved FEL state
 * afterwards.
 * Resetting back into AArch32/EL3 using the RMR always enters the BROM,
 * but we can use the CPU hotplug mechanism to branch back to our code
 * immediately.
 */
return_to_fel:
	/*
	 * the RMR reset will clear all registers, so save the arguments
	 * (LR and SP) in the fel_stash structure, which we read anyways later
	 */
	adr	x2, fel_stash
	str	w0, [x2]
	str	w1, [x2, #4]

	adr	x1, fel_stash_addr	// to find the fel_stash address in AA32
	str	w2, [x1]

	ldr	w0, =0xfa50392f		// CPU hotplug magic
	ldr	w2, =(0x07000400 + 0x1c0)
	str	w0, [x2], #0x4
	adr	x0, back_in_32
	str	w0, [x2]

	dsb	sy
	isb	sy
	mov	x0, #2			// RMR reset into AArch32
	dsb	sy
	msr	RMR_EL3, x0
	isb	sy
1:	wfi
	b	1b

fel_stash_addr:			// must immediately precede back_in_32:
	.word   0x00000000	// receives fel_stash addr, by AA64 code above

/* AArch32 code to restore the state from fel_stash and return back to FEL. */
back_in_32:
	.word	0xe51f000c	// ldr	r0, [pc, #-12]  ; load fel_stash address
	.word	0xe5901008	// ldr	r1, [r0, #8]
	.word	0xe129f001	// msr	CPSR_fc, r1
	.word	0xf57ff06f	// isb
	.word	0xe590d000	// ldr	sp, [r0]
	.word	0xe590e004	// ldr	lr, [r0, #4]
	.word	0xe5901014	// ldr  r1, [r0, #20]
	.word	0xe121f301	// msr  SP_irq, r1
	.word	0xe5901010	// ldr	r1, [r0, #16]
	.word	0xee0c1f10	// mcr	15, 0, r1, cr12, cr0, {0} ; VBAR
	.word	0xe590100c	// ldr	r1, [r0, #12]
	.word	0xee011f10	// mcr	15, 0, r1, cr1, cr0, {0}  ; SCTLR
	.word	0xf57ff06f	// isb
	.word	0xe12fff1e	// bx	lr		; return to FEL