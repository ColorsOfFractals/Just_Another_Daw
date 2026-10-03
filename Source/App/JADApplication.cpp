#include "JADApplication.h"
#include "UpdateChecker.h"

#include "../UI/MainComponent.h"

class JADApplication::MainWindow :
    public juce::DocumentWindow
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
        setContentOwned (
            new MainComponent (context),
            true
        );

        centreWithSize (
            getWidth(),
            getHeight()
        );

        setVisible (true);
    }

    void showUpdateAvailable (
        const UpdateChecker::ReleaseInfo& release)
    {
        auto message =
            "Your little music machine has a new gear ready.\n\n"
            "Installed: "
            + juce::String (JAD_VERSION_STRING)
            + "\nAvailable: "
            + release.version;

        if (release.name.isNotEmpty()
            && release.name != release.version)
        {
            message += "\n\n" + release.name;
        }

        auto options =
            juce::MessageBoxOptions()
                .withIconType (
                    juce::MessageBoxIconType::InfoIcon
                )
                .withTitle (
                    "A NEW JAD IS READY"
                )
                .withMessage (message)
                .withButton ("UPDATE NOW")
                .withButton ("LATER")
                .withAssociatedComponent (this);

        juce::AlertWindow::showAsync (
            options,
            [releasePage = release.releasePage]
            (int result)
            {
                if (result == 1)
                    releasePage.launchInDefaultBrowser();
            }
        );
    }

    void closeButtonPressed() override
    {
        juce::JUCEApplication::getInstance()
            ->systemRequestedQuit();
    }
};

JADApplication::JADApplication() = default;
JADApplication::~JADApplication() = default;

const juce::String
JADApplication::getApplicationName()
{
    return "JAD";
}

const juce::String
JADApplication::getApplicationVersion()
{
    return JAD_VERSION_STRING;
}

void JADApplication::initialise (
    const juce::String&)
{
    mainWindow =
        std::make_unique<MainWindow> (context);

    auto safeWindow =
        juce::Component::SafePointer<MainWindow> (
            mainWindow.get()
        );

    updateChecker =
        std::make_unique<UpdateChecker> (
            [safeWindow]
            (std::optional<UpdateChecker::ReleaseInfo> result)
            {
                if (safeWindow != nullptr
                    && result.has_value())
                {
                    safeWindow->showUpdateAvailable (
                        *result
                    );
                }
            }
        );

    updateChecker->checkNow();
}

void JADApplication::shutdown()
{
    updateChecker.reset();
    mainWindow.reset();
}