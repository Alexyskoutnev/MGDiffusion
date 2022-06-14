#include "MultiAppVariablePointerTransfer.h"
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "MooseVariableFEBase.h"
#include "UserObject.h"

registerMooseObject("MooseApp", MultiAppVariablePointerTransfer);

InputParameters
MultiAppVariablePointerTransfer::validParams()
{
    InputParameters params = MultiAppTransfer::validParams();
    params.addRequiredParam<MultiAppName>("multi_app", "The name of the multiapp that the user object interacts with");
    params.addRequiredParam<std::vector<VariableName>>("variable", "The name of the variable that this object operates on");
    params.addRequiredParam<UserObjectName>("user_object_name", "The name of the userobject that this object operates on");
    params.addClassDescription("obtains the multiapp FEProblem pointer to be used by subapps");
  return params;
}

MultiAppVariablePointerTransfer::MultiAppVariablePointerTransfer(const InputParameters & parameters) : MultiAppTransfer(parameters), 
_from_var_names(getParam<std::vector<VariableName>>("variable")), _user_object_name(getParam<UserObjectName>("user_object_name")), UserObjectInterface(this),
_from_problem(_multi_app->problemBase())
{
}

MultiAppVariablePointerTransfer::~MultiAppVariablePointerTransfer() {}

void
MultiAppVariablePointerTransfer::execute()
{
  MooseVariableFieldBase & from_var = _from_problem.getVariable(0, getFromVarNames()[0], Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_ANY);
  for (unsigned int i = 0; i < _multi_app->numGlobalApps(); i++){
    FEProblemBase & to_problem  = _multi_app->appProblemBase(i);
    MutliAppLayeredIntegral & layered_integral_object = getUserObject<MutliAppLayeredIntegral>(_user_object_name);
    layered_integral_object.setVarPointer(&from_var);
  }
}