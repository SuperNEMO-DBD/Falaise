// -*- mode: c++ ; -*-

// Ourselves:
#include "mock_calorimeter_s2c_module_utils.h"

namespace snemo {

  namespace processing {

    double pol3d (const double *x, const double *p) {

      const double X = x[0];
      const double Y = x[1];
      const double Z = x[2];

      double value = 0;

      switch ((int) (p[0])) {

      case 10:
        value += p[224]*X*X*X*X*X*X*X*X*X*X + p[225]*Y*Y*Y*Y*Y*Y*Y*Y*Y*Y + p[226]*Z*Z*Z*Z*Z*Z*Z*Z*Z*Z;

        value += p[227]*X*X*X*X*X*X*X*X*X*Y + p[228]*X*X*X*X*X*X*X*X*X*Z + p[229]*Y*Y*Y*Y*Y*Y*Y*Y*Y*X \
          + p[230]*Y*Y*Y*Y*Y*Y*Y*Y*Y*Z + p[231]*Z*Z*Z*Z*Z*Z*Z*Z*Z*X + p[232]*Z*Z*Z*Z*Z*Z*Z*Z*Z*Y;

        value += p[233]*X*X*X*X*X*X*X*X*Y*Y + p[234]*X*X*X*X*X*X*X*X*Z*Z + p[235]*X*X*X*X*X*X*X*X*Y*Z \
          + p[236]*Y*Y*Y*Y*Y*Y*Y*Y*X*X + p[237]*Y*Y*Y*Y*Y*Y*Y*Y*Z*Z + p[238]*Y*Y*Y*Y*Y*Y*Y*Y*X*Z \
          + p[239]*Z*Z*Z*Z*Z*Z*Z*Z*X*X + p[240]*Z*Z*Z*Z*Z*Z*Z*Z*Y*Y + p[241]*Z*Z*Z*Z*Z*Z*Z*Z*X*Y;

        value += p[242]*X*X*X*X*X*X*X*Y*Y*Y + p[243]*X*X*X*X*X*X*X*Y*Y*Z + p[244]*X*X*X*X*X*X*X*Y*Z*Z \
          + p[245]*X*X*X*X*X*X*X*Z*Z*Z + p[246]*Y*Y*Y*Y*Y*Y*Y*X*X*X + p[247]*Y*Y*Y*Y*Y*Y*Y*X*X*Z \
          + p[248]*Y*Y*Y*Y*Y*Y*Y*Z*Z*Z + p[249]*Y*Y*Y*Y*Y*Y*Y*Z*Z*X + p[250]*Z*Z*Z*Z*Z*Z*Z*X*X*X \
          + p[251]*Z*Z*Z*Z*Z*Z*Z*X*X*Y + p[252]*Z*Z*Z*Z*Z*Z*Z*X*Y*Y + p[253]*Z*Z*Z*Z*Z*Z*Y*Y*Y;
      
        value += p[254]*X*X*X*X*X*X*Y*Y*Y*Y + p[255]*X*X*X*X*X*X*Y*Y*Y*Z + p[256]*X*X*X*X*X*X*Y*Y*Z*Z \
          + p[257]*X*X*X*X*X*X*Y*Z*Z*Z + p[258]*X*X*X*X*X*X*Z*Z*Z*Z + p[259]*Y*Y*Y*Y*Y*Y*X*X*X*X \
          + p[260]*Y*Y*Y*Y*Y*Y*X*X*X*Z + p[261]*Y*Y*Y*Y*Y*Y*X*X*Z*Z + p[262]*Y*Y*Y*Y*Y*Y*X*Z*Z*Z \
          + p[263]*Y*Y*Y*Y*Y*Y*Z*Z*Z*Z + p[264]*Z*Z*Z*Z*Z*Z*X*X*X*X + p[265]*Z*Z*Z*Z*Z*Z*X*X*X*Y \
          + p[266]*Z*Z*Z*Z*Z*Z*X*X*Y*Y + p[267]*Z*Z*Z*Z*Z*Z*X*Y*Y*Y + p[268]*Z*Z*Z*Z*Z*Z*Y*Y*Y*Y;

        value += p[269]*X*X*X*X*X*Y*Y*Y*Y*Y + p[270]*X*X*X*X*X*Y*Y*Y*Y*Z + p[271]*X*X*X*X*X*Y*Y*Y*Z*Z \
          + p[272]*X*X*X*X*X*Y*Y*Z*Z*Z + p[273]*X*X*X*X*X*Y*Z*Z*Z*Z + p[274]*X*X*X*X*X*Z*Z*Z*Z*Z \
          + p[275]*Y*Y*Y*Y*Y*X*X*X*X*X + p[276]*Y*Y*Y*Y*Y*X*X*X*X*Z + p[277]*Y*Y*Y*Y*Y*X*X*X*Z*Z \
          + p[278]*Y*Y*Y*Y*Y*X*X*Z*Z*Z + p[279]*Y*Y*Y*Y*Y*X*Z*Z*Z*Z + p[280]*Y*Y*Y*Y*Y*Z*Z*Z*Z*Z \
          + p[281]*Z*Z*Z*Z*Z*X*X*X*X*X + p[282]*Z*Z*Z*Z*Z*X*X*X*X*Y + p[283]*Z*Z*Z*Z*Z*X*X*X*Y*Y \
          + p[284]*Z*Z*Z*Z*Z*X*X*Y*Y*Y + p[285]*Z*Z*Z*Z*Z*X*Y*Y*Y*Y + p[286]*Z*Z*Z*Z*Z*Y*Y*Y*Y*Y;
      
      case 9:
        value += p[167]*X*X*X*X*X*X*X*X*X + p[168]*Y*Y*Y*Y*Y*Y*Y*Y*Y + p[169]*Z*Z*Z*Z*Z*Z*Z*Z*Z;

        value += p[170]*X*X*X*X*X*X*X*X*Y + p[171]*X*X*X*X*X*X*X*X*Z + p[172]*Y*Y*Y*Y*Y*Y*Y*Y*X \
          + p[173]*Y*Y*Y*Y*Y*Y*Y*Y*Z + p[174]*Z*Z*Z*Z*Z*Z*Z*Z*X + p[175]*Z*Z*Z*Z*Z*Z*Z*Z*Y;

        value += p[176]*X*X*X*X*X*X*X*Y*Y + p[177]*X*X*X*X*X*X*X*Z*Z + p[178]*X*X*X*X*X*X*X*Y*Z \
          + p[179]*Y*Y*Y*Y*Y*Y*Y*X*X + p[180]*Y*Y*Y*Y*Y*Y*Y*Z*Z + p[181]*Y*Y*Y*Y*Y*Y*Y*X*Z + \
          p[182]*Z*Z*Z*Z*Z*Z*Z*X*X + p[183]*Z*Z*Z*Z*Z*Z*Z*Y*Y + p[184]*Z*Z*Z*Z*Z*Z*Z*X*Y;

        value += p[185]*X*X*X*X*X*X*Y*Y*Y + p[186]*X*X*X*X*X*X*Y*Y*Z + p[187]*X*X*X*X*X*X*Y*Z*Z \
          + p[188]*X*X*X*X*X*X*Z*Z*Z + p[189]*Y*Y*Y*Y*Y*Y*X*X*X + p[190]*Y*Y*Y*Y*Y*Y*X*X*Z \
          + p[191]*Y*Y*Y*Y*Y*Y*Z*Z*Z + p[192]*Y*Y*Y*Y*Y*Y*Z*Z*X + p[193]*Z*Z*Z*Z*Z*Z*X*X*X \
          + p[194]*Z*Z*Z*Z*Z*Z*X*X*Y + p[195]*Z*Z*Z*Z*Z*Z*X*Y*Y + p[196]*Z*Z*Z*Z*Z*Y*Y*Y;

        value += p[197]*X*X*X*X*X*Y*Y*Y*Y + p[198]*X*X*X*X*X*Y*Y*Y*Z + p[199]*X*X*X*X*X*Y*Y*Z*Z \
          + p[200]*X*X*X*X*X*Y*Z*Z*Z + p[201]*X*X*X*X*X*Z*Z*Z*Z + p[202]*Y*Y*Y*Y*Y*X*X*X*X \
          + p[203]*Y*Y*Y*Y*Y*X*X*X*Z + p[204]*Y*Y*Y*Y*Y*X*X*Z*Z + p[205]*Y*Y*Y*Y*Y*X*Z*Z*Z \
          + p[206]*Y*Y*Y*Y*Y*Z*Z*Z*Z + p[207]*Z*Z*Z*Z*Z*X*X*X*X + p[208]*Z*Z*Z*Z*Z*X*X*X*Y \
          + p[209]*Z*Z*Z*Z*Z*X*X*Y*Y + p[210]*Z*Z*Z*Z*Z*X*Y*Y*Y + p[211]*Z*Z*Z*Z*Z*Y*Y*Y*Y;

        value += p[212]*X*X*X*X*Y*Y*Y*Y*Z + p[213]*X*X*X*X*Y*Y*Y*Z*Z + p[214]*X*X*X*X*Y*Y*Z*Z*Z \
          + p[215]*X*X*X*X*Y*Z*Z*Z*Z + p[216]*Y*Y*Y*Y*X*X*X*X*Z + p[217]*Y*Y*Y*Y*X*X*X*Z*Z \
          + p[218]*Y*Y*Y*Y*X*X*Z*Z*Z + p[219]*Y*Y*Y*Y*X*Z*Z*Z*Z + p[220]*Z*Z*Z*Z*X*X*X*X*Y \
          + p[221]*Z*Z*Z*Z*X*X*X*Y*Y + p[222]*Z*Z*Z*Z*X*X*Y*Y*Y + p[223]*Z*Z*Z*Z*X*Y*Y*Y*Y;

      case 8:
        value += p[121]*X*X*X*X*X*X*X*X + p[122]*Y*Y*Y*Y*Y*Y*Y*Y + p[123]*Z*Z*Z*Z*Z*Z*Z*Z;

        value += p[124]*X*X*X*X*X*X*X*Y + p[125]*X*X*X*X*X*X*X*Z + p[126]*Y*Y*Y*Y*Y*Y*Y*X \
          + p[127]*Y*Y*Y*Y*Y*Y*Y*Z + p[128]*Z*Z*Z*Z*Z*Z*Z*X + p[129]*Z*Z*Z*Z*Z*Z*Z*Y;

        value += p[130]*X*X*X*X*X*X*Y*Y + p[131]*X*X*X*X*X*X*Z*Z + p[132]*X*X*X*X*X*X*Y*Z \
          + p[133]*Y*Y*Y*Y*Y*Y*X*X + p[134]*Y*Y*Y*Y*Y*Y*Z*Z + p[135]*Y*Y*Y*Y*Y*Y*X*Z \
          + p[136]*Z*Z*Z*Z*Z*Z*X*X + p[137]*Z*Z*Z*Z*Z*Z*Y*Y + p[138]*Z*Z*Z*Z*Z*Z*X*Y;

        value += p[139]*X*X*X*X*X*Y*Y*Y + p[140]*X*X*X*X*X*Y*Y*Z + p[141]*X*X*X*X*X*Z*Z*Z \
          + p[142]*X*X*X*X*X*Z*Z*Y + p[143]*Y*Y*Y*Y*Y*X*X*X + p[144]*Y*Y*Y*Y*Y*X*X*Z \
          + p[145]*Y*Y*Y*Y*Y*Z*Z*Z + p[146]*Y*Y*Y*Y*Y*Z*Z*X + p[147]*Z*Z*Z*Z*Z*X*X*X \
          + p[148]*Z*Z*Z*Z*Z*X*X*Y + p[149]*Z*Z*Z*Z*Z*X*Y*Y + p[150]*Z*Z*Z*Z*Z*X*Y*Y \
          + p[151]*Z*Z*Z*Z*Z*Y*Y*Y;

        value += p[152]*X*X*X*X*Y*Y*Y*Y + p[153]*X*X*X*X*Y*Y*Y*Z + p[154]*X*X*X*X*Y*Y*Z*Z \
          + p[155]*X*X*X*X*Y*Z*Z*Z + p[156]*X*X*X*X*Z*Z*Z*Z + p[157]*Y*Y*Y*Y*X*X*X*Z \
          + p[158]*Y*Y*Y*Y*X*X*Z*Z + p[159]*Y*Y*Y*Y*X*Z*Z*Z + p[160]*Y*Y*Y*Y*Z*Z*Z*Z \
          + p[161]*Z*Z*Z*Z*X*X*X*Y + p[162]*Z*Z*Z*Z*X*X*Y*Y + p[163]*Z*Z*Z*Z*X*Y*Y*Y;

        value += p[164]*X*X*X*Y*Y*Y*Z*Z + p[165]*X*X*X*Y*Y*Z*Z*Z + p[166]*Y*Y*Y*X*X*Z*Z*Z;

      case 7:
        value += p[86]*X*X*X*X*X*X*X + p[87]*Y*Y*Y*Y*Y*Y*Y + p[88]*Z*Z*Z*Z*Z*Z*Z;

        value += p[89]*X*X*X*X*X*X*Y + p[90]*X*X*X*X*X*X*Z + p[91]*Y*Y*Y*Y*Y*Y*X \
          + p[92]*Y*Y*Y*Y*Y*Y*Z + p[93]*Z*Z*Z*Z*Z*Z*X + p[94]*Z*Z*Z*Z*Z*Z*Y;

        value += p[95]*X*X*X*X*X*Y*Y + p[96]*X*X*X*X*X*Z*Z + p[97]*X*X*X*X*X*Y*Z \
          + p[98]*Y*Y*Y*Y*Y*X*X + p[99]*Y*Y*Y*Y*Y*Z*Z + p[100]*Y*Y*Y*Y*Y*X*Z \
          + p[101]*Z*Z*Z*Z*Z*X*X + p[102]*Z*Z*Z*Z*Z*Y*Y + p[103]*Z*Z*Z*Z*Z*X*Y;

        value += p[104]*X*X*X*X*Y*Y*Y + p[105]*X*X*X*X*Z*Z*Z + p[106]*X*X*X*X*Y*Y*Z \
          + p[107]*X*X*X*X*Z*Z*Y + p[108]*Y*Y*Y*Y*X*X*X + p[109]*Y*Y*Y*Y*Z*Z*Z \
          + p[110]*Y*Y*Y*Y*X*X*Z + p[111]*Y*Y*Y*Y*Z*Z*X + p[112]*Z*Z*Z*Z*X*X*X \
          + p[113]*Z*Z*Z*Z*Y*Y*Y + p[114]*Z*Z*Z*Z*X*X*Y + p[115]*Z*Z*Z*Z*Y*Y*X;

        value += p[116]*X*X*X*Y*Y*Y*Z + p[117]*X*X*X*Z*Z*Z*Y + p[118]*X*X*X*Z*Z*Y*Y \
          + p[119]*Y*Y*Y*Z*Z*Z*X + p[120]*Y*Y*Y*X*X*Z*Z + p[121]*Z*Z*Z*Y*Y*X*X;

      case 6:
        value += p[58]*X*X*X*X*X*X + p[59]*Y*Y*Y*Y*Y*Y + p[60]*Z*Z*Z*Z*Z*Z;

        value += p[61]*X*X*X*X*X*Y + p[62]*X*X*X*X*X*Z + p[63]*Y*Y*Y*Y*Y*X \
          + p[64]*Y*Y*Y*Y*Y*Z + p[65]*Z*Z*Z*Z*Z*X + p[66]*Z*Z*Z*Z*Z*Y;

        value += p[67]*X*X*X*X*Y*Y + p[68]*X*X*X*X*Z*Z + p[69]*X*X*X*X*Y*Z \
          + p[70]*Y*Y*Y*Y*X*X + p[71]*Y*Y*Y*Y*Z*Z + p[72]*Y*Y*Y*Y*X*Z \
          + p[73]*Z*Z*Z*Z*X*X + p[74]*Z*Z*Z*Z*Y*Y + p[75]*Z*Z*Z*Z*X*Y;

        value += p[76]*X*X*X*Y*Y*Y + p[77]*X*X*X*Z*Z*Z + p[78]*X*X*X*Y*Y*Z \
          + p[79]*X*X*X*Z*Z*Y + p[80]*Y*Y*Y*Z*Z*Z + p[81]*Y*Y*Y*X*X*Z \
          + p[82]*Y*Y*Y*Z*Z*X + p[83]*Z*Z*Z*X*X*Y + p[84]*Z*Z*Z*Y*Y*X;

        value += p[85]*X*X*Y*Y*Z*Z;

      case 5:
        value += p[36]*X*X*X*X*X + p[37]*Y*Y*Y*Y*Y + p[38]*Z*Z*Z*Z*Z;

        value += p[39]*X*X*X*X*Y + p[40]*X*X*X*X*Z + p[41]*Y*Y*Y*Y*X \
          + p[42]*Y*Y*Y*Y*Z + p[43]*Z*Z*Z*Z*X + p[44]*Z*Z*Z*Z*Y;

        value += p[45]*X*X*X*Y*Y + p[46]*X*X*X*Z*Z + p[47]*X*X*X*Y*Z \
          + p[48]*Y*Y*Y*X*X + p[49]*Y*Y*Y*Z*Z + p[50]*Y*Y*Y*X*Z \
          + p[51]*Z*Z*Z*X*X + p[52]*Z*Z*Z*Y*Y + p[53]*Z*Z*Z*X*Z;

        value += p[54]*X*X*Y*Y*Z + p[55]*X*X*Z*Z*Y \
          + p[56]*Y*Y*Z*Z*X + p[57]*Z*Z*Y*Y*X;

      case 4:
        value += p[21]*X*X*X*X + p[22]*Y*Y*Y*Y + p[23]*Z*Z*Z*Z;

        value += p[24]*X*X*X*Y + p[25]*X*X*X*Z + p[26]*Y*Y*Y*X 
          + p[27]*Y*Y*Y*Z + p[28]*Z*Z*Z*X + p[29]*Z*Z*Z*Y;

        value += p[30]*X*X*Y*Z + p[31]*X*X*Y*Y + p[32]*X*X*Z*Z \
          + p[33]*Y*Y*X*Z + p[34]*Y*Y*Z*Z + p[35]*Z*Z*Y*X;

      case 3:
        value += p[11]*X*X*X + p[12]*Y*Y*Y + p[13]*Z*Z*Z;

        value += p[14]*X*X*Y + p[15]*X*X*Z + p[16]*Y*Y*X \
          + p[17]*Y*Y*Z + p[18]*Z*Z*X + p[19]*Z*Z*Y;

        value += p[20]*X*Y*Z;

      case 2:
        value += p[5]*X*X + p[6]*Y*Y + p[7]*Z*Z;

        value += p[8]*X*Y + p[9]*X*Z + p[10]*Y*Z;

      case 1:
        value += p[2]*X + p[3]*Y + p[4]*Z;

      case 0:
        value += p[1];

      }

      return value;
    }

  }  // end of namespace processing

}  // end of namespace snemo
