/*
 AuxKernel for outputting a value from a Material Property of type RealEigenVector to an AuxVariable.
 */

#pragma once

// MOOSE includes
#include "MaterialAuxBase.h"

// Forward declarations
template <bool>
class MaterialRealArrayValueAuxTempl;
typedef MaterialRealArrayValueAuxTempl<false> MaterialRealArrayValueAux;
typedef MaterialRealArrayValueAuxTempl<true> ADMaterialRealArrayValueAux;

// template <>
// InputParameters validParams<MaterialRealArrayValueAux>();

/**
 * AuxKernel for outputting a RealEigenVector material property component to an AuxVariable
 */
template <bool is_ad>
class MaterialRealArrayValueAuxTempl : public MaterialAuxBaseTempl<RealEigenVector, is_ad>
{
public:
  static InputParameters validParams();

  /**
   * Class constructor
   * @param parameters The input parameters for this object
   */
  MaterialRealArrayValueAuxTempl(const InputParameters & parameters);

protected:
  virtual Real getRealValue() override;

  /// The vector component to output
  unsigned int _index;
};
