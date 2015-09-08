#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>


#define dev_node "/dev/mycdrv"

int main()
{
	char message[6]= "e1cd\n";
	char receive[6]="";
	int fd1, fd2,rc;

	//Open two file desriptors
	printf("Opening two files desriptors\n");
	fd1= open(dev_node, O_RDWR);
	fd2= open(dev_node, O_RDWR);

	//Write to first file descriptors
	rc= write(fd1, message, sizeof(message));
	printf("Write \" %s \" to first file descriptor, result: %d\n", message, rc);
	
	//Read from first file descriptor
	rc= read(fd2, receive, 6);
	printf("Read from first descriptor: %s\n", receive);

	close(fd1);
	exit(0);
}

