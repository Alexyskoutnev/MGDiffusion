#pragma once

#include "MultiAppTransfer.h"
#include "UserObjectInterface.h"
#include "MultiAppLayeredIntegral.h"

class MultiAppVariablePointerTransfer : public MultiAppTransfer, public UserObjectInterface
{
public:
  static InputParameters validParams();

  MultiAppVariablePointerTransfer(const InputParameters & parameters);
  
  virtual ~MultiAppVariablePointerTransfer();

  void execute() override;

protected:
  virtual std::vector<VariableName> getFromVarNames() const { return _from_var_names; }
  const UserObjectName & _user_object_name;
  FEProblemBase & _from_problem;
  const std::vector<VariableName> & _from_var_names;  
  const MooseVariableFieldBase * _multi_app_var;

  template <class T>
  T &
  getUserObject(const std::string & param_name)
  {
    return castUserObject<T>(getUserObjectBase(param_name), param_name);
  }

  UserObject &
  getUserObjectBase(const std::string & param_name)
  {
    auto object_name = _user_object_name; 
    return const_cast<UserObject &>(getUserObjectBaseByName(object_name));
  }

  const UserObject &
  getUserObjectBaseByName(const UserObjectName & object_name)
  {
    FEProblemBase & to_problem  = _multi_app->appProblemBase(0);
    return to_problem.getUserObjectBase(object_name, 0);
  }

  template <class T>
  T &
  castUserObject(UserObject & uo_base, const std::string & param_name)
  {
    T * uo = dynamic_cast<T *>(&uo_base);
    return *uo;
  }

};