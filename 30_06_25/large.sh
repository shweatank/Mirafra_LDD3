x=100
y=110
z=200
if [ "$x" -gt "$y" ] && [ "$x" -gt "$z" ]
then
	echo "$x is largest"
elif [ "$x" -lt "$y" ] && [ "$z" -lt "$y" ]
then 
	echo "$y is largest"
else
	echo "$z is largest"
fi
