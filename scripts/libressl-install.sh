#!/bin/bash
# https://github.com/palkan/acli/blob/master/travis-install-libressl.sh
# Install dependencies for a linux machine
## LibreSSL
## tmux

set +e

sudo apt-get update
sudo apt-get install tmux
sudo yum install update
sudo yum upgrade
sudo yum install tmux
sudo yum group install "Development Tools"
sudo amazon-linux-extras install epel
sudo yum install s3fs-fuse

INSTALL_PATH="/opt" # or $HOME/opt

# only install LibreSSL if it is not installed
if [ ! -d "$INSTALL_PATH/libressl/lib" ]; then
	cd "$INSTALL_PATH"
	sudo git clone https://github.com/libressl-portable/portable libressl
	cd libressl
	./autogen.sh
	./configure --prefix="$INSTALL_PATH/libressl"
	echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PATH/libressl/lib" >> ~/.bashrc
	echo "export LDFLAGS=$INSTALL_PATH/libressl/lib" >> ~/.bashrc
	echo "export CPPFLAGS=$INSTALL_PATH/libressl/include" >> ~/.bashrc
	make && make check
	sudo make install
	sudo mkdir etc/ # cert.pem is not installed correctly
	cd etc
	sudo mkdir ssl/
	cd ..
	sudo cp apps/openssl/cert.pem etc/ssl
	echo "$INSTALL_PATH/libressl" | sudo tee /etc/ld.so.conf.d/libressl.conf
	sudo ldconfig
	echo "export PATH_TO_SEARCH_ENGINE=/home/ec2-user/SearchEngine" >> ~/.bashrc
	source ~/.bashrc
else
	echo 'Using cached directory.'
fi
