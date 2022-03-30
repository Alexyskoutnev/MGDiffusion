#include "IntegralUserObject.h"
#include "DelimitedFileReader.h"
#include "UserObjectInterface.h"


using namespace std;


std::vector<VectorPostprocessorValue> get_temperature_data(VectorPostprocessorValue & q_triple_prime_array_integral, VectorPostprocessorValue & q_triple_prime_array, double r_i, double r_o, double k_clad, double k_fuel, double cp_clad, double cp_fuel, double cp_coolant, double h_cool, double t_cool_in, double m_dot, double n_rod, double power, double E_d_E_r_Ratio, double reactor_height){
    double mass_flow_rate = m_dot;
    double temp_base0 = t_cool_in;
    double r_ci = r_i;
    double area = (r_i*r_i)*3.1415;
    double h = h_cool;
    double r_co =  r_o;
    double k_c = k_clad;
    double k_f = k_fuel;
    double c_p_clad = cp_clad;
    double c_p_fuel = cp_fuel;
    double c_p_coolant = cp_coolant;
    int num_data_points = q_triple_prime_array_integral.size();
    cout << "the num data points" << num_data_points << endl;
    double t_coolant;
    double t_co;
    double t_ci;
    double t_f;
    vector<VectorPostprocessorValue> data;
    double q_triple_prime;
    for (int index = 0; index < num_data_points; index++){
        VectorPostprocessorValue datapoints;
        int size = q_triple_prime_array_integral.size();
        t_coolant = temp_coolant(temp_base0, mass_flow_rate, c_p_coolant, area, q_triple_prime_array_integral, index);
        //not 100% if this is the right formulation for q_triple_prime
        q_triple_prime = ((q_triple_prime_array[index]/(reactor_height*r_i*r_i) * power * E_d_E_r_Ratio)/(n_rod));
        t_co = temp_co(t_coolant, r_ci, r_co, h, q_triple_prime, index);
        t_ci = temp_ci(t_co, r_ci, r_co, k_c, q_triple_prime, index);
        t_f = temp_f(t_ci, r_ci, k_f, q_triple_prime, index);
        cout << "t_coolant_z: " << t_coolant << endl;
        cout << "t_co: " << t_co << endl;
        cout << "t_ci: " << t_ci << endl;
        cout << "t_f: " << t_f << endl;
        cout << "q_triple_prime: " << q_triple_prime << endl;
        datapoints.push_back(t_coolant);
        datapoints.push_back(t_co);
        datapoints.push_back(t_ci);
        datapoints.push_back(t_f);
        data.push_back(datapoints);
    }
    return data;
}


double temp_coolant(double temp_base0, double flow_rate, double c_p_coolant, double area, VectorPostprocessorValue & q_triple_prime_array_integral, int index){
    double temp_coolant = temp_base0 + (1/(flow_rate*c_p_coolant) * area * q_triple_prime_array_integral[index]);
    return temp_coolant;
}


double temp_co(double temp_b, double r_ci, double r_co, double h, double q_triple_prime, int index){
    double temp_co = temp_b + q_triple_prime * ((r_ci*r_ci)/(2*h*r_co));
    return temp_co;
}

double temp_ci(double temp_co, double r_ci, double r_co, double k_c, double q_triple_prime, int index){
    double temp_ci = temp_co + q_triple_prime * ((r_ci*r_ci)/(2*k_c))*log(r_co/r_ci);
    return temp_ci;
}

double temp_f(double temp_ci, double r_ci, double k_f, double q_triple_prime, int index){
    double temp_f_val = temp_ci + q_triple_prime * ((r_ci*r_ci)/(4*k_f));
    return temp_f_val;
}


registerMooseObject("MGDiffusionApp", IntegralUserObject);

// defineLegacyParams(IntegralUserObject);

InputParameters
IntegralUserObject::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();

  params.addRequiredParam<UserObjectName>("intergated_temperature_uo",
                                          "The userobject whose values hold the cummulative temperature values");
  params.addRequiredParam<UserObjectName>("temperatures_uo",
                                          "The userobject whose values hold the cummulative temperature values");
  // params.addRequiredCoupledVar("kappa_fission_phi", "q_triple_prime");
  params.addRequiredParam<double>("r_i","inner clad radius");
  params.addRequiredParam<double>("r_o","outer clad radius");
  params.addRequiredParam<double>("k_clad","outer clad radius");
  params.addRequiredParam<double>("k_fuel","outer clad radius");
  params.addRequiredParam<double>("c_p_clad","outer clad radius");
  params.addRequiredParam<double>("c_p_fuel","outer clad radius");
  params.addRequiredParam<double>("c_p_coolant","heat capacity of coolant");
  params.addRequiredParam<double>("h_cool","heat transfer coefficent");
  params.addRequiredParam<double>("t_cool_in","inlet coolant temperature");
  params.addRequiredParam<double>("m_dot","mass flow rate");
  params.addRequiredParam<double>("n_rod","number of rods");
  params.addRequiredParam<double>("power","reactor power output");
  params.addRequiredParam<double>("E_d_E_r_Ratio","E_d and E_r ratio");
  params.addRequiredParam<double>("reactor_height","reactor height");




  params.addClassDescription("Outputs the values of a coolant temperature along the z-axis");

  return params;
}

IntegralUserObject::IntegralUserObject(
    const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _uo_vec(declareVector(MooseUtils::shortName(parameters.get<std::string>("_object_name")))),
    _uo_vec1(declareVector("q_triple_prime_vector")),
    _uo_vec2(declareVector("temperature_clad_outer")),
    _uo_vec3(declareVector("temperature_clad_inner")),
    _uo_vec4(declareVector("temperature_fuel")),
    _integral_uo(getUserObject<UserObject>("intergated_temperature_uo")),
    _spatial_uo(getUserObject<UserObject>("temperatures_uo")),
    _r_i(getParam<double>("r_i")),
    _r_o(getParam<double>("r_o")),
    _k_clad(getParam<double>("k_clad")),
    _k_fuel(getParam<double>("k_fuel")),
    _c_p_clad(getParam<double>("c_p_clad")),
    _c_p_fuel(getParam<double>("c_p_fuel")),
    _c_p_coolant(getParam<double>("c_p_coolant")),
    _h_cool(getParam<double>("h_cool")),
    _t_cool_in(getParam<double>("t_cool_in")),
    _m_dot(getParam<double>("m_dot")),
    _n_rod(getParam<double>("n_rod")),
    _power(getParam<double>("power")),
    _E_d_E_r_Ratio(getParam<double>("E_d_E_r_Ratio")),
    _reactor_height(getParam<double>("reactor_height"))
{
  fillPoints();
  _uo_vec.resize(_points.size());
}

void
IntegralUserObject::fillPoints()
{
  _points = _integral_uo.spatialPoints();
  _points_q_triple_prime = _spatial_uo.spatialPoints();
}

void
IntegralUserObject::initialize()
{
  _uo_vec.clear();
  _uo_vec1.clear();
  _uo_vec2.clear();
  _uo_vec3.clear();
  _uo_vec4.clear();
}

void
IntegralUserObject::execute()
{
  int i = 0; 
  for (const auto & pt : _points){
    _uo_vec.push_back(_integral_uo.spatialValue(pt));
    _uo_vec1.push_back(_spatial_uo.spatialValue(pt));
  }
  vector<VectorPostprocessorValue> data_holder;
  data_holder = get_temperature_data(_uo_vec, _uo_vec1, _r_i, _r_o, _k_clad, _k_fuel, _c_p_fuel, _c_p_fuel, _c_p_coolant, _h_cool, _t_cool_in, _m_dot, _n_rod, _power, _E_d_E_r_Ratio, _reactor_height);
  _uo_vec.clear();
  for (const auto val : data_holder){
    _uo_vec.push_back(val[0]);
    _uo_vec2.push_back(val[1]);
    _uo_vec3.push_back(val[2]);
    _uo_vec4.push_back(val[3]);
  }

}
