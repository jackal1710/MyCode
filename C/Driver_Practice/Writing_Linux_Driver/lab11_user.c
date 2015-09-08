#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>


#define dev_node "/dev/mycdrv"

int main()
{
	int fd, rc;
	int val= 10;
	char *send_data;

	//Open two file desriptors
	fd= open(dev_node, O_RDWR);
	printf("Opening files desriptori: %d\n", fd);

	//Write to first file descriptors
	send_data= malloc(sizeof(int));	
	memcpy(send_data, &val, sizeof(int));
	rc= write(fd, send_data, sizeof(int));

	free(send_data);	
	printf("\n");
	exit(0);
}

