/*
 * Copyright (C) 2020-2021 Sonu Abraham
 * Copyright (C) 2020-2021 Embedded Tech Labs Pty Ltd
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an   
 * acknowledgment appears in derived source files. 
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>


int main()
{
   int len, fd1, fd2, rc;
   char *devname = "/dev/ldddev";
   char msg[] = "Data from character driver\n";

   len = sizeof(msg);
   fd1 = open(devname, O_RDWR);
   printf("First file descriptor opened for writing: %d\n", fd1);
   fd2 = open(devname, O_RDWR);
   printf("Second file descriptor opened for reading: %d\n", fd2);
  
   rc = write(fd1, msg, len);
   printf("Write returned: %d on %d,write msg: %s\n", rc, fd1,
       msg);

   memset(msg, 0, len);

   rc = read(fd2, msg, len);
   printf("Read returned: %d on %d, read msg: %s\n", rc, fd2,
       msg);

   close(fd1);
   return 0;
}
