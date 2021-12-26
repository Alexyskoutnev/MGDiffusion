[Mesh]
      type = FileMesh
      file =  merged_two_cubes_test1.e
      dim = 3
[]

[Variables]
  [temperature]
  []
[]

[ICs]
        [block1_]
          type = ConstantIC
          block = 3
          variable = temperature
          value = 300
        []
        [block2_]
          type = ConstantIC
          block = 4
          variable = temperature
          value = 300
        []
[]

[Kernels]
  [heat_conduction]
    type = HeatConduction
    variable = temperature
    block = '3 4'
  []
  [./ie]
    type = TimeDerivative
    variable = temperature
  []
[]

[BCs]
  [inlet_temperature]
    type = DirichletBC
    variable = temperature
    boundary = 4
    value = 500 # (K)
  []
  [outlet_temperature]
    type = DirichletBC
    variable = temperature
    boundary = 12
    value = 1000 # (K)
  []
[]


[Materials]
  [steel]
    type = BlockAverageDiffusionMaterial
    block_average_userobject = bav
  []
[]

[UserObjects]
  [./bav]
    type = BlockAverageValue
    variable = temperature
    execute_on = timestep_begin
    outputs = none
  [../]
  [./layered_integral]
    type = LayeredIntegral
    direction = z
    num_layers = 20
    variable = temperature
    execute_on = TIMESTEP_END
    cumulative = true
  [../]
[]

[VectorPostprocessors]
  [./integral_out]
    type = IntegralUserObject
    userobject = layered_integral
    q_triple_prime = reader
  []
  [./read_data]
    type = CSVReader
    csv_file = q_triple_prime.csv
    outputs = none
    delimiter = ","
  []
[]

[Executioner]
  type = Transient
  num_steps = 5
  dt = 1
  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  file_base = 'temperature_profile'
  csv = true
  exodus = true
[]