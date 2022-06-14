#pragma once

#include "ElementIntegralVariableUserObject.h"
#include "LayeredBase.h"
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "MooseVariableFieldBase.h"
#include "MooseVariableFE.h"

/**
 * This UserObject computes volume integrals of a variable storing partial sums for the specified
 * number of intervals in a direction (x,y,z).
 */
class MutliAppLayeredIntegral : public ElementIntegralUserObject, public LayeredBase
{
public:
  static InputParameters validParams();
  MutliAppLayeredIntegral(const InputParameters & parameters);

  virtual Real spatialValue(const Point & p) const override { return integralValue(p); }
  virtual const std::vector<Point> spatialPoints() const override;
  virtual void initialize() override;
  virtual void execute() override;
  virtual void finalize() override;
  virtual void threadJoin(const UserObject & y) override;
  void setVarPointer(MooseVariableFieldBase * ptr);
  VariableValue _u;

protected:
  MooseVariableFieldBase * _multi_app_var;
  MooseVariableFE<Real> * _variable = nullptr;
  virtual Real computeQpIntegral() override;
};