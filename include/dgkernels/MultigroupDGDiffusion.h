#pragma once

#include "ArrayDGKernel.h"

class MultigroupDGDiffusion;

// template <>
// InputParameters validParams<MultigroupDGDiffusion>();

/**
 * Array version of DGDiffusion
 */
class MultigroupDGDiffusion : public ArrayDGKernel
{
public:
  static InputParameters validParams();

  MultigroupDGDiffusion(const InputParameters & parameters);

protected:
  virtual void initQpResidual(Moose::DGResidualType type) override;
  virtual void computeQpResidual(Moose::DGResidualType type, RealEigenVector & residual) override;
  virtual RealEigenVector computeQpJacobian(Moose::DGJacobianType type) override;
    
  Real _epsilon;
  Real _sigma;
  const MaterialProperty<RealEigenVector> & _adf;
  const MaterialProperty<RealEigenVector> & _adf_neighbor;
  const MaterialProperty<RealEigenVector> & _diff;
  const MaterialProperty<RealEigenVector> & _diff_neighbor;
  const bool _do_adf;

  RealEigenVector _res1;
  RealEigenVector _res2;
  RealEigenVector _adf_jump;
    
};
