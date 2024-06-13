#!/bin/bash

# Actualizar y mejorar los paquetes del sistema
sudo apt update && sudo apt upgrade -y

# Instalar Python 3.1
sudo apt update && sudo apt upgrade -y
sudo apt install software-properties-common -y
sudo add-apt-repository ppa:deadsnakes/ppa
sudo apt install python3.10 -y
sudo apt install python3-pip -y

# Instalar Arduino
sudo apt install arduino -y

sudo apt install pulseaudio -y

# Descargar el archivo RustDesk
wget https://github.com/rustdesk/rustdesk/releases/download/1.1.8/rustdesk-1.1.8.deb

# Instalar RustDesk
sudo apt install ./rustdesk-1.1.8.deb -y

# Limpiar el archivo descargado
rm ./rustdesk-1.1.8.deb

echo "Todos los programas han sido instalados correctamente."
