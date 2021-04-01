#include <stdio.h>
#include "OVR_Audio.h"

int main() {
	ovrAudioContextConfiguration config = {};
    
    config.acc_Size = sizeof( config );
    config.acc_SampleRate = 48000;
    config.acc_BufferLength = 512;
    config.acc_MaxNumSources = 16;
    
    ovrAudioContext context;
    
    if ( ovrAudio_CreateContext( &context, &config ) != ovrSuccess )
    {
      printf( "WARNING: Could not create context!\n" );
      return 1; 
    }

	return 0;
}
