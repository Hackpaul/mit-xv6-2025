#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main (int argc,char *argv[]){
   
   if(argc == 2){
      pause(10 * atoi(argv[1]));
   } else {
      fprintf(2,"invalid parameters !\n");
      exit(1);
   }
   exit(0);
}
