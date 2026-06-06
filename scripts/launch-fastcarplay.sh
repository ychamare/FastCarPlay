#!/bin/bash
# FastCarPlay smart launcher for Raspberry Pi
#
# - Auto-restarts if the app crashes (ran < 30 seconds)
# - Stays closed if you manually exit (ran >= 30 seconds)
#
# Usage: place this in labwc autostart:
#   sleep 3 && /path/to/scripts/launch-fastcarplay.sh &
#
# Or run manually:
#   DISPLAY=:0 XDG_RUNTIME_DIR=/run/user/1000 ./scripts/launch-fastcarplay.sh

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
APP="${SCRIPT_DIR}/../out/app"
CONF="${SCRIPT_DIR}/../conf/settings.txt"
LOG=/tmp/fastcarplay-restarts.log

log() { echo "$(date '+%H:%M:%S'): $*" >> "$LOG"; }
log 'FastCarPlay launcher started'

while true; do
    start=$(date +%s)
    "$APP" "$CONF"
    code=$?
    runtime=$(( $(date +%s) - start ))

    if [ $runtime -lt 30 ]; then
        log "Crash after ${runtime}s (code $code), retrying in 3s..."
        sleep 3
        continue
    fi

    log "Clean exit after ${runtime}s, staying closed"
    break
done
