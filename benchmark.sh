#!/usr/bin/env bash

progs="bc yes wc head"

for prog in $progs; do
  if [ ! command -v ${prog} &> /dev/null ]; then
    echo "This script requires '${prog}' to be installed"
  fi
done

fmt_bytes() {
  local i=${1:-0} d="" s=0 S=("Bytes" "KiB" "MiB" "GiB" "TiB" "PiB" "EiB" "YiB" "ZiB")
  while ((i > 1024 && s < ${#S[@]}-1)); do
    printf -v d ".%02d" $((i % 1024 * 100 / 1024))
    i=$((i / 1024))
    s=$((s + 1))
  done
  echo "$i$d ${S[$s]}"
}

count() {
  yes "$1" | head -n "$2" | wc -c
}

run() {
  bytes="$(count "$1" "$2")"
  echo "bytes: $(fmt_bytes "${bytes}")"
  echo "First line of input: $1"$'\033[0m'
  echo "First line of output: $(echo "$1" | ./unansi)"
  seconds="$(yes "$1" | head -n "$2" | { command time -p ./unansi 2>&1 > /dev/null; } | grep real | awk '{print $2}')"
  echo "Took ${seconds}s"
  throughput="$(bc <<< "$(bc -l <<< "${bytes}/${seconds}") / 1")"
  echo "$(fmt_bytes "${throughput}")/s"
}

echo "No escapes"
run "test123123123123123123123123123123123123123213" 50000000

echo
echo "Lots of escapes"
run $'\u001B[0;31mtest\u001B[0;32m23123\u001B[0;33m123\u001B[0;34m123123' 50000000
