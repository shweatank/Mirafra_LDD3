#include<stdio.h>
#include<unistd.h>
struct fp_ops{
	int (*open)(char *);
	int (*read)(int,void *,size_t);
	int (*write)(int,void *,size_t);
	int (*close)(int);
}fops;
