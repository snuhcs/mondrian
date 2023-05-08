#!/usr/bin/env bash

watch -n 1 'cat $(cat .result)/log/device_$(cat .device).log > .device_log; echo "\nDEVICE:" >> .device_log; cat .device >> .device_log; cat .device_log | tail -n$(($(tput lines)-2))'
