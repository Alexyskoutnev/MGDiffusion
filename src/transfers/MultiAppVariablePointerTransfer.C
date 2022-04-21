#include "MultiAppVariablePointerTransfer.h"
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "MooseVariableFEBase.h"
// #include "MultiAppUserObjectProvider.h"


registerMooseObject("MooseApp", MultiAppVariablePointerTransfer);

InputParameters
MultiAppVariablePointerTransfer::validParams()
{
    InputParameters params = MultiAppTransfer::validParams();
    params.addRequiredParam<MultiAppName>("multi_app", "The name of the multiapp that the user object interacts with");
    params.addRequiredParam<std::vector<VariableName>>("variable", "The name of the variable that this object operates on");
    params.addRequiredParam<std::string>("user_object_type", "The name of the userobject type that this object operates on");
    params.addRequiredParam<UserObjectName>("user_object_name", "The name of the userobject that this object operates on");
    params.addClassDescription("obtains the multiapp FEProblem pointer to be used by subapps");
  return params;
}

MultiAppVariablePointerTransfer::MultiAppVariablePointerTransfer(const InputParameters & parameters) : MultiAppTransfer(parameters), 
_from_var_names(getParam<std::vector<VariableName>>("variable")), _user_object_type(getParam<std::string>("user_object_type")), _user_object_name(getParam<UserObjectName>("user_object_name"))
// _user_object_(getUserObject<MultiAppUserObjectProvider>("user_object_name"))
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
  FEProblemBase & from_problem = _multi_app->problemBase();
  _console << "app count: " <<  _multi_app->numGlobalApps() << std::endl;

  MooseVariableFieldBase & from_var = from_problem.getVariable(0, getFromVarNames()[0], Moose::VarKindType::VAR_ANY, Moose::VarFieldType::VAR_FIELD_ANY);
  for (unsigned int i = 0; i < _multi_app->numGlobalApps(); i++){
    FEProblemBase & to_problem  = _multi_app->appProblemBase(i);
    // const UserObject & layered_integral_object = getUserObject<UserObject>(_user_object_name, 0);
    // _console << layered_integral_object._name << " " << layered_integral_object._type << std::endl;
    // layered_integral_object.setVarPointer(from_var);
  }
  _console << "Finished MultiAppVariablePointerTransfer " << name() << std::endl;
  
}