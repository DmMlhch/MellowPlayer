#!/usr/bin/env bash

pacman --noconfirm --quiet -Syyu
pacman --noconfirm --quiet -S base-devel qt5-quickcontrols2 cmake git log4cplus
git submodule update --init --recursive
