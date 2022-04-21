//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MultiAppElementIntegralVariableUserObject.h"
#include "MultiAppUserObjectProvider.h"

registerMooseObject("MooseApp", MultiAppElementIntegralVariableUserObject);

InputParameters
MultiAppElementIntegralVariableUserObject::validParams()
{
  InputParameters params = ElementIntegralUserObject::validParams();
  params.addClassDescription("computes a volume integral of a variable.");
  std::cout << "MultiAppElement ValidParams" << std::endl;
  params.addRequiredParam<std::vector<VariableName>>("variable", "The name of the variable that this object operates on");
  // params.addRequiredParam<MultiAppName>("multi_app", "The name of the multiapp that the user object interacts with");
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
  std::cout << "Start MultiAppLayeredIntergral" << std::endl;
  addMooseVariableDependency(&mooseVariableField());
}

Real
MultiAppElementIntegralVariableUserObject::computeQpIntegral()
{
  return _u[_qp];
}