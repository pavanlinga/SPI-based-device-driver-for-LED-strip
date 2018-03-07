#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/ioctl.h>


#define IOCTL_Constant 'k'
#define SET_PARAMETERS _IOW(IOCTL_Constant, 1, int)
#define CONFIG_PINS _IOW(IOCTL_Constant, 2, int)

int fd, temp;
long result;

struct pins{
	int io_pin;
};

int main(int argc, char **argv)
{
	//int num=9,i;
/*	char buf[num];
  fd = open("/dev/ws2812", O_RDWR);
  if (fd < 0 ){
		printf("Error in opening the device file\n");
		printf("%d \n", errno);
		return 0;
	}
  //result = ioctl(fd,CONFIG_PINS,&gpio);
  //result  = ioctl(fd,SET_PARAMETERS,&parameters);
	buf[0] =0;
	for(i=0; i< num;i++){
		buf[i+1] = buf[i] + 50;
	}
  if(write(fd, buf, num)<0)
		printf("write not successful\n");
  sleep(1);
  printf("out of main func\n");
  //temp = read(fd,&d,sizeof(d));
  //printf("AVERAGE DISTANCE = %lld\n",d.distance);*/
  return 0;
}
