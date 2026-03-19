

print(f"#@title=Mock tracker cell plasma propagation time (PPT) table for test only")
print(f"#@author=F.Mauger")
print(f"#@date=2026-03-19")
print(f"#cell_num;start_time;stop_time;ppt_type;ppt[ns];ppt_error[ns]")

startTimeStr="2025-04-01 00:00:00"
stopTimeStr="2026-04-01 00:00:00"
ppt_ns=55250.0
pptError_ns=1250.0

startTimeStr="2026-04-01 00:00:00"
stopTimeStr="2027-04-01 00:00:00"
ppt_ns=53250.0
pptError_ns=1750.0

nbCells = 2034
for cellNum in range(nbCells) :
    pptType="normal"
    print(f"{cellNum};{startTimeStr};{stopTimeStr};{pptType};{ppt_ns};{pptError_ns}")

exit(0)
    
