#include <stdio.h>
#include "phonon.h"

int main() {
    IPLhandle phononContext;
    IPLerror status;
    status = iplCreateContext(NULL, NULL, NULL, &phononContext);
    if (status != IPL_STATUS_SUCCESS) {
      printf( "Failed to initialize Steam Audio\n" );
    }
    printf( "Initialized Steam Audio\n" );

	return 0;
}
