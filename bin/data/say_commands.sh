#!/bin/sh

COMMANDS=("center" "up" "down" "left" "right" "center up" "center down" "center left" "center right" "up left" "up right" "down left" "down right" "top left halfway" "top right halfway" "down left halfway" "down right halfway")
for command in "${COMMANDS[@]}"; do
  filename=$(echo "$command" | sed -e 's/ /_/g')
  say -v Samantha --data-format=LEF32@22050 -o $filename".wav" "$command"
done
