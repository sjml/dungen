
Set-Location -Path $PSScriptRoot
Set-Location -Path ..\..

meson --buildtype=release --prefix=$pwd/build/DunGen.dist --bindir='' build
meson compile -C build
Set-Location -Path build
meson install

Set-Location -Path ..
