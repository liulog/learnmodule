# learnmodule
存放 linux module 的仓库, 学习使用

### example1:
helloworld module -- only support insmod/rmmod.

### example2:
hellodevice module -- simply communicate with /dev/lkm_example.
> This example is from https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234.

### example3:
helloioctl module -- simply demonstrate usage of ioctl.

### riscv_hpm:
This is a simple example which uses riscv's hpmevent and hpmcounter.
And its structure is just like below.

![riscv_hpm](./assets/hpm_mod.png)