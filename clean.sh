#!/bin/bash

if [[ -e "buildout/" ]]; then
echo "Deleting folder buildout"
rm buildout/*.*
rmdir buildout
fi