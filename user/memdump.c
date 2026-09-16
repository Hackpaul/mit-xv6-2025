#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include <stdint.h>

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  int i = 0;
  while(fmt[i] != '\0'){
   if (fmt[i] == 'i'){
         printf("%d\n",*((uint32_t *)data));
         data += sizeof(uint32_t);
   } else if(fmt[i] == 'p'){
         printf("%lx\n",*((uint64_t *) data));
         data += sizeof(uint64_t);
   } else if(fmt[i] == 'h'){
         printf("%d\n",*((uint16_t *)data));
        data += sizeof(uint16_t);
   } else if(fmt[i] == 'c'){
         printf("%c\n",data[0]);
          data += sizeof(char);
   } else if(fmt[i] == 's'){
          printf("%s\n",*((char **)data));
           data += sizeof(char**);
   } else if(fmt[i] == 'S'){
         printf("%s\n",data);
         data += sizeof(strlen(data));
   } else {
      fprintf(2,"memdump : format is not valid !\n");
      return;
   }
   i++;
  }

   return;

}
