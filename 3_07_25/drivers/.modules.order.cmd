cmd_/home/mirafra/ldd/3_07_25/drivers/modules.order := {   echo /home/mirafra/ldd/3_07_25/drivers/mymod_keyboard.ko; :; } | awk '!x[$$0]++' - > /home/mirafra/ldd/3_07_25/drivers/modules.order
