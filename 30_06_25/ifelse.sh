num=101
if [ $num -gt 1000 ]
then 
	echo "That a huge number!"
elif [ $num -gt 100 ]
then
	echo "That's a big number"
else
	echo "that's a small number."
fi
