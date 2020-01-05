# rsync a local file to a remote ec2

if [ $# -ne 3 ]
then
	echo "Usage: ./rsync_file.sh path/to/file path/to/pem host"
	echo "number of args $#"
	exit 65
fi

file=$1
key=$2
user=ec2-user
host=$3

rsync --recursive --stats -e "ssh -i $key" $file $user@$host:~/