#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/compat.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi.h>
//#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/gpio.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include <linux/time.h>
#include <linux/unistd.h>
#include <linux/platform_device.h>

#define WS2812DEV_MAJOR 154
#define DEVICE_NAME "ws2812"
#define SPI_MODE_MASK		(SPI_CPHA | SPI_CPOL | SPI_CS_HIGH \
				| SPI_LSB_FIRST | SPI_3WIRE | SPI_LOOP \
				| SPI_NO_CS | SPI_READY)

static dev_t led_devt;
struct cdev chardev;
static struct class *ws2812dev_class;


#define IOCTL_Constant 'k'
#define CONFIG_PINS _IOW(IOCTL_Constant, 1, int)

static LIST_HEAD(device_list);
static DEFINE_MUTEX(device_list_lock);
//static unsigned bufsiz = 4096;

char mosi_gpio, mosi_mux1;

struct mode_val{
	int mode;
};

struct spidev_data {
	dev_t			led_dev;
	spinlock_t		spi_lock;
	struct spi_device	*spi;
	struct list_head	device_entry;
	struct mutex		buf_lock;
	unsigned		users;
	//unsigned char	*buffer;
}*spidev;

struct data_buf{
	u8 buffer;
};

//unsigned char buffer;

//------------------------------------------------------------------//
static int ws2812_open(struct inode *inode, struct file *filp)
{
		printk("ws2812 device open file_op\n");
		return 0;

}


//-------------------------------------------------------------------//
static int ws2812_release(struct inode *inode, struct file *filp)
{
		printk("ws2812 device release file_op\n");
		return 0;
}
//-------------------------------------------------------------------//
static ssize_t ws2812_read(struct file *filp, char * buf, size_t lbuf, loff_t * ppos)
{
		//printk("In ws2812_read function\n");
		return lbuf;
}

//--------------------------------------------------------------------//
static ssize_t ws2812_write(struct file *filp, const char * buf, size_t lbuf, loff_t * ppos)
{
	//long long data;
	int ret,i,j,k=0,temp;
	bool bit1;
	//struct data_buf d_buf;
	unsigned char d_buf[lbuf];
	unsigned char spi_buf[lbuf*8];
	//struct spidev_data *spidev;
		//printk("In ws2812_write function\n");
		if(copy_from_user(d_buf, buf, lbuf)==0)
		//printk(KERN_ALERT "copy done\n");

		for(i =0; i<lbuf;i++){
		//printk(KERN_ALERT "data received %d\n", d_buf[i]);
	}

		for(i=0; i<lbuf;i++){
			temp = d_buf[i];
			for(j=0;j<8;j++){
				bit1 = temp & 0x80;
				if(bit1 == 1) spi_buf[k] = 0xF0;
				else spi_buf[k] = 0xC0;
				k++;
				temp = temp << 1;
			}
		}

		ret =spi_write(spidev->spi,spi_buf,lbuf*8);
		if(ret == 0) printk(KERN_ALERT "Data transmitted to SPI bus\n");
		return lbuf;
}

//-------------------------------------------------------------//
long ws2812_ioctl(struct file *file, unsigned int cmd, unsigned long args)
{
	int retval;
	struct mode_val *data_rcvd;
	struct spi_device *spi;
	data_rcvd = (struct mode_val*)args;
	printk(KERN_ALERT "Mode value entered is %d \n",data_rcvd->mode);

	spi = spidev->spi;
	spi->mode = data_rcvd->mode;
	retval = spi_setup(spi);
	if (retval < 0)
	 	printk(KERN_ALERT "mode value not set\n");

	gpio_direction_output(24,0);    //set gpio24 as output --level shifter
	gpio_direction_output(44,1);    //set gpio44 as high -- mux1 pin
	gpio_set_value_cansleep(72,1);
	return 0;

 }


 static struct file_operations ws2812dev_fops = {
 	.owner = THIS_MODULE,
 	.open = ws2812_open,
 	.release = ws2812_release,
 	.read = ws2812_read,
 	.write = ws2812_write,
  .unlocked_ioctl = ws2812_ioctl,
  };

static int ws2812_probe(struct spi_device *spi)
{
	int retval;
	spidev = kzalloc(sizeof(struct spidev_data), GFP_KERNEL);
	spidev->led_dev = led_devt;
	if (spidev == NULL)
		return -ENOMEM;

	spidev->spi = spi;
	spin_lock_init(&spidev->spi_lock);
	mutex_init(&spidev->buf_lock);

	INIT_LIST_HEAD(&spidev->device_entry);

	mutex_lock(&device_list_lock);
	spidev->led_dev =(int)device_create(ws2812dev_class, &spi->dev, spidev->led_dev, spidev, DEVICE_NAME);
	if (spidev->led_dev <0) {
		printk(KERN_ALERT "dev not created\n");
	}

	spi->bits_per_word = 32;
	retval = spi_setup(spi);
	if(retval < 0) printk(KERN_ALERT "bits per word not set");
	else printk(KERN_ALERT "bits per word set");


	//gpio_direction_output(72,0);    //set gpio72 as high -- mux2 pin

	printk(KERN_ALERT "Probe succesful\n");
	return 0;
}

static int ws2812_remove(struct spi_device *spi)
{
	//struct spidev_data	*spidev;
	device_destroy(ws2812dev_class, led_devt);
  printk(KERN_ALERT "SPI driver 'ws2812' unregistered\n");
	return 0;
}

static const struct spi_device_id ws2812_id[] = {
	{"ws2812", 0},
	{}
};
MODULE_DEVICE_TABLE(spi, ws2812_id);

static struct spi_driver ws2812_driver = {
	.driver = {
		.name	= "ws2812",
		.owner	= THIS_MODULE,
	},
	.probe		= ws2812_probe,
	.remove		= ws2812_remove,
	.id_table	= ws2812_id,
};



static int __init ws2812dev_init(void)
{
	int status;
	int ret;
  printk(KERN_ALERT "inside driver init \n");
  if(alloc_chrdev_region(&led_devt,0,1,DEVICE_NAME) < 0)
  {
    printk(KERN_DEBUG "Char Device Registration failed\n");
    return -1;
  }
  cdev_init(&chardev,&ws2812dev_fops);
  chardev.owner = THIS_MODULE;
  ret = cdev_add(&chardev, led_devt, 1);
  if (ret)
  {
    return ret;
  }

  ws2812dev_class = class_create(THIS_MODULE,DEVICE_NAME);
  if (IS_ERR(ws2812dev_class)) {
		printk(KERN_ALERT "class not created\n");
		return PTR_ERR(ws2812dev_class);
	}

	status = spi_register_driver(&ws2812_driver);
	if (status < 0) {
		printk(KERN_ALERT "spi not registered");
		class_destroy(ws2812dev_class);
		unregister_chrdev(WS2812DEV_MAJOR, ws2812_driver.driver.name);
	}
	return status;
}

static void __exit ws2812dev_exit(void)
{
	spi_unregister_driver(&ws2812_driver);
	class_destroy(ws2812dev_class);
	unregister_chrdev(WS2812DEV_MAJOR, ws2812_driver.driver.name);
	gpio_free(44);
	gpio_free(24);
	gpio_free(72);
}

module_init(ws2812dev_init);
module_exit(ws2812dev_exit);
MODULE_LICENSE("GPL v2");
