set grid
set key out 
set samples 10000

mu1Skip=1.0
mu1(x)=1.0-x
sig1=1.0
xscale1=0.35
ln1(x)=x<0 ? 0 : exp(-0.5*((log(x*xscale1)-mu1(mu1Skip))/sig1)**2)/(x*xscale1)/sig1/sqrt(2*pi)

sig2=0.5
xscale2=1.1
rayleigh2(x)=x<0 ? 0 : exp(-0.5*((x*xscale2)/sig2)**2)*(x*xscale2)/sig2**2

a2 = 10.0
xscale2=50.0
maxwell2(x)=x<0 ? 0 : sqrt(2./pi)*(x*xscale2)**2*exp(-0.5*(x*xscale2/a2))/a2**3


A1=1.0
A2=8.25
xStart = 0.0
#z(x)=x<xStart ? 0 : A1*ln1(x-xStart)+A2*rayleigh2(x-xStart)
z(x)=x<xStart ? 0 : A1*ln1(x-xStart)+A2*maxwell2(x-xStart)

plot [-2:10] ln1(x), maxwell2(x) # , rayleigh2(x)
pause -1

plot [0:10] z(x)
pause -1

# exit

eStartTcd = 5.5
startTcd(x) = exp(x)
eMuTdc = 1.0
muTdc(x) = exp(x/10)
eLamTdc = 1.0
lamTdc(x) = exp(x/10)
eAmpAdc = 0.95
ampAdc(x) = exp(5*x)
baselineAdc = 3556.0

eLamFact=-1.5
lamFact(x)=0.5+exp(x)
ampRatio=2.0

xTrans(x) = (x - startTcd(eStartTcd)) / 200

fFast(x) = (x < startTcd(eStartTcd)) ? baselineAdc : baselineAdc - ampAdc(eAmpAdc) * sqrt(lamFact(eLamFact)*lamTdc(eLamTdc)/2/pi/(xTrans(x)**3)) * exp(-0.5*lamFact(eLamFact)*lamTdc(eLamTdc)*(xTrans(x)-muTdc(eMuTdc))**2/xTrans(x)/(muTdc(eMuTdc)**2))

fSlow(x) = (x < startTcd(eStartTcd)) ? baselineAdc : baselineAdc - ampRatio * ampAdc(eAmpAdc) * sqrt(lamTdc(eLamTdc)/2/pi/(xTrans(x)**3)) * exp(-0.5*lamTdc(eLamTdc)*(xTrans(x)-muTdc(eMuTdc))**2/xTrans(x)/(muTdc(eMuTdc)**2)) 

f(x)=fFast(x)+fSlow(x)-baselineAdc

adcScale=80.0

corrPedSig = 4.0
corrPedSigR = 40.0
corrPedSkip = 0.25
# corrPedSkip = 0.75
corrPedMu(x) = startTcd(eStartTcd)+corrPedSkip*corrPedSig
corrPedRatio = 0.2
corrPed(x) = x < corrPedMu(eStartTcd) ? \
	   corrPedRatio * exp(-0.5*((x-corrPedMu(eStartTcd))/corrPedSig)**2) : \
	   corrPedRatio * exp(-0.5*((x-corrPedMu(eStartTcd))/corrPedSigR)**2) 
	   
corrPed(x)=0.0

fb(x)=ampAdc(eAmpAdc) * ( z((x-startTcd(eStartTcd))/adcScale) + corrPed(x) )
ffb = 0.25
ffb = 0.125
convolSigma=4.0
convol(x) = exp(-0.5*(x/convolSigma)**2)

f(x)=baselineAdc \
		 - fb(x-5)*convol(-5.0) \
		 - fb(x-4)*convol(-4.0) \
		 - fb(x-3)*convol(-3.0) \
		 - fb(x-2)*convol(-2.0) \
		 - fb(x-1)*convol(-1.0) \
		 - fb(x)*convol(0.0) \
		 - fb(x-1.0)*convol(1.0) \
		 - fb(x-2.0)*convol(2.0) \
		 - fb(x-3.0)*convol(3.0) \
		 - fb(x-4.0)*convol(4.0) \
		 - fb(x-5.0)*convol(5.0)

f(x) = baselineAdc - fb(x)

# print "exp(-ffb)=", exp(-ffb)
# print "exp(-9*ffb)=", exp(-9*ffb)
# f(x)=baselineAdc + fb(x) + exp(-ffb) * fb(x+1) + exp(-2*ffb) * fb(x+2) + exp(-3*ffb) * fb(x+3) + exp(-4*ffb) * fb(x+4) + exp(-5*ffb) * fb(x+5) + exp(-6*ffb) * fb(x+6) + exp(-7*ffb) * fb(x+7) + exp(-8*ffb) * fb(x+8) + exp(-9*ffb) * fb(x+9) + exp(-10*ffb) * fb(x+10)

#exit

set xrange [0:1024]
set yrange [0:4096]
set yrange [*:*]

eventId=1
hitId=0
if (eventId == 4) {
    hitId=1
}
if (eventId == 5) {
    hitId=1
}
if (eventId == 8) {
    hitId=1
}

hitSignalFile=sprintf('_calo_hit_signals-%d.data', eventId)

plot f(x) title "Fit"  lc rgb 'magenta', \
     baselineAdc-ampAdc(eAmpAdc)*corrPed(x) title "corrPed"  lc rgb 'red', \
     hitSignalFile i hitId with steps notitle lc rgb 'orange' 
pause -1

# plot fFast(x) title "Fast" lc rgb 'green'  , \
#      fSlow(x) title "Slow"  lc rgb 'blue' , \
#      f(x) title "Fit"  lc rgb 'magenta', \
#      '_calo_hit_signals-0.data' i hitId with steps notitle lc rgb 'orange' 
# pause -1

#exit

set fit v4
set fit limit 1e-5
set fit prescale
set fit errorvariables

adcErr0=2.0
bs(x)=baselineAdc
fit [0:200] bs(x) hitSignalFile i hitId using 1:2:(adcErr0) via baselineAdc

print "baselineAdc = ", baselineAdc , " +/- ", baselineAdc_err
# exit

adcError(x)=(x<250) ? adcErr0 : (x<300) ? adcErr0 + (x - 250.0)/75 : (x<600) ? adcErr0 + (300 - 250.0)/25 : 1.5*adcErr0
#fit [200:950] f(x)  hitSignalFile i hitId using 1:2:(adcError($1)) via eStartTcd,A2,adcScale,sig1,a2,mu1Skip
#fit [200:950] f(x)  hitSignalFile i hitId using 1:2:(adcError($1)) via eStartTcd,A2,adcScale,sig1,sig2,mu1Skip

fit [200:950] f(x)  hitSignalFile i hitId using 1:2:(adcError($1)) via eStartTcd,A2,adcScale,mu1Skip,sig1#,a2,xscale2


#eMuTdc,eLamTdc,eAmpAdc,eLamFact

print "startTdc = ", exp(eStartTcd)

plot \
     baselineAdc-ampAdc(eAmpAdc)*corrPed(x) title "corrPed"  lc rgb 'red', \
     hitSignalFile i hitId with steps notitle lc rgb 'orange' , \
     hitSignalFile i hitId every 4 using 1:2:(adcError($1)) with yerrorbars notitle pt 1 ps 0.01, \
     f(x) title "Fit"  lc rgb 'magenta'
pause -1

# fFast(x) title "Fast" lc rgb 'green'  , \
#      fSlow(x) title "Slow"  lc rgb 'blue' , \
     