#!/bin/sh

for i in {1..10}
do
    ((startIndex=$i-1))
    ((startPos=$startIndex*1000))
    ((endPos=$i*1000))
    echo $startPos $endPos
    `nohup /usr/local/php-7.0/bin/php main.php $startPos $endPos > "log/"$startPos".log" 2>&1 &`
done
