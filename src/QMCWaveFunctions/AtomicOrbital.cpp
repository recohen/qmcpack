#include "AtomicOrbital.h"

namespace qmcplusplus {

  template<> void   
  AtomicOrbital<complex<double> >::SetBand (int band, Array<complex<double>,2> &spline_data,
					    Array<complex<double>,2> &poly_coefs,
					    PosType twist)
  {
    vector<complex<double> > one_spline(SplinePoints);
    for (int lm=0; lm<Numlm; lm++) {
      int index = band*Numlm + lm;
      for (int i=0; i<SplinePoints; i++)
	one_spline[i] = spline_data(i, lm);
      set_multi_UBspline_1d_z (RadialSpline, index, &one_spline[0]);
      for (int n=0; n<=PolyOrder; n++)
	PolyCoefs(n,band,lm) = poly_coefs (n,lm);
    }
    TwistAngles[band] = twist;
  }


  // Here, we convert the complex Ylm representation to the real Ylm representation
  template<> void   
  AtomicOrbital<double>::SetBand (int band, Array<complex<double>,2> &spline_data,
				  Array<complex<double>,2> &poly_coefs,
				  PosType twist)
  {
    vector<double> one_spline(SplinePoints);
    
    for (int l=0; l<=lMax; l++) {
      double minus_1_to_m = 1.0;

      // Set spline for m=0
      for (int i=0; i<SplinePoints; i++)
	one_spline[i] = spline_data(i, l*(l+1)).real();
      int index = band*Numlm + l*(l+1);
      set_multi_UBspline_1d_d (RadialSpline, index, &one_spline[0]);
      
      // Set poly ofr m=0
      for (int n=0; n<=PolyOrder; n++) 
	PolyCoefs(n,band,l*(l+1)) = poly_coefs (n,l*(l+1)).real();
      
      // Set spline and poly for |m| > 0
      for (int m=1; m<=l; m++) {
	int lmp = l*(l+1) + m;
	int lmm = l*(l+1) - m;
	index = band*Numlm + lmp;
	for (int i=0; i<SplinePoints; i++)
	  one_spline[i] = (spline_data(i, lmp).real() + 
			   minus_1_to_m * spline_data(i, lmm).real());
	set_multi_UBspline_1d_d (RadialSpline, index, &one_spline[0]);

	index = band*Numlm + lmm;
	for (int i=0; i<SplinePoints; i++)
	  one_spline[i] = 0.5*(-spline_data(i, lmp).imag() + 
			       minus_1_to_m * spline_data(i, lmm).imag());
	set_multi_UBspline_1d_d (RadialSpline, index, &one_spline[0]);
	

	for (int n=0; n<=PolyOrder; n++) {
	  PolyCoefs(n,band,lmp) = (poly_coefs (n,lmp).real() +
				   minus_1_to_m * poly_coefs(n,lmm).real());
	  PolyCoefs(n,band,lmm) = (-poly_coefs (n,lmp).imag() +
				   minus_1_to_m * poly_coefs(n,lmm).imag());
	}
	minus_1_to_m *= -1.0;
      }
    }
    TwistAngles[band] = twist;
  }


}
