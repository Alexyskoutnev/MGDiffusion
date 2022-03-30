#include "MultigroupXSMaterial.h"

registerMooseObject("MGDiffusionApp",MultigroupXSMaterial);
// defineLegacyParams(MultigroupXSMaterial);

InputParameters
MultigroupXSMaterial::validParams()
{
    InputParameters params = Material::validParams();
    
    params.addRequiredParam<RealEigenVector>("diffusivity","Diffusion coefficient for each energy group in this material");
    params.addRequiredParam<RealEigenVector>("sigma_t","Total cross section for each energy group in this material");
    params.addRequiredParam<RealEigenVector>("nu_sigma_f","Fission cross section (multiplied by nubar) for each energy group in this material");
    params.addRequiredParam<RealEigenVector>("kappa_sigma_f","Fission energy production coefficient for each energy group in this material");
    params.addRequiredParam<RealEigenVector>("chi","Fission yield for each energy group in this material");
    params.addRequiredParam<RealEigenVector>("adf",
                                     "Assembly discontinuity factor for each energy group in this material region. Default to 1.0.");
    
    params.addRequiredParam<RealEigenMatrix>("sigma_s","Scattering matrix for this material, given as tensor");
    
    params.addClassDescription("Declares a material containing essential nuclear data for multigroup neutron diffusion (cross-sections, yield, etc.)");
    params.set<MooseEnum>("constant_on") = "SUBDOMAIN";
    
    return params;
}

MultigroupXSMaterial::MultigroupXSMaterial(const InputParameters & parameters) :
    Material(parameters),
    _v_diffusivity(     getParam<RealEigenVector>("diffusivity")),
    _v_sigma_t(         getParam<RealEigenVector>("sigma_t")),
    _v_nu_sigma_f(      getParam<RealEigenVector>("nu_sigma_f")),
    _v_kappa_sigma_f(   getParam<RealEigenVector>("kappa_sigma_f")),
    _v_chi(             getParam<RealEigenVector>("chi")),
    _v_adf(             getParam<RealEigenVector>("adf")),
    _v_sigma_s(         getParam<RealEigenMatrix>("sigma_s")),
    _diffusivity(       declareProperty<RealEigenVector>("diffusivity")),
    _sigma_t(           declareProperty<RealEigenVector>("sigma_t")),
    _nu_sigma_f(        declareProperty<RealEigenVector>("nu_sigma_f")),
    _kappa_sigma_f(     declareProperty<RealEigenVector>("kappa_sigma_f")),
    _chi(               declareProperty<RealEigenVector>("chi")),
    _adf(               declareProperty<RealEigenVector>("adf")),
    _sigma_s(           declareProperty<RealEigenMatrix>("sigma_s")),
    _chi_nu_sigma_f(    declareProperty<RealEigenMatrix>("chi_nu_sigma_f"))
{
    
    // Add checking for mis-matched input data dimensions.
    const int _n_groups = _v_diffusivity.size();
    
    mooseAssert( (&_v_sigma_t).size() == _n_groups, "Input for sigma_t does not match number of energy groups");
    mooseAssert( (&_v_nu_sigma_f).size() == _n_groups, "Input for nu_sigma_f does not match number of energy groups");
    mooseAssert( (&_v_kappa_sigma_f).size() == _n_groups, "Input for kappa_sigma_f does not match number of energy groups");
    mooseAssert( (&_v_chi).size() == _n_groups, "Input for chi does not match number of energy groups");
    mooseAssert( (&_v_adf).size() == _n_groups, "Input for adf does not match number of energy groups");
    mooseAssert( (&_v_sigma_s).rows() == _n_groups && (&_v_sigma_s).cols() == _n_groups, "Input for sigma_s does not match number of energy groups");

}

void
MultigroupXSMaterial::initQpStatefulProperties()
{
    computeQpProperties();
}
void
MultigroupXSMaterial::computeQpProperties()
{
    _diffusivity[_qp]       = _v_diffusivity;
    _sigma_t[_qp]           = _v_sigma_t;
    _nu_sigma_f[_qp]        = _v_nu_sigma_f;
    _kappa_sigma_f[_qp]     = _v_kappa_sigma_f;
    _chi[_qp]               = _v_chi;
    _adf[_qp]               = _v_adf;
    _sigma_s[_qp]           = _v_sigma_s;
    _chi_nu_sigma_f[_qp]    = _v_chi * _v_nu_sigma_f.transpose();
    
}
