//
// Phase Vocoder Algorithm Class
// By Stephen Kyne
// Creation Date: 17th August 2010
// 

#include "PV.h"

PV::PV (int windowSize_, int fftSize_, int hopSize_) :
	windowSize (windowSize_),
	currentFrame (0),
	pitchRatio (1.0f),				// default pitch ratio
	timeRatio (1.0f),				// default time ratio
	numOverlaps (4),				// 4 overlaps for hanning window
	hopCount (hopSize_),
	fftSize (fftSize_),
	binCount (fftSize_ / 2 + 1),	// number of bins
	audioInSize (windowSize_ * 3),	// max size of the audio in buffer
	audioOutSize (windowSize_ * 2),	// max size of the audio out buffer	
	aOutWritePos (0),
	aOutReadPos (0),
	aInWritePos (0),
	aInReadPos (0),
	filled (false),
	phaseLock (true),				// enable phase locking by default
	max (2.0f),
	min (0.5f),
	fft (13, false),
	iFft (13, true)
{
	// minimum amount of samples required before processing
	minBufferFill = max * (float)windowSize;

	// overlapping frame counters, keeps track of the offset for each buffer
	frameCounters = new int[numOverlaps];

	// window function
	window = new float[windowSize];

	// audioIn circular buffer holds the samples coming in from a source, file or stream
	// it needs to be big enough to hold at least 'fft_size * 2' as well as
	// the 'maximum pitch_ratio * fft_size', if the pitch is being altered
	audioIn = new float[audioInSize];
	audioOut = new float[audioOutSize];

	frontFrame = new float[fftSize];
	backFrame = new float[fftSize];

	fFrameSpec = new FFT::Complex[fftSize];
	bFrameSpec = new FFT::Complex[fftSize];
	lastSpec = new FFT::Complex[fftSize];

	// frames to hold data before FFT and aft
	// overlap buffer
	overlapBuffer = new float*[numOverlaps];

	for (size_t i = 0; i < numOverlaps; i++)
		overlapBuffer[i] = new float[windowSize];

	// zero buffers
	memset (frameCounters, 0, sizeof (int) * numOverlaps);
	memset (audioIn, 0, sizeof (float) * audioInSize);
	memset (audioOut, 0, sizeof (float) * audioOutSize);
	memset (frontFrame, 0, sizeof (float) * fftSize);
	memset (backFrame, 0, sizeof (float) * fftSize);
	memset (fFrameSpec, 0, sizeof (FFT::Complex) * fftSize);
	memset (bFrameSpec, 0, sizeof (FFT::Complex) * fftSize);
	memset (lastSpec, 0, sizeof (FFT::Complex) * fftSize);

	for (size_t i = 0; i < numOverlaps; i++)
		memset (overlapBuffer[i], 0, sizeof (float) * windowSize);

	// generate Hann window
	for (size_t i = 0; i < windowSize; i++)
		window[i] = (float)(0.5 * (1.0 - cos (2 * double_Pi*i / windowSize)));
}

PV::~PV ()
{
	delete[] frameCounters; frameCounters = nullptr;
	delete[] window; window = nullptr;
	delete[] audioIn; audioIn = nullptr;
	delete[] audioOut; audioOut = nullptr;
	delete[] frontFrame; frontFrame = nullptr;
	delete[] backFrame; backFrame = nullptr;
	delete[] fFrameSpec; fFrameSpec = nullptr;
	delete[] bFrameSpec; bFrameSpec = nullptr;
	delete[] lastSpec; lastSpec = nullptr;

	for (size_t i = 0; i < numOverlaps; i++) {
		delete[] overlapBuffer[i];
		overlapBuffer[i] = nullptr;
	}

	delete[] overlapBuffer; overlapBuffer = nullptr;
}

// set pitch transposition, check boundaries
void PV::setPitch (float pitch)
{
	if (pitch > max) pitchRatio = 2.0f;
	else if (pitch < min) pitchRatio = 0.5;
	else pitchRatio = pitch;
}

// set time ratio, check boundaries
void PV::setTimeScale (float time)
{
	if (time > max) timeRatio = 2.0f;
	else if (time < min) timeRatio = 0.5;
	else timeRatio = time;
}

// set phase locking switch
void PV::setPhaseLock (bool state)
{
	phaseLock = state;
}

/*
	This process method is intended for real-time pitch alteration.
 */
void PV::process (const float* input, float* output, size_t bufferSize, size_t hopSizeA)
{
	for (size_t n = 0; n < bufferSize; ++n)
	{
		/*
			We need a minimum of 'maxPitchRatio x fftSize' samples
			before we begin processing
		 */

		 // add in sample to the buffer
		audioIn[aInWritePos] = input[n];

		// increment and check reading boundary
		if (++aInWritePos == audioInSize) 
			aInWritePos = 0;

		// keep reading in data until we reach the minimum amount of
		// samples needed
		if (aInWritePos >= minBufferFill + 1 || filled)
		{
			// keep processing from now on
			filled = true;

			/*
				The next section of code runs at the beginning and every 'hop_size_a'
				samples thereafter. It adds a new block of processed data to the
				overlapped buffer every 4 'hop_size_a' samples or when 'frame_cur = 0'
			*/

			// process new frames when the hop size is reached
			if (hopCount >= hopSizeA)
			{
				// fill two frames of data
				size_t iposa, iposb;
				float frac;

				readPosF = (float)aInReadPos;
				readPosB = (aInReadPos - hopSizeA < 0) ? 
					(float)(audioInSize - hopSizeA) :
					(float)(aInReadPos - hopSizeA);

				for (size_t i = 0; i < windowSize; ++i)
				{
					// front frame
					iposa = (int)readPosF;
					iposb = (iposa + 1 == audioInSize) ? 0 : iposa + 1;
					frac = readPosF - iposa;

					frontFrame[i] =
						(audioIn[iposa] +
							frac * (audioIn[iposb] - audioIn[iposa]))
						* window[i];

					// back frame
					iposa = (int)readPosB;
					iposb = (iposa + 1 == audioInSize) ? 0 : iposa + 1;
					frac = readPosB - iposa;

					backFrame[i] =
						(audioIn[iposa] +
							frac * (audioIn[iposb] - audioIn[iposa]))
						* window[i];

					// increment position by the pitch ratio
					readPosF += pitchRatio;
					readPosB += pitchRatio;

					// check reading boundary
					if (readPosF >= audioInSize) 
						readPosF -= audioInSize;
					
					if (readPosB >= audioInSize) 
						readPosB -= audioInSize;
				}

				// increment reading position, shift position by the hopsize amount
				aInReadPos += hopSizeA;

				// wrap 'read_pos' if it reaches the end of the buffer
				if (aInReadPos >= audioInSize) 
					aInReadPos = 0;

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
				frameCounters[currentFrame] = 0;

				// fill current frame with new data	
				for (size_t i = 0; i < windowSize; ++i)
				{
					// scale output from iFFT
					//front_frame[i] /= (float)fft_size;

					// write to overlapped buffer and window at the same time
					overlapBuffer[currentFrame][i] = frontFrame[i] * window[i];
				}

				// move to next frame
				// reset back to starting frame if last frame is reached
				if (++currentFrame == numOverlaps) 
					currentFrame = 0;

				// reset hop count
				hopCount = 0;
			}

			/*
				This section combines the overlapped buffer to write the output sample
				This code runs for every loop iteration until 'buffer_size' is reached
			*/

			// write to output buffer
			output[n] = 0.0f;

			for (size_t i = 0; i < numOverlaps; ++i)
			{
				output[n] += overlapBuffer[i][frameCounters[i]];
				frameCounters[i]++;
			}

			// accumulate overlapping windows and scale output to prevent clipping
			output[n] *= (float)(1.95 / 3.0f);

			hopCount++;
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
void PV::processOffline (const float *input, float *output,	
						 size_t hopSizeA, size_t bufferSize)
{
	for (size_t n = 0; n < bufferSize; ++n)
	{
		/*
			The next section of code runs at the beginning and every 'hop_size_a'
			samples thereafter. It adds a new block of processed data to the
			overlapped buffer every 4 'hop_size_a' samples or when 'frame_cur = 0'
		*/

		// process new frames when the hop size is reached
		if (hopCount >= hopSizeA)
		{
			// fill two frames of data
			int	  iposa, iposb;			// some variables for the linear
			float frac;					// interpolation

			readPosF = (float)hopSizeA;
			readPosB = 0.f;

			for (size_t i = 0; i < windowSize; ++i)
			{
				// front frame
				iposa = (int)readPosF;
				iposb = (int)readPosF + 1;
				frac = readPosF - iposa;

				frontFrame[i] =
					(input[iposa] +
						frac * (input[iposb] - input[iposa]))
					* window[i];

				// back frame
				iposa = (int)readPosB;
				iposb = (int)readPosB + 1;
				frac = readPosB - iposa;

				backFrame[i] =
					(input[iposa] +
						frac * (input[iposb] - input[iposa]))
					* window[i];

				// increment position by the pitch ratio
				readPosF += pitchRatio;
				readPosB += pitchRatio;
			}

			fft.performRealOnlyForwardTransform (frontFrame);
			fft.performRealOnlyForwardTransform (backFrame);

			/*
				Phase Vocoder Processing
			*/
			double phase[2], temp[2], div;

			for (size_t bin = 0; bin < binCount; ++bin)
			{
				// get the phases of the last output frame
				// by dividing by the magnitude
				// add small number to avoid divide by zero
				div = 1.0 / (hypotf (lastSpec[bin].i, lastSpec[bin].r) + 1e-20);
				phase[REAL] = lastSpec[bin].r * div;
				phase[IMAG] = lastSpec[bin].i * div;

				// sum the phases of the last output and the back frame
				temp[REAL] = bFrameSpec[bin].r * phase[REAL] + bFrameSpec[bin].i * phase[IMAG];
				temp[IMAG] = bFrameSpec[bin].r * phase[IMAG] - bFrameSpec[bin].i * phase[REAL];

				// assign the result to the back frame
				bFrameSpec[bin].r = (float)temp[REAL];
				bFrameSpec[bin].i = (float)temp[IMAG];
			}

			for (size_t bin = 0; bin < binCount; ++bin)
			{
				// phase locking enabled
				if (phaseLock)
				{
					// all other bins
					if (bin != 0 && bin < binCount)
					{
						temp[REAL] = bFrameSpec[bin].r + bFrameSpec[bin - 1].r + bFrameSpec[bin + 1].r;
						temp[IMAG] = bFrameSpec[bin].i + bFrameSpec[bin - 1].i + bFrameSpec[bin + 1].i;
					}
					// Nyquist
					else if (bin == binCount - 1)
					{
						temp[REAL] = bFrameSpec[bin].r + bFrameSpec[bin - 1].r;
						temp[IMAG] = 0.0f;
					}
					// DC
					else if (bin == 0)
					{
						temp[REAL] = bFrameSpec[bin].r + bFrameSpec[bin + 1].r;
						temp[IMAG] = 0.0f;
					}
				}
				// no phase locking
				else
				{
					temp[REAL] = bFrameSpec[bin].r;
					temp[IMAG] = bFrameSpec[bin].i;
				}

				temp[REAL] += 1e-20;	// very small number avoiding 0

				// get the phases for temp
				div = 1.0 / hypot (temp[REAL], temp[IMAG]);
				phase[REAL] = temp[REAL] * div;
				phase[IMAG] = temp[IMAG] * div;

				// add the phases of  temp and the front window
				// the magnitudes of the front frame will be kept
				temp[REAL] = bFrameSpec[bin].r * phase[REAL] - bFrameSpec[bin].i * phase[IMAG];
				temp[IMAG] = bFrameSpec[bin].r * phase[IMAG] + bFrameSpec[bin].i * phase[REAL];

				// output and store result
				lastSpec[bin].r = bFrameSpec[bin].r = (float)temp[REAL];
				lastSpec[bin].i = bFrameSpec[bin].i = (float)temp[IMAG];
			}

			// perform inverse FFT
			iFft.performRealOnlyInverseTransform (frontFrame);

			// reset counter for current frame
			frameCounters[currentFrame] = 0;

			// fill current frame with new data	
			for (size_t i = 0; i < windowSize; ++i)
			{
				// scale output from iFFT
				frontFrame[i] /= (float)fftSize;

				// write to overlapped buffer and window at the same time
				overlapBuffer[currentFrame][i] = frontFrame[i] * window[i];
			}

			// move to next frame
			// reset back to starting frame if last frame is reached
			if (++currentFrame == numOverlaps) 
				currentFrame = 0;

			// reset hop count
			hopCount = 0;
		}

		/*
			This section combines the overlapped buffer to write the output sample
			This code runs for every loop iteration until 'hop_size_a' is reached
		*/

		// write to output buffer
		output[n] = 0.0f;

		for (size_t i = 0; i < numOverlaps; ++i)
		{
			output[n] += overlapBuffer[i][frameCounters[i]];
			frameCounters[i]++;
		}

		// accumulate overlapping windows and scale output to prevent clipping
		// TODO this shouldn't be required, remove if possible
		output[n] *= (float)(1.95 / 3.0);

		hopCount++;
	}
}
