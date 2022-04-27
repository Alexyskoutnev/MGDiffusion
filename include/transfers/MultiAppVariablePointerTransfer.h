#pragma once

#include "MultiAppTransfer.h"
#include "UserObjectInterface.h"


// class UserObjectInterface;


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
  // FEProblemBase & _to_problem;
  const std::vector<VariableName> & _from_var_names;  
  const UserObject & getUserObject(const std::string &) const;
  const UserObject & getUserObjectBaseByName(const UserObjectName & object_name) const;
  const UserObject & getUserObjectBase(const std::string & param_name) const;
  const UserObject & castUserObject(const UserObject & uo_base, const std::string & param_name) const;
  const MooseVariableFieldBase * _multi_app_var;

  // const std::string & _user_object_type;
  // FEProblemBase & _subfeproblem;
};