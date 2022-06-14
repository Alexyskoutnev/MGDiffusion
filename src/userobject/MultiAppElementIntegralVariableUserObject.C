#include "MultiAppElementIntegralVariableUserObject.h"
  
registerMooseObject("MooseApp", MultiAppElementIntegralVariableUserObject);

InputParameters
MultiAppElementIntegralVariableUserObject::validParams()
{
  InputParameters params = ElementIntegralUserObject::validParams();
  params.addClassDescription("computes a volume integral the inputted variable.");
  params.addRequiredParam<std::vector<VariableName>>("variable", "The name of the variable that this object operates on");
  return params;
}

MultiAppElementIntegralVariableUserObject::MultiAppElementIntegralVariableUserObject(
    const InputParameters & parameters)
  : ElementIntegralUserObject(parameters),
    MooseVariableInterfaceTransfer<Real>(this,
                                 false,
                                 "variable",
                                 Moose::VarKindType::VAR_ANY,
                                 Moose::VarFieldType::VAR_FIELD_STANDARD),
    _u(value())
{
  addMooseVariableDependency(&mooseVariableField());
}

Real
MultiAppElementIntegralVariableUserObject::computeQpIntegral()
{
  return _u[_qp];
}