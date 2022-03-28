[Mesh]
   [QuarterCore]
    type = CartesianMeshGenerator
    dim = 3
    dx = '10.71'
    dy = '10.71'
    dz = '30 300 30'
    ix = '1'
    iy = '1'
    iz = '2 30 2'
    subdomain_id = '99
             1
            99'
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
  type = Transient
  solve_type = 'Newton'
  free_power_iterations = 4
  nl_abs_tol = 1e-2
  nl_max_its = 100

  line_search = none

  l_abs_tol = 1e-3
[]

[AuxVariables]
  [normalized_power]
    family = L2_LAGRANGE
    order = FIRST
  []
[]

[UserObjects]
# Computes layered averages axially of kappa sigma f
  [./integrate_kappa_fission_by_layer_1]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = normalized_power
    execute_on = timestep_end
    cumulative = true
    positive_cumulative_direction = true
    block = 1
  []
  [./average_kappa_fission_phi_1]
    type = LayeredAverage
    variable = normalized_power
    direction = z
    num_layers = 20
    sample_type = direct
    block = 1
  [../]
[]



[VectorPostprocessors]
  [./integral_out]
    type = IntegralUserObject
    intergated_temperature_uo = integrate_kappa_fission_by_layer_1
    temperatures_uo = average_kappa_fission_phi_1
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
  [./kappa_fission_phi_layered_avg_1]
    type = SpatialUserObjectVectorPostprocessor
    userobject = average_kappa_fission_phi_1
  []
  [axial_power_out_1]
    type = SpatialUserObjectVectorPostprocessor
    userobject = integrate_kappa_fission_by_layer_1
  []
[]

[AuxKernels]
[]

[Outputs]
  file_base = 'Old_Multi_App_Sub_Output'
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

