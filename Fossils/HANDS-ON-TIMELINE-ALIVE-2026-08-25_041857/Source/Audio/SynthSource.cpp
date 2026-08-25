#include "SynthSource.h"

#include <cmath>

void SynthSource::prepare (
    double sampleRate,
    int
)
{
    currentSampleRate = sampleRate;

    phase = 0.0;
    appliedGate = false;

    envelope.reset();

    if (currentSampleRate > 0.0)
        envelope.setSampleRate (currentSampleRate);

    juce::ADSR::Parameters parameters;

    parameters.attack  = 0.08f;
    parameters.decay   = 0.22f;
    parameters.sustain = 0.65f;
    parameters.release = 0.55f;

    envelope.setParameters (
        parameters
    );
}

void SynthSource::reset()
{
    requestedGate.store (false);

    appliedGate = false;

    envelope.reset();

    phase = 0.0;
}

void SynthSource::setGate (
    bool enabled
) noexcept
{
    requestedGate.store (
        enabled
    );
}

bool SynthSource::isGateEnabled() const noexcept
{
    return requestedGate.load();
}

void SynthSource::setFrequency (
    double newFrequency
) noexcept
{
    frequencyHz.store (
        juce::jlimit (
            40.0,
            2000.0,
            newFrequency
        )
    );
}

double SynthSource::getFrequency() const noexcept
{
    return frequencyHz.load();
}

void SynthSource::setGain (
    float newGain
) noexcept
{
    outputGain.store (
        juce::jlimit (
            0.0f,
            0.20f,
            newGain
        )
    );
}

float SynthSource::getGain() const noexcept
{
    return outputGain.load();
}

void SynthSource::setWaveform (
    Waveform newWaveform
) noexcept
{
    waveform.store (
        static_cast<int> (
            newWaveform
        )
    );
}

SynthSource::Waveform
SynthSource::getWaveform() const noexcept
{
    return static_cast<Waveform> (
        waveform.load()
    );
}

void SynthSource::updateEnvelopeGate() noexcept
{
    const auto requested =
        requestedGate.load();

    if (requested == appliedGate)
        return;

    if (requested)
        envelope.noteOn();

    if (! requested)
        envelope.noteOff();

    appliedGate = requested;
}

float SynthSource::renderOscillatorSample() noexcept
{
    const auto selected =
        static_cast<Waveform> (
            waveform.load()
        );

    if (selected == Waveform::saw)
    {
        return static_cast<float> (
            (phase
                / juce::MathConstants<double>::pi)
            - 1.0
        );
    }

    if (selected == Waveform::square)
    {
        return phase
            < juce::MathConstants<double>::pi
                ? 1.0f
                : -1.0f;
    }

    return static_cast<float> (
        std::sin (phase)
    );
}

void SynthSource::render (
    juce::AudioBuffer<float>& destination,
    int numSamples
)
{
    if (currentSampleRate <= 0.0)
        return;

    if (numSamples <= 0)
        return;

    updateEnvelopeGate();

    const auto frequency =
        frequencyHz.load();

    const auto gain =
        outputGain.load();

    const auto phaseIncrement =
        juce::MathConstants<double>::twoPi
        * frequency
        / currentSampleRate;

    const auto channels =
        std::min (
            2,
            destination.getNumChannels()
        );

    for (
        int sample = 0;
        sample < numSamples;
        ++sample
    )
    {
        const auto envelopeLevel =
            envelope.getNextSample();

        const auto value =
            renderOscillatorSample()
            * gain
            * envelopeLevel;

        for (
            int channel = 0;
            channel < channels;
            ++channel
        )
        {
            destination.setSample (
                channel,
                sample,
                value
            );
        }

        phase += phaseIncrement;

        while (
            phase
            >= juce::MathConstants<double>::twoPi
        )
        {
            phase -=
                juce::MathConstants<double>::twoPi;
        }
    }
}
