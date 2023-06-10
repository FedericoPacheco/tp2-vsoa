#!/usr/bin/env bash

# $1: dir ip del cliente

gcc -g -o clientSRPC serverInteract_clnt.c clientSRPC.c serverInteract_xdr.c -I/usr/include/tirpc -lnsl -ltirpc
scp clientSRPC ubuntuadm@$1:/home/ubuntuadm
ssh ubuntuadm@$1
