#!/bin/bash
ls /etc
if [ $? -eq 0 ]; then
	echo "succeeded exit status:$?"
else
	echo "failed exit status:$?"
fi
