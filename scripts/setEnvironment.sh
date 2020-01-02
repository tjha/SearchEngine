#!/bin/bash

echo "export LD_LIBRARY_PATH=/opt/libressl/lib" >> ~/.bashrc
echo "export LDFLAGS=/opt/libressl/lib" >> ~/.bashrc
echo "export CPPFLAGS=/opt/libressl/include" >> ~/.bashrc
echo "export PATH_TO_SEARCH_ENGINE="$(pwd) >> ~/.bashrc
