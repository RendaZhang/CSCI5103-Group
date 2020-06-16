#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

char line[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int main(int argc, char* argv[])
{
  if(argc < 4) {
    printf("usage: ./producer [id] [length] [sleep time]\n");
    exit(0);
  }
  
  int count = atoi(argv[2]);
  
  char* buffer = calloc(sizeof(char), count);
  int i;
  for (i=0; i<count; i++) {
    buffer[i] = line[i%62];
  }
  
  int n = 0;
  int fd = open("/dev/scullBuffer0",O_WRONLY);

  sleep(atoi(argv[3]));
  
  n = write(fd, buffer, count);

  printf("#%d Producer wrote %d bytes to scullBuffer\n", atoi(argv[1]), n);
  close(fd);
  return 0;
}