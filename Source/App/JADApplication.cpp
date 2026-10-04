#include "JADApplication.h"

#include "LaunchSplash.h"

#include "../UI/MainComponent.h"

namespace
{
    juce::String quoteArgument (
        juce::String value)
    {
        value =
            value.replace (
                "\"",
                "\\\""
            );

        return "\"" + value + "\"";
    }

    juce::String updaterPowerShell()
    {
        return R"JADPS(
param(
    [Parameter(Mandatory = $true)]
    [string] $ZipUrl,

    [string] $ChecksumUrl = '',

    [Parameter(Mandatory = $true)]
    [string] $TargetExe,


    [Parameter(Mandatory = $true)]
    [string] $Version
)

$ErrorActionPreference = 'Stop'

Add-Type -AssemblyName PresentationFramework
Add-Type -AssemblyName PresentationCore
Add-Type -AssemblyName WindowsBase

[xml] $Xaml = @"
<Window
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    Title="JAD Updater"
    Width="620"
    Height="390"
    WindowStartupLocation="CenterScreen"
    ResizeMode="NoResize"
    Background="#110D28"
    Foreground="White">

    <Border
        Margin="10"
        CornerRadius="20"
        BorderThickness="2"
        BorderBrush="#21DAEB">

        <Border.Background>
            <LinearGradientBrush StartPoint="0,0" EndPoint="1,1">
                <GradientStop Color="#35103D" Offset="0"/>
                <GradientStop Color="#202052" Offset="0.52"/>
                <GradientStop Color="#06394A" Offset="1"/>
            </LinearGradientBrush>
        </Border.Background>

        <Grid Margin="28">
            <Grid.RowDefinitions>
                <RowDefinition Height="Auto"/>
                <RowDefinition Height="Auto"/>
                <RowDefinition Height="*"/>
                <RowDefinition Height="Auto"/>
                <RowDefinition Height="Auto"/>
            </Grid.RowDefinitions>

            <TextBlock
                Grid.Row="0"
                Text="JAD UPDATE MACHINE"
                FontSize="28"
                FontWeight="Bold"
                Foreground="#FF248B"/>

            <TextBlock
                Grid.Row="1"
                Margin="0,8,0,0"
                Text="YOUR LITTLE MUSIC MACHINE IS GROWING"
                FontSize="13"
                FontWeight="Bold"
                Foreground="#21DAEB"/>

            <StackPanel
                Grid.Row="2"
                VerticalAlignment="Center">

                <TextBlock
                    Name="VersionText"
                    HorizontalAlignment="Center"
                    FontSize="21"
                    FontWeight="Bold"
                    TextAlignment="Center"/>

                <TextBlock
                    Name="StatusText"
                    Margin="0,22,0,0"
                    HorizontalAlignment="Center"
                    FontSize="15"
                    Foreground="#E7E8FF"
                    Text="Preparing update..."
                    TextAlignment="Center"/>

                <ProgressBar
                    Name="Progress"
                    Height="18"
                    Margin="0,24,0,0"
                    Minimum="0"
                    Maximum="100"
                    Value="2"
                    Foreground="#FF248B"
                    Background="#19152E"/>
            </StackPanel>

            <TextBlock
                Grid.Row="3"
                Margin="0,8,0,0"
                Text="JAD will close, install the new build, and relaunch automatically."
                HorizontalAlignment="Center"
                FontSize="12"
                Foreground="#AEB9D9"/>

            <Button
                Name="CancelButton"
                Grid.Row="4"
                Width="130"
                Height="36"
                Margin="0,18,0,0"
                HorizontalAlignment="Center"
                Content="CANCEL"
                Background="#6B236D"
                Foreground="White"
                FontWeight="Bold"/>
        </Grid>
    </Border>
</Window>
"@

$Reader = New-Object System.Xml.XmlNodeReader $Xaml
$Window = [Windows.Markup.XamlReader]::Load($Reader)

$VersionText = $Window.FindName('VersionText')
$StatusText  = $Window.FindName('StatusText')
$Progress    = $Window.FindName('Progress')
$Cancel      = $Window.FindName('CancelButton')

$VersionText.Text = "INSTALLING JAD $Version"

$UpdateRoot = Join-Path `
    ([System.IO.Path]::GetTempPath()) `
    ("JAD-Update-" + [guid]::NewGuid().ToString('N'))

$ZipPath     = Join-Path $UpdateRoot 'JAD-update.zip'
$ExtractPath = Join-Path $UpdateRoot 'Extracted'

New-Item -Path $UpdateRoot -ItemType Directory -Force | Out-Null
New-Item -Path $ExtractPath -ItemType Directory -Force | Out-Null

$LogDirectory = Join-Path `
    ([Environment]::GetFolderPath('LocalApplicationData')) `
    'JAD'

$LogPath = Join-Path $LogDirectory 'Updater.log'

New-Item `
    -Path $LogDirectory `
    -ItemType Directory `
    -Force |
    Out-Null

function Write-UpdateLog {
    param(
        [string] $Message
    )

    $Stamp = Get-Date -Format 'yyyy-MM-dd HH:mm:ss.fff'

    Add-Content `
        -LiteralPath $LogPath `
        -Value "[$Stamp] $Message" `
        -Encoding UTF8
}

function Set-UpdateStage {
    param(
        [string] $Message,
        [double] $ProgressValue,
        [bool] $Indeterminate = $false
    )

    $StatusText.Text = $Message
    $Progress.IsIndeterminate = $Indeterminate

    if (-not $Indeterminate) {
        $Progress.Value = $ProgressValue
    }

    $Window.Dispatcher.Invoke(
        [action] {},
        [Windows.Threading.DispatcherPriority]::Render
    )
}

function Stop-UpdateWithError {
    param(
        [string] $Message
    )

    Write-UpdateLog "FAILED: $Message"

    $script:UpdateFailed = $true
    $script:UpdateFinished = $true

    $Timer.Stop()

    $StatusText.Text = "UPDATE STOPPED`n$Message"
    $Progress.IsIndeterminate = $false
    $Progress.Value = 0
    $Cancel.Content = 'CLOSE'
    $Cancel.IsEnabled = $true
}

Write-UpdateLog '============================================================'
Write-UpdateLog "Updater started for $Version"
Write-UpdateLog "Target executable: $TargetExe"
Write-UpdateLog "ZIP URL: $ZipUrl"
Write-UpdateLog "Checksum URL: $ChecksumUrl"

$script:UpdateStarted  = $false
$script:UpdateFinished = $false
$script:UpdateFailed   = $false
$script:DownloadTask   = $null

Add-Type -AssemblyName System.Net.Http

$HttpClient = New-Object System.Net.Http.HttpClient

$HttpClient.DefaultRequestHeaders.UserAgent.ParseAdd(
    'JAD-Updater/0.3.2'
)

$Timer = New-Object Windows.Threading.DispatcherTimer

$Timer.Interval = [TimeSpan]::FromMilliseconds(100)

$Timer.Add_Tick({
    if (
        $script:UpdateFinished -or
        -not $script:DownloadTask
    ) {
        return
    }

    if (-not $script:DownloadTask.IsCompleted) {
        return
    }

    $Timer.Stop()

    try {
        if ($script:DownloadTask.IsCanceled) {
            throw 'The update download was cancelled.'
        }

        if ($script:DownloadTask.IsFaulted) {
            $DownloadFailure =
                $script:DownloadTask.Exception.GetBaseException().Message

            throw "Download failed: $DownloadFailure"
        }

        Set-UpdateStage `
            -Message 'Saving the new JAD package...' `
            -ProgressValue 38

        $ZipBytes =
            $script:DownloadTask.GetAwaiter().GetResult()

        if (-not $ZipBytes -or $ZipBytes.Length -le 0) {
            throw 'GitHub returned an empty update package.'
        }

        [System.IO.File]::WriteAllBytes(
            $ZipPath,
            $ZipBytes
        )

        Write-UpdateLog "Downloaded $($ZipBytes.Length) bytes."

        if (-not (Test-Path -LiteralPath $ZipPath)) {
            throw 'The release ZIP did not arrive.'
        }

        if ($ChecksumUrl) {
            Set-UpdateStage `
                -Message 'Verifying release integrity...' `
                -ProgressValue 52

            $ChecksumText = $HttpClient.GetStringAsync($ChecksumUrl).GetAwaiter().GetResult()

            $ChecksumMatch =
                [regex]::Match(
                    $ChecksumText,
                    '(?i)\b[A-F0-9]{64}\b'
                )

            if (-not $ChecksumMatch.Success) {
                throw 'The published SHA-256 checksum is invalid.'
            }

            $ExpectedHash =
                $ChecksumMatch.Value.ToUpperInvariant()

            $ActualHash = (
                Get-FileHash `
                    -LiteralPath $ZipPath `
                    -Algorithm SHA256
            ).Hash.ToUpperInvariant()

            Write-UpdateLog "Expected SHA256: $ExpectedHash"
            Write-UpdateLog "Actual SHA256:   $ActualHash"

            if ($ExpectedHash -ne $ActualHash) {
                throw 'SHA-256 verification failed. The update was not installed.'
            }
        }

        Set-UpdateStage `
            -Message 'Unpacking the new build...' `
            -ProgressValue 67

        if (Test-Path -LiteralPath $ExtractPath) {
            Remove-Item `
                -LiteralPath $ExtractPath `
                -Recurse `
                -Force
        }

        Expand-Archive `
            -LiteralPath $ZipPath `
            -DestinationPath $ExtractPath `
            -Force

        $NewExe = Get-ChildItem `
            -LiteralPath $ExtractPath `
            -Filter 'JAD.exe' `
            -File `
            -Recurse |
            Select-Object -First 1

        if (-not $NewExe) {
            throw 'JAD.exe was not found inside the update package.'
        }

        Write-UpdateLog "Replacement executable: $($NewExe.FullName)"

        Set-UpdateStage `
            -Message 'Waiting for JAD to hand over the keys...' `
            -ProgressValue 78

        Start-Sleep -Milliseconds 1200

        $Installed = $false

        for ($Attempt = 1; $Attempt -le 30; $Attempt++) {
            try {
                Copy-Item `
                    -LiteralPath $NewExe.FullName `
                    -Destination $TargetExe `
                    -Force

                $Installed = $true
                Write-UpdateLog "Executable replaced on attempt $Attempt."
                break
            }
            catch {
                Write-UpdateLog "Replacement attempt $Attempt failed: $($_.Exception.Message)"
                Start-Sleep -Milliseconds 400
            }
        }

        if (-not $Installed) {
            throw 'Windows would not release the old JAD.exe.'
        }

        Set-UpdateStage `
            -Message 'Update complete - relaunching JAD!' `
            -ProgressValue 100

        $Cancel.IsEnabled = $false

        Start-Process `
            -FilePath $TargetExe `
            -WorkingDirectory (
                Split-Path `
                    -Parent `
                    $TargetExe
            )

        Write-UpdateLog 'Replacement JAD launched successfully.'

        $script:UpdateFinished = $true

        Start-Sleep -Milliseconds 1200

        try {
            Remove-Item `
                -LiteralPath $UpdateRoot `
                -Recurse `
                -Force `
                -ErrorAction SilentlyContinue
        }
        catch {
            Write-UpdateLog "Temporary cleanup warning: $($_.Exception.Message)"
        }

        $Window.Close()
    }
    catch {
        Stop-UpdateWithError `
            -Message $_.Exception.Message
    }
})

$Cancel.Add_Click({
    if ($script:UpdateFinished) {
        $Window.Close()
        return
    }

    if ($script:DownloadTask -and -not $script:DownloadTask.IsCompleted) {
        try {
            $HttpClient.CancelPendingRequests()
        }
        catch {
        }
    }

    Write-UpdateLog 'Update cancelled by the user.'

    $script:UpdateFinished = $true
    $Timer.Stop()
    $Window.Close()
})

$Window.Add_ContentRendered({
    if ($script:UpdateStarted) {
        return
    }

    $script:UpdateStarted = $true

    try {
        Set-UpdateStage `
            -Message 'Downloading the newest JAD...' `
            -ProgressValue 12 `
            -Indeterminate $true

        Write-UpdateLog 'Beginning asynchronous package download.'

        $script:DownloadTask =
            $HttpClient.GetByteArrayAsync($ZipUrl)

        $Timer.Start()
    }
    catch {
        Stop-UpdateWithError `
            -Message $_.Exception.Message
    }
})

$Window.Add_Closed({
    try {
        $Timer.Stop()
    }
    catch {
    }

    try {
        $HttpClient.Dispose()
    }
    catch {
    }
})

$null = $Window.ShowDialog()
)JADPS";
    }
}

class JADApplication::MainWindow :
    public juce::DocumentWindow
{
public:
    explicit MainWindow (
        JADContext& context)
        : DocumentWindow (
            "JAD",
            juce::Colour::fromRGB (
                18,
                18,
                24
            ),
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

        setVisible (false);
    }

    void showUpdateAvailable (
        const UpdateChecker::ReleaseInfo& release,
        std::function<void()> installUpdate)
    {
        auto message =
            "Your little music machine has a new gear ready.\n\n"
            "Installed: "
            + juce::String (
                JAD_VERSION_STRING
            )
            + "\nAvailable: "
            + release.version;

        if (
            release.name.isNotEmpty()
            && release.name != release.version
        )
        {
            message +=
                "\n\n"
                + release.name;
        }

        if (release.zipUrl.isEmpty())
        {
            message +=
                "\n\nThe automatic package is unavailable, "
                "so GitHub will open instead.";
        }
        else
        {
            message +=
                "\n\nJAD can download, verify, install, "
                "and relaunch this update automatically.";
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
            [
                installUpdate =
                    std::move (installUpdate),
                releasePage =
                    release.releasePage,
                hasPackage =
                    release.zipUrl.isNotEmpty()
            ]
            (int result)
            {
                if (result != 1)
                    return;

                if (hasPackage && installUpdate)
                    installUpdate();
                else
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
        std::make_unique<MainWindow> (
            context
        );

    launchSplash =
        std::make_unique<LaunchSplash> (
            [this]
            {
                revealMainWindow();
            },
            [this]
            {
                completeLaunch();
            }
        );
}

void JADApplication::revealMainWindow()
{
    if (mainWindow == nullptr)
        return;

    mainWindow->setVisible (true);
    mainWindow->toFront (true);
}

void JADApplication::completeLaunch()
{
    launchSplash.reset();
    beginUpdateCheck();
}

void JADApplication::beginUpdateCheck()
{
    auto safeWindow =
        juce::Component::SafePointer<MainWindow> (
            mainWindow.get()
        );

    updateChecker =
        std::make_unique<UpdateChecker> (
            [this, safeWindow]
            (std::optional<UpdateChecker::ReleaseInfo> result)
            {
                if (
                    safeWindow == nullptr
                    || ! result.has_value()
                )
                {
                    return;
                }

                const auto release = *result;

                safeWindow->showUpdateAvailable (
                    release,
                    [this, release]
                    {
                        startSelfUpdate (release);
                    }
                );
            }
        );

    updateChecker->checkNow();
}

void JADApplication::startSelfUpdate (
    const UpdateChecker::ReleaseInfo& release)
{
    if (release.zipUrl.isEmpty())
    {
        release.releasePage.launchInDefaultBrowser();
        return;
    }

    const auto updaterDirectory =
        juce::File::getSpecialLocation (
            juce::File::tempDirectory
        ).getChildFile (
            "JAD-Updater"
        );

    if (
        ! updaterDirectory.exists()
        && ! updaterDirectory.createDirectory()
    )
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "UPDATE COULD NOT START",
            "JAD could not create its temporary updater folder."
        );

        return;
    }

    const auto scriptFile =
        updaterDirectory.getNonexistentChildFile (
            "JAD-Updater",
            ".ps1",
            false
        );

    if (
        ! scriptFile.replaceWithText (
            updaterPowerShell(),
            false,
            false,
            "\r\n"
        )
    )
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "UPDATE COULD NOT START",
            "JAD could not write its updater helper."
        );

        return;
    }

    const auto currentExecutable =
        juce::File::getSpecialLocation (
            juce::File::currentExecutableFile
        );

    juce::String parameters;

    parameters +=
        "-NoProfile "
        "-ExecutionPolicy Bypass "
        "-File "
        + quoteArgument (
            scriptFile.getFullPathName()
        )
        + " -ZipUrl "
        + quoteArgument (
            release.zipUrl
        )
        + " -ChecksumUrl "
        + quoteArgument (
            release.checksumUrl
        )
        + " -TargetExe "
        + quoteArgument (
            currentExecutable.getFullPathName()
        )

        + " -Version "
        + quoteArgument (
            release.version
        );

    const auto launched =
        juce::Process::openDocument (
            "powershell.exe",
            parameters
        );

    if (! launched)
    {
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "UPDATE COULD NOT START",
            "Windows could not launch the JAD updater."
        );

        return;
    }

    juce::MessageManager::callAsync (
        []
        {
            if (
                auto* app =
                    juce::JUCEApplication::getInstance()
            )
            {
                app->systemRequestedQuit();
            }
        }
    );
}

void JADApplication::shutdown()
{
    updateChecker.reset();
    launchSplash.reset();
    mainWindow.reset();
}