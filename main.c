#include <stdio.h>
#include "OVR_Audio.h"

int main() {
	ovrAudioContextConfiguration config = {};
    
    config.acc_Size = sizeof( config );
    config.acc_SampleRate = 48000;
    config.acc_BufferLength = 512;
    config.acc_MaxNumSources = 16;
    
    ovrAudioContext ovrContext;
    
    if ( ovrAudio_CreateContext( &ovrContext, &config ) != ovrSuccess )
    {
      printf( "Failed to initialize Oculus Audio\n" );
      return 1; 
    }
    printf( "Initialized Oculus Audio\n" );

	return 0;
}
