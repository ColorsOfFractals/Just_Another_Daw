#include <juce_gui_extra/juce_gui_extra.h>

class JADMainComponent : public juce::Component
{
public:
    JADMainComponent()
    {
        setSize (720, 420);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour::fromRGB (18, 18, 24));

        g.setColour (juce::Colours::white);
        g.setFont (32.0f);

        g.drawFittedText (
            "JAD",
            getLocalBounds().reduced (20),
            juce::Justification::centred,
            1
        );
    }
};

class JADApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override
    {
        return "JAD";
    }

    const juce::String getApplicationVersion() override
    {
        return "0.0.1";
    }

    void initialise (const juce::String&) override
    {
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

private:
    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)
            : DocumentWindow (
                name,
                juce::Colour::fromRGB (18, 18, 24),
                DocumentWindow::allButtons
            )
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new JADMainComponent(), true);
            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION (JADApplication)
