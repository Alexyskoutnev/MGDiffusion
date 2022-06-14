#include "MultiAppLayeredIntegral.h"
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "MooseVariableFEBase.h"


#include "libmesh/mesh_tools.h"

registerMooseObject("MooseApp", MutliAppLayeredIntegral);

InputParameters
MutliAppLayeredIntegral::validParams()
{
  InputParameters params = ElementIntegralUserObject::validParams();
  params += LayeredBase::validParams();
  params.addClassDescription("Compute variable integrals over layers.");
  return params;
}

MutliAppLayeredIntegral::MutliAppLayeredIntegral(const InputParameters & parameters)
  : ElementIntegralUserObject(parameters), LayeredBase(parameters)
{
  std::cout << "Start MutliAppLayeredIntegral" << std::endl;
}

void
MutliAppLayeredIntegral::initialize()
{
  ElementIntegralUserObject::initialize();
  LayeredBase::initialize();
}

void
MutliAppLayeredIntegral::execute()
{
  unsigned int varSize = _variable->sln().size();
  _u.resize(varSize);
  _u = _variable->sln();  //Need to double check this formulation
  //################## BUG ##############################
  //_u is only recieving zero value instead of actual computed values from the main app)
  std::cout << "val 1: " << _u[0] << std::endl;
  std::cout << "val 2: " << _u[1] << std::endl;
  std::cout << "val 3: " << _u[2] << std::endl;
  std::cout << "val 4: " << _u[3] << std::endl;
  Real integral_value = computeIntegral(); //Integral value is zero when test (not supposed to be that)
  unsigned int layer = getLayer(_current_elem->vertex_average());
  setLayerValue(layer, getLayerValue(layer) + integral_value);
}

void
MutliAppLayeredIntegral::finalize()
{
  LayeredBase::finalize();
}

void
MutliAppLayeredIntegral::threadJoin(const UserObject & y)
{
  ElementIntegralUserObject::threadJoin(y); //Change to ElementIntegralUserObject 
  LayeredBase::threadJoin(y);
}

const std::vector<Point>
MutliAppLayeredIntegral::spatialPoints() const
{
  std::vector<Point> points;


  for (const auto & l : _layer_centers)
  {
    Point pt(0.0, 0.0, 0.0);
    pt(_direction) = l;
    points.push_back(pt);
  }

  return points;
}

void 
MutliAppLayeredIntegral::setVarPointer(MooseVariableFieldBase * ptr){
  _multi_app_var = ptr;
  _variable = dynamic_cast<MooseVariableFE<Real> *>(_multi_app_var);
}
Real
MutliAppLayeredIntegral::computeQpIntegral(){ 
  return _u[_qp];
}