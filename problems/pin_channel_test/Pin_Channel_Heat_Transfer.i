[Mesh]
  [QuarterCore]
    type = CartesianMeshGenerator
    dim = 3
    dx = '10.71 10.71'
    dy = '10.71 10.71'
    dz = '30 300 30'
    ix = '2 2'
    iy = '2 2'
    iz = '2 30 2'
    subdomain_id = '99 99
		    99 99

		     1  1
		     1  1
		    
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
  nl_abs_tol = 1e-6
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
  [fuel_temperature]
    family = MONOMIAL
    order = CONSTANT
  []
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
  #[compute_pin_channel_temperature]
  #  type = AnalyticFlowChannelHeatTransfer
  #  variable = fuel_temperature
  #  volumetric_heat = kappa_fission_phi
  #  r_i = 4.8195
  #  r_o = 5.355
  #  k_clad = 123
  #  k_fuel = 45
  #  c_p_clad = 1254
  #  c_p_fuel = 1254
  #  c_p_coolant = 1
  #  h_cool = 32500
  #  t_cool_in = 400
  #  m_dot = 0.00731562520642246
  #  Coolant_Temp = integral_out
[]

[UserObjects]
# Computes layered averages axially of kappa sigma f
  [integrate_kappa_fission_by_layer]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = kappa_fission_phi
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
  []
  [./layered_integral]
    type = LayeredIntegral
    direction = z
    num_layers = 10
    variable = kappa_fission_phi     
    execute_on = TIMESTEP_END
    cumulative = true
    positive_cumulative_direction = true
  []
  [kappa_fission_phi_1d]
    type = NearestPointLayeredAverage
    direction = z
    num_layers = 10
    variable = kappa_fission_phi
    points = '0 10.71
             0 10.71
             0 300'

  []
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
  [axial_power_out]
    type = SpatialUserObjectVectorPostprocessor
    userobject = integrate_kappa_fission_by_layer
  []
  [axial_power_out2]
    type = SpatialUserObjectVectorPostprocessor
    userobject = layered_integral
  []
  [./integral_out]
    type = IntegralUserObject
    userobject = layered_integral
    userobject2 = kappa_fission_phi_1d
    r_i = 4.8195
    r_o = 5.355
    k_clad = 123
    k_fuel = 45
    c_p_clad = 1254
    c_p_fuel = 1254
    c_p_coolant = 1
    h_cool = 32500
    t_cool_in = 400
    m_dot = 0.00731562520642246
    kappa_fission_phi = kappa_fission_phi
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
     block =  1 
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


