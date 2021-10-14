#!/usr/bin/env bash

if [[ $1 == 'mac' ]]; then
  platform=macosx
  out_dir=${MESON_INSTALL_PREFIX}/Contents/Resources
  tmp_plist=${MESON_BUILD_ROOT}/tmp-mac.plist
  out_plist=${MESON_INSTALL_PREFIX}/Contents/Info.plist
  source_assets=${MESON_SOURCE_ROOT}/platform/macOS/Assets.xcassets
else
  platform=iphoneos
  out_dir=${MESON_INSTALL_PREFIX}
  tmp_plist=${MESON_BUILD_ROOT}/tmp-ios.plist
  out_plist=${MESON_INSTALL_PREFIX}/Info.plist
  source_assets=${MESON_SOURCE_ROOT}/platform/iOS/Assets.xcassets
fi


xcrun actool \
  --compile $out_dir \
  --platform $platform \
  --minimum-deployment-target 11.0 \
  --app-icon AppIcon \
  --output-partial-info-plist $tmp_plist \
  $source_assets

/usr/libexec/PlistBuddy -x -c "Merge $tmp_plist" $out_plist
