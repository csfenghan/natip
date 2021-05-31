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

/***********************************************************************
 *	硬件层操作
 *********************************************************************/
#define LED_NUMBER 3

volatile unsigned long *gpfcon;
volatile unsigned long *gpfdat;

static void led_init() {
        gpfcon = (volatile unsigned long *)ioremap(0x56000050, 16);
        gpfdat = gpfcon + 1;
}

// index为0则打开所有led，否则打开对应的单个led
static void led_set(int index, bool on) {
        int led1 = 0, led2 = 0, led3 = 0;

        switch (index) {
        case 0:
                led1 = 1, led2 = 1, led3 = 1;
                break;
        case 1:
                led1 = 1;
                break;
        case 2:
                led2 = 1;
                break;
        case 3:
                led3 = 1;
                break;
        }
        //打开还是关闭
        if (on) {
                *gpfdat &= ~((led1 << 4) | (led2 << 5) | (led3 << 6));
        } else {
                *gpfdat |= ((led1 << 4) | (led2 << 5) | (led3 << 6));
        }
}

static void led_open(int index) {
        int led1 = 0, led2 = 0, led3 = 0;

        switch (index) {
        case 0:
                led1 = 1, led2 = 1, led3 = 1;
                break;
        case 1:
                led1 = 1;
                break;
        case 2:
                led2 = 1;
                break;
        case 3:
                led3 = 1;
                break;
        }
        *gpfcon &= ~((led1 * 0x3 << (2 * 4)) | (led2 * 0x3 << (2 * 5)) | (led3 * 0x3 << (2 * 6)));
        *gpfcon |= ((led1 * 0x1 << (2 * 4)) | (led2 * 0x1 << (2 * 5)) | (led3 * 0x1 << (2 * 6)));

        led_set(0, 0);
}

/************************************************************************
 *	file_operations操作
 **********************************************************************/
static int led_drv_open(struct inode *inode, struct file *file) {
        led_open(MINOR(inode->i_rdev));
        return 0;
}

static ssize_t led_drv_write(struct file *file, const char __user *buf, size_t count,
                             loff_t *loff) {
        int val, minor;

        copy_from_user(&val, buf, 4);
        minor = MINOR(file->f_dentry->d_inode->i_rdev);
        led_set(minor, val);

        return 0;
}

static ssize_t led_drv_read(struct file *file, char __user *buf, size_t count, loff_t *loff) {
        return 0;
}

static struct file_operations fop = {
    .owner = THIS_MODULE,
    .open = led_drv_open,
    .write = led_drv_write,
    .read = led_drv_read,
};

/*********************************************************************
 *	设备注册与初始化
 *********************************************************************/

// mdev注册
static struct class *led_class;
static struct class_device *led_device[LED_NUMBER + 1];

int major = 0;
static int led_drv_init() {
        int minor = 0;
        // io地址映射
        led_init();

        // 注册设备号
        major = register_chrdev(0, "led", &fop);
        led_class = class_create(THIS_MODULE, "led");

        // 创建/dev/led*设备
        led_device[0] = class_device_create(led_class, NULL, MKDEV(major, 0), NULL, "led");
        for (minor = 1; minor < LED_NUMBER + 1; minor++) {
                led_device[minor] =
                    class_device_create(led_class, NULL, MKDEV(major, minor), NULL, "led%d", minor);
        }

        printk(KERN_INFO "led_init\n");
        return 0;
}

static void led_drv_exit() {
        int minor = 0;

        //注销设备号
        unregister_chrdev(major, "led");

        // 注销/dev/led*设备
        for (minor = 0; minor < LED_NUMBER + 1; minor++) {
                class_device_destroy(led_class, MKDEV(major, minor));
        }
        class_destroy(led_class);

        printk(KERN_INFO "led_exit\n");
}

module_init(led_drv_init);
module_exit(led_drv_exit);
