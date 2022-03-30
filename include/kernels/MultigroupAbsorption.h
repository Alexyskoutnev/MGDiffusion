#pragma once

#include "ArrayKernel.h"

class MultigroupAbsorption;

// template<>
// InputParameters validParams<MultigroupAbsorption>();

class MultigroupAbsorption : public ArrayKernel
{
public:
    static InputParameters validParams();
    
    MultigroupAbsorption(const InputParameters & parameters);
    
protected:
    virtual void initQpResidual() override;
    virtual void computeQpResidual(RealEigenVector & residual) override;
    virtual RealEigenVector computeQpJacobian() override;
    virtual RealEigenMatrix computeQpOffDiagJacobian(const MooseVariableFEBase & jvar) override;
    
    // Total interaction coefficient, stored as num_groups x 1 array (Sigma_t for each group in material region)
    const MaterialProperty<RealEigenVector> * _sigma_t;
    
    // Number of energy groups in system.
    const int _n_groups;
};


