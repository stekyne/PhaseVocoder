#include <stdio.h>
#include <string>
#include <cmath>

#include "fftw3.h"	// fft library
#include "../JuceLibraryCode/JuceHeader.h"

class PV
{
public:
	PV( int	window_size, int fft_size, int init_hopsize );
	~PV();

	/* 
		Streaming version of the algorithm, can pitch shift
		a real time signal.
	 */
	void process( const float *input, 
				  float *output,
				  int	buffer_size,
				  int	hop_size_a );

	/*
		Non-real time version of the above algorithm. Requires
		the data to be arranged before sent to the function.
	 */
	void processOffline( const float *input,
						 float	*output,
						 int	hop_size_a,
						 int	buffer_size );

	void setPitch( float pitch );
	void setTimeScale( float time );
	void setPhaseLock( bool state );
	
private:
	enum TYPE { REAL = 0, IMAG = 1 };

	// counters and position information
	const int	bin_count,		numOverlaps,
				audioIn_size,	audioOut_size;
			  
	int			frame_cur,
				aout_read_pos,	aout_write_pos,
				ain_read_pos,	ain_write_pos;

	ScopedPointer <int> frame_counters;
	ScopedPointer <float> window;
	float		min_buffer_fill;
	bool		filled, phase_lock;

	// application parameters and settings
	float		pitch_ratio, time_ratio,
				sr,	read_posf, read_posb,
				max, min;

	int			fft_size, hop_count,
				window_size;

	// data buffers
	ScopedPointer<float> audioIn, audioOut;
						 
	float *front_frame,	*back_frame;

	fftwf_complex	*fFrame_spec, *bFrame_spec,
					*last_spec;

	float **overlap_buffer;

	// fftw plans
	fftwf_plan	front_frame_fft, front_frame_ifft,
				back_frame_fft,	 back_frame_ifft;
};