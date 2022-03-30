#include "MultigroupFission.h"

registerMooseObject("MGDiffusionApp",MultigroupFission);
// defineLegacyParams(MultigroupFission);

InputParameters
MultigroupFission::validParams()
{
    InputParameters params = ArrayKernel::validParams();
    
    params.addRequiredParam<MaterialPropertyName>("nu_sigma_f",
    "Name of fission cross-section defined in Material object, should have size num_groups x 1.");
    
    params.addRequiredParam<MaterialPropertyName>("chi","Name of fission yield (chi) defined in Material object, should have size num_groups x 1.");
    
    params.addClassDescription(
                               "Multigroup fission kernel for  ($ \\frac{1}{k} \\chi_g \\nu\\Sigma_{f,g} \\Phi_g$)");
    
    return params;
}

MultigroupFission::MultigroupFission(const InputParameters & parameters) :
    ArrayKernel(parameters),
    _chi_nu_sigma_f(hasMaterialProperty<RealEigenMatrix>("chi_nu_sigma_f") ? &getMaterialProperty<RealEigenMatrix>("chi_nu_sigma_f") : nullptr),
    _chi(hasMaterialProperty<RealEigenVector>("chi") ? &getMaterialProperty<RealEigenVector>("chi") : nullptr),
    _nu_sigma_f(hasMaterialProperty<RealEigenVector>("nu_sigma_f") ? &getMaterialProperty<RealEigenVector>("nu_sigma_f") : nullptr),
    _n_groups(_var.count())
{
    if (!_nu_sigma_f)
    {
        MaterialPropertyName mat = getParam<MaterialPropertyName>("nu_sigma_f");
        mooseError("Material property " + mat + " is of unsupported type for MultigroupFission");
    }
    
    if (!_chi)
    {
        MaterialPropertyName mat = getParam<MaterialPropertyName>("chi");
        mooseError("Material property " + mat + " is of unsupported type for MultigroupFission");
    }
    
    if (!_chi_nu_sigma_f)
    {
        mooseError("Material object is not configured to precompute chi_nu_sigma_f, as required by MultigroupFission.");
    }
}

void
MultigroupFission::initQpResidual()
{
    mooseAssert( (*_nu_sigma_f)[_qp].size() == _n_groups,
                  "nu_sigma_f is of incorrect shape for a multigroup fission cross-section. Value should be a num_groups x 1 array.");
    mooseAssert( (*_chi)[_qp].size() == _n_groups,
                  "chi is of incorrect shape for yield coefficients. Value should be a num_groups x 1 array.");
    mooseAssert( (*_chi_nu_sigma_f)[_qp].rows() == _n_groups && (*_chi_nu_sigma_f)[_qp].cols() == _n_groups,
                "Precomputed chi_nu_sigma_f is of incorrect shape, should be num_groups x num_groups");
}

void
MultigroupFission::computeQpResidual(RealEigenVector & residual)
{
    
    residual.noalias() = -(*_chi_nu_sigma_f)[_qp] * _u[_qp] * _test[_i][_qp];
}

RealEigenVector
MultigroupFission::computeQpJacobian()
{
    return 0 * (_phi[_j][_qp] * _test[_i][_qp] * (*_chi_nu_sigma_f)[_qp].diagonal());
}

RealEigenMatrix
MultigroupFission::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
    // Off-diagonal contribution is the g'=\=g Fission coefficients.
    if (jvar.number() == _var.number())
        return 0 * _phi[_j][_qp] * _test[_i][_qp] * (*_chi_nu_sigma_f)[_qp];
    else
        return ArrayKernel::computeQpOffDiagJacobian(jvar);
}


