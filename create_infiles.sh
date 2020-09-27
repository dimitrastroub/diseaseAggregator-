#!/bin/bash
echo total arguments are $#
if (($# != 5))
then
   echo ERROR; exit 1
fi

re='^[0-9]+'

if ! [[ $4 =~ $re ]] ; 
then
   echo "error: Not a number or a positive number" >&2; exit 1
fi

if ! [[ $5 =~ $re ]] ; 
then
   echo "error: Not a number or a positive number" >&2; exit 1
fi

mkdir $3

filename=$2.txt
while read line; do
	mkdir $3/$line
	#echo $line 
	for (( i=1;i < ($4+1); i++ ))
	do
		datefile=$(($RANDOM%30+1))-$(($RANDOM%12+1))-$(($RANDOM%120+1900))
		touch $3/$line/$datefile.txt
		for (( j=1;j < ($5+1); j++ ))
		do
			recordId=$(($RANDOM%1000+1))
			binary=$(($RANDOM%2))
			if (($binary == 0))
			then
				hospital='ENTER'
			else
				hospital='EXIT'
			fi
			age=$(($RANDOM%120+1))
			pwdSize1=$(($RANDOM%10+3))
			pwdSize2=$(($RANDOM%10+3))
			patientFirstName=0
			patientLastName=0
			if [ $patientFirstName = 0 ]; 
			then
			    patientFirstName=`head -c $pwdSize1 /dev/urandom | base64|tr -d '0-9'|tr -dc '[:alnum:]\n\r' | tr '[:upper:]' '[:lower:]'`
			fi
			if [ $patientLastName = 0 ]; 
			then
			    patientLastName=`head -c $pwdSize2 /dev/urandom | base64|tr -d '0-9'|tr -dc '[:alnum:]\n\r' | tr '[:upper:]' '[:lower:]'`
			    patientFirstName=${patientFirstName^}
			    patientLastName=${patientLastName^}
			fi
			lines=$(wc -l $1.txt | awk '{ print $1 }')
			chooseLine=$(($RANDOM%$lines+1))
			a=$(head -$chooseLine $1.txt | tail -1 )
		    echo $recordId $hospital $patientFirstName $patientLastName $a $age>> $3/$line/$datefile.txt
		done
	done
done < $filename