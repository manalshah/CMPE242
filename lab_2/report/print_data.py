from pylab import *
import math
import fileinput

power = []

for line in fileinput.input():
    (raw_real, raw_img, fr, fi) = line.split(',')
    fft_real = float(fr)
    fft_img = float(fr)
    power_m = math.sqrt( fft_real*fft_real + fft_img*fft_img )
    power.append( power_m )

N = len(power)

x = range(1,N+1 )

#print x,power

bar(x,power, width=1, color='g')
xlabel('m')
ylabel('Power')
title('Power Spectrum, 0-3.3VDC, N = %d' % N )
grid(True)

savefig('power_spectrum_0_33_%d.pdf'%N, format='pdf')
show()

