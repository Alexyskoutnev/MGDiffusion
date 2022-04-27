#include "MultiAppVariablePointerTransfer.h"
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "MooseVariableFEBase.h"
#include "UserObject.h"
// #include "UserObjectInterface.h"


// registerMooseObject("MGDiffusionApp", MultiAppVariablePointerTransfer);
registerMooseObject("MooseApp", MultiAppVariablePointerTransfer);

InputParameters
MultiAppVariablePointerTransfer::validParams()
{
    InputParameters params = MultiAppTransfer::validParams();
    params.addRequiredParam<MultiAppName>("multi_app", "The name of the multiapp that the user object interacts with");
    params.addRequiredParam<std::vector<VariableName>>("variable", "The name of the variable that this object operates on");
    // params.addRequiredParam<std::string>("user_object_type", "The name of the userobject type that this object operates on");
    params.addRequiredParam<UserObjectName>("user_object_name", "The name of the userobject that this object operates on");
    params.addClassDescription("obtains the multiapp FEProblem pointer to be used by subapps");
  return params;
}

MultiAppVariablePointerTransfer::MultiAppVariablePointerTransfer(const InputParameters & parameters) : MultiAppTransfer(parameters), 
_from_var_names(getParam<std::vector<VariableName>>("variable")), _user_object_name(getParam<UserObjectName>("user_object_name")), UserObjectInterface(this),
_from_problem(_multi_app->problemBase())
{

    // MultiAppVariablePointerTransfer MultiappProvider = MultiAppVariablePointerTransfer(parameters);
    // MultiAppVariablePointerTransfer _user_object = MultiappProvider.getUserObject<MultiAppUserObjectProvider>("user_object_name");
    std::cout << "In MultiAppVariablePointerTransfer" << std::endl;
    // _user_object_ = MultiAppVariablePointerTransfer.getUserObject
//   /* Right now, most of derived transfers support one variable only */
//   _to_var_name = _to_var_names[0];
//   _from_var_name = _from_var_names[0];
}

MultiAppVariablePointerTransfer::~MultiAppVariablePointerTransfer() {}

void
MultiAppVariablePointerTransfer::execute()
{
  _console << "Beginning MultiAppVariablePointerTransfer " << name() << std::endl;
  // FEProblemBase & _from_problem = _multi_app->problemBase();
  _console << "app count: " <<  _multi_app->numGlobalApps() << std::endl;

  MooseVariableFieldBase & from_var = _from_problem.getVariable(0, getFromVarNames()[0], Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_ANY);
  for (unsigned int i = 0; i < _multi_app->numGlobalApps(); i++){
    FEProblemBase & to_problem  = _multi_app->appProblemBase(i);
    const UserObject & layered_integral_object = getUserObject(_user_object_name);
    _console << layered_integral_object.typeAndName() << " " << std::endl;
    // layered_integral_object.setVarPointer(from_var);
  }
  _console << "Finished MultiAppVariablePointerTransfer " << name() << std::endl;
  
}

const UserObject &
MultiAppVariablePointerTransfer::getUserObject(const std::string & param_name) const
{
  return castUserObject(getUserObjectBase(param_name), param_name);
}

const UserObject &
MultiAppVariablePointerTransfer::getUserObjectBase(const std::string & param_name) const
{
  const auto object_name = _user_object_name;
  std::cout << object_name << std::endl;
  return getUserObjectBaseByName(object_name);
}

const UserObject &
MultiAppVariablePointerTransfer::getUserObjectBaseByName(const UserObjectName & object_name) const
{
  FEProblemBase & to_problem  = _multi_app->appProblemBase(0);
  return to_problem.getUserObjectBase(object_name, 0);
}

const UserObject &
MultiAppVariablePointerTransfer::castUserObject(const UserObject & uo_base, const std::string & param_name) const
{
  const UserObject * uo = dynamic_cast<const UserObject *>(&uo_base);
  return *uo;
}
