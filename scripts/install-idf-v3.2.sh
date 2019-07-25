#!/bin/bash

cd ~/

wget https://github.com/espressif/esp-idf/releases/download/v3.2/esp-idf-v3.2.zip
unzip -q esp-idf-v3.2.zip
python -m pip install --user -r esp-idf-v3.2/requirements.txt