#include <linux/module.h>   // 内核模块必需的头文件
#include <linux/kernel.h>   // 包含 printk 函数
#include <linux/init.h>     // 包含 __init 和 __exit 宏

// 模块初始化函数
static int __init hello_init(void)
{
    printk(KERN_INFO "Hello, World!\n");
    return 0;
}

// 模块退出函数
static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, World!\n");
}

// 声明模块的入口和出口
module_init(hello_init);
module_exit(hello_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("liulog");
MODULE_DESCRIPTION("A Hello World Kernel Module");