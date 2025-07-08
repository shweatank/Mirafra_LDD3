savedcmd_/home/mirafra/pritesh/code/interrupt/int.mod := printf '%s\n'   int.o | awk '!x[$$0]++ { print("/home/mirafra/pritesh/code/interrupt/"$$0) }' > /home/mirafra/pritesh/code/interrupt/int.mod
