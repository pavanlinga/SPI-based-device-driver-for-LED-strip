
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
 #include <linux/jiffies.h>
 #include <linux/hrtimer.h>
 #include <linux/ktime.h>
 //#include <linux/delay.h>
 #include <asm/delay.h>
 #include <linux/time.h>
 //#define MS_TO_NS(x) (x * 1E6L)

 struct pin_brightness{
   unsigned char green;
   unsigned char red;
   unsigned char blue;
 };

bool x;

 struct pin_brightness p[16] = {
   {255,0,0},
   {255,0,0},
   {0,255,0},
   {0,255,0},
   {255,0,0},
   {0,0,255},
   {0,0,255},
   {0,0,255},
   {0,0,255},
   {255,0,0},
   {255,0,0},
   {255,0,0},
   {0,0,255},
   {255,0,0},
   {0,0,255},
   {0,0,255},
};


/*#define CLOCK CLOCK_MONOTONIC
static struct hrtimer hr_timer;
ktime_t ktime;
unsigned long timer_interval_ns;*/

//unsigned char temp;
void light_bit(unsigned char temp){
//  printk(KERN_ALERT "temp value %d\n",temp);
x =temp & 0x80;
  if (x==1){
    gpio_set_value_cansleep(15,1);
    ndelay(400);  //700ns
    gpio_set_value_cansleep(15,0);
    ndelay(300);  //600ns
    }
  else {
      gpio_set_value_cansleep(15,1);
      ndelay(150);  //350ns
      gpio_set_value_cansleep(15,0);
      ndelay(550);  //800ns
    }
  //return 0;
  }



/*enum hrtimer_restart hrtimer_callback( struct hrtimer *timer )
{
    printk( "hrtimer_callback called (%ld).\n", jiffies );
    return HRTIMER_RESTART;
}


static int __init timer_init(void)
{
  unsigned long delay_in_msTH0 = 0.00035L;

  printk("HR Timer module installing\n");

  ktime = ktime_set( 0, MS_TO_NS(delay_in_ms) );

    hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );

    hr_timer.function = &hrtimer_callback;

    printk( "Starting timer to fire in %ldms (%ld)\n", delay_in_ms, jiffies );
    printk("hrtimer init successfull\n");
    hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

    return 0;
}

static void __exit timer_exit(void) {
	int ret;
  	ret = hrtimer_cancel( &hr_timer );
  	if (ret) printk("The timer in use\n");
  	printk("hr timer successfully exited\n");

}*/

static int __init gpiobit_init(void){
   int i,j,k,n = 0;
   unsigned char temp1, temp2, temp3;
   unsigned char buff[48];
   gpio_request(42,"led");
   gpio_direction_output(42,0);
   gpio_request(15,"ledout");
   gpio_set_value_cansleep(15,1);

 printk(KERN_INFO "gpio bit bang init succesful\n");
 //for(i=0;i<48;i++) buff[i] =0;
   for(i=0;i<48;i++) {
    //  temp1=p[i].green;
    //  temp2=p[i].red;
    //  temp3=p[i].blue;
    temp1=0xFF;
    //temp2=0x00;
    //temp3=0xFF;
     for(j=0;j<8;j++){
       light_bit(temp1);
       temp1=temp1<<1;
     }

  //   buff[i*3]=0x00;
    //  for(k=0;k<8;k++){
    //    light_bit(temp2);
    //    temp2=temp2<<1;
    //  }
    // for(n=0;n<8;n++){
    //  light_bit(temp3);
    //  temp3=temp3<<1;
    //  }
     //printk(KERN_ALERT "out of for\n");
}
return 0;
}

static void __exit gpiobit_exit(void){
  //gpio_unexport(5);
   gpio_free(42);
   //gpio_free(66);
   gpio_free(15);

  printk(KERN_INFO "gpio bitbang successfuly terminated\n");

}
MODULE_LICENSE("GPL");
module_init(gpiobit_init);
module_exit(gpiobit_exit);
