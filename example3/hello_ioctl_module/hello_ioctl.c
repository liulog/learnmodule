#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h> // for copy_to_user and copy_from_user
#include <linux/ioctl.h>

#define DEVICE_NAME "ioctl_example"
#define IOCTL_MAGIC 'k'

// Define commands using different macros
#define IOCTL_CMD_READ  _IOR(IOCTL_MAGIC, 0, unsigned long)
#define IOCTL_CMD_WRITE _IOW(IOCTL_MAGIC, 1, unsigned long)
#define IOCTL_CMD_RDWR  _IOWR(IOCTL_MAGIC, 2, unsigned long)
#define IOCTL_CMD_NONE  _IO(IOCTL_MAGIC, 3)

static int major_num;
static char msg_buffer[1024] = "Hello from Kernel!";
static char *msg_ptr = msg_buffer;

// ioctl cmd handler
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret_val = 0;
    unsigned long karg;

    switch (cmd) {
        case IOCTL_CMD_READ:
            printk(KERN_INFO "Handling IOCTL_CMD_READ\n");
            break;
            
        case IOCTL_CMD_WRITE:
            printk(KERN_INFO "Handling IOCTL_CMD_WRITE\n");
            break;
            
        case IOCTL_CMD_RDWR:
            printk(KERN_INFO "Handling IOCTL_CMD_RDWR\n");
            break;
            
        case IOCTL_CMD_NONE:
            printk(KERN_INFO "Handling IOCTL_CMD_NONE\n");
            break;
            
        default:
            printk(KERN_ALERT "Invalid ioctl command.\n");
            ret_val = -EINVAL;
            break;
    }

    return ret_val;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
};

static int __init ioctl_init(void)
{
    // Register the character device
    major_num = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_num < 0) {
        printk(KERN_ALERT "Registering the character device failed with %d\n", major_num);
        return major_num;
    }

    printk(KERN_INFO "ioctl_example module loaded with device major number %d\n", major_num);
    return 0;
}

static void __exit ioctl_exit(void)
{
    // Unregister the character device
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "ioctl_example module unloaded\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Qwen3");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("A simple example Linux module that demonstrates ioctl usage.");