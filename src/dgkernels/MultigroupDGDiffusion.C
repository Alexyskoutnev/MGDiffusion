#include "MultigroupDGDiffusion.h"

// MOOSE includes
#include "MooseVariableFE.h"

#include "libmesh/utility.h"

registerMooseObject("MooseApp", MultigroupDGDiffusion);

// defineLegacyParams(MultigroupDGDiffusion);

InputParameters
MultigroupDGDiffusion::validParams()
{
  InputParameters params = ArrayDGKernel::validParams();
  params.addParam<bool>("use_adf",false,"Set to true to use assembly discontinuity factors in the IIP DG formulation (see 'Use of discontinuity factors in high-order finite element methods', Annals of Nuclear Energy 87 (2016) 728-738 for kernel definitions)");
  params.addParam<Real>("sigma", 4, "Sigma in generic IIP method");
  params.addParam<Real>("epsilon", 1, "Epsilon in generic IIP method");
  params.addClassDescription("Implements interior penalty method for array multigroup diffusion equations.");
  return params;
}

MultigroupDGDiffusion::MultigroupDGDiffusion(const InputParameters & parameters)
  : ArrayDGKernel(parameters),
    _epsilon(getParam<Real>("epsilon")),
    _sigma(getParam<Real>("sigma")),
    _adf(getMaterialProperty<RealEigenVector>("adf")),
    _adf_neighbor(getNeighborMaterialProperty<RealEigenVector>("adf")),
    _diff(getMaterialProperty<RealEigenVector>("diffusivity")),
    _diff_neighbor(getNeighborMaterialProperty<RealEigenVector>("diffusivity")),
    _do_adf(getParam<bool>("use_adf")),
    _res1(_count),
    _res2(_count),
    _adf_jump(_count)
{
    if (_do_adf)
    {
        // It is inappropriate to use symmetrization in IP formulation using ADF's, as the jump is asymmetric. Epsilon should be set to zero in this case.
        
        _epsilon = 0;
        
    }
}

void
MultigroupDGDiffusion::initQpResidual(Moose::DGResidualType type)
{
    
    mooseAssert(_diff[_qp].size() == _count && _diff_neighbor[_qp].size() == _count,
                  "'diff' size is inconsistent with the number of components of array "
                  "variable");
    mooseAssert(_adf[_qp].size() == _count,"'adf' size is inconsistent with the number of components of array variable.");
    const unsigned int elem_b_order = _var.order();
    const Real h_elem =
          _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);
    
    _res1.noalias() = _diff[_qp].asDiagonal() * _grad_u[_qp] * _array_normals[_qp];
    _res1.noalias() += _diff_neighbor[_qp].asDiagonal() * _grad_u_neighbor[_qp] * _array_normals[_qp];
    _res1 *= 0.5;
    
    if (_do_adf) { // Using Disc Factor formulation
        _adf_jump.noalias() = _adf_neighbor[_qp].asDiagonal() * _u_neighbor[_qp] - _adf[_qp].asDiagonal() * _u[_qp];
        _res1 += _adf_jump * _sigma / h_elem;
    }
    
    else { // doing generic IIP
      _res1 -= (_u[_qp] - _u_neighbor[_qp]) * _sigma / h_elem;
    }
    
    switch (type)
      {
        case Moose::Element:
          _res2.noalias() = _diff[_qp].asDiagonal() * (_u[_qp] - _u_neighbor[_qp]) * _epsilon * 0.5;
          break;

        case Moose::Neighbor:
          _res2.noalias() =
              _diff_neighbor[_qp].asDiagonal() * (_u[_qp] - _u_neighbor[_qp]) * _epsilon * 0.5;
          break;
              
      }
    
}

void
MultigroupDGDiffusion::computeQpResidual(Moose::DGResidualType type, RealEigenVector & residual)
{
  switch (type)
  {
    case Moose::Element:
      residual = -_res1 * _test[_i][_qp] + _res2 * (_grad_test[_i][_qp] * _normals[_qp]);
      break;

    case Moose::Neighbor:
      residual =
          _res1 * _test_neighbor[_i][_qp] + _res2 * (_grad_test_neighbor[_i][_qp] * _normals[_qp]);
      break;
  }
}

RealEigenVector
MultigroupDGDiffusion::computeQpJacobian(Moose::DGJacobianType type)
{
    RealEigenVector r = RealEigenVector::Zero(_count);

    const unsigned int elem_b_order = _var.order();
    const double h_elem =
      _current_elem_volume / _current_side_volume * 1. / Utility::pow<2>(elem_b_order);
    
      switch (type)
      {
        case Moose::ElementElement:
          r -= _grad_phi[_j][_qp] * _normals[_qp] * _test[_i][_qp] * 0.5 * _diff[_qp];
          r += _grad_test[_i][_qp] * _normals[_qp] * _epsilon * 0.5 * _phi[_j][_qp] * _diff[_qp];
          r += RealEigenVector::Constant(_count, _sigma / h_elem * _phi[_j][_qp] * _test[_i][_qp]);
          break;

        case Moose::ElementNeighbor:
          r -= _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test[_i][_qp] * 0.5 * _diff_neighbor[_qp];
          r -= _grad_test[_i][_qp] * _normals[_qp] * _epsilon * 0.5 * _phi_neighbor[_j][_qp] * _diff[_qp];
              
          r -= RealEigenVector::Constant(_count,
                                         _sigma / h_elem * _phi_neighbor[_j][_qp] * _test[_i][_qp]);
          break;

        case Moose::NeighborElement:
          r += _grad_phi[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp] * 0.5 * _diff[_qp];
          r += _grad_test_neighbor[_i][_qp] * _normals[_qp] * _epsilon * 0.5 * _phi[_j][_qp] * _diff_neighbor[_qp];
              
          r -= RealEigenVector::Constant(_count,
                                         _sigma / h_elem * _phi[_j][_qp] * _test_neighbor[_i][_qp]);
          break;

        case Moose::NeighborNeighbor:
          r += _grad_phi_neighbor[_j][_qp] * _normals[_qp] * _test_neighbor[_i][_qp] * 0.5 * _diff_neighbor[_qp];
          r -= _grad_test_neighbor[_i][_qp] * _normals[_qp] * _epsilon * 0.5 * _phi_neighbor[_j][_qp] * _diff_neighbor[_qp];

          r += RealEigenVector::Constant(
              _count, _sigma / h_elem * _phi_neighbor[_j][_qp] * _test_neighbor[_i][_qp]);
          break;
      }

  return r;
}
