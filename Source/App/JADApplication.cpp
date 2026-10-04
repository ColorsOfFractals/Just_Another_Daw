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

$Worker = New-Object System.ComponentModel.BackgroundWorker
$Worker.WorkerSupportsCancellation = $true

$Cancel.Add_Click({
    $Cancel.IsEnabled = $false
    $StatusText.Text = 'Cancelling safely...'
    $Worker.CancelAsync()
})

$Worker.Add_DoWork({
    param($Sender, $EventArgs)

    try {
        $Window.Dispatcher.Invoke(
            [action] {
                $StatusText.Text = 'Downloading the newest JAD...'
                $Progress.Value = 5
            }
        )

        $Client = New-Object System.Net.WebClient
        $Client.Headers.Add(
            'User-Agent',
            'JAD-Updater'
        )

        $Client.Add_DownloadProgressChanged({
            param($DownloadSender, $DownloadArgs)

            $Percent =
                5 + [math]::Floor(
                    $DownloadArgs.ProgressPercentage * 0.65
                )

            $Window.Dispatcher.BeginInvoke(
                [action] {
                    $Progress.Value = $Percent
                    $StatusText.Text =
                        "Downloading JAD... $($DownloadArgs.ProgressPercentage)%"
                }
            ) | Out-Null
        })

        $Client.DownloadFileAsync(
            [uri] $ZipUrl,
            $ZipPath
        )

        while ($Client.IsBusy) {
            if ($Worker.CancellationPending) {
                $Client.CancelAsync()
                $EventArgs.Cancel = $true
                return
            }

            Start-Sleep -Milliseconds 100
        }

        if (-not (Test-Path -LiteralPath $ZipPath)) {
            throw 'The release ZIP did not arrive.'
        }

        if ($ChecksumUrl) {
            $Window.Dispatcher.Invoke(
                [action] {
                    $StatusText.Text = 'Verifying release integrity...'
                    $Progress.Value = 74
                }
            )

            $HashText =
                $Client.DownloadString(
                    [uri] $ChecksumUrl
                )

            $ExpectedMatch =
                [regex]::Match(
                    $HashText,
                    '(?i)\b[A-F0-9]{64}\b'
                )

            if (-not $ExpectedMatch.Success) {
                throw 'The published SHA-256 checksum is invalid.'
            }

            $Expected =
                $ExpectedMatch.Value.ToUpperInvariant()

            $Actual =
                (Get-FileHash `
                    -LiteralPath $ZipPath `
                    -Algorithm SHA256
                ).Hash.ToUpperInvariant()

            if ($Expected -ne $Actual) {
                throw 'SHA-256 verification failed. The update was not installed.'
            }
        }

        $Window.Dispatcher.Invoke(
            [action] {
                $StatusText.Text = 'Unpacking the new music machine...'
                $Progress.Value = 80
            }
        )

        Expand-Archive `
            -LiteralPath $ZipPath `
            -DestinationPath $ExtractPath `
            -Force

        $NewExe =
            Get-ChildItem `
                -LiteralPath $ExtractPath `
                -Filter 'JAD.exe' `
                -File `
                -Recurse |
            Select-Object -First 1

        if (-not $NewExe) {
            throw 'JAD.exe was not found inside the update package.'
        }

        $Window.Dispatcher.Invoke(
            [action] {
                $StatusText.Text = 'Waiting for JAD to hand over the keys...'
                $Progress.Value = 88
            }
        )

        # JAD exits immediately after launching this detached helper.
        # Give Windows a moment to release the executable handle.
        Start-Sleep -Milliseconds 1200

        $Installed = $false

        for ($Attempt = 1; $Attempt -le 20; $Attempt++) {
            try {
                Copy-Item `
                    -LiteralPath $NewExe.FullName `
                    -Destination $TargetExe `
                    -Force

                $Installed = $true
                break
            }
            catch {
                Start-Sleep -Milliseconds 400
            }
        }

        if (-not $Installed) {
            throw 'Windows would not release the old JAD.exe.'
        }

        $Window.Dispatcher.Invoke(
            [action] {
                $StatusText.Text = 'Update complete â€” relaunching JAD!'
                $Progress.Value = 100
                $Cancel.IsEnabled = $false
            }
        )

        Start-Process `
            -FilePath $TargetExe `
            -WorkingDirectory (
                Split-Path `
                    -Parent `
                    $TargetExe
            )

        Start-Sleep -Milliseconds 1400
    }
    catch {
        $EventArgs.Result = $_.Exception.Message
    }
})

$Worker.Add_RunWorkerCompleted({
    param($Sender, $EventArgs)

    if ($EventArgs.Cancelled) {
        $StatusText.Text = 'Update cancelled. Your current JAD is untouched.'
        $Progress.Value = 0
        $Cancel.Content = 'CLOSE'
        $Cancel.IsEnabled = $true

        $Cancel.Add_Click({
            $Window.Close()
        })

        return
    }

    if ($EventArgs.Result) {
        $StatusText.Text =
            "UPDATE STOPPED`n$($EventArgs.Result)"

        $Progress.Value = 0
        $Cancel.Content = 'CLOSE'
        $Cancel.IsEnabled = $true

        $Cancel.Add_Click({
            $Window.Close()
        })

        return
    }

    $Window.Close()

    try {
        Remove-Item `
            -LiteralPath $UpdateRoot `
            -Recurse `
            -Force `
            -ErrorAction SilentlyContinue
    }
    catch {
    }
})

$Window.Add_ContentRendered({
    $Worker.RunWorkerAsync()
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