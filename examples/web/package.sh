#!/usr/bin/env bash
# Package the browser examples and check that each one runs.
#
# Usage: examples/web/package.sh <bin> <out>
#   bin  The folder with the Emscripten build output (example_*.html etc)
#   out  Where to write one folder per example, with its index.html, .js,
#        .wasm, .data and a screenshot.png, plus examples.json
#
# Each example plays its scripted run in Chrome. The script fails if one
# throws an error or does not finish. Needs node, npm, python3 and jq.

set -euo pipefail

bin=$1
out=$2
here=$(cd "$(dirname "$0")" && pwd)
port=${PORT:-8000}

rm -rf "$out"
mkdir -p "$out"

for page in "$bin"/example_*.html; do
  target=$(basename "$page" .html)
  dir="$out/${target#example_}"
  mkdir -p "$dir"
  cp "$bin/$target".{js,wasm,data} "$dir/"
  cp "$page" "$dir/index.html"
done

# Serve the examples and take a screenshot of each one's last frame
tools=$(mktemp -d)
npm install --silent --prefix "$tools" playwright-core@1

python3 -m http.server "$port" --directory "$out" >/dev/null 2>&1 &
server=$!

# Stop the server, but keep the script's own exit status
cleanup() {
  local status=$?
  kill "$server" 2>/dev/null || true
  wait "$server" 2>/dev/null || true
  rm -rf "$tools"
  exit "$status"
}
trap cleanup EXIT

for _ in $(seq 60); do
  curl -sf "http://localhost:$port/" >/dev/null && break
  sleep 0.5
done

NODE_PATH="$tools/node_modules" node "$here/screenshots.cjs" "http://localhost:$port" "$out"

(cd "$out" && ls -d */ | tr -d / | jq -R . | jq -s . > examples.json)
