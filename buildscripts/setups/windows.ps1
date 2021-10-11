
Set-Location -Path $PSScriptRoot
Set-Location -Path ..\..

meson build
meson compile -C build

Set-Location -Path ..
