#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>


#define dev_node "/dev/mycdrv"

int main()
{
	int fd, rc;
	char receive[10];
	char i;

	//Open two file desriptors
	fd= open(dev_node, O_RDWR);
	printf("Opening files desriptori: %d\n", fd);

	//Write to first file descriptors
	for (i=0; i<10; i++) {
		rc= write(fd, &i, sizeof(i));
		printf("Write \" %d \" to first file descriptor, result: %d\n", i, rc);
	}
	
	//Read from first file descriptor
	rc= read(fd, receive, 6);
	for (i=0; i< sizeof(receive); i++) {
		printf("%d ", receive[i]);
	}
	printf("\n");

		i= 6;
		rc= write(fd, &i, sizeof(i));
		printf("Write \" %d \" to first file descriptor, result: %d\n", i, rc);

	rc= read(fd, receive, 10);
	for (i=0; i< sizeof(receive); i++) {
		printf("%d ", receive[i]);
	}
	
	printf("\n");
	exit(0);
}

