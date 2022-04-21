#pragma once

#include "MultiAppTransfer.h"
#include "GeneralUserObject.h"


// class MooseVariableFieldBase;
// namespace libMesh
// {
// class DofObject;
// }

class MultiAppUserObjectProvider;

class MultiAppVariablePointerTransfer : public MultiAppTransfer, GeneralUserObject
{
public:
  static InputParameters validParams();

  MultiAppVariablePointerTransfer(const InputParameters & parameters);
  virtual ~MultiAppVariablePointerTransfer();

  // virtual void initialSetup();

  void execute() override;
protected:
  // virtual std::vector<VariableName> getFromVarNames() const = 0;
  // virtual std::vector<AuxVariableName> getToVarNames() const = 0;
  virtual std::vector<VariableName> getFromVarNames() const { return _from_var_names; }
  // virtual std::vector<AuxVariableName> getToVarNames() const override { return _to_var_names; }
  const std::vector<VariableName> & _from_var_names;  
  const std::string & _user_object_type;
  const UserObjectName & _user_object_name;
  // MultiAppUserObjectProvider & _user_object_;



  // const std::vector<AuxVariableName> _to_var_names;
// protected:
//   /**
//    * Performs the transfer of a variable between two problems if they have the same mesh.
//    */
//   void transfer(FEProblemBase & to_problem, FEProblemBase & from_problem);

//   /**
//    * Performs the transfer of values between a node or element.
//    */
//   void transferDofObject(libMesh::DofObject * to_object,
//                          libMesh::DofObject * from_object,
//                          MooseVariableFieldBase & to_var,
//                          MooseVariableFieldBase & from_var,
//                          NumericVector<Number> & to_solution,
//                          NumericVector<Number> & from_solution);

//   /// Virtual function defining variables to be transferred
//   virtual std::vector<VariableName> getFromVarNames() const = 0;
//   /// Virtual function defining variables to transfer to
//   virtual std::vector<AuxVariableName> getToVarNames() const = 0;
};