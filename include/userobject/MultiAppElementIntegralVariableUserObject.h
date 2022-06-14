#pragma once

#include "ElementIntegralUserObject.h"
#include "MooseVariableInterfaceTransfer.h"

class MultiAppElementIntegralVariableUserObject : public ElementIntegralUserObject,
                                          public MooseVariableInterfaceTransfer<Real>
{
public:
  static InputParameters validParams();

  MultiAppElementIntegralVariableUserObject(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;
  /// Holds the solution at current quadrature points
  const VariableValue & _u;
};