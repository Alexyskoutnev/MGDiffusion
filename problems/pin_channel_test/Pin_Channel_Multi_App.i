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
    subdomain_id = '
	    99 99
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
 []

[UserObjects]
 [integrate_kappa_fission_by_layer]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = normalized_power
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
    block = 1
  []
[]

[Postprocessors]
[]

[VectorPostprocessors]
  [integral_out]
    type = ConstantVectorPostprocessor
    vector_names = 'channel1'
    value = '0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0;'
  []
  [power_normalization_main_app]
    type = SpatialUserObjectVectorPostprocessor
    userobject = integrate_kappa_fission_by_layer
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
     block =  " 1 1 1 1 "
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

[MultiApps]
  [sub_app]
    type = FullSolveMultiApp
    input_files = 'Multi_App_Pin1_Sub.i'
    positions = '0   0   0'
    execute_on = TIMESTEP_END
  []
[]

[Transfers]
  [push_u]
    type = MultiAppVariableValueSampleTransfer

    multi_app = sub_app
    # Transfer to the sub-app from this app
    direction = to_multiapp

    # The name of the variable in this app
    source_variable = kappa_fission_phi

    # The name of the auxiliary variable in the sub-app
    variable = normalized_power1
    execute_on = TIMESTEP_END
  []
  [vpp_from_vpp_0]
    type = MultiAppReporterTransfer
    to_reporters = 'integral_out/channel1'
    from_reporters = 'integral_out/temperature_fuel'
    direction = from_multiapp
    multi_app = sub_app
    subapp_index = 0
    execute_on = FINAL
  []
  #[vpp_from_vpp_1]
  #type = MultiAppReporterTransfer
  #to_reporters = 'integral_out/channel2'
  #from_reporters = 'integral_out/temperature_fuel'
  #direction = from_multiapp
  #multi_app = sub_app
  #subapp_index = 1
  #execute_on = TIMESTEP_END
  #[]
  #[vpp_from_vpp_2]
  #type = MultiAppReporterTransfer
  #to_reporters = 'integral_out/channel3'
  #from_reporters = 'integral_out/temperature_fuel'
  #direction = from_multiapp
  #multi_app = sub_app
  #subapp_index = 2
  #execute_on = TIMESTEP_END
  #[]
  #[vpp_from_vpp_3]
  #type = MultiAppReporterTransfer
  #to_reporters = 'integral_out/channel4'
  #from_reporters = 'integral_out/temperature_fuel'
  #direction = from_multiapp
  #multi_app = sub_app
  #subapp_index = 3
  #execute_on = TIMESTEP_END
  #[]
[]

[Executioner]
  type = Eigenvalue
  solve_type = 'Newton'
  free_power_iterations = 4
  nl_abs_tol = 1e-4
  nl_max_its = 1000

  line_search = none

  l_abs_tol = 1e-4
[]