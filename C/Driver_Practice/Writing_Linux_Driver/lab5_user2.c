#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>


#define dev_node "/dev/mycdrv"

int main()
{
	int fd, rc;
	fd= open(dev_node, O_RDWR);
	printf("Opening files desriptori: %d\n", fd);

	printf("\n");
	exit(0);
}

