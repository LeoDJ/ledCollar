#pragma once

/**
 * A library for the Goertzel filter as described in https://www.embedded.com/the-goertzel-algorithm/
 * Author: @LeoDJ
 */

class GoertzelFilter {
    public:
        /** Initialize the filter with the necessary parameters
         *  @param targetFrequency  Should ideally be an integer of samplingFrequency/binSize 
         *                          for frequencies to distribute equally around the target
         *                          The highest frequency is samplingFrequency/2
         *  @param binSize          Number of samples needed before doing a conversion. 
         *                          A higher binCount takes more time to process.
         *                          Bin width in Hz = sampleRate/binCount.
         *  @param sampleRate       Specify the sample rate of your application */
        GoertzelFilter(float targetFrequency, int binSize, float sampleRate);
        
        /** Process a single data sample
         *  You have to call this function exactly binSize times before executing calculateMagnitude()
         *  @param sample   the sample directly from your ADC */
        void processSample(float sample);

        /** Calculate the magnitude of the filtered signal and also reset the internal variables
         *  @param doReset  don't reset the internal variables
         *  @returns        the square root of the squared magnitude */
        float calculateMagnitude(bool doReset = true);
        
        // Reset internal variables
        void reset();

    private:
        void precomputeConstants();
        float _targetFrequency, _binSize, _sampleRate;
        float _sine, _cosine, _coeff;
        float _Q1, _Q2;
};

#ifndef PI
    #define PI 3.141592653589793
#endif