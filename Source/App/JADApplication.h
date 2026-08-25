#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Core/JADContext.h"

class JADApplication : public juce::JUCEApplication
{
public:
    JADApplication();
    ~JADApplication() override;

    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;

    void initialise (const juce::String&) override;
    void shutdown() override;

private:
    class MainWindow;

    JADContext context;
    std::unique_ptr<MainWindow> mainWindow;
};


