#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ..

xcodebuild -sdk macosx -scheme "DunGen (Release)" -project DunGen.xcodeproj -archivePath ./build/archives/DunGen_macOS.xcarchive archive
xcodebuild -sdk iphoneos -scheme "DunGen iOS (Release)" -project DunGen.xcodeproj -archivePath ./build/archives/DunGen_iOS.xcarchive archive
