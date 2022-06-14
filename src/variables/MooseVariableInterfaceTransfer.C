#include "MooseVariableInterfaceTransfer.h"

#include "Assembly.h"
#include "MooseError.h" // mooseDeprecated
#include "MooseTypes.h"
#include "MooseVariableFE.h"
#include "MooseVariableFV.h"
#include "Problem.h"
#include "SubProblem.h"


// New includes for MultiApp stuff
#include "MultiApp.h"
#include "InputParameters.h"

template <typename T>
MooseVariableInterfaceTransfer<T>::MooseVariableInterfaceTransfer(const MooseObject * moose_object,
                                                  bool nodal,
                                                  std::string var_param_name,
                                                  Moose::VarKindType expected_var_type,
                                                  Moose::VarFieldType expected_var_field_type)        
  : _nodal(nodal), _moose_object(*moose_object)
{
  const InputParameters & parameters = _moose_object.parameters();
  THREAD_ID tid = parameters.get<THREAD_ID>("_tid");
  std::string variable_name = parameters.getMooseType(var_param_name);
  _fe_problem = _moose_object.getCheckedPointerParam<FEProblemBase *>("_fe_problem_base");
  _multi_app = _fe_problem->getMultiApp(_moose_object.getParam<MultiAppName>("multi_app"));
  FEProblemBase & master_problem = _multi_app->problemBase();

  if (variable_name == "")
  {
    auto vec = parameters.getVecMooseType(var_param_name);
    if (vec.empty())
      mooseError("Error constructing object '",
                 _moose_object.name(),
                 "' while retrieving value for '",
                 var_param_name,
                 "' parameter! Did you set ",
                 var_param_name,
                 " = '' (empty string) by accident?");
    variable_name = vec[0];
  }
  std::cout << "Variable name: " << variable_name << std::endl;
  _var = &master_problem.getVariable(tid, variable_name, expected_var_type, expected_var_field_type);
  if (!(_variable = dynamic_cast<MooseVariableFE<T> *>(_var)))
    _fv_variable = dynamic_cast<MooseVariableFV<T> *>(_var);
  _mvi_assembly = &master_problem.assembly(tid);
}

template <typename T>
MooseVariableInterfaceTransfer<T>::~MooseVariableInterfaceTransfer()
{
}

template <typename T>
MooseVariableFV<T> *
MooseVariableInterfaceTransfer<T>::mooseVariableFV() const
{
  if (!_fv_variable)
    mooseError("_fv_variable is null in ",
               _moose_object.name(),
               ". Did you forget to set fv = true in the Variables block?");
  return _fv_variable;
}

template <typename T>
MooseVariableFE<T> *
MooseVariableInterfaceTransfer<T>::mooseVariable() const
{
  if (!_variable)
    mooseError(
        "_variable is null in ", _moose_object.name(), ". Are you using a finite volume variable?");
  return _variable;
}

template <typename T>
const typename OutputTools<T>::VariableValue &
MooseVariableInterfaceTransfer<T>::value()
{
  if (_nodal)
    return _variable->dofValues();
  else
    return _variable->sln();
}

template <>
const VectorVariableValue &
MooseVariableInterfaceTransfer<RealVectorValue>::value()
{
  if (_nodal)
    mooseError("Dofs are scalars while vector variables have vector values. Mismatch");
  else
    return _variable->sln();
}

template <typename T>
const typename OutputTools<T>::VariableValue &
MooseVariableInterfaceTransfer<T>::valueOld()
{
  if (_nodal)
    return _variable->dofValuesOld();
  else
    return _variable->slnOld();
}

template <>
const VectorVariableValue &
MooseVariableInterfaceTransfer<RealVectorValue>::valueOld()
{
  if (_nodal)
    mooseError("Dofs are scalars while vector variables have vector values. Mismatch");
  else
    return _variable->slnOld();
}

template <typename T>
const typename OutputTools<T>::VariableValue &
MooseVariableInterfaceTransfer<T>::valueOlder()
{
  if (_nodal)
    return _variable->dofValuesOlder();
  else
    return _variable->slnOlder();
}

template <>
const VectorVariableValue &
MooseVariableInterfaceTransfer<RealVectorValue>::valueOlder()
{
  if (_nodal)
    mooseError("Dofs are scalars while vector variables have vector values. Mismatch");
  else
    return _variable->slnOlder();
}

template <typename T>
const typename OutputTools<T>::VariableValue &
MooseVariableInterfaceTransfer<T>::dot()
{
  if (_nodal)
    return _variable->dofValuesDot();
  else
    return _variable->uDot();
}

template <typename T>
const typename OutputTools<T>::VariableValue &
MooseVariableInterfaceTransfer<T>::dotDot()
{
  if (_nodal)
    return _variable->dofValuesDotDot();
  else
    return _variable->uDotDot();
}

template <typename T>
const typename OutputTools<T>::VariableValue &
MooseVariableInterfaceTransfer<T>::dotOld()
{
  if (_nodal)
    return _variable->dofValuesDotOld();
  else
    return _variable->uDotOld();
}

template <typename T>
const typename OutputTools<T>::VariableValue &
MooseVariableInterfaceTransfer<T>::dotDotOld()
{
  if (_nodal)
    return _variable->dofValuesDotDotOld();
  else
    return _variable->uDotDotOld();
}

template <>
const VectorVariableValue &
MooseVariableInterfaceTransfer<RealVectorValue>::dot()
{
  if (_nodal)
    mooseError("Dofs are scalars while vector variables have vector values. Mismatch");
  else
    return _variable->uDot();
}

template <>
const VectorVariableValue &
MooseVariableInterfaceTransfer<RealVectorValue>::dotDot()
{
  if (_nodal)
    mooseError("Dofs are scalars while vector variables have vector values. Mismatch");
  else
    return _variable->uDotDot();
}

template <>
const VectorVariableValue &
MooseVariableInterfaceTransfer<RealVectorValue>::dotOld()
{
  if (_nodal)
    mooseError("Dofs are scalars while vector variables have vector values. Mismatch");
  else
    return _variable->uDotOld();
}

template <>
const VectorVariableValue &
MooseVariableInterfaceTransfer<RealVectorValue>::dotDotOld()
{
  if (_nodal)
    mooseError("Dofs are scalars while vector variables have vector values. Mismatch");
  else
    return _variable->uDotDotOld();
}

template <typename T>
const VariableValue &
MooseVariableInterfaceTransfer<T>::dotDu()
{
  if (_nodal)
    return _variable->dofValuesDuDotDu();
  else
    return _variable->duDotDu();
}

template <typename T>
const VariableValue &
MooseVariableInterfaceTransfer<T>::dotDotDu()
{
  if (_nodal)
    return _variable->dofValuesDuDotDotDu();
  else
    return _variable->duDotDotDu();
}

template <typename T>
const typename OutputTools<T>::VariableGradient &
MooseVariableInterfaceTransfer<T>::gradient()
{
  if (_nodal)
    mooseError("gradients are not defined at nodes");

  return _variable->gradSln();
}

template <typename T>
const typename OutputTools<T>::VariableGradient &
MooseVariableInterfaceTransfer<T>::gradientOld()
{
  if (_nodal)
    mooseError("gradients are not defined at nodes");

  return _variable->gradSlnOld();
}

template <typename T>
const typename OutputTools<T>::VariableGradient &
MooseVariableInterfaceTransfer<T>::gradientOlder()
{
  if (_nodal)
    mooseError("gradients are not defined at nodes");

  return _variable->gradSlnOlder();
}

template <typename T>
const typename OutputTools<T>::VariableSecond &
MooseVariableInterfaceTransfer<T>::second()
{
  if (_nodal)
    mooseError("second derivatives are not defined at nodes");

  return _variable->secondSln();
}

template <typename T>
const typename OutputTools<T>::VariableSecond &
MooseVariableInterfaceTransfer<T>::secondOld()
{
  if (_nodal)
    mooseError("second derivatives are not defined at nodes");

  return _variable->secondSlnOld();
}

template <typename T>
const typename OutputTools<T>::VariableSecond &
MooseVariableInterfaceTransfer<T>::secondOlder()
{
  if (_nodal)
    mooseError("second derivatives are not defined at nodes");

  return _variable->secondSlnOlder();
}

template <typename T>
const typename OutputTools<T>::VariableTestSecond &
MooseVariableInterfaceTransfer<T>::secondTest()
{
  if (_nodal)
    mooseError("second derivatives are not defined at nodes");

  return _variable->secondPhi();
}

template <typename T>
const typename OutputTools<T>::VariableTestSecond &
MooseVariableInterfaceTransfer<T>::secondTestFace()
{
  if (_nodal)
    mooseError("second derivatives are not defined at nodes");

  return _variable->secondPhiFace();
}

template <typename T>
const typename OutputTools<T>::VariablePhiSecond &
MooseVariableInterfaceTransfer<T>::secondPhi()
{
  if (_nodal)
    mooseError("second derivatives are not defined at nodes");

  return _mvi_assembly->secondPhi(*_variable);
}

template <typename T>
const typename OutputTools<T>::VariablePhiSecond &
MooseVariableInterfaceTransfer<T>::secondPhiFace()
{
  if (_nodal)
    mooseError("second derivatives are not defined at nodes");

  return _mvi_assembly->secondPhiFace(*_variable);
}

template <typename T>
MooseVariableField<T> &
MooseVariableInterfaceTransfer<T>::mooseVariableField()
{
  if (_variable)
    return *_variable;
  else
  {
    if (!_fv_variable)
      mooseError("Either _variable or _fv_variable must be non-null in MooseVariableInterface");

    return *_fv_variable;
  }
}

template class MooseVariableInterfaceTransfer<Real>;
template class MooseVariableInterfaceTransfer<RealVectorValue>;
template class MooseVariableInterfaceTransfer<RealEigenVector>;