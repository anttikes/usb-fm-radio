<#
.SYNOPSIS
    Initialize Git submodules in the repository.

.DESCRIPTION
    Runs `git submodule update --init` for the selected submodules. Supports optional reinitialization if required,
    and choosing either individual submodule or all of them.

.PARAMETER Submodules
    List of submodule names to operate on. Allowed values: All, tinyusb, STM32F0xx_HAL_Driver, STM32F0xx.
    "All" is the default, and selects the other three. If used, it must appear on its own.
    
.PARAMETER Reinit
    Deinitialize specified submodules first, then initialize them again. Useful to recover from broken submodule state.
    
.EXAMPLE
    Initialize-Submodules -Submodules 'tinyusb'
    Initializes the 'tinyusb' submodule

.EXAMPLE
    Initialize-Submodules -Submodules 'All'
    Initializes all submodules

.EXAMPLE
    Initialize-Submodules -Submodules 'All' -Reinit
    Performs submodule reinitialization, and then initializes all modules
#>

[CmdletBinding()]
param(
    [ValidateSet('All', 'tinyusb', 'STM32F0xx_HAL_Driver', 'STM32F0xx')]
    [string[]] $Submodules = 'All',

    [switch] $Reinit,

    [switch] $Help
)

function Invoke-Git {
    param(
        [string[]] $Arguments,

        [string] $WorkingDirectory = (Get-Location).ProviderPath
    )

    $psi = New-Object System.Diagnostics.ProcessStartInfo
    $psi.FileName = 'git'
    $psi.Arguments = $Arguments -join ' '
    $psi.WorkingDirectory = $WorkingDirectory
    $psi.RedirectStandardOutput = $true
    $psi.RedirectStandardError = $true
    $psi.UseShellExecute = $false
    $p = New-Object System.Diagnostics.Process
    $p.StartInfo = $psi
    $p.Start() | Out-Null
    $stdout = $p.StandardOutput.ReadToEnd()
    $stderr = $p.StandardError.ReadToEnd()
    $p.WaitForExit()
            
    return @{
        ExitCode = $p.ExitCode
        StdOut   = $stdout
        StdErr   = $stderr
    }
}
        
Set-StrictMode -Version Latest

if ($Help) {
    Get-Help $PSCommandPath -Examples
    exit
}

# Verify git is installed
try {
    $null = & git --version 2>$null
}
catch {
    throw "git not found on PATH. Install git or add it to PATH."
}

$check = Invoke-Git -Arguments @('rev-parse', '--git-dir')
if ($check.ExitCode -ne 0) {
    throw "Path '$RepoPath' is not a git repository. git rev-parse failed: $($check.StdErr.Trim())"
}

# Expand "All" to actual choices
$Repositories = New-Object System.Collections.Generic.List[string]
if ($Submodules -contains 'All') {
    if ($Submodules.Length -gt 1) {
        throw "All must appear alone in the 'Submodules' parameter"
    }

    $Repositories.Add('tinyusb')
    $Repositories.Add('STM32F0xx_HAL_Driver')
    $Repositories.Add('STM32F0xx')
}
else {
    foreach ($s in $Submodules) { $Repositories.Add($s) }
}
        
if ($Reinit) {
    Write-Host "Reinitializing submodules..."    
    $deinit = Invoke-Git -Arguments @('submodule', 'deinit', '--all', '-f')
            
    if ($deinit.ExitCode -ne 0) {
        throw "Reinitialization of git submodules failed: $($deinit.ExitCode): $($deinit.StdErr.Trim())"
    }
}

# Initialize 'tinyusb'
if ($Repositories.Contains('tinyusb')) {
    Write-Host "Initializing 'tinyusb'..."
    $initTinyUsb = Invoke-Git -Arguments @('submodule', 'update', '--init', 'ThirdParty/tinyusb')
            
    if ($initTinyUsb.ExitCode -ne 0) {                
        throw "Initialization of 'tinyusb' failed: $($initTinyUsb.ExitCode): $($initTinyUsb.StdErr.Trim())"
    }
}

# If HAL or CMSIS Device are selected, the parent must be initialized first
if (($Repositories.Contains('STM32F0xx_HAL_Driver')) -or ($Repositories.Contains('STM32F0xx'))) {
    Write-Host "Ensuring 'STM32CubeF0' is initialized..."
    $initCubeParent = Invoke-Git -Arguments @('submodule', 'update', '--init', 'ThirdParty/STM32CubeF0')
            
    if ($initCubeParent.ExitCode -ne 0) {                
        throw "Initialization of 'STM32CubeF0' failed: $($initCubeParent.ExitCode): $($initCubeParent.StdErr.Trim())"
    }
}

# Finally initialize the sub-submodules
if ($Repositories.Contains('STM32F0xx_HAL_Driver')) {
    Write-Host "Initializing 'STM32F0xx_HAL_Driver'..."
    $initHAL = Invoke-Git -Arguments @('submodule', 'update', '--init', 'Drivers/STM32F0xx_HAL_Driver') -WorkingDirectory (Join-Path (Get-Location).ProviderPath 'ThirdParty/STM32CubeF0')
            
    if ($initHAL.ExitCode -ne 0) {                
        throw "Initialization of 'STM32CubeF0' failed: $($initHAL.ExitCode): $($initHAL.StdErr.Trim())"
    }
}

if ($Repositories.Contains('STM32F0xx')) {
    Write-Host "Initializing 'STM32F0xx'..."
    $initCMSIS = Invoke-Git -Arguments @('submodule', 'update', '--init', 'Drivers/CMSIS/Device/ST/STM32F0xx') -WorkingDirectory (Join-Path (Get-Location).ProviderPath 'ThirdParty/STM32CubeF0')
            
    if ($initCMSIS.ExitCode -ne 0) {                
        throw "Initialization of 'STM32CubeF0' failed: $($initCMSIS.ExitCode): $($initCMSIS.StdErr.Trim())"
    }
}

# Collect final submodule status for reporting
$status = Invoke-Git -Arguments @('submodule', 'status', '--recursive')
Write-Host "`n--- Final submodule status ---`n"
Write-Host $status.StdOut
