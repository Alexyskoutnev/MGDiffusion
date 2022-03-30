#include "MultigroupScattering.h"

registerMooseObject("MGDiffusionApp",MultigroupScattering);
// defineLegacyParams(MultigroupScattering);

InputParameters
MultigroupScattering::validParams()
{
    InputParameters params = ArrayKernel::validParams();
    params.addRequiredParam<MaterialPropertyName>("sigma_s",
    "Name of scattering matrix defined in Material object, should have size num_groups x num_groups.");
    
    params.addClassDescription(
                               "Multigroup scattering kernel for  ($\\Sigma_{s,g\\rightarrow g'} \\Phi_g$)");
    
    return params;
}

MultigroupScattering::MultigroupScattering(const InputParameters & parameters) :
    ArrayKernel(parameters),
    _sigma_s(hasMaterialProperty<RealEigenMatrix>("sigma_s") ? &getMaterialProperty<RealEigenMatrix>("sigma_s") : nullptr),
    _n_groups(_var.count())
{
    if (!_sigma_s)
    {
        MaterialPropertyName mat = getParam<MaterialPropertyName>("sigma_s");
        mooseError("Material property " + mat + " is of unsupported type for MultigroupScattering");
    }
}

void
MultigroupScattering::initQpResidual()
{
    mooseAssert( ((*_sigma_s)[_qp].cols() == _n_groups) && ((*_sigma_s)[_qp].rows() == _n_groups),
                  "sigma_s is of incorrect shape for a multigroup scattering matrix. Value should be a num_groups x num_groups array, with sigma_s(i,j) being the scattering coefficient from group i to group j.");
}

void
MultigroupScattering::computeQpResidual(RealEigenVector & residual)
{
    residual.noalias() = -(*_sigma_s)[_qp] * _u[_qp] * _test[_i][_qp];
}

RealEigenVector
MultigroupScattering::computeQpJacobian()
{
    return -(_phi[_j][_qp] * _test[_i][_qp] * (*_sigma_s)[_qp].diagonal());
}

RealEigenMatrix
MultigroupScattering::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
    // Off-diagonal contribution is the g'=\=g scattering coefficients.
    if (jvar.number() == _var.number())
        return -_phi[_j][_qp] * _test[_i][_qp] * (*_sigma_s)[_qp];
    else
        return ArrayKernel::computeQpOffDiagJacobian(jvar);
}


