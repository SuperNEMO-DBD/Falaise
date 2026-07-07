computeHistos=1

runId=4548
minOm=0
maxOm=711
baselineNbins=25
baselineK=5.0
rateK=5.0
rateNbins=100

print "\n================= runId = ", runId
# maxOm=14

max(x,y)= (x >= y) ? x : y
min(x,y)= (x >= y) ? x : y

if (computeHistos > 0) {
    print "\n======= Compute histograms ======="
    do for [omNum=0:maxOm:1] {
	print "========= OM #", omNum

	print "========= RATE ========="
	omRatePath=sprintf("_udd_channel_mon-%d/om_%d-rate.data", runId, omNum)
	#print "omRatePath = ", omRatePath
	stats omRatePath nooutput
	if (GPVAL_ERRNO) {
            print "[error] missing file : ", GPVAL_ERRMSG
            continue
	} else {
	    rateCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 8", omNum, runId)
	    rate=system(rateCmd)
	    if (rate == "nan") {
		print "[error] found nan rate"
		continue
	    }
	    rateErrCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 9", omNum, runId)
	    rateErr=system(rateErrCmd)
	    rateMin=int(max(0, rate - rateK * rateErr))
	    rateMax=int(rate + rateK * rateErr)+1
	    rateNbins=max(0,int(rateMax)-int(rateMin))
	    #print "rateMin:rateMax:rateNbins = ", rateMin, rateMax, rateNbins
	    omRateHistPath=sprintf("_udd_channel_mon-%d/om_%d-rate.his", runId, omNum)
	    #print "omRateHistPath = ", omRateHistPath
	    system(sprintf(" grep -v '^#' '%s' | grep -v 'nan' | cut -d' ' -f2 | flhistogram %f %f %d 12 > '%s' ", omRatePath, rateMin, rateMax, rateNbins, omRateHistPath))
	}
    # }
    
	print "\n======= BASELINE ======="
    # do for [omNum=0:maxOm:1] {
    # 	print "\n========= OM : ", omNum
	omBaselinePath=sprintf("_udd_channel_mon-%d/om_%d-baseline.data", runId, omNum)
	print "omBaselinePath = ", omBaselinePath
	stats omBaselinePath nooutput
	if (GPVAL_ERRNO) {
            print "[error] missing file : ", GPVAL_ERRMSG
            continue
	} else {
	    baselineCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 11", omNum, runId)
	    print " * baselineCmd = <", baselineCmd, ">"
	    baseline=system(baselineCmd)
	    print " * baseline = <", baseline, ">"
	    if (baseline == "nan") {
		print "[error] found baseline rate"
		continue
	    }

	    baselineErrCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 12", omNum, runId)
	    baselineErr=system(baselineErrCmd)
	    #print " * baseline = ", baseline, " +/- ", baselineErr
	    
	    omBaselineHistPath=sprintf("_udd_channel_mon-%d/om_%d-baseline.his", runId, omNum)
	    #print "omBaselineHistPath = ", omBaselineHistPath
	    baselineMin=baseline - baselineK * baselineErr
	    baselineMax=baseline + baselineK * baselineErr
	    #print "baselineMin:baselineMax:baselineNbins = ", baselineMin, baselineMax, baselineNbins
	    system(sprintf(" grep -v '^#' '%s' | grep -v 'nan' | cut -d' ' -f2 | flhistogram %f %f %d 12 > '%s' ", omBaselinePath, baselineMin, baselineMax, baselineNbins, omBaselineHistPath))
	}
    }
    
}
exit

set grid
set samples 2000

#maxOm=711

# minOm=169
# maxOm=169
# minOm=247
# maxOm=247
# minOm=269
# maxOm=269
# minOm=273
# maxOm=274
# minOm=305
# maxOm=305
# minOm=604
# maxOm=604
# minOm=635
# maxOm=635
# minOm=692
# maxOm=692
# minOm=709
# maxOm=709

print "\n======= Plots ======="

do for [omNum=minOm:maxOm:1] {
    print "=== omNum :", omNum

    nbPeriodsCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 7",omNum,runId)
    nbPeriods=system(nbPeriodsCmd)
    print " * nbPeriods = ", nbPeriods
    
    rateCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 8",omNum,runId)
    rate=system(rateCmd)
    print " * rate = ", rate

    baselineCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 11",omNum,runId)
    baseline=system(baselineCmd)
    print " * baseline = ", baseline

    baselineErrCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 12",omNum,runId)
    baselineErr=system(baselineErrCmd)
    print " * baselineErr = ", baselineErr

    set title sprintf(" Run %d - OM #%d  ", runId, omNum)
    
    # Rate:
    omRatePath=sprintf("_udd_channel_mon-%d/om_%d-rate.data", runId, omNum)
    stats omRatePath nooutput
    if (GPVAL_ERRNO) {
        print "[error] missing file : ", GPVAL_ERRMSG
        continue
    }
    set xlabel " Period ID (5 minutes) "
    set ylabel " Rate (5 minutes) "
    rateCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 8", omNum, runId)
    rate=system(rateCmd)
    rateErrCmd=sprintf("grep '^%d;' _udd_channel_mon-%d/oms.data | cut -d';' -f 9", omNum, runId)
    rateErr=system(rateErrCmd)
    rateMin=int(max(0, rate - rateK * rateErr))
    rateMax=int(rate + rateK * rateErr)+1
    rateNbins=max(0,int(rateMax)-int(rateMin))
    print "rateMin:rateMax:rateNbins = ", rateMin, rateMax, rateNbins
    # rateMin = 0.0
    # rateMax = rate+6*sqrt(rate)
    # rateMax = 200.0
    set xrange [0:nbPeriods+1]
    set yrange [rateMin:rateMax]
    poisson(x) = exp(log(rate) * x - lgamma(x+1) - rate)
    plot omRatePath u 1:2:(sqrt($2)) notitle w yerrorbars pt 7 ps 0.5 
    pause -1
    
    set xlabel "Rate (5 minutes)"
    set ylabel "Count"
    set xrange [rateMin:rateMax]
    set yrange [0:*]
    omRateHistPath=sprintf("_udd_channel_mon-%d/om_%d-rate.his", runId, omNum)
    stats omRateHistPath nooutput
    if (GPVAL_ERRNO) {
        print "[error] missing file : ", GPVAL_ERRMSG
        continue
    }
    plot omRateHistPath using (0.5*(($1)+($2))):3 with histeps notitle, nbPeriods * poisson(x) with lines notitle
    pause -1

    # Baseline:
    omBaselinePath=sprintf("_udd_channel_mon-%d/om_%d-baseline.data", runId, omNum)
    stats omBaselinePath nooutput
    if (GPVAL_ERRNO) {
        print "[error] missing file : ", GPVAL_ERRMSG
        continue
    }
    set xlabel " Period ID (5 minutes) "
    set ylabel " Baseline (volt) "
    baselineMin = baseline - baselineK * baselineErr #2.16
    baselineMax = baseline + baselineK * baselineErr #2.18
    set xrange [0:nbPeriods+5]
    set yrange [baselineMin:baselineMax]
    plot omBaselinePath u 1:2:($3) notitle w yerrorbars pt 7 ps 0.5 
    pause -1

    normal(x) = exp(-0.5*((x - baseline)/baselineErr)**2) / sqrt(2*pi) / baselineErr
    
    set xlabel "Baseline (5 minutes)"
    set ylabel "Count"
    # set xrange [baselineMin:baselineMax]
    set xrange [*:*]
    set yrange [0:*]
    omBaselineHistPath=sprintf("_udd_channel_mon-%d/om_%d-baseline.his", runId, omNum)
    stats omBaselineHistPath nooutput
    if (GPVAL_ERRNO) {
        print "[error] missing file : ", GPVAL_ERRMSG
        continue
    }
    wbin = (baselineMax - baselineMin) / baselineNbins
    print "wbin = ", wbin
    plot omBaselineHistPath using (0.5*(($1)+($2))):3 with histeps notitle, nbPeriods * normal(x) * wbin with lines notitle
    pause -1

}



exit

