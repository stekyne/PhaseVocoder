//
// Phase Vocoder Algorithm Class
// By Stephen Kyne
// Creation Date: 17th August 2010
// 

#include "PV.h"

PV::PV( int	window_size,
		int	fft_size,
		int	init_hopsize )
	:	window_size		( window_size ),
		frame_cur		( 0 ),
		pitch_ratio		( 1.0f ),				// default pitch ratio
		time_ratio		( 1.0f ),				// default time ratio
		numOverlaps		( 4 ),					// 4 overlaps for hanning window
		hop_count		( init_hopsize ),	
		fft_size		( fft_size ),
		bin_count		( fft_size / 2 + 1 ),	// number of bins
		audioIn_size	( window_size * 3 ),	// max size of the audio in buffer
		audioOut_size	( window_size * 2 ),	// max size of the audio out buffer	
		aout_write_pos	( 0 ),
		aout_read_pos	( 0 ),
		ain_write_pos	( 0 ),
		ain_read_pos	( 0 ),							 
		filled			( false ),
		phase_lock		( true ),				// enable phase locking by default
		max				( 2.0f ),
		min				( 0.5f )
{
	// minimum amount of samples required before processing
	min_buffer_fill = max * (float)window_size;

	// overlapping frame counters, keeps track of the offset for each buffer
	frame_counters = new int[ numOverlaps ];

	// window function
	window = new float[ window_size ];

	// audioIn circular buffer holds the samples coming in from a source, file or stream
	// it needs to be big enough to hold at least 'fft_size * 2' as well as
	// the 'maximum pitch_ratio * fft_size', if the pitch is being altered
	audioIn  = new float[ audioIn_size ];
	audioOut = new float[ audioOut_size ];

	// frames to hold data before FFT and after
	front_frame = (float*)fftwf_malloc( sizeof(float) * fft_size );
	back_frame  = (float*)fftwf_malloc( sizeof(float) * fft_size );
		
	// spectral data containers
	fFrame_spec = (fftwf_complex*)fftwf_malloc( sizeof(fftwf_complex) * fft_size );
	bFrame_spec = (fftwf_complex*)fftwf_malloc( sizeof(fftwf_complex) * fft_size );
	last_spec	= (fftwf_complex*)fftwf_malloc( sizeof(fftwf_complex) * fft_size );

	// overlap buffer
	overlap_buffer = new float*[ numOverlaps ];
	
	for( int i=0; i < numOverlaps; i++ ) 
		overlap_buffer[i] = new float[ window_size ];

	// zero buffers
	memset( frame_counters, 0, sizeof(int)	 * numOverlaps );
	memset( audioIn,		0, sizeof(float) * audioIn_size );
	memset( audioOut,		0, sizeof(float) * audioOut_size );
	memset( front_frame,	0, sizeof(float) * fft_size );
	memset( back_frame,		0, sizeof(float) * fft_size );
	memset( fFrame_spec,	0, sizeof(fftwf_complex) * fft_size );
	memset( bFrame_spec,	0, sizeof(fftwf_complex) * fft_size );
	memset( last_spec,		0, sizeof(fftwf_complex) * fft_size );

	for( int i=0; i < numOverlaps; i++ )
		memset( overlap_buffer[i], 0, sizeof(float) * window_size );

	/* end of data buffers section */

	// generate Hann window
	for( int i=0; i < window_size; i++ ) 
	{
		window[i] = (float)(0.5 * ( 1.0 - cos( 2*double_Pi*i / window_size ) ));
	}

	// create plans for FFT and inverse FFT
	front_frame_fft  = fftwf_plan_dft_r2c_1d( fft_size, front_frame, fFrame_spec, FFTW_MEASURE );
	front_frame_ifft = fftwf_plan_dft_c2r_1d( fft_size, fFrame_spec, front_frame, FFTW_MEASURE );

	back_frame_fft   = fftwf_plan_dft_r2c_1d( fft_size, back_frame, bFrame_spec, FFTW_MEASURE );
	back_frame_ifft  = fftwf_plan_dft_c2r_1d( fft_size, bFrame_spec, back_frame, FFTW_MEASURE );
}

PV::~PV()
{
	for( int i=0; i < numOverlaps; i++ )
		delete [] overlap_buffer[i];
	delete [] overlap_buffer;
	
	fftwf_free( front_frame );
	fftwf_free( back_frame );
	fftwf_free( fFrame_spec );
	fftwf_free( bFrame_spec );
	fftwf_free( last_spec );

	fftwf_destroy_plan( front_frame_fft );
	fftwf_destroy_plan( front_frame_ifft );
	fftwf_destroy_plan( back_frame_fft );
	fftwf_destroy_plan( back_frame_ifft );
}

// set pitch transposition, check boundaries
void PV::setPitch( float pitch )	
{ 
	if( pitch > max ) pitch_ratio = 2.0f;
	else if( pitch < min ) pitch_ratio = 0.5;
	else pitch_ratio = pitch; 
}

// set time ratio, check boundaries
void PV::setTimeScale( float time ) 
{
	if( time > max ) time_ratio = 2.0f;
	else if( time < min ) time_ratio = 0.5;
	else time_ratio = time; 
}

// set phase locking switch
void PV::setPhaseLock( bool state )
{
	phase_lock = state; 
}

/*
	This process method is intended for real-time pitch alteration.
 */
void PV::process( const float *input,
				  float	*output,
			      int buffer_size,
				  int hop_size_a )
{
	for( int n=0; n < buffer_size; ++n ) 
	{
		/*
			We need a minimum of 'max_pitch_ratio x fft_size' samples
			before we begin processing
		 */

		// add in sample to the buffer
		audioIn[ ain_write_pos ] = input[n];

		// increment and check reading boundary
		if( ++ain_write_pos == audioIn_size ) ain_write_pos = 0;

		// keep reading in data until we reach the minimum amount of
		// samples needed
		if( ain_write_pos >= min_buffer_fill+1 || filled ) 
		{
			// keep processing from now on
			filled = true;

			/*	
				The next section of code runs at the beginning and every 'hop_size_a'
				samples thereafter. It adds a new block of processed data to the 
				overlapped buffer every 4 'hop_size_a' samples or when 'frame_cur = 0'
			*/
			
			// process new frames when the hop size is reached
			if( hop_count >= hop_size_a )
			{
				// fill two frames of data
				int	  iposa, iposb;			// some variables for the linear
				float frac;					// interpolation
	
				read_posf = ain_read_pos;
				read_posb = (ain_read_pos - hop_size_a < 0) 
							? audioIn_size - hop_size_a:
							  ain_read_pos - hop_size_a;
	
				for( int i=0; i < window_size; ++i ) 
				{	
					// front frame
					iposa  = (int)read_posf;
					iposb  = (iposa + 1 == audioIn_size)? 0 : iposa + 1;
					frac   = read_posf - iposa;

					front_frame[i] = 
						(audioIn[ iposa ] + 
						frac * ( audioIn[ iposb ] - audioIn[ iposa ] ))
						* window[i];
					
					// back frame
					iposa  = (int)read_posb;
					iposb  = (iposa + 1 == audioIn_size)? 0 : iposa + 1;
					frac   = read_posb - iposa;

					back_frame[i] = 
						(audioIn[ iposa ] + 
						frac * ( audioIn[ iposb ] - audioIn[ iposa ] ))
						* window[i];
					
					// increment position by the pitch ratio
					read_posf += pitch_ratio;
					read_posb += pitch_ratio;

					// check reading boundary
					if( read_posf >= audioIn_size ) read_posf -= audioIn_size;
					if( read_posb >= audioIn_size ) read_posb -= audioIn_size;
				}

				// increment reading position, shift position by the hopsize amount
				ain_read_pos += hop_size_a;
				
				// wrap 'read_pos' if it reaches the end of the buffer
				if( ain_read_pos >= audioIn_size ) ain_read_pos = 0;	

				// perform FFTs on frames
				//fftwf_execute( front_frame_fft );
				//fftwf_execute( back_frame_fft );

				///* 
				//	Phase Vocoder Processing 	
				// */
				//float phase[2], temp[2], div;

				//for( int bin=0; bin < bin_count; ++bin ) 
				//{
				//	// get the phases of the last output frame
				//	// by dividing by the magnitude
				//	// add small number to avoid divide by zero
				//	div = 1.0 / (hypotf( last_spec[bin][IMAG], last_spec[bin][REAL] ) + 1e-20);
				//	phase[REAL] = last_spec[bin][REAL] * div;
				//	phase[IMAG] = last_spec[bin][IMAG] * div;
				//	
				//	// sum the phases of the last output and the back frame
				//	temp[REAL] =  bFrame_spec[bin][REAL] * phase[REAL] + bFrame_spec[bin][IMAG] * phase[IMAG];
				//	temp[IMAG] =  bFrame_spec[bin][REAL] * phase[IMAG] - bFrame_spec[bin][IMAG] * phase[REAL];
				//	
				//	// assign the result to the back frame
				//	bFrame_spec[bin][REAL] = temp[REAL];
				//	bFrame_spec[bin][IMAG] = temp[IMAG];
				//}

				//for( int bin=0; bin < bin_count; ++bin ) 
				//{
				//	// phase locking enabled
				//	if( phase_lock ) 
				//	{
				//		// all other bins
				//		if( bin != 0 && bin < bin_count )
				//		{
				//			temp[REAL] = bFrame_spec[bin][REAL] + bFrame_spec[bin-1][REAL] + bFrame_spec[bin+1][REAL];
				//			temp[IMAG] = bFrame_spec[bin][IMAG] + bFrame_spec[bin-1][IMAG] + bFrame_spec[bin+1][IMAG];
				//		}
				//		// Nyquist
				//		else if( bin == bin_count-1 )
				//		{ 
				//			temp[REAL] = bFrame_spec[bin][REAL] + bFrame_spec[bin-1][REAL];
				//			temp[IMAG] = 0.0f;
				//		}
				//		// DC
				//		else if( bin == 0 )
				//		{ 
				//			temp[REAL] = bFrame_spec[bin][REAL] + bFrame_spec[bin+1][REAL];
				//			temp[IMAG] = 0.0f;
				//		}
				//	}
				//	// no phase locking
				//	else 
				//	{	
				//		temp[REAL] = bFrame_spec[bin][REAL];
				//		temp[IMAG] = bFrame_spec[bin][IMAG];
				//	}

				//	temp[REAL] += 1e-20;	// must not be zero
				//	
				//	// get the phases for temp
				//	div = 1.0 / hypotf( temp[REAL], temp[IMAG] );
				//	phase[REAL] = temp[REAL] * div;
				//	phase[IMAG] = temp[IMAG] * div;
	   // 
				//	// add the phases of  temp and the front window
				//	// magnitudes of front frame are kept
				//	temp[REAL] = fFrame_spec[bin][REAL] * phase[REAL] - fFrame_spec[bin][IMAG] * phase[IMAG];
				//	temp[IMAG] = fFrame_spec[bin][REAL] * phase[IMAG] + fFrame_spec[bin][IMAG] * phase[REAL];

				//	// output and store result
				//	last_spec[bin][REAL] = fFrame_spec[bin][REAL] = temp[REAL];
				//	last_spec[bin][IMAG] = fFrame_spec[bin][IMAG] = temp[IMAG];
				//}

				///*
				//	End of Phase Vocoder Processing 
				//*/

				//// perform inverse FFT
				//fftwf_execute( front_frame_ifft );

				// reset counter for current frame
				frame_counters[ frame_cur ] = 0;

				// fill current frame with new data	
				for( int i=0; i < window_size; ++i ) 
				{
					// scale output from iFFT
					//front_frame[i] /= (float)fft_size;

					// write to overlapped buffer and window at the same time
					overlap_buffer[ frame_cur ][i] = front_frame[i] * window[i];
				}
				
				// move to next frame
				// reset back to starting frame if last frame is reached
				if( ++frame_cur == numOverlaps ) frame_cur = 0;

				// reset hop count
				hop_count = 0;
			}
			
			/*	
				This section combines the overlapped buffer to write the output sample 
				This code runs for every loop iteration until 'buffer_size' is reached
			*/

			// write to output buffer
			output[n] = 0.0f;
			for( int i=0; i < numOverlaps; ++i )
			{
				output[n] += overlap_buffer[i][ frame_counters[i] ];
				frame_counters[i]++;
			}
			
			// accumulate overlapping windows and scale output to prevent clipping
			output[n] *= 1.95/3.0;

			hop_count++;
		}
		else 
		{
			// output zeros if we don't have enough samples in the buffer yet
			output[n] = 0.0f;
		}
	}
}

/*
	The offline method expects the data to be properly ordered
	when it is received. It expects enough data for the front and back
	window including the maximum pitch size.

	It will then output a hopsize worth of data.
 */
void PV::processOffline( const float *input,
						 float *output,
						 int hop_size_a,
				  	     int buffer_size )
{
	for( int n=0; n < buffer_size; ++n ) 
	{
		/*	
			The next section of code runs at the beginning and every 'hop_size_a'
			samples thereafter. It adds a new block of processed data to the 
			overlapped buffer every 4 'hop_size_a' samples or when 'frame_cur = 0'
		*/
			
		// process new frames when the hop size is reached
		if( hop_count >= hop_size_a ) 
		{
			// fill two frames of data
			int	  iposa, iposb;			// some variables for the linear
			float frac;					// interpolation
	
			read_posf = hop_size_a;
			read_posb = 0;
	
			for( int i=0; i < window_size; ++i ) 
			{	
				// front frame
				iposa  = (int)read_posf;
				iposb  = (int)read_posf + 1;
				frac   = read_posf - iposa;

				front_frame[i] = 
					(input[ iposa ] + 
					frac * ( input[ iposb ] - input[ iposa ] ))
					* window[i];
					
				// back frame
				iposa  = (int)read_posb;
				iposb  = (int)read_posb + 1;
				frac   = read_posb - iposa;

				back_frame[i] = 
					(input[ iposa ] + 
					frac * ( input[ iposb ] - input[ iposa ] ))
					* window[i];
					
				// increment position by the pitch ratio
				read_posf += pitch_ratio;
				read_posb += pitch_ratio;
			}

			// perform FFTs on frames
			fftwf_execute( front_frame_fft );
			fftwf_execute( back_frame_fft );

			/* 
				Phase Vocoder Processing 	
			*/
			float phase[2], temp[2], div;

			for( int bin=0; bin < bin_count; ++bin ) 
			{
				// get the phases of the last output frame
				// by dividing by the magnitude
				// add small number to avoid divide by zero
				div = 1.0 / (hypotf( last_spec[bin][IMAG], last_spec[bin][REAL] ) + 1e-20);
				phase[REAL] = last_spec[bin][REAL] * div;
				phase[IMAG] = last_spec[bin][IMAG] * div;
					
				// sum the phases of the last output and the back frame
				temp[REAL] =  bFrame_spec[bin][REAL] * phase[REAL] + bFrame_spec[bin][IMAG] * phase[IMAG];
				temp[IMAG] =  bFrame_spec[bin][REAL] * phase[IMAG] - bFrame_spec[bin][IMAG] * phase[REAL];
					
				// assign the result to the back frame
				bFrame_spec[bin][REAL] = temp[REAL];
				bFrame_spec[bin][IMAG] = temp[IMAG];
			}

			for( int bin=0; bin < bin_count; ++bin ) 
			{
				// phase locking enabled
				if( phase_lock ) 
				{
					// all other bins
					if( bin != 0 && bin < bin_count )
					{
						temp[REAL] = bFrame_spec[bin][REAL] + bFrame_spec[bin-1][REAL] + bFrame_spec[bin+1][REAL];
						temp[IMAG] = bFrame_spec[bin][IMAG] + bFrame_spec[bin-1][IMAG] + bFrame_spec[bin+1][IMAG];
					}
					// Nyquist
					else if( bin == bin_count-1 )
					{ 
						temp[REAL] = bFrame_spec[bin][REAL] + bFrame_spec[bin-1][REAL];
						temp[IMAG] = 0.0f;
					}
					// DC
					else if( bin == 0 )
					{ 
						temp[REAL] = bFrame_spec[bin][REAL] + bFrame_spec[bin+1][REAL];
						temp[IMAG] = 0.0f;
					}
				}
				// no phase locking
				else 
				{	
					temp[REAL] = bFrame_spec[bin][REAL];
					temp[IMAG] = bFrame_spec[bin][IMAG];
				}

				temp[REAL] += 1e-20;	// must not be zero
					
				// get the phases for temp
				div = 1.0 / hypotf( temp[REAL], temp[IMAG] );
				phase[REAL] = temp[REAL] * div;
				phase[IMAG] = temp[IMAG] * div;
	    
				// add the phases of  temp and the front window
				// the magnitudes of the front frame will be kept
				temp[REAL] = fFrame_spec[bin][REAL] * phase[REAL] - fFrame_spec[bin][IMAG] * phase[IMAG];
				temp[IMAG] = fFrame_spec[bin][REAL] * phase[IMAG] + fFrame_spec[bin][IMAG] * phase[REAL];

				// output and store result
				last_spec[bin][REAL] = fFrame_spec[bin][REAL] = temp[REAL];
				last_spec[bin][IMAG] = fFrame_spec[bin][IMAG] = temp[IMAG];
			}

			/*
				End of Phase Vocoder Processing 
			*/

			// perform inverse FFT
			fftwf_execute( front_frame_ifft );

			// reset counter for current frame
			frame_counters[ frame_cur ] = 0;

			// fill current frame with new data	
			for( int i=0; i < window_size; ++i ) 
			{
				// scale output from iFFT
				front_frame[i] /= (float)fft_size;

				// write to overlapped buffer and window at the same time
				overlap_buffer[ frame_cur ][i] = front_frame[i] * window[i];
			}
				
			// move to next frame
			// reset back to starting frame if last frame is reached
			if( ++frame_cur == numOverlaps ) frame_cur = 0;

			// reset hop count
			hop_count = 0;
		}
			
		/*	
			This section combines the overlapped buffer to write the output sample 
			This code runs for every loop iteration until 'hop_size_a' is reached
		*/

		// write to output buffer
		output[n] = 0.0f;
		for( int i=0; i < numOverlaps; ++i )
		{
			output[n] += overlap_buffer[i][ frame_counters[i] ];
			frame_counters[i]++;
		}
			
		// accumulate overlapping windows and scale output to prevent clipping
		output[n] *= 1.95/3.0;
			
		hop_count++;
	}
}
