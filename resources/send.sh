#!/usr/bin/env bash

if [[ -z "$1" || -z "$2" ]]; then
    echo "Sends a file to the machine '/dev/shm' directory" >&2
    echo "Usage: $0 LEVEL LOCAL_FILE" >&2
    exit 1
fi

if [ "$1" -ge 10 ]; then
    user="bonus$((10#$1 - 10))"
else
    user="level$1"
fi

if [ "$1" -eq 10 ]; then
    previous_level="9"
    previous_user="level9"
elif [ "$1" -ge 10 ]; then
    previous_level="$((10#$1 - 11))"
    previous_user="bonus$previous_level"
else
    previous_level="$((10#$1 - 1))"
    previous_user="level$previous_level"
fi

if [[ "$1" -eq "0" ]]; then
    export SSHPASS="$user"
else
    flag_file="../$previous_user/flag"
    if [[ ! -f "$flag_file" ]]; then
        echo "Error: flag file $flag_file not found" >&2
        exit 1
    fi
    export SSHPASS=$(cat "$flag_file")
fi

sshpass -e scp -P 4242 "$2" "$user"'@localhost:/dev/shm'

exit 0
