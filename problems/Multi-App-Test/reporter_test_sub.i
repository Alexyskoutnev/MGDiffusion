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
             1  1
             1  1
            
            99 99
            99 99'
  []
[]
[Variables]
  [./v]
    order = FIRST
    family = L2_LAGRANGE
  []
[]
[Kernels]
   [dummy_calulation]
    type = NullKernel
    variable = v
  []
[]
[BCs]
[]
[Executioner]
  type = Steady
  solve_type = 'Newton'
  free_power_iterations = 4
  nl_abs_tol = 1e-2
  nl_max_its = 100
  line_search = none
  l_abs_tol = 1e-3
[]
[AuxVariables]
  [aux_normalized_power]
    family = L2_LAGRANGE
    order = FIRST
  []
[]

[AuxKernels]
 # [compute_kappa_fission_phi]
 #   type = ReactionRateAux
 #   variable = kappa_fission_phi
 #   scalar_flux_variable = scalar_flux
 #   cross_section = kappa_sigma_f
 # []
[]
[Outputs]
  file_base = 'New_Multi_App_Sub_Output'
  [exo]
    type = Exodus 
    execute_on = "timestep_begin"
  []
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



[UserObjects]
  [./integrate_kappa_fission_by_layer_1]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = aux_normalized_power
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
    block = 1
  []
#  [integrate_kappa_fission_by_layer_1]
#    type = MultiAppLayeredIntergral
#    variable = normalized_power
#    direction = z
#    num_layers = 20
#    cumulative = true
#    multi_app = multiapp
#    execute_on = timestep_end
#    block = 1
#  [../]
[]

