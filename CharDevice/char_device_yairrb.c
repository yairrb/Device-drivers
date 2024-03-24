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
static char BUFFER[BUF_LEN]; // buffer to storage data
static ssize_t message_length = 0;

static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

/* file operations*/
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = device_write,
    .open = device_open,
    .read = device_read,
    .release = device_release};

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
{
  // Desregistrar el char device
  unregister_chrdev(major_number, DEVICE_NAME);
  printk(KERN_INFO "Char device has been unregistered\n");
}

// write function
static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
  ssize_t bytes_written = 0;

  if (length > BUF_LEN)
  {
    return -ENOSPC; // not enough space
  }

  // check buffer space so I can avoid overflow
  ssize_t bytes_to_write = min(length, (size_t)(BUF_LEN - *offset));

  // Copy data to kernel
  if (copy_from_user(BUFFER, buffer, length))
  {
    return -EFAULT; // fail on copying from user space
  }

  BUFFER[length] = '\0'; // Null
  message_length = length;

  // Print data to the kernel
  printk(KERN_INFO "YRUIZ: Mensaje recibido: %s\n", BUFFER);
  bytes_written = length;
  return bytes_written;
}

// device read function
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
  ssize_t bytes_read = 0;
  size_t i;

  if (message_length == 0)
  {
    return 0; // no messages
  }

  // Copy message byte per byte
  for (i = 0; i < message_length; ++i)
  {
    // check if it's failing the copy
    if (put_user(BUFFER[message_length - i - 1], buffer + i))
    {
      return -EFAULT;
    }
  }

  bytes_read = message_length;

  message_length = 0;
  return bytes_read;
}

static int device_open(struct inode *inode, struct file *file)
{
  // things that I may need on open
  printk(KERN_INFO "YRUIZ: Dispositivo abierto\n");
  return 0;
}
static int device_release(struct inode *inode, struct file *file)
{
  // release operations
  printk(KERN_INFO "YRUIZ: Dispositivo cerrado\n");
  return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YRUIZ");
MODULE_DESCRIPTION("Un primer driver");
