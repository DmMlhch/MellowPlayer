#!/usr/bin/env bash

echo "Copying plugins to .app"

app_dir=build/release/install-root/Applications/MellowPlayer.app

mkdir -p ${app_dir}/Contents/PlugIns/services
mkdir -p ${app_dir}/Contents/Resources

cp -R src/plugins/web ${app_dir}/Contents/PlugIns/services
cp scripts/packaging/osx/mellowplayer.icns ${app_dir}/Contents/Resources
cp scripts/packaging/osx/Info.plist ${app_dir}/Contents

mkdir -p  ${app_dir}/Contents/Resources/qml/QtQuick/
cp -R /usr/local/opt/qt/qml/QtQuick/Controls ${app_dir}/Contents/Resources/qml/QtQuick/

macdeployqt ${app_dir} -dmg -qmldir=$PWD/src -verbose=1

mkdir -p dist
cp build/release/install-root/Applications/MellowPlayer.dmg dist/
