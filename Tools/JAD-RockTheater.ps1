function Show-JADSpaceRock {
    param(
        [Parameter(Mandatory)]
        [string]$Expedition,

        [Parameter(Mandatory)]
        [string]$Output,

        [string]$Evidence = ''
    )

    $RockClass = 'MYSTERY METEOR'
    $Creature  = '🥔'
    $Remark    = '...that seems important.'

    if ($Output -match 'CMake Error') {
        $RockClass = 'BLUEPRINT BOULDER'
        $Creature  = '🪨'
        $Remark    = 'aw beans. blueprint rock.'
    }

    if ($Output -match 'error C\d{4}') {
        $RockClass = 'SYNTAX METEOR'
        $Creature  = '☄️'
        $Remark    = 'the runes are displeased.'
    }

    if ($Output -match 'LNK\d{4}') {
        $RockClass = 'LINKER MOON'
        $Creature  = '🌑'
        $Remark    = 'two things refuse to hold hands.'
    }

    if ($Output -match 'fatal error') {
        $RockClass = 'FATAL SPACE POTATO'
        $Creature  = '🥔'
        $Remark    = 'oh. this potato has authority.'
    }

    if ($Output -match 'could not find|cannot find|not found|missing') {
        $RockClass = 'DEPENDENCY CHEESE ROCK'
        $Creature  = '🧀'
        $Remark    = 'someone ate a dependency.'
    }

    $Interesting = @(
        $Output -split "`r?`n" |
        Where-Object {
            $_ -match 'error|fatal|CMake|LNK|FAILED|cannot|missing|undefined|not found'
        } |
        Select-Object -Last 10
    )

    Write-Host ""
    Write-Host "             .          ✦             ." -ForegroundColor DarkGray
    Write-Host "       ✧                           ." -ForegroundColor DarkMagenta
    Write-Host ""
    Write-Host "          WARNING: APPROACHING SPACE ROCK" -ForegroundColor Red
    Write-Host ""
    Write-Host "                    _______" -ForegroundColor DarkYellow
    Write-Host "                .-''       ``-." -ForegroundColor DarkYellow
    Write-Host "              .'      •  •      `." -ForegroundColor Yellow
    Write-Host '             /         ___         \' -ForegroundColor Yellow
    Write-Host '            |         /___\         |' -ForegroundColor Yellow
    Write-Host '             \                     /' -ForegroundColor Yellow
    Write-Host "              ``-._             _.-'" -ForegroundColor DarkYellow
    Write-Host "                   ``-----------'" -ForegroundColor DarkYellow
    Write-Host ""
    Write-Host "                    $Creature  BLORP" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "                `"$Remark`"" -ForegroundColor Gray
    Write-Host ""
    Write-Host "        Rock Class : $RockClass" -ForegroundColor Magenta
    Write-Host "        Expedition : $Expedition" -ForegroundColor Cyan
    Write-Host "        Forge      : BLOCKED" -ForegroundColor Red
    Write-Host "        Mutation   : PRESERVED" -ForegroundColor Green

    if ($Evidence) {
        Write-Host "        Evidence   : $Evidence" -ForegroundColor DarkGray
    }

    Write-Host ""
    Write-Host "    ─────────── ROCK INSCRIPTION ───────────" -ForegroundColor DarkCyan
    Write-Host ""

    if ($Interesting.Count -eq 0) {
        Write-Host "        The rock refuses to explain itself." -ForegroundColor Yellow
        Write-Host "        Raw evidence remains preserved." -ForegroundColor DarkGray
    }

    $Interesting | ForEach-Object {
        Write-Host "      $_" -ForegroundColor Gray
    }

    Write-Host ""
    Write-Host "    ────────────────────────────────────────" -ForegroundColor DarkCyan
    Write-Host ""
    Write-Host "                       ⛺" -ForegroundColor Cyan
    Write-Host "                      🥟" -ForegroundColor Yellow
    Write-Host ""
    Write-Host '              Fu remains at camp.' -ForegroundColor DarkYellow
    Write-Host '          Nothing gets fossilized yet.' -ForegroundColor DarkGray
    Write-Host ""
}
