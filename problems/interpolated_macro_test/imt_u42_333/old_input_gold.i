[Mesh]
  [QuarterCore]
    type = CartesianMeshGenerator
    dim = 2
    dx = '21.42'
    dy = '21.42'
    ix = '1'
    iy = '1'
    subdomain_id = '2'
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
  [ReflectingBoundaries]
    type = ArrayNeumannBC
    variable = scalar_flux
    boundary = '0 1 2 3'
    value = '0 0'
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
     [u42_2250]
     type = MultigroupXSMaterial
     diffusivity = " 1.384068e+00  3.821048e-01 "
     sigma_t = " 5.606430e-01  1.470931e+00 "
     nu_sigma_f = " 7.659950e-03  1.546240e-01 "
     chi = " 1  0 "
     kappa_sigma_f = " 9.934980e-14  2.058270e-12 "
     adf = " 1.010920e+00  1.004800e+00 "
     sigma_s = " 5.325880e-01  0 ;
                 1.740210e-02  1.344240e+00 "
     block =  2 
     []
[]








