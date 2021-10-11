
Set-Location -Path $PSScriptRoot
Set-Location -Path ..\..

meson --buildtype=release --prefix=$pwd/build/DunGen_dist --bindir='' build
meson compile -C build
Set-Location -Path build
meson install

Set-Location -Path ..
