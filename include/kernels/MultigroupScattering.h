#pragma once

#include "ArrayKernel.h"

class MultigroupScattering;

// template<>
// InputParameters validParams<MultigroupScattering>();

class MultigroupScattering : public ArrayKernel
{
public:
    static InputParameters validParams();
    
    MultigroupScattering(const InputParameters & parameters);
    
protected:
    virtual void initQpResidual() override;
    virtual void computeQpResidual(RealEigenVector & residual) override;
    virtual RealEigenVector computeQpJacobian() override;
    virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;
    
    // Scattering coefficient, stored as num_groups x num_groups array (Compute group->group scattering)
    const MaterialProperty<RealEigenMatrix> * _sigma_s;
    
    // Number of energy groups in system.
    const int _n_groups;
};

