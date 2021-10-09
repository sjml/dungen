#!/usr/bin/env bash

xcrun actool \
  --compile ${MESON_INSTALL_PREFIX}/Contents/Resources \
  --platform macosx \
  --minimum-deployment-target 10.13 \
  --app-icon AppIcon \
  --output-partial-info-plist ${MESON_BUILD_ROOT}/tmp-mac.plist \
  ${MESON_SOURCE_ROOT}/platform/macOS/Assets.xcassets

/usr/libexec/PlistBuddy -x -c "Merge ${MESON_BUILD_ROOT}/tmp-mac.plist" ${MESON_INSTALL_PREFIX}/Contents/Info.plist
