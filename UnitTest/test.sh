
# usage: ./test.sh makefile
#         command    $1


TEST_PATH="/home/liutao/workspace/github/ygs-omp/UnitTest"
cd ${TEST_PATH}
# date: YYYYmmddHHMMSS
date=$(date +%Y%m%d%H%M%S) 
logfile="/home/liutao/workspace/ygs_log/omp/log_omp_${1}_${date}"
touch $logfile

make clean
make -f $1 clean
make -f $1

./clean_data.sh /dev/shm/ygs_data $1

export OMP_NESTED=true

for i in 1 2 4 8 12 16
do
    ./clean_data.sh /dev/shm/ygs_data $1
    export OMP_NUM_THREADS=$i
    sleep 6s 
    echo "threads: $i" >> $logfile
    /usr/bin/time -p -a -o $logfile ./test
    echo "" >> $logfile
done

make -f $1 clean
