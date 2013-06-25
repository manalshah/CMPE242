from pylab import *
import math
import fileinput

power = []

for line in fileinput.input():
    (raw_real, raw_img, fr, fi, pwr) = line.split(',')
    fft_real = float(fr)
    fft_img = float(fi)
    power_m = math.sqrt( fft_real*fft_real + fft_img*fft_img )
    #power.append( power_m )
    power.append( float(pwr) )

power.append(0)

N = len(power)

x = range(1,N+1)

bar(x,power, width=1, color='g')
xlabel('m')
ylabel('Power')
title('Power Spectrum, ADC, N = %d' % N )
grid(True)

savefig('power_spectrum_adc_%d.pdf'%N, format='pdf')
show()

