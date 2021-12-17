[Mesh]
  [QuarterCore]
    type = CartesianMeshGenerator
    dim = 2
    dx = '21.42'
    dy = '21.42'
    ix = '4'
    iy = '4'
    subdomain_id = '55'
  []
[]

[Variables]
  [scalar_flux]
    order = FIRST
    family = L2_LAGRANGE
    components = 4
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
  [ReflectingBoundaries]
    type = ArrayNeumannBC
    variable = scalar_flux
    boundary = '0 1 2 3'
    value = '0 0 0 0'
  []

[]

[Executioner]
  type = Eigenvalue
  solve_type = 'Newton'
  free_power_iterations = 4
  nl_abs_tol = 1e-10
  nl_max_its = 10000

  line_search = none

  l_abs_tol = 1e-10

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

[Outputs]
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
     [u45_2000_rodded]
     type = MultigroupXSMaterial
     diffusivity = " 1.762298e+00  7.510180e-01  7.297683e-01  3.765815e-01 "
     sigma_t = " 3.976051e-01  8.751180e-01  9.103974e-01  1.464319e+00 "
     nu_sigma_f = " 3.946410e-03  1.067820e-02  1.686940e-02  1.754300e-01 "
     chi = " 9.996800e-01  3.199790e-04  0  0 "
     kappa_sigma_f = " 4.822760e-14  1.382020e-13  2.200120e-13  2.224340e-12 "
     adf = " 1.038090e+00  1.077980e+00  1.193690e+00  1.332500e+00 "
     sigma_s = " 3.563750e-01  0  0  0 ;
                 3.804580e-02  7.758150e-01  0  0 ;
                 1.215410e-05  4.992670e-02  5.833740e-01  2.286560e-03 ;
                 0  9.192750e-03  2.562950e-01  1.306040e+00 "
     block =  55 
     []
[]
