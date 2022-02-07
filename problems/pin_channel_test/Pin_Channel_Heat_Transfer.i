[Mesh]
  [QuarterCore]
    type = CartesianMeshGenerator
    dim = 3
    dx = '10.71 10.71'
    dy = '10.71 10.71'
    dz = '30 300 30'
    ix = '3 3'
    iy = '3 3'
    iz = '2 30 2'
    subdomain_id = '99 99
            99 99
             1  2
             3  4
            
            99 99
            99 99'
  []
[]

[Variables]
  [scalar_flux]
    order = FIRST
    family = L2_LAGRANGE
    components = 2
  []
[]

[Kernels]
  [diffusion]
    type = ArrayDiffusion
    variable = scalar_flux
    diffusion_coefficient = diffusivity
  []
  [absorption]
    type = MultigroupAbsorption
    variable = scalar_flux
    sigma_t = sigma_t
  []
  [scattering]
    type = MultigroupScattering
    variable = scalar_flux
    sigma_s = sigma_s
  []
  [fission]
    type = MultigroupFission
    variable = scalar_flux
    nu_sigma_f = nu_sigma_f
    chi = chi
    extra_vector_tags = 'eigen'
  []
[]

[DGKernels]
  [dgdiffusion]
    type = MultigroupDGDiffusion
    variable = scalar_flux
    use_adf = true
    epsilon = 0
  []
[]

[BCs]
  [VacuumBoundaries]
    type = ArrayPenaltyDirichletBC
    variable = scalar_flux
    boundary = '0 1 2 3 4 5'
    value = '0 0'
  []
[]

[Executioner]
  type = Eigenvalue
  solve_type = 'Newton'
  free_power_iterations = 4
  nl_abs_tol = 1e-5
  nl_max_its = 1000

  line_search = none

  l_abs_tol = 1e-6
[]

[AuxVariables]
# copy array variable components
  [flux_fast]
    family = L2_LAGRANGE
    order = FIRST
  []
  [flux_thermal]
    family = L2_LAGRANGE
    order = FIRST
  []
# store integrated flux over each element
  [integrated_flux_fast]
    family = MONOMIAL
    order = CONSTANT
  []
  [integrated_flux_thermal]
    family = MONOMIAL
    order = CONSTANT
  []
# store reaction rate (fission power)
  [kappa_fission_phi]
    family = L2_LAGRANGE
    order = FIRST
  []
  [normalized_power]
    family = L2_LAGRANGE
    order = FIRST
  []
   [layered_average]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[AuxKernels]
  [copy_flux_fast]
    type = ArrayVariableComponent
    array_variable = scalar_flux
    variable = flux_fast
    component = 0 
  []
  [copy_flux_thermal]
    type = ArrayVariableComponent
    array_variable = scalar_flux
    variable = flux_thermal
    component = 1 
  []
  [integrate_flux_fast] 
    type = ElementLpNormAux
    variable = integrated_flux_fast
    coupled_variable = flux_fast
    p = 1
  []
  [integrate_flux_thermal] 
    type = ElementLpNormAux
    variable = integrated_flux_thermal
    coupled_variable = flux_thermal
    p = 1
  []
  [compute_kappa_fission_phi]
    type = ReactionRateAux
    variable = kappa_fission_phi
    scalar_flux_variable = scalar_flux
    cross_section = kappa_sigma_f
  []
  [power_normalization]
    type = NormalizationAux
    variable = normalized_power
    source_variable = kappa_fission_phi
    normalization = volume_integrated_power
    normal_factor = 1.5e5
    execute_on = TIMESTEP_END
  []
  [layered_average]
    type = SpatialUserObjectAux
    variable = layered_average
    execute_on = timestep_end
    user_object = average_kappa_fission_phi_1
  [../]
 []

[UserObjects]
# Computes layered averages axially of kappa sigma f
  [integrate_kappa_fission_by_layer_1]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = normalized_power
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
    block = 1
  []
  [integrate_kappa_fission_by_layer_2]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = normalized_power
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
    block = 2
  []
  [integrate_kappa_fission_by_layer_3]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = normalized_power
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
    block = 3
  []
  [integrate_kappa_fission_by_layer_4]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = normalized_power
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
    block = 4
  []
  [./average_kappa_fission_phi_1]
    type = LayeredAverage
    variable = normalized_power
    direction = z
    num_layers = 20
    sample_type = interpolate
    block = 1
  [../]
  [./average_kappa_fission_phi_2]
    type = LayeredAverage
    variable = normalized_power
    direction = z
    num_layers = 20
    sample_type = interpolate
    block = 2
  [../]
   [./average_kappa_fission_phi_3]
    type = LayeredAverage
    variable = normalized_power
    direction = z
    num_layers = 20
    sample_type = interpolate
    block = 3
  [../]
    [./average_kappa_fission_phi_4]
    type = LayeredAverage
    variable = normalized_power
    direction = z
    num_layers = 20
    sample_type = interpolate
    block = 4
  [../]
[]

[Postprocessors]
  [mem_per_process]
    type = MemoryUsage
    mem_units = megabytes
    value_type = average
    outputs = 'csv'
  []
  [perf_graph]
    type = PerfGraphData
    data_type = TOTAL
    section_name = 'Root'
    outputs = 'csv'
  []
[]

[VectorPostprocessors]
  [flux_sampler]
    type = ElementValueSampler
    variable = 'integrated_flux_fast integrated_flux_thermal'
    sort_by = id
    execute_on = 'TIMESTEP_END'
  []
# output axial power integral
  [axial_power_out_1]
    type = SpatialUserObjectVectorPostprocessor
    userobject = integrate_kappa_fission_by_layer_1
  []
  [axial_power_out_2]
    type = SpatialUserObjectVectorPostprocessor
    userobject = integrate_kappa_fission_by_layer_2
  []
  [axial_power_out_3]
    type = SpatialUserObjectVectorPostprocessor
    userobject = integrate_kappa_fission_by_layer_3
  []
  [axial_power_out_4]
    type = SpatialUserObjectVectorPostprocessor
    userobject = integrate_kappa_fission_by_layer_4
  []
  [./integral_out]
    type = IntegralUserObject
    userobject = integrate_kappa_fission_by_layer_1
    userobject2 = average_kappa_fission_phi_1
    r_i = 0.0939
    r_o = 0.1071
    k_clad = 123
    k_fuel = 45
    c_p_clad = 1
    c_p_fuel = 1
    c_p_coolant = 1254
    h_cool = 32500
    t_cool_in = 400
    m_dot = 0.00731562520642246
    n_rod = 6500
    power = 15000000
    E_d_E_r_Ratio = 0.9
    reactor_height = 3.6
  []
  [kappa_fission_phi_layered_avg_1]
    type = SpatialUserObjectVectorPostprocessor
    userobject = average_kappa_fission_phi_1
  []
[]

[Postprocessors]
  [volume_integrated_power]
    type = ElementIntegralVariablePostprocessor
    variable = kappa_fission_phi
  []
[]

[Outputs]
  file_base = 'flux_sampler'
  exodus = true
  csv = true
  [pgraph]
    type = PerfGraphOutput
    execute_on = 'final'
    level = 2
    outputs = 'csv'
  []
[]

[Materials]
     [u42_0015]
     type = MultigroupXSMaterial
     diffusivity = " 1.373845e+00  3.792145e-01 "
     sigma_t = " 5.577729e-01  1.473914e+00 "
     nu_sigma_f = " 7.625890e-03  1.578920e-01 "
     chi = " 1  0 "
     kappa_sigma_f = " 9.889730e-14  2.101800e-12 "
     adf = " 1.010790e+00  1.003070e+00 "
     sigma_s = " 5.303440e-01  0 ;
                 1.748660e-02  1.355580e+00 "
     block =  " 1 2 3 4 "
     []
     [reflector]
     type = MultigroupXSMaterial
     diffusivity = " 1.104166e+00  2.719601e-01 "
     sigma_t = " 6.700725e-01  2.027498e+00 "
     nu_sigma_f = " 0  0 "
     chi = " 1  0 "
     kappa_sigma_f = " 0  0 "
     adf = " 1.176390e+00  2.275500e-01 "
     sigma_s = " 6.401210e-01  0 ;
                 2.752620e-02  1.990260e+00 "
     block =  99 
     []
[]


