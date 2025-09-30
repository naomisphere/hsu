#!/bin/bash
export HARO_BASE="/Library/haro"
export HARO_BIN="${HARO_BASE}/bin"
export CTMPDIR="$(mktemp -d)"
D_HSU_PATH="/Library/haro/bin/hsu"

if [ "$USER" = "root" ]; then
    if [ "$HOME" = "/var/root" ]; then
        echo "You may not run this script logged in as the root user."
        echo "You shall, however, run it with sudo."
        exit 1
    fi
    USER="${SUDO_USER}"
    echo "Installing as ${USER}"
fi

echo "Compiling hsu..."
# cd ${CTMPDIR}
curl -so $CTMPDIR/hsu.c file://${PWD}/hsu.c
# git clone https://github.com/naomisphere/hsu
gcc -o $CTMPDIR/hsu $CTMPDIR/hsu.c

if [ ! -x /Library/haro ]; then
    echo "Creating /Library/haro..."
    sudo mkdir -p /Library/haro/bin
    sudo mkdir -p /Library/haro/root/home
fi

echo "Running stage-two setup..."
sudo mv "${CTMPDIR}/hsu" "${HARO_BIN}/hsu"
sudo chown root:wheel "${HARO_BIN}/hsu"
sudo chmod u+s "${HARO_BIN}/hsu"

echo "Finishing off..."
sudo ln -sf "${HARO_BIN}/hsu" "${HARO_BIN}/su"

echo "\nalias hsu="${D_HSU_PATH}"" >> /Users/$USER/.zshrc
source /Users/$USER/.zshrc

echo "..done!"
echo ""
echo "Installed as ${D_HSU_PATH}"
echo "Run source ~/.zshrc or newgrp to apply changes"