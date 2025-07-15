cmd_/home/mirafra/ldd/14_07_25/modules.order := {   echo /home/mirafra/ldd/14_07_25/mymod.ko; :; } | awk '!x[$$0]++' - > /home/mirafra/ldd/14_07_25/modules.order
