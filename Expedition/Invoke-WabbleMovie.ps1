param(
    [ValidateSet('ALLOW','DENY','UNKNOWN')]
    [string]$Decision = 'ALLOW',

    [string]$Candidate = 'Microsoft.VisualStudio.BuildTools',

    [string]$Name = 'Visual Studio BuildTools 2026',

    [string]$Version = '18.9.1',

    [string]$Publisher = 'Microsoft Corporation',

    [string]$Evidence = 'JAD\Evidence\Wabble.json'
)

function Frame {
    param(
        [int]$Milliseconds = 250
    )

    Start-Sleep -Milliseconds $Milliseconds
}

Clear-Host

Write-Host ""
Write-Host "============================================================" -ForegroundColor DarkMagenta
Write-Host "                    WABBLE — THE BOUNCER" -ForegroundColor Magenta
Write-Host "============================================================" -ForegroundColor DarkMagenta

Frame 450

Write-Host ""
Write-Host "                         ║" -ForegroundColor DarkCyan
Write-Host "                    ╔════╩════╗" -ForegroundColor DarkCyan
Write-Host "                    ║ SECURITY║" -ForegroundColor Cyan
Write-Host "                    ║   GATE  ║" -ForegroundColor Cyan
Write-Host "                    ╚════╦════╝" -ForegroundColor DarkCyan
Write-Host "                         ║" -ForegroundColor DarkCyan

Frame 500

Write-Host ""
Write-Host "                      (       )" -ForegroundColor Magenta
Write-Host "                    (   •ᴗ•   )" -ForegroundColor Magenta
Write-Host "                     (       )" -ForegroundColor Magenta
Write-Host "                      ᒐ     ᒐ" -ForegroundColor Magenta

Frame 350

Write-Host ""
Write-Host "                 wabble..." -ForegroundColor DarkMagenta
Frame 300

Write-Host "                      wabble..." -ForegroundColor DarkMagenta
Frame 300

Write-Host "                           wabble..." -ForegroundColor DarkMagenta

Frame 450

Write-Host ""
Write-Host '             "hehehe lemme see that :)"' -ForegroundColor Magenta

Frame 500

Write-Host ""
Write-Host "                    [ REQUESTING ENTRY ]" -ForegroundColor DarkGray
Write-Host ""
Write-Host "              $Candidate" -ForegroundColor Yellow

Frame 450

Write-Host ""
Write-Host "                         ." -ForegroundColor DarkGray
Frame 300
Write-Host "                         ." -ForegroundColor DarkGray
Frame 300
Write-Host "                         ." -ForegroundColor DarkGray

Frame 550

if ($Decision -eq 'ALLOW') {

    Write-Host ""
    Write-Host "                      (       )" -ForegroundColor Magenta
    Write-Host "                    (   ^ᴗ^   )" -ForegroundColor Magenta
    Write-Host "                     (       )" -ForegroundColor Magenta
    Write-Host "                      ᒐ     ᒐ" -ForegroundColor Magenta

    Frame 300

    Write-Host ""
    Write-Host "                       *BOING*" -ForegroundColor Green

    Frame 200

    Write-Host "                    *BOING BOING*" -ForegroundColor Green

    Frame 400

    Write-Host ""
    Write-Host "                       ALLOW" -ForegroundColor Green
    Write-Host ""
    Write-Host '              "oooo okay you can come in :D"' -ForegroundColor Magenta
}

if ($Decision -eq 'DENY') {

    Write-Host ""
    Write-Host "                      (       )" -ForegroundColor Magenta
    Write-Host "                    (   •ᴗ•   )" -ForegroundColor Magenta
    Write-Host "                     (       )" -ForegroundColor Magenta
    Write-Host "                      ᒐ     ᒐ" -ForegroundColor Magenta

    Frame 350

    Write-Host ""
    Write-Host "                       DENY" -ForegroundColor Red
    Write-Host ""
    Write-Host '                  "hehehe no :)"' -ForegroundColor Magenta
}

if ($Decision -eq 'UNKNOWN') {

    Write-Host ""
    Write-Host "                      (       )" -ForegroundColor Magenta
    Write-Host "                    (   •_•   )" -ForegroundColor Magenta
    Write-Host "                     (       )" -ForegroundColor Magenta
    Write-Host "                      ᒐ     ᒐ" -ForegroundColor Magenta

    Frame 700

    Write-Host ""
    Write-Host "                      UNKNOWN" -ForegroundColor Yellow

    Frame 500

    Write-Host ""
    Write-Host "               Wabble stops wobbling." -ForegroundColor Yellow
}

Frame 600

Write-Host ""
Write-Host "                packing tiny vessel..." -ForegroundColor Cyan

Frame 500

Write-Host ""
Write-Host "                         ╭───────╮" -ForegroundColor Cyan
Write-Host "                      ╱             ╲" -ForegroundColor Cyan
Write-Host ("                     │    {0,-7}   │" -f $Decision) -ForegroundColor Cyan
Write-Host "                      ╲             ╱" -ForegroundColor Cyan
Write-Host "                         ╰───────╯" -ForegroundColor Cyan

Frame 450

Write-Host ""
Write-Host '                    "hehehe catch :)"' -ForegroundColor Magenta

Frame 350

Write-Host ""
Write-Host "                         ◉" -ForegroundColor Cyan
Frame 180

Write-Host "                              ◉" -ForegroundColor Cyan
Frame 180

Write-Host "                                   ◉" -ForegroundColor Cyan
Frame 180

Write-Host "                                        ◉" -ForegroundColor Cyan
Frame 180

Write-Host "                                             ◉ ~ ~ ~ >" -ForegroundColor Cyan

$Vessel = @"
╔════════════════════════════════════════════════════════════╗
║                  WABBLE ADMISSION VESSEL                  ║
╠════════════════════════════════════════════════════════════╣
║ Decision   : $Decision
║ Candidate  : $Candidate
║ Name       : $Name
║ Version    : $Version
║ Publisher  : $Publisher
║ Scope      : JAD native C++ foundation
║ Evidence   : $Evidence
╚════════════════════════════════════════════════════════════╝
"@

$Vessel | Set-Clipboard

Frame 400

Write-Host ""
Write-Host "============================================================" -ForegroundColor DarkMagenta
Write-Host "                     WABBLE COMPLETE" -ForegroundColor Magenta
Write-Host "============================================================" -ForegroundColor DarkMagenta

Write-Host ""
Write-Host "📋 Tiny Vessel handed to clipboard." -ForegroundColor Green
Write-Host ""
