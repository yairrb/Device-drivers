#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "YRUIZ" // device name
#define BUF_LEN 1024        // Max length of the message from the device

/*
Global variables are declared as static, so are global within the file.
*/

static int major_number;     // major number so we can talk to the device
static char buffer[BUF_LEN]; // buffer to storage data

/* file operations*/

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = device_write};

int init_module(void)
{ /* Constructor */
    printk(KERN_INFO "YRUIZ : Driver registrado\n");
    // get major number
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT "registering char device failed with %d\n", major_number);
        return major_number;
    }

    printk(KERN_INFO "I was assigned major number %d. To talk to\n", major_number);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major_number);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");
    return 0;
}

void cleanup_module(void)
{ /* Destructor */
    // Desregistrar el char device
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Char device has been unregistered\n");
}

// write function
static ssize_t device_write(struct file *filp, const char *user_buffer, size_t length, loff_t *offset)
{
    ssize_t bytes_written = 0;

    if (*offset >= BUF_LEN)
    {
        return -ENOSPC; // not enough space
    }

    // check buffer space so I can avoid overflow
    ssize_t bytes_to_write = min(length, (size_t)(BUF_LEN - *offset));

    // copy data to kernel
    if (copy_from_user(buffer + *offset, user_buffer, bytes_to_write))
    {
        return -EFAULT; // Error de copia
    }

    // Update offset
    *offset += bytes_to_write;

    // update bytes to write
    bytes_written = bytes_to_write;

    // Print data to the kernel
    printk(KERN_INFO "YRUIZ: Mensaje recibido: %.*s\n", bytes_written, buffer);

    return bytes_written;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YRUIZ");
MODULE_DESCRIPTION("Un primer driver");
