#include <stdio.h>
#include <string.h>
#include "people.h"

int main()
{   char answer[10];
    printf("Who you think the GOAT? LeBron or Kobe \n");
    scanf("%s", answer);

    if (strcasecmp(answer, "LeBron") == 0)
    {
        lebrontheking();
        return 0;
    }
    else if (strcasecmp(answer, "Kobe") == 0)
    {
        kobethemamba();
        return 0;
    }
    else 
    {
        dumbperson();
        return 0;
    }
    return 0;
}