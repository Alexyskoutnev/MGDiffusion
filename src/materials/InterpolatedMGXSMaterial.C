#include "InterpolatedMGXSMaterial.h"
registerMooseObject("MGDiffusionApp", InterpolatedMGXSMaterial);
// defineLegacyParams(InterpolatedMGXSMaterial);

InputParameters
InterpolatedMGXSMaterial::validParams()
{
    InputParameters params = Material::validParams();
    
    params.addRequiredParam<std::string>("xs_table","Name of the file containing interpolation tables for multigroup cross-sections and discontinuity factors");
    
    params.addRequiredParam<unsigned int>("n_groups","Number of energy groups.");

    // TODO: Update this to only require one coupled variable - ArrayVariable state.
    params.addRequiredCoupledVar("mod_dens_var", "Coupled aux variable storing moderator density throughout the domain. Should be of type MONOMIAL CONSTANT.");
    params.addRequiredCoupledVar("boron_var", "Coupled aux variable storing boron concentrations throughout the domain. Should be of type MONOMIAL CONSTANT..");
    params.addRequiredCoupledVar("fuel_temp_var", "Coupled aux variable storing centerline fuel temperatures throughout the domain. Should be of type MONOMIAL CONSTANT");
    params.addRequiredCoupledVar("burnup_var", "Coupled aux variable storing burnups. Should be of type MONOMIAL CONSTANT");
    
    params.addClassDescription("Declares a material containing essential nuclear data for multigroup neutron diffusion (cross-sections, yield, etc.) using interpolation tables");
    params.set<MooseEnum>("constant_on") = "SUBDOMAIN";
    
    return params;
}

InterpolatedMGXSMaterial::InterpolatedMGXSMaterial(const InputParameters & parameters) :
    Material(parameters),
    _n_groups(          getParam<unsigned int>("n_groups")),
    _xs_table_file(     getParam<std::string>("xs_table")),
    _moderator_density(   coupledValue("mod_dens_var")),
    _boron_concentration( coupledValue("boron_var")),
    _fuel_temperature(    coupledValue("fuel_temp_var")),
    _burnup(              coupledValue("burnup_var")),
    _diffusivity(       declareProperty<RealEigenVector>("diffusivity")),
    _sigma_t(           declareProperty<RealEigenVector>("sigma_t")),
    _nu_sigma_f(        declareProperty<RealEigenVector>("nu_sigma_f")),
    _kappa_sigma_f(     declareProperty<RealEigenVector>("kappa_sigma_f")),
    _chi(               declareProperty<RealEigenVector>("chi")),
    _adf(               declareProperty<RealEigenVector>("adf")),
    _sigma_s(           declareProperty<RealEigenMatrix>("sigma_s")),
    _chi_nu_sigma_f(    declareProperty<RealEigenMatrix>("chi_nu_sigma_f"))
{
    
    // Initialize interpolation tables by reading from provided interp_file.
    
    //std::cout << "Initializing interpolation tables for: " << _xs_table_file << "..." << std::endl;
    for (int i = 0; i < _n_groups; i++) {
        initInterpTables( _xs_table_file , i );
    }
    //std::cout << "Material: " << _xs_table_file << " finished." << std::endl;

}

void
InterpolatedMGXSMaterial::initQpStatefulProperties()
{
    //std::cout << "Initializing QP properties..." << std::endl;
    computeQpProperties();
    
}

void
InterpolatedMGXSMaterial::computeQpProperties()
{
    
    // Set size of _v containers.
    
    /// Values computed  in computeQpProperties
    RealEigenVector _v_diffusivity;
    RealEigenVector _v_sigma_t;
    RealEigenVector _v_nu_sigma_f;
    RealEigenVector _v_kappa_sigma_f;
    RealEigenVector _v_chi;
    RealEigenVector _v_adf;
    RealEigenMatrix _v_sigma_s;
    
    _v_diffusivity.resize(_n_groups,1);
    _v_sigma_t.resize(_n_groups,1);
    _v_nu_sigma_f.resize(_n_groups,1);
    _v_kappa_sigma_f.resize(_n_groups,1);
    _v_adf.resize(_n_groups,1);
    _v_sigma_s.resize(_n_groups,_n_groups);
    _v_chi.resize(_n_groups,1);
    
    _v_chi[0] = 1;
    _v_chi[1] = 0;
    
    // TODO: Update this to work with array-variable
    // Obtain qp values for state parameters,

    std::vector<Real> state(4);
    
    std::vector<Real> test_state(4);
    test_state[0] = _burnup[_qp];
    test_state[1] = _moderator_density[_qp];
    test_state[2] = _boron_concentration[_qp];
    test_state[3] = _fuel_temperature[_qp];
    
    for (int i = 0; i < 4; i++) {
        
        // Is the point >= the smallest value and <= the largest value
        if (is_valid_query( test_state[i], _base_points[i] ) ) { state[i] = test_state[i]; }
        else {
            state[i] = test_state[i];
        }
        
    }
    
    // Sample interpolation objects, looping over groups.
    for (int i = 0; i < _n_groups ; i++) {
        
        //std::cout << "Interpolating..." << std::endl;
        _v_diffusivity[i]   = _diffusivity_interp[i].multiLinearInterpolation( state );
        _v_sigma_t[i]       = _sigma_t_interp[i].multiLinearInterpolation( state );
        _v_nu_sigma_f[i]    = _nu_sigma_f_interp[i].multiLinearInterpolation( state );
        _v_kappa_sigma_f[i] = _kappa_sigma_f_interp[i].multiLinearInterpolation( state );
        _v_adf[i]           = _adf_interp[i].multiLinearInterpolation( state );
        
        for (int j = 0; j < _n_groups ; j++) {
            _v_sigma_s(j,i) = _sigma_s_interp[i][j].multiLinearInterpolation( state );
        }
        
    }
    
    // Return _qp properties.
    _diffusivity[_qp]       = _v_diffusivity;
    _sigma_t[_qp]           = _v_sigma_t;
    _nu_sigma_f[_qp]        = _v_nu_sigma_f;
    _kappa_sigma_f[_qp]     = _v_kappa_sigma_f;
    _chi[_qp]               = _v_chi;
    _adf[_qp]               = _v_adf;
    _sigma_s[_qp]           = _v_sigma_s;
    _chi_nu_sigma_f[_qp]    = _v_chi * _v_nu_sigma_f.transpose();
    
}

bool
InterpolatedMGXSMaterial::initInterpTables(const std::string & param_file_name, int group_num)
{
    
    if (true)
    {
    
        // Abscissa -- stored as 2D real array
        
        std::vector<std::vector<Real>> abscissa;
        
        std::ifstream xs_file;
        xs_file.open(param_file_name);
        
        std::string axis_name;
        MultiIndex<Real>::size_type dim;
        
        bool found_table = false;
        bool found_group = false;
        std::string data_name;
        std::string group_name = "GROUP " + std::to_string(group_num);
        std::vector<std::vector<double>> temp_data;
        
        // First, read abscissa.
        bool end_abscissa_flag = false;
        
        while (xs_file.good() && !end_abscissa_flag) {
            
            std::string line;
            getline(xs_file, line);
            
            // Read abscissa
            if (line.find("#") != std::string::npos ){
            
                std::vector<double> axis_values;
                getline(xs_file, line, '\n');
                axis_values = split_by_token(line, ",");
                
                abscissa.push_back(axis_values);
                dim.push_back( axis_values.size() );
                
            }
            
            if (line.find("&") != std::string::npos) {
                end_abscissa_flag = true;
            }
            
        }
        
        // Reset file stream.
        xs_file.clear();
        xs_file.seekg( 0, std::ios::beg );
        
        // MultiIndex --> Moose util for >2 indexed rectangular data. Required input type for MultiDimensionalInterpolation.setData().lize without constructor, because dimensions are currently unknown.;
        
        std::map< std::string, MultiIndex<Real> > mi_map;
        mi_map.insert( std::pair< std::string, MultiIndex<Real> >("DIFFUSIVITY"   , MultiIndex<Real>(dim) ) );
        mi_map.insert( std::pair< std::string, MultiIndex<Real> >("SIGMA_T"       , MultiIndex<Real>(dim) ) );
        mi_map.insert( std::pair< std::string, MultiIndex<Real> >("NU_SIGMA_F"    , MultiIndex<Real>(dim) ) );
        mi_map.insert( std::pair< std::string, MultiIndex<Real> >("KAPPA_SIGMA_F" , MultiIndex<Real>(dim) ) );
        mi_map.insert( std::pair< std::string, MultiIndex<Real> >("ADF"           , MultiIndex<Real>(dim) ) );
        mi_map.insert( std::pair< std::string, MultiIndex<Real> >("SIGMA_S"       , MultiIndex<Real>(dim) ) );

        int scatter_group = 0;
        std::vector< MultiIndex<Real> > v_mi_sigma_s( _n_groups, MultiIndex<Real>(dim) );

        bool group_finished = false;
        
        while (xs_file.good() && !group_finished) {

            std::string line;
            getline(xs_file, line);
                    
            if (line.find("GROUP") != std::string::npos) {
                // True if it's the correct group, false otherwise.
                found_group = (line.find(group_name) != std::string::npos);
            }
            
            // Found datatable - reading...
            if (!found_table && found_group) {
                if (line.find("&") != std::string::npos) {
                    found_table = true;
                    data_name = line;
                }
            }
            
            else if (found_table && found_group) {
                
                //std::cout << line << std::endl;
                
                if (line.find("$") != std::string::npos) {

                    // Compare against pre-defined material property keys. If found, populate MI.
                    
                    std::map<std::string, MultiIndex<Real>>::iterator it;
                    
                    for (it = mi_map.begin(); it != mi_map.end(); it++) {
                        if (data_name.find(it->first) != std::string::npos) {
                            
                            MultiIndex<Real>::size_type mi_shape = (it->second).size();
                            
                            fill_mi_from_data( dim , temp_data , it->second );
                            
                        }
                    }
                    
                    // Check if this is a scattering table,
                    if (data_name.find("SIGMA_S") != std::string::npos) {
                        
                        std::vector<Real> mi_raw_data = mi_map.at("SIGMA_S").getRawData();
                        
                        v_mi_sigma_s[scatter_group] = MultiIndex<Real>( dim, mi_raw_data );
                        scatter_group++;
                        if (scatter_group == _n_groups) { group_finished = true; }
                        
                    }
                    
                    // Done with this table. Cleanup.
                    found_table = false;
                    data_name = "";
                    temp_data.clear();
                    
                } else {
                    
                    // Append
                    std::vector<double> data_values;
                    data_values = split_by_token(line, ",");
                    temp_data.push_back(data_values);
                    
                }
            }
            
            
        }
        
        // TODO: ERROR HANDLING: Check that dimensions of axes match dimensions of data-tables, check that all multi-index were filled when reading from the file, etc
        //std::cout << "Gracefully leaving loop" << std::endl;
        
        if (group_num == 0) {
            for (int i = 0; i < abscissa.size(); i++) {
                _base_points.push_back(abscissa[i]);
            }
        }
        
        // MultiIndex tables and ascissa populated - initialize interp tables for this group.
        
        MultiDimensionalInterpolation new_diff_interp;
        MultiDimensionalInterpolation new_sig_t_interp;
        MultiDimensionalInterpolation new_nu_sig_f_interp;
        MultiDimensionalInterpolation new_kappa_sig_f_interp;
        MultiDimensionalInterpolation new_adf_interp;
        MultiDimensionalInterpolation new_sig_s_interp;

        new_diff_interp.setData(abscissa, mi_map.at("DIFFUSIVITY") );
        _diffusivity_interp.push_back( new_diff_interp );
        
        new_sig_t_interp.setData( abscissa , mi_map.at("SIGMA_T") );
        _sigma_t_interp.push_back(new_sig_t_interp);
        
        new_nu_sig_f_interp.setData( abscissa , mi_map.at("NU_SIGMA_F") );
        _nu_sigma_f_interp.push_back(new_nu_sig_f_interp);
        
        new_kappa_sig_f_interp.setData( abscissa , mi_map.at("KAPPA_SIGMA_F") );
        _kappa_sigma_f_interp.push_back(new_kappa_sig_f_interp);
        
        new_adf_interp.setData( abscissa , mi_map.at("ADF") );
        _adf_interp.push_back(new_adf_interp);
        
        std::vector<MultiDimensionalInterpolation> v_sigma_s_interp;
        
        for (int i = 0; i < _n_groups; i++){

            std::vector<Real> mi_sig_s_data = v_mi_sigma_s[i].getRawData();
            MultiIndex<Real> mi_in = MultiIndex<Real>( dim, mi_sig_s_data );
            
            new_sig_s_interp.setData( abscissa, mi_in );
            v_sigma_s_interp.push_back(new_sig_s_interp);
            
        }
        
        _sigma_s_interp.push_back(v_sigma_s_interp);
        
        return true;
        
    }
    
    return false;
    
}

std::vector<double> InterpolatedMGXSMaterial::split_by_token(std::string str, std::string token){
    
    // Helper function for splitting rows of .csv files by a token.
    std::vector<double> data;
    while(str.size()){
        int index = str.find(token);
        if (index != std::string::npos){
            std::string sub_string = str.substr(0,index);
            sub_string.erase(std::remove_if(sub_string.begin(), sub_string.end(), isspace),sub_string.end());
            data.push_back(std::stof(sub_string));
            str = str.substr(index+token.size()+1);
        }
        else if (str.size() != 0){
            int index = str.find("\n");
            std::string sub_string = str.substr(0,index);
            sub_string.erase(std::remove_if(sub_string.begin(), sub_string.end(), isspace),sub_string.end());
            data.push_back(std::stof(sub_string));
            break;
        }
    }
    return data;
}

void InterpolatedMGXSMaterial::fill_mi_from_data(MultiIndex<Real>::size_type max, std::vector<std::vector<double>> data, MultiIndex<Real>& mi) {
    
    int depth = max.size();
    
    MultiIndex<Real>::size_type slots(depth);
    
    std::vector<int> slots_w(depth);
    
    for (int i = 0; i < depth; i++){
        
        slots[i] = 0;
    }
    
    for (int i = depth-1; i >= 0; i--){
        
        if (i==depth-1) {slots_w[i] = 0;}
        if (i==depth-2) {slots_w[i] = 1;}
        if (i<=depth-3) {slots_w[i] = max[i+1] * slots_w[i+1];}
        
    }
    
    int index = depth-1;
    int row;
    int column;
    
    while (true)
    {
    
        // Find which value in the table sits at this position in MultiIndex.
        column = slots[depth-1];
        row = 0;
        
        for (int i = 0; i < depth-1; i ++) { row = row + slots[i]*slots_w[i];}

        mi(slots) = data[row][column];

        // Increment and carry.
        slots[index]++;
        
        while (slots[index] == max[index])
        {
            if (index == 0) {
                return;
            }
            slots[index--] = 0;
            slots[index]++;
        }
        
        index = depth-1;
    }

}

bool InterpolatedMGXSMaterial::is_valid_query( Real query, std::vector<Real> & axis) {

    return ( query >= axis[0] && query <= axis[axis.size()-1] );
    
}

