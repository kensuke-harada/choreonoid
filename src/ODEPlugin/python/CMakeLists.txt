
# @author Hisashi Ikari

if(BUILD_ODE_PLUGIN)
  set(ode ODE)
endif()
if(BUILD_GAZEBO_ODE_PLUGIN)
  set(gazebo_ode GAZEBO_ODE)
endif()
set(versions ${ode} ${gazebo_ode})

foreach(version ${versions})
  set(target Py${version}Plugin)
  add_cnoid_python_module(${target} PyODEPlugin.cpp)
  target_link_libraries(${target} Cnoid${version}Plugin CnoidPyBase ${PYTHON_LIBRARIES} ${Boost_PYTHON_LIBRARY})
endforeach()
