savedcmd_/home/mirafra/pritesh/code/wait_q/wait_q.mod := printf '%s\n'   wait_q.o | awk '!x[$$0]++ { print("/home/mirafra/pritesh/code/wait_q/"$$0) }' > /home/mirafra/pritesh/code/wait_q/wait_q.mod
