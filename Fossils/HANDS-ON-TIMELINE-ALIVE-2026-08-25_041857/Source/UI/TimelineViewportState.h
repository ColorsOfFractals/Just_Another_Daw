#pragma once

#include <juce_core/juce_core.h>

#include <cmath>

class TimelineViewportState
{
public:
    double getStartBeat() const noexcept
    {
        return startBeat;
    }

    double getVisibleBeats() const noexcept
    {
        return visibleBeats;
    }

    double getEndBeat() const noexcept
    {
        return startBeat + visibleBeats;
    }

    double getSnapBeats() const noexcept
    {
        return snapBeats;
    }

    bool isSnapEnabled() const noexcept
    {
        return snapEnabled;
    }

    void setSnapEnabled (
        bool enabled) noexcept
    {
        snapEnabled = enabled;
    }

    void toggleSnap() noexcept
    {
        snapEnabled = ! snapEnabled;
    }

    void setSnapBeats (
        double beats) noexcept
    {
        snapBeats =
            juce::jlimit (
                0.0625,
                4.0,
                beats
            );
    }

    void zoomIn() noexcept
    {
        const auto centre =
            startBeat
            + visibleBeats * 0.5;

        visibleBeats =
            juce::jlimit (
                4.0,
                128.0,
                visibleBeats / 1.35
            );

        startBeat =
            juce::jmax (
                0.0,
                centre
                - visibleBeats * 0.5
            );
    }

    void zoomOut() noexcept
    {
        const auto centre =
            startBeat
            + visibleBeats * 0.5;

        visibleBeats =
            juce::jlimit (
                4.0,
                128.0,
                visibleBeats * 1.35
            );

        startBeat =
            juce::jmax (
                0.0,
                centre
                - visibleBeats * 0.5
            );
    }

    void scrollBy (
        double beats) noexcept
    {
        startBeat =
            juce::jmax (
                0.0,
                startBeat + beats
            );
    }

    void scrollLeft() noexcept
    {
        scrollBy (
            -visibleBeats * 0.25
        );
    }

    void scrollRight() noexcept
    {
        scrollBy (
            visibleBeats * 0.25
        );
    }

    double snapBeat (
        double beat) const noexcept
    {
        const auto safeBeat =
            juce::jmax (
                0.0,
                beat
            );

        if (! snapEnabled)
            return safeBeat;

        return std::round (
            safeBeat / snapBeats
        ) * snapBeats;
    }

private:
    double startBeat = 0.0;
    double visibleBeats = 32.0;

    double snapBeats = 0.25;

    bool snapEnabled = true;
};
