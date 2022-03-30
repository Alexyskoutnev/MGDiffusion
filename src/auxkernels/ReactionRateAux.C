#include "ReactionRateAux.h"
#include "Assembly.h"

registerMooseObject("MooseApp", ReactionRateAux);

// defineLegacyParams(ReactionRateAux);

InputParameters
ReactionRateAux::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Compute reaction rate for array flux variable with array cross-section, $RR=Sigma_x u$");
  params.addRequiredCoupledVar("scalar_flux_variable", "The name of the variable");
  params.addRequiredParam<MaterialPropertyName>(
      "cross_section",
      "The name of the cross section material property used in computation.");
  return params;
}

ReactionRateAux::ReactionRateAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _phi(coupledArrayValue("scalar_flux_variable")),
    _xs(getMaterialProperty<RealEigenVector>("cross_section"))
{
    
}

Real
ReactionRateAux::computeValue()
{
    
    RealEigenVector rr_by_group = _xs[_qp].asDiagonal() * _phi[_qp];
    Real rr = 0;
    
    for (int i = 0; i < rr_by_group.size(); i++ ){
        rr = rr+rr_by_group[i];
    }
    
    return rr;
    
}
