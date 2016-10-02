#!/bin/bash

OUTPUT_FILE="temp-$(date).txt";

while true
do
	echo -e "\n" | tee -a "${OUTPUT_FILE}"
	date | tee -a "${OUTPUT_FILE}"
	#echo -e "\n" | tee -a "${OUTPUT_FILE}"
	./test /sys/bus/w1/devices/28-000006dc1ebe/w1_slave | tee -a "${OUTPUT_FILE}"
	sleep 1m
done
