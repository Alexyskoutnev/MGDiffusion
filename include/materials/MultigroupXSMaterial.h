#pragma once

#include "Material.h"

class MultigroupXSMaterial;

// template<>
// InputParameters validParams<MultigroupXSMaterial>();

class MultigroupXSMaterial : public Material
{
public:
    static InputParameters validParams();
    
    MultigroupXSMaterial(const InputParameters & parameters);
    
protected:
    virtual void initQpStatefulProperties() override;
    virtual void computeQpProperties() override;
    
    const RealEigenVector & _v_diffusivity;
    const RealEigenVector & _v_sigma_t;
    const RealEigenVector & _v_nu_sigma_f;
    const RealEigenVector & _v_kappa_sigma_f;
    const RealEigenVector & _v_chi;
    const RealEigenVector & _v_adf;
    const RealEigenMatrix & _v_sigma_s;
    
    MaterialProperty<RealEigenVector> & _diffusivity;
    MaterialProperty<RealEigenVector> & _sigma_t;
    MaterialProperty<RealEigenVector> & _nu_sigma_f;
    MaterialProperty<RealEigenVector> & _kappa_sigma_f;
    MaterialProperty<RealEigenVector> & _chi;
    MaterialProperty<RealEigenVector> & _adf;
    MaterialProperty<RealEigenMatrix> & _sigma_s;
    MaterialProperty<RealEigenMatrix> & _chi_nu_sigma_f;
    
};
