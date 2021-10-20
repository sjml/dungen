
Set-Location -Path $PSScriptRoot
Set-Location -Path ..\..

if (Test-Path build)
{
    Remove-Item build -Recurse
}

meson build
meson compile -C build

Set-Location -Path ..
