#!/bin/bash

root_dir=$(pwd)
echo $root_dir

# Common denpendencies
echo "> fetching common submodules.."

cd $root_dir
mkdir dependencies
cd dependencies
echo "> $(pwd)"

git clone https://github.com/Forairaaaaa/smooth_ui_toolkit.git
git clone --depth 1 https://github.com/lovyan03/LovyanGFX.git
git clone https://github.com/Forairaaaaa/mooncake.git
# git clone https://github.com/nayuki/QR-Code-generator.git

# # Desktop
# echo "> fetching submodules for desktop build.."

# cd $root_dir
# echo "> $(pwd)"

# echo "> :)"

# VAMeter
echo "> fetching submodules for vameter build.."

cd $root_dir
mkdir platforms/vameter/components
cd platforms/vameter/components
echo "> $(pwd)"

git clone https://github.com/bblanchon/ArduinoJson.git
git clone https://github.com/Forairaaaaa/arduino_lite.git arduino-esp32
git clone https://github.com/Forairaaaaa/ESP32Encoder.git
# git clone https://github.com/me-no-dev/AsyncTCP.git
# git clone https://github.com/me-no-dev/ESPAsyncWebServer.git
git clone git@github.com:Forairaaaaa/PsychicHttp.git

echo "> done"
