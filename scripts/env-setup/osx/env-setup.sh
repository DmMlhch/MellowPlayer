#!/usr/bin/env bash

wget --progress=bar:force:noscroll --retry-connrefused --waitretry=1 -O qt.zip "https://www.dropbox.com/s/il5hin8teaf5liu/qt-5.9.0-osx.zip?dl=1"
unzip -qq qt.zip
mv clang_64/ qt/
