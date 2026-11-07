/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//             LOBACHEVSKY STATE UNIVERSITY OF NIZHNY NOVGOROD             //
//                                                                         //
//                       Copyright (c) 2015 by UNN.                        //
//                          All Rights Reserved.                           //
//                                                                         //
//  File:      evolvent.h                                                  //
//                                                                         //
//  Purpose:   Header file for evolvent classes                            //
//                                                                         //
//  Author(s): Barkalov K., Sysoyev A.                                     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef __EVOLVENT_H__
#define __EVOLVENT_H__

const int MaxDim = 5;

// ------------------------------------------------------------------------------------------------
class TEvolvent
{
protected:
  int      m;             // accuracy of decomposition of hypercube
  int      N;             // dimension
  double   A[MaxDim];     // left and
  double   B[MaxDim];     // right bounds of search area

  const int MaxM = 10;

  double*  y;             // y point from hypercube [-1/2, 1/2]^N

  const double extNull; // = 0.0;
  const double extOne;  // = 1.0;
  const double extHalf; // = 0.5;
  double nexpExtended;

  void CalculateNumbr(double *s, int *u, int *v, int *l);
  void CalculateNode(double is, int n, int *u, int *v, int *l);
  void transform_P_to_D(); // transformation from hypercube P to hyperinterval D
  void transform_D_to_P(); // transformation from hyperinterval D to hypercube P
  double* GetYOnX(const double& _x);
  double GetXOnY();
public:
  TEvolvent(int _N = 2, int _m = 10);
  virtual ~TEvolvent();
  void SetBounds(const double* _A, const double* _B);
  virtual void GetImage(const double& x, double* _y, int EvolventNum = 0);
  virtual void GetPreimages(double* _y, double *x);
};

#endif
// - end of file ----------------------------------------------------------------------------------
