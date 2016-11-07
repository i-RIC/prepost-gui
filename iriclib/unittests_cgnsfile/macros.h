#include <stdio.h>

#define VERIFY_LOG(msg, result) printf("%s %d: %s ", __FILE__, __LINE__, msg);if(result){printf("[OK]\n");}else{printf("[NG]\n");}
