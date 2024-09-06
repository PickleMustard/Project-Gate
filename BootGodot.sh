#!/bin/bash

echo "$1"
if [[ $1 == "compile" ]]; then
  scons platform=linux
fi
godot --editor --path ./project-gate
