#include "UpdateChecker.h"

#include <algorithm>

namespace
{
    juce::String cleanVersion (juce::String version)
    {
        version = version.trim();

        if (version.startsWithIgnoreCase ("v"))
            version = version.substring (1);

        return version.upToFirstOccurrenceOf (
            "-",
            false,
            false
        );
    }

    juce::StringArray splitVersion (
        const juce::String& version)
    {
        juce::StringArray pieces;

        pieces.addTokens (
            cleanVersion (version),
            ".",
            {}
        );

        return pieces;
    }
}

UpdateChecker::UpdateChecker (
    ResultCallback callbackToUse)
    : juce::Thread ("JAD Update Checker"),
      callback (std::move (callbackToUse))
{
}

UpdateChecker::~UpdateChecker()
{
    signalThreadShouldExit();
    stopThread (6000);
}

void UpdateChecker::checkNow()
{
    if (! isThreadRunning())
    {
        startThread (
            juce::Thread::Priority::background
        );
    }
}

bool UpdateChecker::isNewerVersion (
    const juce::String& candidate,
    const juce::String& current)
{
    const auto candidateParts =
        splitVersion (candidate);

    const auto currentParts =
        splitVersion (current);

    const auto count = std::max (
        candidateParts.size(),
        currentParts.size()
    );

    for (int index = 0; index < count; ++index)
    {
        const auto candidatePart =
            index < candidateParts.size()
                ? candidateParts[index].getIntValue()
                : 0;

        const auto currentPart =
            index < currentParts.size()
                ? currentParts[index].getIntValue()
                : 0;

        if (candidatePart > currentPart)
            return true;

        if (candidatePart < currentPart)
            return false;
    }

    return false;
}

void UpdateChecker::run()
{
    const juce::URL endpoint (
        "https://api.github.com/repos/"
        "ColorsOfFractals/"
        "Just_Another_Daw/"
        "releases/latest"
    );

    const auto options =
        juce::URL::InputStreamOptions (
            juce::URL::ParameterHandling::inAddress
        )
        .withConnectionTimeoutMs (8000)
        .withExtraHeaders (
            "Accept: application/vnd.github+json\r\n"
            "X-GitHub-Api-Version: 2022-11-28\r\n"
            "User-Agent: JAD-Updater\r\n"
        );

    auto stream =
        endpoint.createInputStream (options);

    if (threadShouldExit() || stream == nullptr)
        return;

    const auto response =
        stream->readEntireStreamAsString();

    if (threadShouldExit() || response.isEmpty())
        return;

    const auto parsed =
        juce::JSON::parse (response);

    if (! parsed.isObject())
        return;

    ReleaseInfo release;

    release.version =
        parsed.getProperty (
            "tag_name",
            {}
        ).toString();

    release.name =
        parsed.getProperty (
            "name",
            {}
        ).toString();

    release.notes =
        parsed.getProperty (
            "body",
            {}
        ).toString();

    release.releasePage =
        juce::URL (
            parsed.getProperty (
                "html_url",
                {}
            ).toString()
        );

    if (
        auto* assets =
            parsed.getProperty (
                "assets",
                {}
            ).getArray()
    )
    {
        for (const auto& asset : *assets)
        {
            if (! asset.isObject())
                continue;

            const auto assetName =
                asset.getProperty (
                    "name",
                    {}
                ).toString();

            const auto assetUrl =
                asset.getProperty (
                    "browser_download_url",
                    {}
                ).toString();

            if (
                assetName.endsWithIgnoreCase (
                    "-windows-x64.zip"
                )
            )
            {
                release.zipUrl = assetUrl;
            }
            else if (
                assetName.endsWithIgnoreCase (
                    ".sha256"
                )
            )
            {
                release.checksumUrl = assetUrl;
            }
        }
    }

    if (
        release.version.isEmpty()
        || ! isNewerVersion (
            release.version,
            JAD_VERSION_STRING
        )
    )
    {
        return;
    }

    const auto callbackCopy = callback;

    juce::MessageManager::callAsync (
        [callbackCopy, release]
        {
            if (callbackCopy)
                callbackCopy (release);
        }
    );
}