cmd_/home/mirafra/ldd/1_07_25/drivers/modules.order := {   echo /home/mirafra/ldd/1_07_25/drivers/mymod.ko; :; } | awk '!x[$$0]++' - > /home/mirafra/ldd/1_07_25/drivers/modules.order
