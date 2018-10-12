#! /bin/sh

allSubDir=("baselib" "3rd/mqtt" "3rd/jsoncpp")
CURDIR=`pwd`
echo $CURDIR
 
for subdir in ${allSubDir[@]}
do
	cd $CURDIR"/"$subdir
	make clean
	make
done


cd $CURDIR"/"iot_device_async
make clean; make

cd $CURDIR"/"iot_device_sync
make clean; make
