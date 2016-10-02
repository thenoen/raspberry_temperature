#!/bin/bash

COMMAND='./test /sys/bus/w1/devices/28-000006dc1ebe/w1_slave'

while true
do
	OUTPUT_FILE="temperature_$(date +%Y_%m_%d).txt";
	echo -e "\n" | tee -a "${OUTPUT_FILE}"
	date | tee -a "${OUTPUT_FILE}"
	eval $COMMAND | tee -a "${OUTPUT_FILE}"
	sleep 1m
done
