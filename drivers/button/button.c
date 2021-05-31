#include <linux/autoconf.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#include <asm/hardware.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

volatile unsigned long *gpfconf;
volatile unsigned long *gpfdat;

volatile unsigned long *gpgconf;
volatile unsigned long *gpgdat;

// file_operations结构
static int second_drv_open(struct inode *inode, struct file *file) {
        *gpfconf &= ~((0x3 << (0 * 2)) | (0x3 << (2 * 2)));
        *gpgconf &= ~((0x3 << (3 * 2)) | (0x3 << (11 * 2)));

        return 0;
}
static int second_drv_read(struct file *file, char __user *buf, size_t size, loff_t *ppos) {
        unsigned char key_vals[4];
        int retval;

        retval = *gpfdat;
        key_vals[0] = (retval & (1 << 0)) ? 1 : 0;
        key_vals[1] = (retval & (1 << 2)) ? 1 : 0;

        retval = *gpgdat;
        key_vals[2] = (retval & (1 << 3)) ? 1 : 0;
        key_vals[3] = (retval & (1 << 11)) ? 1 : 0;

        copy_to_user(buf, key_vals, sizeof(key_vals));

        return sizeof(key_vals);
}
struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = second_drv_open,
    .read = second_drv_read,
};

// mdev
static struct class *seconddrv_class;
static struct class_device *seconddev_class_dev;

int major;
static int second_drv_init() {
        //设备号分配和device创建
        major = register_chrdev(0, "second_drv", &fops);
        seconddrv_class = class_create(THIS_MODULE, "second");
        seconddev_class_dev =
            class_device_create(seconddrv_class, NULL, MKDEV(major, 0), NULL, "buttons");

        //初始化io
        gpfconf = (volatile unsigned long *)ioremap(0x56000050, 16);
        gpfdat = gpfconf + 1;

        gpgconf = (volatile unsigned long *)ioremap(0x56000060, 16);
        gpgdat = gpgconf + 1;

        return 0;
}

static void second_drv_exit() {
        iounmap(gpfconf);
        iounmap(gpgconf);
        unregister_chrdev(major, "second_drv");
        class_destroy(seconddrv_class);
}

module_init(second_drv_init);
module_exit(second_drv_exit);
