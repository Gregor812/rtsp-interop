#include "remuxing.h"

#include <cstdio>

void Log()
{
    printf("CALLBACK CALLED\n");
}

int main()
{
    remux(Log);
    return 0;
}
