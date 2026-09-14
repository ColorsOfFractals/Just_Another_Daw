#include "PluginSource.h"

#include <algorithm>


PluginSource::PluginSource (
    std::unique_ptr<juce::AudioPluginInstance> instanceToOwn)
    : plugin (std::move (instanceToOwn))
{
}


PluginSource::~PluginSource()
{
    clearPlugin();
}


void PluginSource::setPlugin (
    std::unique_ptr<juce::AudioPluginInstance> instanceToOwn)
{
    clearPlugin();

    plugin =
        std::move (instanceToOwn);

    preparePluginIfPossible();
}


void PluginSource::clearPlugin()
{
    if (plugin != nullptr)
        plugin->releaseResources();

    plugin.reset();
    midiBuffer.clear();

    if (currentSampleRate > 0.0)
        midiCollector.reset (currentSampleRate);
}


bool PluginSource::hasPlugin() const noexcept
{
    return plugin != nullptr;
}


juce::AudioPluginInstance*
PluginSource::getPlugin() noexcept
{
    return plugin.get();
}


const juce::AudioPluginInstance*
PluginSource::getPlugin() const noexcept
{
    return plugin.get();
}


juce::String
PluginSource::getPluginName() const
{
    if (plugin == nullptr)
        return {};

    return plugin->getName();
}


void PluginSource::addMidiMessage (
    const juce::MidiMessage& message)
{
    midiCollector.addMessageToQueue (
        message
    );
}


void PluginSource::prepare (
    double sampleRate,
    int maximumBlockSize)
{
    currentSampleRate =
        sampleRate;

    currentMaximumBlockSize =
        maximumBlockSize;

    midiCollector.reset (
        currentSampleRate
    );

    preparePluginIfPossible();
}


void PluginSource::reset()
{
    midiBuffer.clear();

    if (currentSampleRate > 0.0)
        midiCollector.reset (currentSampleRate);

    if (plugin != nullptr)
        plugin->reset();
}


void PluginSource::preparePluginIfPossible()
{
    if (plugin == nullptr)
        return;

    if (currentSampleRate <= 0.0)
        return;

    if (currentMaximumBlockSize <= 0)
        return;

    plugin->setRateAndBufferSizeDetails (
        currentSampleRate,
        currentMaximumBlockSize
    );

    plugin->prepareToPlay (
        currentSampleRate,
        currentMaximumBlockSize
    );
}


void PluginSource::render (
    juce::AudioBuffer<float>& buffer,
    int numSamples)
{
    const int samplesToProcess =
        std::min (
            numSamples,
            buffer.getNumSamples()
        );

    buffer.clear();

    if (plugin == nullptr)
        return;

    if (samplesToProcess <= 0)
        return;

    juce::AudioBuffer<float> block (
        buffer.getArrayOfWritePointers(),
        buffer.getNumChannels(),
        0,
        samplesToProcess
    );

    midiBuffer.clear();

    midiCollector.removeNextBlockOfMessages (
        midiBuffer,
        samplesToProcess
    );

    plugin->processBlock (
        block,
        midiBuffer
    );
}