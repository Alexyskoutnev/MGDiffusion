#include "MaterialRealArrayValueAux.h"

#include "metaphysicl/raw_type.h"

registerMooseObject("MooseApp", MaterialRealArrayValueAux);
registerMooseObject("MooseApp", ADMaterialRealArrayValueAux);

// defineLegacyParams(MaterialRealArrayValueAux);

template <bool is_ad>
InputParameters
MaterialRealArrayValueAuxTempl<is_ad>::validParams()
{
  InputParameters params = MaterialAuxBaseTempl<RealEigenVector, is_ad>::validParams();
  params.addClassDescription(
      "Capture a component of a vector material property in an auxiliary variable.");
  params.addParam<unsigned int>("index", 0, "The array index to consider for this kernel");

  return params;
}

template <bool is_ad>
MaterialRealArrayValueAuxTempl<is_ad>::MaterialRealArrayValueAuxTempl(
    const InputParameters & parameters)
  : MaterialAuxBaseTempl<RealEigenVector, is_ad>(parameters),
    _index(this->template getParam<unsigned int>("index"))
{
    // Normally would check for index out of range error here - however, for RealEigenVector MaterialProperties, the size of the container is not assigned until after this constructor is called.
}

template <bool is_ad>
Real
MaterialRealArrayValueAuxTempl<is_ad>::getRealValue()
{
    
  if (_index > this->_prop[this->_qp].size() )
  { this->mooseError( "The provided index ", _index, " is not in array bounds for property ", this->_prop.name()); }

  return MetaPhysicL::raw_value(this->_prop[this->_qp][_index]);
    
}

template class MaterialRealArrayValueAuxTempl<false>;
template class MaterialRealArrayValueAuxTempl<true>;


