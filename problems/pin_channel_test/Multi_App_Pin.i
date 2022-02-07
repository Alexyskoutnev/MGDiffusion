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

[MultiApps]
  [sub_app]
    type = TransientMultiApp
    input_files = 'Multi_App_Pin1_Sub.i'
    positions = '0   0   0'
    execute_on = timestep_end
    output_in_position = true
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
    #source_variable = scalar_flux

    # The name of the auxiliary variable in the sub-app
    variable = flux
  []
[]