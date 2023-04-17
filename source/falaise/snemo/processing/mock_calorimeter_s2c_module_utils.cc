// -*- mode: c++ ; -*-

// Ourselves:
#include "mock_calorimeter_s2c_module_utils.h"

// Standard library:
#include <cstdint>

// Special macro to mute fallthrough warning in enum
#if defined(__GNUC__) && __GNUC__ >= 7 || defined(__clang__) && __clang_major__ >= 10
// __cplusplus > 201402L for C++14
#if __cplusplus >= 201703L  
#define FALL_THROUGH [[fallthrough]]
#else
#define FALL_THROUGH __attribute__ ((fallthrough))
#endif
#else
#define FALL_THROUGH ((void)0)
#endif /* __GNUC__ >= 7 */

namespace snemo {

  namespace processing {

    enum pol3d_power_type : std::uint8_t
      {
       POL3D_POW0 = 0,
       POL3D_POW1 = 1,
       POL3D_POW2 = 2,
       POL3D_POW3 = 3,
       POL3D_POW4 = 4,
       POL3D_POW5 = 5,
       POL3D_POW6 = 6,
       POL3D_POW7 = 7,
       POL3D_POW8 = 8,
       POL3D_POW9 = 9,
       POL3D_POW10 = 10
      };

    double pol3d (const double *x, const double *p) {

      const double X = x[0];
      const double Y = x[1];
      const double Z = x[2];
      double value = 0.0;
      // Optimize computations:
      double X2 = X*X;
      double Y2 = Y*Y;
      double Z2 = Z*Z;
      double X3 = X2*X;
      double Y3 = Y2*Y;
      double Z3 = Z2*Z;
      double X4 = X2*X2;
      double Y4 = Y2*Y2;
      double Z4 = Z2*Z2;
      double X5 = X4*X;
      double Y5 = Y4*Y;
      double Z5 = Z4*Z;
      double X6 = X4*X2;
      double Y6 = Y4*Y2;
      double Z6 = Z4*Z2;
      double X7 = X4*X3;
      double Y7 = Y4*Y3;
      double Z7 = Z4*Z3;
      double X8 = X4*X4;
      double Y8 = Y4*Y4;
      double Z8 = Z4*Z4;
      double X9 = X5*X4;
      double Y9 = Y5*Y4;
      double Z9 = Z5*Z4;
      double X10 = X5*X5;
      double Y10 = Y5*Y5;
      double Z10 = Z5*Z5;
      auto power = static_cast<pol3d_power_type>(p[0]);
      switch (power) {

      case POL3D_POW10:
        value += p[224]*X10 + p[225]*Y10 + p[226]*Z10;
        
        value += p[227]*X9*Y + p[228]*X9*Z      \
          +      p[229]*Y9*X + p[230]*Y9*Z      \
          +      p[231]*Z9*X + p[232]*Z9*Y;
        
        value += p[233]*X8*Y2 + p[234]*X8*Z2 + p[235]*X8*Y*Z    \
          +      p[236]*Y8*X2 + p[237]*Y8*Z2 + p[238]*Y8*X*Z    \
          +      p[239]*Z8*X2 + p[240]*Z8*Y2 + p[241]*Z8*X*Y;
        
        value += p[242]*X7*Y3 + p[243]*X7*Y2*Z + p[244]*X7*Y*Z2 + p[245]*X7*Z3 \
          +      p[246]*Y7*X3 + p[247]*Y7*X2*Z + p[248]*Y7*Z3   + p[249]*Y7*Z2*X \
          +      p[250]*Z7*X3 + p[251]*Z7*X2*Y + p[252]*Z7*X*Y2 + p[253]*Z7*Y3;
        
        value += p[254]*X6*Y4 + p[255]*X6*Y3*Z + p[256]*X6*Y2*Z2 + p[257]*X6*Y*Z3 + p[258]*X6*Z4 \
          +      p[259]*Y6*X4 + p[260]*Y6*X3*Z + p[261]*Y6*X2*Z2 + p[262]*Y6*X*Z3 + p[263]*Y6*Z4 \
          +      p[264]*Z6*X4 + p[265]*Z6*X3*Y + p[266]*Z6*X2*Y2 + p[267]*Z6*X*Y3 + p[268]*Z6*Y4;

        value += p[269]*X5*Y5 + p[270]*X5*Y4*Z + p[271]*X5*Y3*Z2 + p[272]*X5*Y2*Z3 + p[273]*X5*Y*Z4 + p[274]*X5*Z5 \
          +      p[275]*Y5*X5 + p[276]*Y5*X4*Z + p[277]*Y5*X3*Z2 + p[278]*Y5*X2*Z3 + p[279]*Y5*X*Z4 + p[280]*Y5*Z5 \
          +      p[281]*Z5*X5 + p[282]*Z5*X4*Y + p[283]*Z5*X3*Y2 + p[284]*Z5*X2*Y3 + p[285]*Z5*X*Y4 + p[286]*Z5*Y5;
        FALL_THROUGH;
        
      case POL3D_POW9:
        value += p[167]*X9 + p[168]*Y9 + p[169]*Z9;

        value += p[170]*X8*Y + p[171]*X8*Z \
          +      p[172]*Y8*X + p[173]*Y8*Z \
          +      p[174]*Z8*X + p[175]*Z8*Y;

        value += p[176]*X7*Y2 + p[177]*X7*Z2 + p[178]*X7*Y*Z \
          +      p[179]*Y7*X2 + p[180]*Y7*Z2 + p[181]*Y7*X*Z \
          +      p[182]*Z7*X2 + p[183]*Z7*Y2 + p[184]*Z7*X*Y;

        value += p[185]*X6*Y3 + p[186]*X6*Y2*Z + p[187]*X6*Y*Z2 + p[188]*X6*Z3  \
          +      p[189]*Y6*X3 + p[190]*Y6*X2*Z + p[191]*Y6*Z3   + p[192]*Y6*Z2*X \
          +      p[193]*Z6*X3 + p[194]*Z6*X2*Y + p[195]*Z6*X*Y2 + p[196]*Z6*Y3;

        value += p[197]*X5*Y4 + p[198]*X5*Y3*Z + p[199]*X5*Y2*Z2 + p[200]*X5*Y*Z3 + p[201]*X5*Z4 \
          +      p[202]*Y5*X4 + p[203]*Y5*X3*Z + p[204]*Y5*X2*Z2 + p[205]*Y5*X*Z3 + p[206]*Y5*Z4 \
          +      p[207]*Z5*X4 + p[208]*Z5*X3*Y + p[209]*Z5*X2*Y2 + p[210]*Z5*X*Y3 + p[211]*Z5*Y4;

        value += p[212]*X4*Y4*Z + p[213]*X4*Y3*Z2 + p[214]*X4*Y2*Z3 + p[215]*X4*Y*Z4 \
          +      p[216]*Y4*X4*Z + p[217]*Y4*X3*Z2 + p[218]*Y4*X2*Z3 + p[219]*Y4*X*Z4 \
          +      p[220]*Z4*X4*Y + p[221]*Z4*X3*Y2 + p[222]*Z4*X2*Y3 + p[223]*Z4*X*Y4;
        FALL_THROUGH;

      case POL3D_POW8:
        value += p[121]*X8 + p[122]*Y8 + p[123]*Z8;

        value += p[124]*X7*Y + p[125]*X7*Z + p[126]*Y7*X \
          +      p[127]*Y7*Z + p[128]*Z7*X + p[129]*Z7*Y;

        value += p[130]*X6*Y2 + p[131]*X6*Z2 + p[132]*X6*Y*Z \
          +      p[133]*Y6*X2 + p[134]*Y6*Z2 + p[135]*Y6*X*Z \
          +      p[136]*Z6*X2 + p[137]*Z6*Y2 + p[138]*Z6*X*Y;

        value += p[139]*X5*Y3 + p[140]*X5*Y2*Z + p[141]*X5*Z3 + p[142]*X5*Z2*Y \
          +      p[143]*Y5*X3 + p[144]*Y5*X2*Z + p[145]*Y5*Z3 + p[146]*Y5*Z2*X \
          +      p[147]*Z5*X3 + p[148]*Z5*X2*Y + p[149]*Z5*X*Y2 + p[150]*Z5*X*Y2 \
          +      p[151]*Z5*Y3;

        value += p[152]*X4*Y4   + p[153]*X4*Y3*Z  + p[154]*X4*Y2*Z2 + p[155]*X4*Y*Z3 + p[156]*X4*Z4 \
          +      p[157]*Y4*X3*Z + p[158]*Y4*X2*Z2 + p[159]*Y4*X*Z3  + p[160]*Y4*Z4 \
          +      p[161]*Z4*X3*Y + p[162]*Z4*X2*Y2 + p[163]*Z4*X*Y3;

        value += p[164]*X3*Y3*Z2 + p[165]*X3*Y2*Z3 + p[166]*Y3*X2*Z3;
        FALL_THROUGH;

      case POL3D_POW7:
        value += p[86]*X7 + p[87]*Y7 + p[88]*Z7;

        value += p[89]*X6*Y + p[90]*X6*Z \
          +      p[91]*Y6*X + p[92]*Y6*Z \
          +      p[93]*Z6*X + p[94]*Z6*Y;

        value += p[95]*X5*Y2  + p[96]*X5*Z2  + p[97]*X5*Y*Z \
          +      p[98]*Y5*X2  + p[99]*Y5*Z2  + p[100]*Y5*X*Z \
          +      p[101]*Z5*X2 + p[102]*Z5*Y2 + p[103]*Z5*X*Y;

        value += p[104]*X4*Y3 + p[105]*X4*Z3 + p[106]*X4*Y2*Z + p[107]*X4*Z2*Y  \
          +      p[108]*Y4*X3 + p[109]*Y4*Z3 + p[110]*Y4*X2*Z + p[111]*Y4*Z2*X \
          +      p[112]*Z4*X3 + p[113]*Z4*Y3 + p[114]*Z4*X2*Y + p[115]*Z4*Y2*X;

        value += p[116]*X3*Y3*Z + p[117]*X3*Z3*Y  + p[118]*X3*Z2*Y2 \
          +      p[119]*Y3*Z3*X + p[120]*Y3*X2*Z2 \
          +      p[121]*Z3*Y2*X2;
        FALL_THROUGH;

      case POL3D_POW6:
        value += p[58]*X6 + p[59]*Y6 + p[60]*Z6;

        value += p[61]*X5*Y + p[62]*X5*Z \
          +      p[63]*Y5*X + p[64]*Y5*Z \
          +      p[65]*Z5*X + p[66]*Z5*Y;

        value += p[67]*X4*Y2 + p[68]*X4*Z2 + p[69]*X4*Y*Z \
          +      p[70]*Y4*X2 + p[71]*Y4*Z2 + p[72]*Y4*X*Z \
          +      p[73]*Z4*X2 + p[74]*Z4*Y2 + p[75]*Z4*X*Y;

        value += p[76]*X3*Y3   + p[77]*X3*Z3   + p[78]*X3*Y2*Z + p[79]*X3*Z2*Y \
          +      p[80]*Y3*Z3   + p[81]*Y3*X2*Z + p[82]*Y3*Z2*X \
          +      p[83]*Z3*X2*Y + p[84]*Z3*Y2*X;

        value += p[85]*X2*Y2*Z2;
        FALL_THROUGH;

      case POL3D_POW5:
        value += p[36]*X5 + p[37]*Y5 + p[38]*Z5;

        value += p[39]*X4*Y + p[40]*X4*Z \
          +      p[41]*Y4*X + p[42]*Y4*Z \
          +      p[43]*Z4*X + p[44]*Z4*Y;

        value += p[45]*X3*Y2 + p[46]*X3*Z2 + p[47]*X3*Y*Z \
          +      p[48]*Y3*X2 + p[49]*Y3*Z2 + p[50]*Y3*X*Z \
          +      p[51]*Z3*X2 + p[52]*Z3*Y2 + p[53]*Z3*X*Z;

        value += p[54]*X2*Y2*Z + p[55]*X2*Z2*Y \
          +      p[56]*Y2*Z2*X + p[57]*Z2*Y2*X;
        FALL_THROUGH;

      case POL3D_POW4:
        value += p[21]*X4 + p[22]*Y4 + p[23]*Z4;

        value += p[24]*X3*Y + p[25]*X3*Z \
          +      p[26]*Y3*X + p[27]*Y3*Z \
          +      p[28]*Z3*X + p[29]*Z3*Y;

        value += p[30]*X2*Y*Z + p[31]*X2*Y2 + p[32]*X2*Z2 \
          +      p[33]*Y2*X*Z + p[34]*Y2*Z2 \
          +      p[35]*Z2*Y*X;
        FALL_THROUGH;

      case POL3D_POW3:
        value += p[11]*X3 + p[12]*Y3 + p[13]*Z3;

        value += p[14]*X2*Y + p[15]*X2*Z \
          +      p[16]*Y2*X + p[17]*Y2*Z \
          +      p[18]*Z2*X + p[19]*Z2*Y;

        value += p[20]*X*Y*Z;
        FALL_THROUGH;

      case POL3D_POW2:
        value += p[5]*X2 + p[6]*Y2 + p[7]*Z2;

        value += p[8]*X*Y + p[9]*X*Z + p[10]*Y*Z;
        FALL_THROUGH;

      case POL3D_POW1:
        value += p[2]*X + p[3]*Y + p[4]*Z;
        FALL_THROUGH;

      case POL3D_POW0:
        value += p[1];
        break;
      }

      return value;
    }

  }  // end of namespace processing

}  // end of namespace snemo
