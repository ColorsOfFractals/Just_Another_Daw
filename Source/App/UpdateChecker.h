#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include <functional>
#include <optional>

class UpdateChecker : private juce::Thread
{
public:
    struct ReleaseInfo
    {
        juce::String version;
        juce::String name;
        juce::String notes;

        juce::URL releasePage;

        juce::String zipUrl;
        juce::String checksumUrl;
    };

    using ResultCallback =
        std::function<void (std::optional<ReleaseInfo>)>;

    explicit UpdateChecker (ResultCallback callbackToUse);
    ~UpdateChecker() override;

    void checkNow();

    static bool isNewerVersion (
        const juce::String& candidate,
        const juce::String& current
    );

private:
    void run() override;

    ResultCallback callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        UpdateChecker
    )
};