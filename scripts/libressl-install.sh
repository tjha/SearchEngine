#!/bin/bash
# https://github.com/palkan/acli/blob/master/travis-install-libressl.sh
# Install LibreSSL in linux environment
set +e

INSTALL_PATH="/opt" # or $HOME/opt

# check if libressl is already installed
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
else
	echo 'Using cached directory.'
fi
