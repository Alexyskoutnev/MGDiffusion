//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "MultiAppLayeredIntergral.h"

#include "libmesh/mesh_tools.h"

registerMooseObject("MooseApp", MultiAppLayeredIntergral);

InputParameters
MultiAppLayeredIntergral::validParams()
{
  InputParameters params = MultiAppElementIntegralVariableUserObject::validParams();
  params += LayeredBase::validParams();
  params.addClassDescription("Compute variable integrals over layers.");

  return params;
}

MultiAppLayeredIntergral::MultiAppLayeredIntergral(const InputParameters & parameters)
  : MultiAppElementIntegralVariableUserObject(parameters), LayeredBase(parameters)
{
  std::cout << "Start MultiAppLayeredIntergral" << std::endl;
}

void
MultiAppLayeredIntergral::initialize()
{
  MultiAppElementIntegralVariableUserObject::initialize();
  LayeredBase::initialize();
}

void
MultiAppLayeredIntergral::execute()
{
  Real integral_value = computeIntegral();

  unsigned int layer = getLayer(_current_elem->vertex_average());

  setLayerValue(layer, getLayerValue(layer) + integral_value);
}

void
MultiAppLayeredIntergral::finalize()
{
  LayeredBase::finalize();
}

void
MultiAppLayeredIntergral::threadJoin(const UserObject & y)
{
  MultiAppElementIntegralVariableUserObject::threadJoin(y);
  LayeredBase::threadJoin(y);
}

const std::vector<Point>
MultiAppLayeredIntergral::spatialPoints() const
{
  std::vector<Point> points;

  for (const auto & l : _layer_centers)
  {
    Point pt(0.0, 0.0, 0.0);
    pt(_direction) = l;
    points.push_back(pt);
  }

  return points;
}