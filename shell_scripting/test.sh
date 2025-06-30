#gt greater than lt lesser than
#!/bin/bin/env bash
greeting="hello world"
echo $greeting
num=1010
if [ $num -gt 100 ] && [ $num -lt 1000 ]
then
	echo "thats a big number"
elif [ $num -gt 1000 ]
then 
	echo "thats a bigger number"
else
	echo "thats a small number"
	
fi
