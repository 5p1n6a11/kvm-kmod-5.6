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
/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) 2019 Arm Ltd. */

#ifndef __KVM_ARM_HYPERCALLS_H
#define __KVM_ARM_HYPERCALLS_H

#include <asm/kvm_emulate.h>

int kvm_hvc_call_handler(struct kvm_vcpu *vcpu);

static inline u32 smccc_get_function(struct kvm_vcpu *vcpu)
{
	return vcpu_get_reg(vcpu, 0);
}

static inline unsigned long smccc_get_arg1(struct kvm_vcpu *vcpu)
{
	return vcpu_get_reg(vcpu, 1);
}

static inline unsigned long smccc_get_arg2(struct kvm_vcpu *vcpu)
{
	return vcpu_get_reg(vcpu, 2);
}

static inline unsigned long smccc_get_arg3(struct kvm_vcpu *vcpu)
{
	return vcpu_get_reg(vcpu, 3);
}

static inline void smccc_set_retval(struct kvm_vcpu *vcpu,
				    unsigned long a0,
				    unsigned long a1,
				    unsigned long a2,
				    unsigned long a3)
{
	vcpu_set_reg(vcpu, 0, a0);
	vcpu_set_reg(vcpu, 1, a1);
	vcpu_set_reg(vcpu, 2, a2);
	vcpu_set_reg(vcpu, 3, a3);
}

#endif
