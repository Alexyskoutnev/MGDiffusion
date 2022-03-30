#include "MultigroupAbsorption.h"

registerMooseObject("MGDiffusionApp",MultigroupAbsorption);
// defineLegacyParams(MultigroupAbsorption);

InputParameters
MultigroupAbsorption::validParams()
{
    InputParameters params = ArrayKernel::validParams();
    params.addRequiredParam<MaterialPropertyName>("sigma_t",
    "Name of absorption cross-section used in Material object, should have size num_groups x 1.");
    
    params.addClassDescription(
                               "Multigroup Absorption kernel for  ($\\Sigma_{a,g} \\Phi_g$)");
    
    return params;
}

MultigroupAbsorption::MultigroupAbsorption(const InputParameters & parameters) :
    ArrayKernel(parameters),
    _sigma_t(hasMaterialProperty<RealEigenVector>("sigma_t") ? &getMaterialProperty<RealEigenVector>("sigma_t") : nullptr),
    _n_groups(_var.count())
{
    if (!_sigma_t)
    {
        MaterialPropertyName mat = getParam<MaterialPropertyName>("sigma_t");
        mooseError("Material property " + mat + " is of unsupported type for MultigroupAbsorption");
    }
}

void
MultigroupAbsorption::initQpResidual()
{
    mooseAssert( (*_sigma_t)[_qp].size() == _n_groups,
                  "sigma_t is of incorrect shape for a multigroup cross-section. Value should be a num_groups x 1 array, with a total interaction coefficient for each energy group flux in the material.");
}

void
MultigroupAbsorption::computeQpResidual(RealEigenVector & residual)
{
    residual.noalias() = (*_sigma_t)[_qp].asDiagonal() * _u[_qp] * _test[_i][_qp];
}

RealEigenVector
MultigroupAbsorption::computeQpJacobian()
{
    return _phi[_j][_qp] * _test[_i][_qp] * (*_sigma_t)[_qp];
}

RealEigenMatrix
MultigroupAbsorption::computeQpOffDiagJacobian(const MooseVariableFEBase & jvar)
{
    // Absorption has no offdiag component - return ArrayKernel::..., which sets it to 0.
    return ArrayKernel::computeQpOffDiagJacobian(jvar);
}
            
