from pylab import *

voltage = [0, 0.251, 0.505, 0.751, 0.998, 1.251, 1.5, 1.751, 1.999, 2.25, 2.5, 2.75, 2.999, 3.252, 3.337 ]

adc = [ 0, 76, 203, 246, 309, 402, 458, 513, 614, 688, 765, 843, 921, 975, 1023, ]
print adc[-1:]

# Develop a simple line with the slope defined as adc(end)/voltage(end) and the zero intercept = 0 
y = []
for v in voltage:
    ypt = adc[-1:][0]/voltage[-1:][0] * v ;
    y.append(ypt)

plot(voltage, adc, 'x', voltage, y, '-')
xlabel('Voltage')
ylabel('ADC Value')
title('Relationship between Voltage and ADC Value')
grid(True)
savefig('linear_data.pdf', format='pdf')
show()
