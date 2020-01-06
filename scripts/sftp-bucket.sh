# Script to sftp your html results into another machine
# 2019-12-07:	init: jhirsh

if [ $# -ne 4 ]
then
	echo "Usage: ./sftp-bucket.sh localHtmlFolder destinationHtmlFolder pemLocation host"
	echo "number of args $#"
	exit 65
fi

key=$3
user=ec2-user
host=$4

rsync --recursive --stats -e "ssh -i $key" $1 $user@$host:~/socket-html/$2/
