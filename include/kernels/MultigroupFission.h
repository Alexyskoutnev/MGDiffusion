#pragma once

#include "ArrayKernel.h"

class MultigroupFission;

// template<>
// InputParameters validParams<MultigroupFission>();

class MultigroupFission : public ArrayKernel
{
public:
    static InputParameters validParams();
    
    MultigroupFission(const InputParameters & parameters);
    
protected:
    virtual void initQpResidual() override;
    virtual void computeQpResidual(RealEigenVector & residual) override;
    virtual RealEigenVector computeQpJacobian() override;
    virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;
    
    // Outer-product of Chi w/ NuSigmaF - get matrix of g'->g fission coefficients.
    const MaterialProperty<RealEigenMatrix> * _chi_nu_sigma_f;
    const MaterialProperty<RealEigenVector> * _chi;
    const MaterialProperty<RealEigenVector> * _nu_sigma_f;
    
    // Number of energy groups in system.
    const int _n_groups;
};

