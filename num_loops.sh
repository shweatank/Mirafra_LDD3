#!/usr/bin/env bash

for num in {1..20}; do
	echo "$num"
	if [ "$num" -gt 10 ]; then
		echo "I'M big!"
	fi
done


