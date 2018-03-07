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
#define RESET _IOW(IOCTL_Constant, 1, int)

int fd, temp;
long result;

struct mode_val{
	int mode;
};

int main(int argc, char **argv)
{
	int num=49,i;
	//unsigned int mode=0;
	struct mode_val data;
	unsigned char buf[num];
	for(i=0;i<num;i++)buf[i]=0x00;
  fd = open("/dev/ws2812", O_RDWR);
  if (fd < 0 ){
		printf("Error in opening the device file\n");
		printf("%d \n", errno);
		return 0;
	}

	do{
	printf("/****RESETTING the SPI mode****/\n");
	printf("For Mode_0 --> 0\n");
	printf("For Mode_1 --> 1\n");
	printf("For Mode_2 --> 2\n");
	printf("For Mode_3 --> 3\n");
	printf("Enter the SPI operation mode ");
	scanf("%d",&(data.mode));
}while((data.mode)>3);

	if(ioctl(fd,RESET,&(data.mode))<0)
		printf("ioctl not successful\n");

	buf[0] =0;
	for(i=0;i<16;i++){
		if(i%3==0) buf[i*3] = 0xFF;
		else if(i%3==1) buf[(i*3)+1] = 0xFF;
		else if(i%3==2) buf[(i*3)+2] = 0xFF;
  	if(write(fd, buf, num)<0)
			printf("write not successful\n");
		sleep(1);
		buf[i*3] = 0x00;
		buf[(i*3)+1] = 0x00;
		buf[(i*3)+2] = 0x00;
	}
	buf[48] = 0x00;
	write(fd, buf, num);
  sleep(1);

  //printf("out of main func\n");
  //temp = read(fd,&d,sizeof(d));
  //printf("AVERAGE DISTANCE = %lld\n",d.distance);
	close(fd);
  return 0;
}
