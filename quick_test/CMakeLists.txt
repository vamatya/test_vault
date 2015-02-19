# Copyright (c) 2007-2012 Hartmut Kaiser
# Copyright (c) 2011      Bryce Lelbach
# Copyright (c) 2015      Vinay C Amatya
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


set(tests
    tuple_future
    migrate_component
   )

#set(copy_component_PARAMETERS
#   LOCALITIES 2
#    THREADS_PER_LOCALITY 2)

set(migrate_component_PARAMETERS
    LOCALITIES 2
    THREADS_PER_LOCALITY 2)

#set(inheritance_2_classes_abstract_FLAGS
#    DEPENDENCIES iostreams_component)

foreach(test ${tests})
  set(sources
      ${test}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(${test}_test
                     SOURCES ${sources}
                     ${${test}_FLAGS}
                     EXCLUDE_FROM_ALL
                     HPX_PREFIX ${HPX_BUILD_PREFIX}
                     FOLDER "Tests/Unit/Components")

  add_hpx_unit_test("components" ${test} ${${test}_PARAMETERS})

  # add a custom target for this example
  add_hpx_pseudo_target(tests.unit.components.${test})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(tests.unit.components
                              tests.unit.components.${test})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(tests.unit.components.${test}
                              ${test}_test_exe)

endforeach()


