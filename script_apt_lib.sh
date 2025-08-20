!#/bin/bash

set -e

echo "Maj paquets..."

sudo apt-get update

echo "Install libyaml..."

sudo apt-get install -y libyaml-dev

echo "Install End."