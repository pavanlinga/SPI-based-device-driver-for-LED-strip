
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
 #include <linux/jiffies.h>
 #include <linux/hrtimer.h>
 #include <linux/ktime.h>
 #include <linux/delay.h>
 #include <asm/delay.h>
 #include <linux/time.h>
 #include <linux/types.h>
 #include <linux/fs.h>
 #include <linux/device.h>
 #include <linux/err.h>
 #include <linux/list.h>
 #include <linux/errno.h>
 #include <linux/mutex.h>
 #include <linux/slab.h>
 #include <linux/compat.h>
 #include <linux/moduleparam.h>
 #include <linux/pinctrl/pinmux.h>
 #include <linux/unistd.h>
 #include <linux/platform_device.h>
 #include <linux/cdev.h>
 #include <linux/pci.h>



 #define MS_TO_NS(x) (x * 1E6L)
 #define DEVICE_NAME "hrtimer_reg"
 static dev_t dev_h;
 struct cdev cdev_h;
 static struct class *hrtimer_class;
 static struct device *hrtimer_device;

 struct pci_dev *pdev;



#define CLOCK CLOCK_MONOTONIC
static struct hrtimer hr_timer;
ktime_t ktime1,ktime0;

int i = 0, j=0,k=0,n=0;unsigned char temp1, temp2, temp3;

int var=0,comp=1;

enum hrtimer_restart hrtimer_callback( struct hrtimer *timer )
{
   ktime_t current_time0,current_time;
    if(var < 768)
    {
      if(comp == 1)
      {
          gpio_set_value_cansleep(5,1);
          unsigned long interval1 = 0.0007L;
          current_time0=ktime_get();
          ktime0=ktime_set(0,MS_TO_NS(interval1));
          hrtimer_forward(timer,current_time0,ktime0);
          var++;
          comp =0;
          return HRTIMER_RESTART;
        }
        else
        {
          current_time=ktime_get();
          gpio_set_value_cansleep(5,0);
          unsigned long interval2 = 0.0006L;
          ktime1 = ktime_set(0,MS_TO_NS(interval2));
          hrtimer_forward(timer,current_time,ktime1);
          var++;
          comp =1;
          return HRTIMER_RESTART;
        }
   }
   return HRTIMER_NORESTART;
}

static int hrtimer_open(struct inode *inode, struct file *filp)
{
		printk("hrtimer device open file_op\n");
		return 0;

}

static int hrtimer_release(struct inode *inode, struct file *filp)
{
		printk("hrtimer device release file_op\n");
		return 0;
}

static struct file_operations hrtimer_fops = {
 .owner = THIS_MODULE,
 .open = hrtimer_open,
 .release =hrtimer_release,
 };


static int __init timer_init(void)
{

    ktime0 = ktime_set(0,0);
    hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
    hr_timer.function = &hrtimer_callback;
    hrtimer_start( &hr_timer, ktime0, HRTIMER_MODE_REL );

    return 0;
}

static void __exit timer_exit(void) {
	int ret;
  	ret = hrtimer_cancel( &hr_timer );
  	if (ret) printk("The timer in use\n");
    printk("hr timer successfully exited\n");

}






static int __init gpiobit_init(void)
{
  int ret;
  int start,len,reg_base;
//  char a[30];
  printk(KERN_ALERT "inside driver init \n");
  if(alloc_chrdev_region(&dev_h,0,1,DEVICE_NAME) < 0)
  {
    printk(KERN_DEBUG "Char Device Registration failed\n");
    return -1;
  }
  cdev_init(&cdev_h,&hrtimer_fops);
  cdev_h.owner = THIS_MODULE;
  ret = cdev_add(&cdev_h, dev_h, 1);
  if (ret)
  {
    return ret;
  }

  hrtimer_class = class_create(THIS_MODULE,DEVICE_NAME);
  if (IS_ERR(hrtimer_class)) {
		printk(KERN_ALERT "class not created\n");
		return PTR_ERR(hrtimer_class);
	}

  hrtimer_device = device_create(hrtimer_class, NULL, dev_h, NULL, DEVICE_NAME);
  if (IS_ERR(hrtimer_device)) {
          printk(KERN_ERR DEVICE_NAME " cant create device %s\n", DEVICE_NAME);
          return PTR_ERR(hrtimer_device);
  }

  pdev = pci_get_device(0x8086, 0x0934, NULL);
  printk(KERN_ALERT "%d\n",pdev);


   gpio_direction_output(24,1);
   gpio_set_value_cansleep(24,0);
   gpio_direction_output(5,0);
   printk("gpio pin set is %d\n",11);
   timer_init();
   timer_exit();
   printk(KERN_INFO "hrtimer init successful\n");

  return 0;
}


static void __exit gpiobit_exit(void){
   gpio_unexport(5);
   gpio_free(5);
   class_destroy(hrtimer_class);
   device_destroy(hrtimer_class,1);
 	 unregister_chrdev_region(dev_h,1);
  printk(KERN_INFO "hrtimer exit successful\n");
}
MODULE_LICENSE("GPL");
module_init(gpiobit_init);
module_exit(gpiobit_exit);
