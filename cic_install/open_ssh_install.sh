#!/bin/bash
sudo apt install openssh-server -y
#sudo nano /etc/ssh/sshd_config
sudo systemctl restart sshd
sudo ufw allow ssh
sudo ufw enable
