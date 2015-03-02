#include "sha.h"
#include <itoa/itoa.h>

int main() {
        int i;
        for(i=1; i< sizeof(cnk)/sizeof(cnk[0]); i++)
                printf("%2d(%2d,%2d)\t%10llu\t%14lld bytes per\t%s\n",
			i,cnksize(i), cnksize(i)+i+i,
			cnk[i],
1ull<<(2*i-2),itoa(cnk[i]<<(2*i-2)));
        return 0;
}
