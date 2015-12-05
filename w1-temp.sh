#!/bin/bash

for(i=0; $i++; $i<100) {
	cat /sys/bus/w1/devices/28-000006dc1ebe/w1_slave >> w1-therm.log
}
