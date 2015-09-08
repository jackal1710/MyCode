#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>


#define dev_node "/dev/mycdrv"
#define READ_SIZE (50 * sizeof(int))

int main()
{
	int fd, rc, i;
	char data[READ_SIZE];
	signed int * receive;

	//Open two file desriptors
	fd= open(dev_node, O_RDWR);
	printf("Opening files desriptori: %d\n", fd);
	
	//Read from file descriptor
	rc= read(fd, data, READ_SIZE);
	receive = (signed int*) data;

	//Print the statistic information of interrupt
	for (i= 0; i< 50; ++i) {
		if (receive[i] != -1)
			printf("IRQ line %d: %d\n", i, receive[i]);
	}
	
	printf("\n");
	exit(0);
}

