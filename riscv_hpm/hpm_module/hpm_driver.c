#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/miscdevice.h>
#include <linux/types.h>

#define DEVICE_NAME "riscv_hpm"
#define IOCTL_MAGIC 'p'

#define IOCTL_ENABLE_COUNTERS   _IOW(IOCTL_MAGIC, 0, __u32)
#define IOCTL_INHIBIT_COUNTERS  _IOW(IOCTL_MAGIC, 1, __u32)
#define IOCTL_CONFIGURE_EVENT   _IOW(IOCTL_MAGIC, 2, struct hpm_event_args)
#define IOCTL_SET_COUNTER       _IOW(IOCTL_MAGIC, 3, struct hpm_counter_args)

struct hpm_event_args {
    __u64 idx;          // hpmevent index
    __u64 event_value;  // hpmevent value
};

struct hpm_counter_args {
    __u64 idx;          // hpmcounter index
    __u64 count_value;  // hpmcounter value
};

struct sbiret {
    long error;
    long value;
};

// SBI:
// a7 = extension ID
// a6 = function ID
// a0~a5 arguments
// a0 = sbi error code
static inline struct sbiret sbi_ecall(unsigned long eid, unsigned long fid, unsigned long a0,
    unsigned long a1, unsigned long a2, unsigned long a3, unsigned long a4, unsigned long a5)
{
    register unsigned long _a0 __asm__("a0") = a0;
    register unsigned long _a1 __asm__("a1") = a1;
    register unsigned long _a2 __asm__("a2") = a2;
    register unsigned long _a3 __asm__("a3") = a3;
    register unsigned long _a4 __asm__("a4") = a4;
    register unsigned long _a5 __asm__("a5") = a5;
    register unsigned long _a6 __asm__("a6") = fid;
    register unsigned long _a7 __asm__("a7") = eid;

    __asm__ volatile("ecall" : "+r"(_a0), "+r"(_a1) : "r"(_a2), "r"(_a3), "r"(_a4), "r"(_a5),
                     "r"(_a6), "r"(_a7) : "memory");

    struct sbiret ret = { .error = (long)_a0, .value = (long)_a1 };
    return ret;
}

// According to the RISC-V SBI specification.
#define SBI_BASE_EXT 0x10
#define SBI_GET_VENDORID 0x4
#define SBI_VENDOR_EXT_BASE 0x09000000
#define SBI_VENDOR_EXT_END 0x09FFFFFF
static unsigned long sbi_vendor_eid = 0;

static uint32_t read_scounteren(void) {
    uint32_t val;
    __asm__ volatile ("csrr %0, scounteren" : "=r" (val));
    return val;
}

static void write_scounteren(uint32_t value) {
    __asm__ volatile ("csrw scounteren, %0" : : "r" (value));
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    void __user *argp = (void __user *)arg;
    switch (cmd) {
        case IOCTL_ENABLE_COUNTERS: {
            __u32 value;
            if (copy_from_user(&value, argp, sizeof(value))) {
                return -EFAULT;
            }
            printk(KERN_INFO "ENABLE_COUNTERS: 0x%x\n", value);
            printk(KERN_INFO "old scounteren: 0x%x\n", read_scounteren());
            write_scounteren(value);    // Write into scounteren reg
            printk(KERN_INFO "new scounteren: 0x%x\n", read_scounteren());
            struct sbiret sbi_ret = sbi_ecall(sbi_vendor_eid, 0x0, value, 0, 0, 0, 0, 0);
            if (sbi_ret.error != 0) {
                printk(KERN_WARNING "SBI call failed with error %ld\n", sbi_ret.error);
            }
            break;
        }
        case IOCTL_INHIBIT_COUNTERS: {
            __u32 value;
            if (copy_from_user(&value, argp, sizeof(value))) {
                return -EFAULT;
            }
            printk(KERN_INFO "INHIBIT_COUNTERS: 0x%x\n", value);
            struct sbiret sbi_ret = sbi_ecall(sbi_vendor_eid, 0x1, value, 0, 0, 0, 0, 0);
            if (sbi_ret.error != 0) {
                printk(KERN_WARNING "SBI call failed with error %ld\n", sbi_ret.error);
            }
            break;
        }
        case IOCTL_CONFIGURE_EVENT: {
            struct hpm_event_args args;
            if (copy_from_user(&args, argp, sizeof(args))) {
                return -EFAULT;
            }
            printk(KERN_INFO "CONFIGURE_EVENT: Index %llu, Value 0x%llx\n", args.idx, args.event_value);
            struct sbiret sbi_ret = sbi_ecall(sbi_vendor_eid, 0x2, args.idx, args.event_value, 0, 0, 0, 0);
            if (sbi_ret.error != 0) {
                printk(KERN_WARNING "SBI call failed with error %ld\n", sbi_ret.error);
            }
            break;
        }
        case IOCTL_SET_COUNTER: {
            struct hpm_counter_args args;
            if (copy_from_user(&args, argp, sizeof(args))) {
                return -EFAULT;
            }
            printk(KERN_INFO "SET_COUNTER: Index %llu, Value 0x%llx\n", args.idx, args.count_value);
            struct sbiret sbi_ret = sbi_ecall(sbi_vendor_eid, 0x3, args.idx, args.count_value, 0, 0, 0, 0);
            if (sbi_ret.error != 0) {
                printk(KERN_WARNING "SBI call failed with error %ld\n", sbi_ret.error);
            }
            break;
        }
        default:
            printk(KERN_ERR "Unknown ioctl command %u\n", cmd);
            return -EINVAL;
    }
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
};

static struct miscdevice hpm_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &fops,
};

static int __init riscv_hpm_init(void)
{
    struct sbiret sbi_ret = sbi_ecall(SBI_BASE_EXT, SBI_GET_VENDORID, 0, 0, 0, 0, 0, 0);
    // The lower 24 bits of vendor specific EID must match the lower 24 bits of the mvendorid value.
    sbi_vendor_eid = (sbi_ret.value & 0xFFFFFF) | SBI_VENDOR_EXT_BASE;
    pr_info("SBI vendor EID: 0x%lx\n", sbi_vendor_eid);
    int ret = misc_register(&hpm_device);
    if (ret) {
        pr_err("Failed to register misc device\n");
        return ret;
    }
    pr_info("riscv_hpm_ioctl module loaded\n");
    return 0;
}

static void __exit riscv_hpm_exit(void)
{
    misc_deregister(&hpm_device);
    pr_info("riscv_hpm_ioctl module unloaded\n");
}

module_init(riscv_hpm_init);
module_exit(riscv_hpm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Qwen");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("Simple RISC-V HPM Driver");