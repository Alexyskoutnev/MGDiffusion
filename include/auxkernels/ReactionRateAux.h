#pragma once

#include "AuxKernel.h"

class ReactionRateAux;

// template <>
// InputParameters validParams<ReactionRateAux>();

/**
 * Auxiliary kernel responsible for computing reaction rates for an array scalar flux variable and array material property for cross-sections.
 */
class ReactionRateAux : public AuxKernel
{
public:
  static InputParameters validParams();

  ReactionRateAux(const InputParameters & parameters);

protected:
  virtual Real computeValue();

  /// Holds the solution value at the current quadrature points
  const ArrayVariableValue & _phi;

  /// Material Property storing cross-sections.
  const MaterialProperty<RealEigenVector> & _xs;

};
