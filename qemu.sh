
export ARCH=riscv&&export CROSS_COMPILE=riscv64-unknown-linux-gnu-

# 1.
echo "Build the busybox rootfs"
cd ./busybox-1.33.1/_install
find . | cpio -o --format=newc | gzip -9 > ../busybox.cpio.gz

# 2.
echo "Build the linux kernel"
cd ~/riscv-linux-devel
make -j8

# 3.
echo "Build opensbi"
cd ~/workspace/opensbi-1.6
make PLATFORM=generic FW_PAYLOAD_PATH=/home/jingyu/riscv-linux-devel/arch/riscv/boot/Image FW_FDT_PATH=./kmh-v2-1core.dtb -j

# 4.
echo "Run qemu"
qemu-system-riscv64 \
    -nographic -machine bosc-kmh \
    -smp 1 -m 2G  \
    -bios ./build/platform/generic/firmware/fw_payload.bin
