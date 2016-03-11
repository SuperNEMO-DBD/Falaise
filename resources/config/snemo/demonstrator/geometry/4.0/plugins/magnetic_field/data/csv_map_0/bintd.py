
import bmdat

def brd(fn='Btable.csv'):

# read in an ascii file of numbers in csv format.

    f1 = open(fn,'r')

# first line is a header
# giving the size, origin and spacing of the grid in x,y and z.

    line = f1.next()
    cols = line.split(',')  
    nx = int( cols[0] )
    ny = int( cols[1] )
    nz = int( cols[2] )    
    hd = [  nx, ny, nz,
            float(cols[3]),float(cols[4]),float(cols[5]),
            float(cols[6]),float(cols[7]),float(cols[8]) ]
    print 'first line , ',hd
    bmdat.bhed.append( hd )
    
    bb = []
    for ax in range(3) :  # the three components of the field, Bx first
        bxyz = []
        for iz in range(nz) : # the z index
            bxy = []
            for iy in range(ny) : # the y index
                line = f1.next()  
                cols = line.split(',')
# each line starts with three redundant numbers, used for cross checks
# also useful to navigate the file in a spreadsheet 
                axi = int( cols[0] )
                iyi = int( cols[1] )
                izi = int( cols[2] )
                if axi!=ax or iyi!=iy or izi!=iz :
                    print ' bad line ',line
                    print 'ax,axi',ax,axi
                    print 'iy,iyi',iy,iyi
                    print 'iz,izi',iz,izi
                    exit()
# the remainder of the line is the field values versus x
                li = []
                for c in cols[3:] :
                    li.append( int(c) )
                bxy.append( li )
            bxyz.append( bxy )
        bb.append(bxyz)
    f1.close()
    bmdat.bmap.append( bb )
    mapno = len( bmdat.bhed ) - 1
    return mapno

def bint(mn=0,x=0.0,y=0.0,z=0.0) :
# linear interpolation in 3D on the map number mn.
# x,y,z is the coordinates of the point in units of metres

    hed = bmdat.bhed[mn]
    amap = bmdat.bmap[mn]
    nx = hed[0]
    ny = hed[1]
    nz = hed[2]
    ox = hed[3]
    oy = hed[4]
    oz = hed[5]
    sx = hed[6]
    sy = hed[7]
    sz = hed[8]

    xu = (x-ox)/sx
    yu = (y-oy)/sy
    zu = (z-oz)/sz    
    ixl = int( xu )
    iyl = int( yu )
    izl = int( zu )
    fx = xu-ixl
    fy = yu-iyl
    fz = zu-izl
    gx = 1 - fx
    gy = 1 - fy
    gz = 1 - fz

    if ixl>=0 and ixl<nx-1 and iyl>=0 and iyl<ny-1 and izl>=0 and izl<nz-1 :
        bout = []
        for ax in range(3) :
            bmap = amap[ax]
            b000 = bmap[izl][iyl][ixl]
            b100 = bmap[izl][iyl][ixl+1]
            b010 = bmap[izl][iyl+1][ixl]
            b001 = bmap[izl+1][iyl][ixl]
            b110 = bmap[izl][iyl+1][ixl+1]
            b011 = bmap[izl+1][iyl+1][ixl]
            b101 = bmap[izl+1][iyl][ixl+1]
            b111 = bmap[izl+1][iyl+1][ixl+1]

            bf00 = gx*b000 + fx*b100 
            bf10 = gx*b010 + fx*b110
            bff0 = gy*bf00 + fy*bf10
            bf01 = gx*b001 + fx*b101 
            bf11 = gx*b011 + fx*b111
            bff1 = gy*bf01 + fy*bf11
            bfff = gz*bff0 + fz*bff1
            bout.append( bfff )
    else :
        print 'Outside map ',mn
        print ' x , y , z , xu , yu , zu ',x , y , z , xu , yu , zu
        print ' ixl, iyl , izl , nx , ny , nz ',ixl, iyl , izl , nx , ny , nz
#        exit()
        bout = [ 0. , 0. , 0. ]
# the output is the three field components [Bx,By,Bz] in units of milliGauss 
    return bout

def binta(mn=0,x=0.0,y=0.0,z=0.0) :
# return a field value at point x,y,z
# coordinates of the point in units of metres

# reflections and inversions for each octant
    if x<0.0 :
        mx = [-1 , 1 , 1 ]
    else :
        mx = [ 1 , 1 , 1 ]
    if y<0.0 :
        my = [-1 , 1 ,-1 ]
    else :
        my = [ 1 , 1 , 1 ]
    if z<0.0 :
        mz = [ 1 , 1 ,-1 ]
    else :
        mz = [ 1 , 1 , 1 ]
        
# get the field value in the +,+,+ octant
    bb = bint(mn, abs(x), abs(y), abs(z) )
    bout = []
    
# apply the sign changes
    for ax in range(3) :
        bout.append( bb[ax]*mx[ax]*my[ax]*mz[ax] )
        
# the output is the three field components [Bx,By,Bz] in units of milliGauss 
    return bout

def testba():
# the coordinate system has its origin in the centre of the source foil.
# X is in the horizontal direction within the foil.
# Y is in the vertical direction within the foil.
# Z is perpendicular to X and Y to make a right-handed cartesian system.
    
    mn1 = brd('MapSmoothPlusDetail.csv')

    x = 1.6231
    y = -1.3692

    for iz in range(100) :
        z = iz*0.007 - 0.4
        bb = binta(mn1,x,y,z)
        print ' At ',x,y,z,' B= ',bb

    return 0
