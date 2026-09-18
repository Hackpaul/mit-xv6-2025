#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int
main(int argc, char *argv[])
{
  char *data;
  for (int i = 0; i < (8*16) ; i++) {
    data = sbrk(4096);
    for (int j = 0 ; j < 4096 - 16 ; j++  ) {
      if(!strcmp(data + j , "This may help.")){
        printf("%s", data + j + 16 );
        exit(1);
      }
    }
  }
  exit(1);
}
