#include <linux/kernel.h>
#include <linux/module.h>
//#include <linux/platform_device.h>
//#include "HCSR_plat_device.h"

#include <linux/init.h>
#include <linux/moduleparam.h>
//#include <linux/gpio.h>
#include <linux/spi/spi.h>

static struct spi_device *spi_led_device;

/*static struct HCSR_sensor HCSR1 = {
		.name	= "HCSR_dev_1",
		.dev_no 	= 20,
		.plf_dev = {
			.name	= "HCSR_01",
			.id	= -1,
		}
};
*/


static int spi_device_init(void)
{
	//int ret = 0;

	struct spi_board_info info = {
		.modalias = "ws2812",
		.max_speed_hz = 6400000,
		.bus_num = 1,
		.chip_select = 1,
		.mode = SPI_MODE_0,
	};

	struct spi_master *master;
	int ret;
	master = spi_busnum_to_master(info.bus_num);
	if( !master )
		return -ENODEV;
	spi_led_device = spi_new_device(master, &info);
	if( !spi_led_device )
		return -ENODEV;
	spi_led_device->bits_per_word = 16;
	ret = spi_setup(spi_led_device);
	if( ret )
		spi_unregister_device(spi_led_device);
	printk(KERN_ALERT "SPI device 'ws2812' registered\n");

	return ret;
}

static void spi_device_exit(void)
{
  spi_unregister_device(spi_led_device);
	printk(KERN_ALERT "SPI device 'ws2812' unregistered\n");
}

module_init(spi_device_init);
module_exit(spi_device_exit);
MODULE_LICENSE("GPL");
