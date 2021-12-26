//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "IntegralUserObject.h"
#include "DelimitedFileReader.h"
#include "UserObjectInterface.h"


using namespace std;


VectorPostprocessorValue get_tempaerature_data(VectorPostprocessorValue & q_triple_prime_array_integral){
    double mass_flow_rate = 0.731562520642246;
    double specific_heat = 1254;
    double temp_base0 = 400;
    double r_ci = 0.00493882665122902;
    double area = (r_ci*r_ci)*3.1415;
    double h = 32500;
    double r_co =  0.00543882665122902;
    double k_c = 123;
    double k_f = 45;
    int num_data_points = q_triple_prime_array_integral.size();
    double t_coolant;
    double t_co;
    double t_ci;
    double t_f;
    VectorPostprocessorValue datapoints;
    // vector<vector<double>> data;
    for (int index = 0; index < num_data_points; index++){
        int size = q_triple_prime_array_integral.size();
        t_coolant = temp_coolant(temp_base0, mass_flow_rate, specific_heat, area, q_triple_prime_array_integral, index);
        // t_co = temp_co(t_coolant, r_ci, r_co, h, current_q_triple_prime);
        // t_ci = temp_ci(t_co, r_ci, r_co, k_c, current_q_triple_prime);
        // t_f = temp_f(t_ci, r_ci, k_f, current_q_triple_prime);
        cout << "t_coolant_z: " << t_coolant << endl;
        // cout << "t_co: " << t_co << endl;
        // cout << "t_ci: " << t_ci << endl;
        // cout << "t_f: " << t_f << endl;
        datapoints.push_back(t_coolant);
        // datapoints.push_back(t_co);
        // datapoints.push_back(t_ci);
        // datapoints.push_back(t_f);
        // data.push_back(datapoints);
    }
    return datapoints;
}


double temp_coolant(double temp_base0, double flow_rate, double specific_heat, double area, VectorPostprocessorValue & q_triple_prime_array_integral, int index){
    double temp_coolant = temp_base0 + (1/(flow_rate*specific_heat) * q_triple_prime_array_integral[index]);
    return temp_coolant;
}


// double temp_co(double temp_b, double r_ci, double r_co, double h, double q_triple_prime){
//     double temp_co = temp_b + q_triple_prime * ((r_ci*r_ci)/(2*h*r_co));
//     return temp_co;
// }

// double temp_ci(double temp_co, double r_ci, double r_co, double k_c, double q_triple_prime){
//     double temp_ci = temp_co + q_triple_prime * ((r_ci*r_ci)/(2*k_c))*log(r_co/r_ci);
//     return temp_ci;
// }

// double temp_f(double temp_ci, double r_ci, double k_f, double q_triple_prime){
//     double temp_f_val = temp_ci + q_triple_prime * ((r_ci*r_ci)/(4*k_f));
//     return temp_f_val;
// }


registerMooseObject("MGDiffusionApp", IntegralUserObject);

defineLegacyParams(IntegralUserObject);

InputParameters
IntegralUserObject::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();

  params.addRequiredParam<UserObjectName>("userobject",
                                          "The userobject whose values hold the cummulative temperature values");

  params.addClassDescription("Outputs the values of a coolant temperature along the z-axis");

  return params;
}

IntegralUserObject::IntegralUserObject(
    const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _uo_vec(declareVector(MooseUtils::shortName(parameters.get<std::string>("_object_name")))),
    _uo(getUserObject<UserObject>("userobject"))
{
  fillPoints();
  _uo_vec.resize(_points.size());
}

void
IntegralUserObject::fillPoints()
{
  _points = _uo.spatialPoints();
}

void
IntegralUserObject::initialize()
{
  _uo_vec.clear();
  
  // std::vector<Point> pt = ["0","0","2.50014"];
  // cout << _uo.spatialValue(pt) << endl;
}

void
IntegralUserObject::execute()
{
  int i = 0; 
  for (const auto & pt : _points)
    _uo_vec.push_back(_uo.spatialValue(pt));
  VectorPostprocessorValue data_holder;
  Point start_point = _points[0];
  double start_point_value = start_point(2);
  Point end_point = _points[_points.size()-1];
  double end_point_value = end_point(2);
  data_holder = get_tempaerature_data(_uo_vec);
  _uo_vec.clear();
  for (const auto val : data_holder){
    _uo_vec.push_back(val);
  }
}
