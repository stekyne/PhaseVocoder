#include <stdio.h>
#include <string>
#include <cmath>

#include "../JuceLibraryCode/JuceHeader.h"

class PV
{
public:
	PV (int	windowSize, int fftSize, int hopSize);
	~PV ();

	/* 
		Streaming version of the algorithm, can pitch shift
		a real time signal.
	 */
	void process (const float *input, float *output,
				  size_t buffer_size, size_t hop_size_a);

	/*
		Non-real time version of the above algorithm. Requires
		the data to be arranged before sent to the function.
	 */
	void processOffline (const float *input, float* output,
						 size_t hop_size_a, size_t buffer_size);

	void setPitch (float pitch);
	void setTimeScale (float time);
	void setPhaseLock (bool state);
	
private:
	enum
	{
		REAL = 0,
		IMAG =1 
	};

	FFT fft;
	FFT iFft;

	// counters and position information
	const size_t binCount {0};
	const size_t numOverlaps {0};
	const size_t audioInSize {0};
	const size_t audioOutSize {0};
			  
	size_t currentFrame {0};
	size_t aOutReadPos {0};
	size_t aOutWritePos {0};
	size_t aInReadPos {0};
	size_t aInWritePos {0};

	ScopedPointer <int> frameCounters;
	ScopedPointer <float> window;
	float minBufferFill {0.f};
	bool filled {false};
	bool phaseLock {false};

	// application parameters and settings
	float pitchRatio {0.f};
	float timeRatio {0.f};
	float sampleRate {0.f};
	float readPosF {0.f};
	float readPosB {0.f};
	float max {0.f};
	float min {0.f};

	size_t fftSize {0};
	size_t hopCount {0};
	size_t windowSize {0};

	// data buffers
	ScopedPointer<float> audioIn;
	ScopedPointer<float> audioOut;
						 
	float* frontFrame {nullptr};
	float* backFrame {nullptr};
	float** overlapBuffer {nullptr};
	FFT::Complex* fFrameSpec {nullptr};
	FFT::Complex* bFrameSpec {nullptr};
	FFT::Complex* lastSpec {nullptr};
};