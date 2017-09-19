//Include files
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>

struct char_device
{
struct semaphore sem;	//Semaphore for accessing the file safely
}char_device;

static char buffer[PAGE_SIZE];	//Common buffer between kernel and user space of size 4096
struct cdev *my_cdev;	//Device handle 
dev_t dev_num;	//Structure that holds device numbers
char driver_name[]="dyn_driver";	//Device name

int device_open(struct inode *inode, struct file *filp)
{
if(down_interruptible(&char_device.sem))  //Acquire semaphore
{
printk(KERN_ALERT "Failed to open file");
return -1;
}
printk(KERN_INFO "File opened");
return 0;
}


static ssize_t device_write(struct file *filp,const char *buf, size_t lbuf,loff_t *pos)
{
int nbytes = lbuf-copy_from_user(buffer+*pos,buf,lbuf);	//function to copy data from user space to kernel space

*pos+=nbytes;

pr_info("Data received from app %s, nbytes=%d\n",buffer,nbytes);
return nbytes;
}


static ssize_t device_read(struct file *filp,char *buf, size_t lbuf,loff_t *pos)
{
int nbytes;
int maxbytes;
int bytes_to_do;

maxbytes=PAGE_SIZE-*pos;

if(maxbytes>lbuf)
bytes_to_do=lbuf;
else
bytes_to_do=maxbytes;

nbytes = bytes_to_do-copy_to_user(buf,buffer+*pos,bytes_to_do);	//function to copy data from kernel space to user space
*pos+=nbytes;
pr_info("Data sent to app %s, nbytes=%d\n",buf,nbytes);
return nbytes;

}

int device_release(struct inode *inode, struct file *filp)
{
printk(KERN_INFO "Closing file");
up(&char_device.sem);	//Release semaphore
return 0;
}



//File operations associated with the device
struct file_operations fops={
.owner=THIS_MODULE,
.write=device_write,
.open=device_open,
.read=device_read,
.release=device_release
};


static int __init driver_entry(void){
int ret_status;

ret_status=alloc_chrdev_region(&dev_num,0,1,driver_name);   //Allocates device number dynamically
if(ret_status)
{
printk(KERN_INFO "Driver not allocated number:error number:%d",ret_status);
return ret_status;
}

pr_info("Major number=%d Minor number=%d\n",MAJOR(dev_num),MINOR(dev_num));

my_cdev=cdev_alloc();	//Allocates memory for device handle

cdev_init(my_cdev,&fops); //Initializes device handle	

sema_init(&char_device.sem,1);	//Initialize the semaphore

ret_status=cdev_add(my_cdev,dev_num,1); //Add device to the kernel

if(ret_status)
{
printk(KERN_INFO "Driver not added:error number:%d",ret_status);
return ret_status;
}

memset(buffer,'\0',PAGE_SIZE);

return 0;
}

static void __exit driver_exit(void){

cdev_del(my_cdev);	//Remove the device from the system
unregister_chrdev_region(dev_num,1);	//Unregister the device
printk(KERN_INFO "Char Driver Cleaned");

}


module_init(driver_entry);
module_exit(driver_exit);
MODULE_LICENSE("GPL");

 
