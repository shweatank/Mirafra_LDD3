true'
#!/usr/bin/env bash
num=0
while [ $num -lt 100 ]
do
	echo $num
	num=$((num+1))
done
#onr bracket is do the operation and other is to dereference and store
'
#!/usr/bin/env bash
num=0
for i in {1..100}
do
	echo "in loop ...$i"
done
