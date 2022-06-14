#pragma once

#include "GeneralVectorPostprocessor.h"

class IntegralUserObject;

class IntegralUserObject : public GeneralVectorPostprocessor
{
public:

  static InputParameters validParams();
  IntegralUserObject(const InputParameters & parameters);

  virtual void initialize() override;
  virtual void execute() override;
  void fillPoints();

protected:
  /// The VectorPostprocessorValue object where the results are stored
  VectorPostprocessorValue & _uo_vec;
  VectorPostprocessorValue & _uo_vec1;
  VectorPostprocessorValue & _uo_vec2;
  VectorPostprocessorValue & _uo_vec3;
  VectorPostprocessorValue & _uo_vec4;

  const UserObject & _integral_uo;
  const UserObject & _spatial_uo;
  
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

  std::vector<Point> _points;
  std::vector<Point> _points_q_triple_prime;

};

//Helper Functions to find analytic heat solutions
std::vector<VectorPostprocessorValue> get_temperature_data(VectorPostprocessorValue &, VectorPostprocessorValue &, double, double, double, double, double, double, double, double, double, double, double, double, double, double);
double temp_coolant(double, double, double, double, VectorPostprocessorValue &, int);
double temp_co(double, double, double, double, double, int);
double temp_ci(double, double, double, double, double, int);
double temp_f(double, double, double, double, int);

