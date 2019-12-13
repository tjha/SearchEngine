# Script to sftp your html results into another machine
# 2019-12-07:	init: jhirsh

if [ $# -ne 2 ]
then
	echo "Usage: ./sftp-bucket.sh bucket-name batch-name"
	echo "number of args $#"
	exit 65
fi

key=.search-engine-key.pem
user=ec2-user
host=ec2-100-24-240-217.compute-1.amazonaws.com

mv $1/html $1/$2/html
rsync --recursive --stats -e "ssh -i $key" $1 $user@$host:~/$2/
