#!/bin/sh
echo -ne '\033c\033]0;Gate of Scalad\a'
base_path="$(dirname "$(realpath "$0")")"
"$base_path/Gate of Scalad.x86_64" "$@"
