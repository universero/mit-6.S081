#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int i;

  for(i = 1; i < argc; i++){
    // write argv[i] to the standout or other redirected file with fd 1
    write(1, argv[i], strlen(argv[i]));
    // if it's middle arg, print the space to split them
    if(i + 1 < argc){
      write(1, " ", 1);
    // else print the '\n' as the end of echo
    } else {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
