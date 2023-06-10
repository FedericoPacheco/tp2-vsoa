#!/bin/bash

# $1: dir ip del servidor

gcc -g -o serverInteract serverInteract_svc.c serverInteract.c serverInteract_xdr.c -I/usr/include/tirpc -lnsl -ltirpc
scp serverInteract ubuntuadm@$1:/home/ubuntuadm
ssh ubuntuadm@$1
