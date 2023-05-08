#!/usr/bin/env bash

watch -n 1 'cat $(cat .result)/log/root.log | tail -n$(($(tput lines)-2))'
