#include "JADApplication.h"
#include "../UI/MainComponent.h"

class JADApplication::MainWindow : public juce::DocumentWindow
{
public:
    explicit MainWindow (JADContext& context)
        : DocumentWindow (
            "JAD",
            juce::Colour::fromRGB (18, 18, 24),
            DocumentWindow::allButtons
        )
    {
        setUsingNativeTitleBar (true);
        setContentOwned (new MainComponent (context), true);
        centreWithSize (getWidth(), getHeight());
        setVisible (true);
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

JADApplication::JADApplication() = default;
JADApplication::~JADApplication() = default;

const juce::String JADApplication::getApplicationName()
{
    return "JAD";
}

const juce::String JADApplication::getApplicationVersion()
{
    return "0.0.2";
}

void JADApplication::initialise (const juce::String&)
{
    mainWindow = std::make_unique<MainWindow> (context);
}

void JADApplication::shutdown()
{
    mainWindow.reset();
}


