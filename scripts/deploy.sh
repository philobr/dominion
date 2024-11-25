#!/bin/bash

set -e

# Fail if the user is not dominion
if [ "$USER" != "dominion" ]; then
    echo "This script must be run as dominion" 1>&2
    echo "This prevents accidental misuse of the script" 1>&2
    exit 1
fi

cd /opt/dominion/dominion

# Pull the latest changes from the repository
git pull

cd docker

docker-compose up -d --build

echo "Deployment complete" 1>&2
