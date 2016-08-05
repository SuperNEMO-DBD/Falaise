
Field map data of one octant of supernemo is in MapSmoothPlusDetail.csv.

An example python programme to read it and provide the interpolated field value 
at any point in the SuperNemo tracker is in bintd.py. Comment lines provide the 
definition of the coordinate system, the units, and the use of symmetry to get 
the field in other octants.There are only 160 lines of code and all in very bland 
python so I hope they are self explanatory.

Within bintd there is a short test called testba, so you can check it like this:

>>> import bintd
>>> bintd.testba()
first line ,  [141, 96, 24, 0.0, 0.0, 0.0, 0.02, 0.02, 0.02]
 At  1.6231 -1.3692 -0.4  B=  [-1702.88, 9064.638699999994, -11355.622899999995]
 At  1.6231 -1.3692 -0.393  B=  [-1697.5033700000001, 9345.492699999993, -11117.347799999996]
 At  1.6231 -1.3692 -0.386  B=  [-1692.12674, 9626.346699999995, -10879.072699999995]
 At  1.6231 -1.3692 -0.379  B=  [-1684.52478, 9901.625709999997, -10637.756999999994]

and you will get several more similar lines.

