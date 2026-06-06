#!/bin/bash
# FastCarPlay smart launcher
# - Auto-restarts if the app crashes (ran < 30 seconds)
# - Stays closed if you tap the X button / exit manually (ran >= 30 seconds)

APP=/home/ychamare/FastCarPlay/out/app
CONF=/home/ychamare/FastCarPlay/conf/settings.txt
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
        sleep 3; continue
    fi

    log "Closed after ${runtime}s, staying closed"
    break
done
