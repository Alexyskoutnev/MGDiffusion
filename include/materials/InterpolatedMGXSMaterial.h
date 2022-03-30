#pragma once

// Includes to handle file-reading.
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

// MOOSE includes
#include "Material.h"
#include "MultiDimensionalInterpolation.h"
#include "MultiIndex.h"

class InterpolatedMGXSMaterial;

// template<>
// InputParameters validParams<InterpolatedMGXSMaterial>();

class InterpolatedMGXSMaterial : public Material
{
public:
    static InputParameters validParams();
    
    InterpolatedMGXSMaterial(const InputParameters & parameters);
    
protected:
    
    /// Overrides for creating a MOOSE Material class.
    virtual void initQpStatefulProperties() override;
    virtual void computeQpProperties() override;
    
    /// Helper function for reading CSV data into interpolation objects for each group..
    bool initInterpTables(const std::string & param_file_name, int group_num);
    
    // Number of energy groups (obtained from user input)
    const unsigned int _n_groups;
    std::string _xs_table_file;
    
    /// State parameters (used as query points in interpolation) - obtained from aux-variable. TODO: Change this to arbitrary-dimension array variable.
    const VariableValue & _moderator_density;
    const VariableValue & _boron_concentration;
    const VariableValue & _fuel_temperature;
    const VariableValue & _burnup;

    /// Material properties
    MaterialProperty<RealEigenVector> & _diffusivity;
    MaterialProperty<RealEigenVector> & _sigma_t;
    MaterialProperty<RealEigenVector> & _nu_sigma_f;
    MaterialProperty<RealEigenVector> & _kappa_sigma_f;
    MaterialProperty<RealEigenVector> & _chi;
    MaterialProperty<RealEigenVector> & _adf;
    MaterialProperty<RealEigenMatrix> & _sigma_s;
    MaterialProperty<RealEigenMatrix> & _chi_nu_sigma_f;
    
    /*
     Multidimensional Interpolation objects used to update _v_*
     We need to be able to do interpolations on vectors and matrices -
     Store these as std::vector<MultiDimensionalInterpolation> and loop over them.
     */
    
    std::vector<std::vector<Real>> _base_points;
    
    std::vector<MultiDimensionalInterpolation> _diffusivity_interp;
    std::vector<MultiDimensionalInterpolation> _sigma_t_interp;
    std::vector<MultiDimensionalInterpolation> _nu_sigma_f_interp;
    std::vector<MultiDimensionalInterpolation> _kappa_sigma_f_interp;
    std::vector<MultiDimensionalInterpolation> _adf_interp;
    std::vector<std::vector<MultiDimensionalInterpolation>> _sigma_s_interp;
    
private:
    
    // Private methods used for constructing interpolation tables and verifying validity of query state-points.
    std::vector<double> split_by_token(std::string str, std::string token);
    
    void fill_mi_from_data(MultiIndex<Real>::size_type max, std::vector<std::vector<double>> data, MultiIndex<Real>& mi);
    
    bool get_state( std::vector<Real> & state );
    
    bool is_valid_query( Real query, std::vector<Real> & axis);
    
};
