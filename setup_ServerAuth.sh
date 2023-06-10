#!/bin/bash

# $1: dir ip del servidor

gcc -o serverAuth serverAuth.c
scp serverAuth ubuntuadm@$1:/home/ubuntuadm
scp info_usuarios.txt ubuntuadm@$1:/home/ubuntuadm
ssh ubuntuadm@$1

