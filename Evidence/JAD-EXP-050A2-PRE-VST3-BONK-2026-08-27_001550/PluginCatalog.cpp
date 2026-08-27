#include "PluginCatalog.h"

PluginCatalog::PluginCatalog()
{
    formatManager.addDefaultFormats();
}

void PluginCatalog::initialise()
{
    load();

    lastScanSummary =
        "Catalog loaded: "
        + juce::String (
            knownPlugins.getNumTypes()
        )
        + " plugin(s)";
}

juce::File PluginCatalog::getCatalogFile() const
{
    auto directory =
        juce::File::getSpecialLocation (
            juce::File::userApplicationDataDirectory
        )
        .getChildFile ("JAD");

    return directory.getChildFile (
        "KnownPlugins.xml"
    );
}

void PluginCatalog::load()
{
    const auto file =
        getCatalogFile();

    if (! file.existsAsFile())
        return;

    auto xml =
        juce::XmlDocument::parse (file);

    if (xml == nullptr)
        return;

    knownPlugins.recreateFromXml (
        *xml
    );
}

void PluginCatalog::save()
{
    const auto file =
        getCatalogFile();

    file.getParentDirectory()
        .createDirectory();

    auto xml =
        knownPlugins.createXml();

    if (xml != nullptr)
        xml->writeTo (file);
}

void PluginCatalog::scanVST3()
{
    juce::VST3PluginFormat vst3;

    const auto searchPaths =
        vst3.getDefaultLocationsToSearch();

    juce::PluginDirectoryScanner scanner (
        knownPlugins,
        vst3,
        searchPaths,
        true,
        juce::File(),
        false
    );

    juce::String pluginBeingScanned;

    int scanned = 0;

    while (
        scanner.scanNextFile (
            true,
            pluginBeingScanned
        )
    )
    {
        ++scanned;
    }

    save();

    lastScanSummary =
        "VST3 scan complete | "
        + juce::String (scanned)
        + " candidate(s) visited | "
        + juce::String (
            knownPlugins.getNumTypes()
        )
        + " plugin(s) catalogued";
}

int PluginCatalog::getNumPlugins() const noexcept
{
    return knownPlugins.getNumTypes();
}

juce::Array<juce::PluginDescription>
PluginCatalog::getPlugins() const
{
    juce::Array<juce::PluginDescription> result;

    for (
        const auto& type :
        knownPlugins.getTypes()
    )
    {
        result.add (type);
    }

    return result;
}

juce::Array<juce::PluginDescription>
PluginCatalog::search (
    const juce::String& query
) const
{
    const auto needle =
        query.trim().toLowerCase();

    if (needle.isEmpty())
        return getPlugins();

    juce::Array<juce::PluginDescription> result;

    for (
        const auto& type :
        knownPlugins.getTypes()
    )
    {
        const auto haystack =
            (
                type.name
                + " "
                + type.manufacturerName
                + " "
                + type.category
                + " "
                + type.pluginFormatName
            )
            .toLowerCase();

        if (haystack.contains (needle))
            result.add (type);
    }

    return result;
}

juce::String
PluginCatalog::getLastScanSummary() const
{
    return lastScanSummary;
}
