#!/usr/bin/env sh
#
# CMPE242 Lab3. PID Controller. Use GPIO, ADC, and PWM.
# Master Makefile for the device drivers.
#
# Written by
# Matthew Lopez
# CMPE 242
# Spring 2013
# San Jose State University
#


DEVICES='matthew_adc matthew_gpio matthew_pwm'

for d in ${DEVICES}
do
	echo ${d}
	make MODULE_NAME=${d}
done
