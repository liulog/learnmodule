
obj-m += hello_ioctl.o   
ARCH ?= riscv
CROSS_COMPILE = riscv64-unknown-linux-gnu-
KERNEL_PATH ?= ~/riscv-linux-devel

all:
	make -C $(KERNEL_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	make -C $(KERNEL_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean