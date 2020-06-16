#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
  
  if(argc < 3) {
    printf("usage: ./consumer [id] [sleep time]\n");
    exit(0);
  }
  
  int n = 0;
  int fd = open("/dev/scullBuffer0",O_RDONLY);
  char* result = calloc(sizeof(char), 513);
  
  sleep(atoi(argv[2]));
  
  n = read(fd, result, 512);

  printf("#%d Consumer read %d bytes from scullBuffer\n", atoi(argv[1]), n);
  if( n != 0) {
    printf("#%d Consumer read \"%s\"\n", atoi(argv[1]), result);
  }
  close(fd);
  return 0;
}
