#
# Calculate the expected voltage as the pot. is changed.
#
R1 = 26.83e3
R2max = 50.60e3
Vcc = 5

print "At exact value"
for i in range(1,11):
    R2 = R2max * (i/10.0)
    V = (R2/(R1+R2))*Vcc
    print R2, V

print ''
print "At -5%"
R1_less5 = R1 * 0.95
for i in range(1,11):
    R2 = R2max * (i/10.0)
    V = (R2/(R1_less5+R2))*Vcc
    print R2, V

print ''
print "At +5%"
R1_add5 = R1 * 1.05
for i in range(1,11):
    R2 = R2max* (i/10.0)
    V = (R2/(R1_add5+R2))*Vcc
    print R2, V
