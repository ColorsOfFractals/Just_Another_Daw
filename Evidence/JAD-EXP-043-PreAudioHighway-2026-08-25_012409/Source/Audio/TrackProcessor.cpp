#include "TrackProcessor.h"

#include "../Model/TrackModel.h"

#include <algorithm>
#include <cmath>

void TrackProcessor::setModel (
    TrackModel* modelToUse
) noexcept
{
    model = modelToUse;
}

void TrackProcessor::setSource (
    AudioSource* sourceToUse
) noexcept
{
    source = sourceToUse;
}

void TrackProcessor::prepare (
    double sampleRate,
    int maximumBlockSize
)
{
    workingBuffer.setSize (
        2,
        std::max (1, maximumBlockSize),
        false,
        true,
        true
    );

    workingBuffer.clear();

    if (source != nullptr)
    {
        source->prepare (
            sampleRate,
            maximumBlockSize
        );
    }
}

void TrackProcessor::reset()
{
    workingBuffer.clear();

    if (source != nullptr)
        source->reset();
}

void TrackProcessor::process (
    juce::AudioBuffer<float>& mixBuffer,
    int numSamples,
    bool anyTrackSoloed
)
{
    if (model == nullptr)
        return;

    if (source == nullptr)
        return;

    if (numSamples <= 0)
        return;

    const auto muted =
        model->isMuted();

    const auto soloed =
        model->isSolo();

    if (muted)
        return;

    if (anyTrackSoloed && ! soloed)
        return;

    if (
        workingBuffer.getNumSamples()
        < numSamples
    )
    {
        workingBuffer.setSize (
            2,
            numSamples,
            false,
            false,
            true
        );
    }

    workingBuffer.clear();

    source->render (
        workingBuffer,
        numSamples
    );

    const auto gain =
        model->getGain();

    const auto pan =
        model->getPan();

    const auto angle =
        static_cast<float> (
            (pan + 1.0f)
            * juce::MathConstants<float>::quarterPi
        );

    const auto leftGain =
        gain * std::cos (angle);

    const auto rightGain =
        gain * std::sin (angle);

    workingBuffer.applyGain (
        0,
        0,
        numSamples,
        leftGain
    );

    workingBuffer.applyGain (
        1,
        0,
        numSamples,
        rightGain
    );

    const auto channels =
        std::min (
            2,
            mixBuffer.getNumChannels()
        );

    for (
        int channel = 0;
        channel < channels;
        ++channel
    )
    {
        mixBuffer.addFrom (
            channel,
            0,
            workingBuffer,
            channel,
            0,
            numSamples
        );
    }
}
