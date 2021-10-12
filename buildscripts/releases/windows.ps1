
Set-Location -Path $PSScriptRoot
Set-Location -Path ..\..

if (Test-Path build)
{
    Remove-Item build -Recurse
}

meson --buildtype=release --prefix=$pwd/build/DunGen_dist --bindir='' build
meson compile -C build
Set-Location -Path build
meson install

Set-Location -Path ..

Copy-Item docs/acknowledgements.md build/DunGen_dist
