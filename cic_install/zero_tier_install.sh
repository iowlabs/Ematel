#!/bin/bash
curl -s https://install.zerotier.com | sudo bash
sudo apt update -y
sudo apt install zerotier-one -y

# Finalmente
sudo zerotier-cli join 56374ac9a4357547
