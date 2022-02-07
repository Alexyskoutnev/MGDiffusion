//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "GeneralVectorPostprocessor.h"

class IntegralUserObject;

template <>
InputParameters validParams<IntegralUserObject>();

/**
 *  SpatialUserObjectVectorPostprocessor is a type of VectorPostprocessor that outputs the
 *  values of a spatial user object as a vector in the order of the provided points.
 */
class IntegralUserObject : public GeneralVectorPostprocessor
{
public:

  


  static InputParameters validParams();

  /**
   * Class constructor
   * @param parameters The input parameters
   */
  IntegralUserObject(const InputParameters & parameters);

  /**
   * Initialize, clears the postprocessor vector
   */
  virtual void initialize() override;

  /**
   * Populates the postprocessor vector of values with the userobject evaluations in space
   */
  virtual void execute() override;

  /**
   * Read the points at which to evaluate from a vector ('points'), a file ('points_file'),
   * or neither (which will read from the user object directly if it satisfies the
   * spatialPoints interface)
   */
  void fillPoints();

protected:
  /// The VectorPostprocessorValue object where the results are stored
  VectorPostprocessorValue & _uo_vec;
  VectorPostprocessorValue & _uo_vec1;
  VectorPostprocessorValue & _uo_vec2;
  VectorPostprocessorValue & _uo_vec3;
  VectorPostprocessorValue & _uo_vec4;

  
  // VectorPostprocessorValue & _uo_vec;

  /// Userobject to evaluate spatially
  const UserObject & _integral_uo;
  const UserObject & _spatial_uo;
  

  // const VariableValue & _kappa_fission_phi;

  const double _r_i;
  const double _r_o;
  const double _k_clad;
  const double _k_fuel;
  const double _c_p_clad;
  const double _c_p_fuel;
  const double _c_p_coolant;
  const double _h_cool;
  const double _t_cool_in;
  const double _m_dot;
  const double _n_rod;
  const double _power;
  const double _E_d_E_r_Ratio;
  const double _reactor_height;



  /// Points at which to evaluate the user object
  std::vector<Point> _points;
  std::vector<Point> _points_q_triple_prime;

};


std::vector<VectorPostprocessorValue> get_tempaerature_data(VectorPostprocessorValue &, VectorPostprocessorValue &, double, double, double, double, double, double, double, double, double, double, double, double, double, double);

double temp_coolant(double, double, double, double, VectorPostprocessorValue &, int);

double temp_co(double, double, double, double, double, int);
double temp_ci(double, double, double, double, double, int);
double temp_f(double, double, double, double, int);

