#ifndef KVM_UNIFDEF_H
#define KVM_UNIFDEF_H

#ifdef __i386__
#ifndef CONFIG_X86_32
#define CONFIG_X86_32 1
#endif
#endif

#ifdef __x86_64__
#ifndef CONFIG_X86_64
#define CONFIG_X86_64 1
#endif
#endif

#if defined(__i386__) || defined (__x86_64__)
#ifndef CONFIG_X86
#define CONFIG_X86 1
#endif
#endif

#ifdef __PPC__
#ifndef CONFIG_PPC
#define CONFIG_PPC 1
#endif
#endif

#ifdef __s390__
#ifndef CONFIG_S390
#define CONFIG_S390 1
#endif
#endif

#endif
// SPDX-License-Identifier: GPL-2.0-only
/*
 * irq.c: API for in kernel interrupt controller
 * Copyright (c) 2007, Intel Corporation.
 * Copyright 2009 Red Hat, Inc. and/or its affiliates.
 *
 * Authors:
 *   Yaozu (Eddie) Dong <Eddie.dong@intel.com>
 */

#include <linux/export.h>
#include <linux/kvm_host.h>

#include "irq.h"
#include "i8254.h"
#include "x86.h"

/*
 * check if there are pending timer events
 * to be processed.
 */
int kvm_cpu_has_pending_timer(struct kvm_vcpu *vcpu)
{
	if (lapic_in_kernel(vcpu))
		return apic_has_pending_timer(vcpu);

	return 0;
}
EXPORT_SYMBOL(kvm_cpu_has_pending_timer);

/*
 * check if there is a pending userspace external interrupt
 */
static int pending_userspace_extint(struct kvm_vcpu *v)
{
	return v->arch.pending_external_vector != -1;
}

/*
 * check if there is pending interrupt from
 * non-APIC source without intack.
 */
static int kvm_cpu_has_extint(struct kvm_vcpu *v)
{
	u8 accept = kvm_apic_accept_pic_intr(v);

	if (accept) {
		if (irqchip_split(v->kvm))
			return pending_userspace_extint(v);
		else
			return v->kvm->arch.vpic->output;
	} else
		return 0;
}

/*
 * check if there is injectable interrupt:
 * when virtual interrupt delivery enabled,
 * interrupt from apic will handled by hardware,
 * we don't need to check it here.
 */
int kvm_cpu_has_injectable_intr(struct kvm_vcpu *v)
{
	/*
	 * FIXME: interrupt.injected represents an interrupt that it's
	 * side-effects have already been applied (e.g. bit from IRR
	 * already moved to ISR). Therefore, it is incorrect to rely
	 * on interrupt.injected to know if there is a pending
	 * interrupt in the user-mode LAPIC.
	 * This leads to nVMX/nSVM not be able to distinguish
	 * if it should exit from L2 to L1 on EXTERNAL_INTERRUPT on
	 * pending interrupt or should re-inject an injected
	 * interrupt.
	 */
	if (!lapic_in_kernel(v))
		return v->arch.interrupt.injected;

	if (kvm_cpu_has_extint(v))
		return 1;

	if (!is_guest_mode(v) && kvm_vcpu_apicv_active(v))
		return 0;

	return kvm_apic_has_interrupt(v) != -1; /* LAPIC */
}

/*
 * check if there is pending interrupt without
 * intack.
 */
int kvm_cpu_has_interrupt(struct kvm_vcpu *v)
{
	/*
	 * FIXME: interrupt.injected represents an interrupt that it's
	 * side-effects have already been applied (e.g. bit from IRR
	 * already moved to ISR). Therefore, it is incorrect to rely
	 * on interrupt.injected to know if there is a pending
	 * interrupt in the user-mode LAPIC.
	 * This leads to nVMX/nSVM not be able to distinguish
	 * if it should exit from L2 to L1 on EXTERNAL_INTERRUPT on
	 * pending interrupt or should re-inject an injected
	 * interrupt.
	 */
	if (!lapic_in_kernel(v))
		return v->arch.interrupt.injected;

	if (kvm_cpu_has_extint(v))
		return 1;

	return kvm_apic_has_interrupt(v) != -1;	/* LAPIC */
}
EXPORT_SYMBOL_GPL(kvm_cpu_has_interrupt);

/*
 * Read pending interrupt(from non-APIC source)
 * vector and intack.
 */
static int kvm_cpu_get_extint(struct kvm_vcpu *v)
{
	if (kvm_cpu_has_extint(v)) {
		if (irqchip_split(v->kvm)) {
			int vector = v->arch.pending_external_vector;

			v->arch.pending_external_vector = -1;
			return vector;
		} else
			return kvm_pic_read_irq(v->kvm); /* PIC */
	} else
		return -1;
}

/*
 * Read pending interrupt vector and intack.
 */
int kvm_cpu_get_interrupt(struct kvm_vcpu *v)
{
	int vector;

	if (!lapic_in_kernel(v))
		return v->arch.interrupt.nr;

	vector = kvm_cpu_get_extint(v);

	if (vector != -1)
		return vector;			/* PIC */

	return kvm_get_apic_interrupt(v);	/* APIC */
}
EXPORT_SYMBOL_GPL(kvm_cpu_get_interrupt);

void kvm_inject_pending_timer_irqs(struct kvm_vcpu *vcpu)
{
	if (lapic_in_kernel(vcpu))
		kvm_inject_apic_timer_irqs(vcpu);
}
EXPORT_SYMBOL_GPL(kvm_inject_pending_timer_irqs);

void __kvm_migrate_timers(struct kvm_vcpu *vcpu)
{
	__kvm_migrate_apic_timer(vcpu);
	__kvm_migrate_pit_timer(vcpu);
}

bool kvm_arch_irqfd_allowed(struct kvm *kvm, struct kvm_irqfd *args)
{
	bool resample = args->flags & KVM_IRQFD_FLAG_RESAMPLE;

	return resample ? irqchip_kernel(kvm) : irqchip_in_kernel(kvm);
}
