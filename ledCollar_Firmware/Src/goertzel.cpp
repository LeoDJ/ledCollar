#include "goertzel.h"
#include "math.h"


void GoertzelFilter::processSample(float sample) {
    float Q0;
    Q0 = _coeff * _Q1 - _Q2 + sample;
    _Q2 = _Q1;
    _Q1 = Q0;
}

float GoertzelFilter::calculateMagnitude(bool doReset) {
    float magnitudeSquared = _Q1 * _Q1 + _Q2 * _Q2 - _Q1 * _Q2 * _coeff;
    if(doReset) {
        reset();
    }
    return sqrt(magnitudeSquared);
}

void GoertzelFilter::reset() {
    _Q1 = 0;
    _Q2 = 0;
}

void GoertzelFilter::precomputeConstants() {
    int k;
    float omega;

    k = (int)(0.5 + ((_binSize * _targetFrequency) / _sampleRate));
    omega = (2.0 * PI * k) / _binSize;
    // omega = (2.0 * PI * TARGET_FREQ) / SAMPLING_RATE; // implementation of the Goertzel Arduino lib
    _sine = sin(omega);
    _cosine = cos(omega);
    _coeff = 2.0 * _cosine;
}

GoertzelFilter::GoertzelFilter(float targetFrequency, int binSize, float sampleRate) {
    _targetFrequency = targetFrequency;
    _binSize = binSize;
    _sampleRate = sampleRate;

    precomputeConstants();
    reset();
}