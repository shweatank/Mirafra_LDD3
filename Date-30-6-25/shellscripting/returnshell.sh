result=$(($RANDOM % 2))
if [ $result -eq 0 ]
then 
	true
	echo "$?"
else
	false
	echo "$?"
fi
