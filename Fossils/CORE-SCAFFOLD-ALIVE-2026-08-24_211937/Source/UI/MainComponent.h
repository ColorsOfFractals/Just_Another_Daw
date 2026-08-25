#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Core/JADContext.h"

class MainComponent : public juce::Component
{
public:
    explicit MainComponent (JADContext&);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JADContext& context;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
