#!/bin/sh

echo "rmmod kvm_intel"
sudo rmmod kvm_intel

echo "rmmod kvm"
sudo rmmod kvm

echo "insmod kvm"
sudo insmod x86/kvm.ko

echo "insmod kvm-intel"
sudo insmod x86/kvm-intel.ko
