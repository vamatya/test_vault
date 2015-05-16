# Copyright (c) 2007-2012 Hartmut Kaiser
# Copyright (c) 2011      Bryce Lelbach
# Copyright (c) 2015      Vinay C Amatya
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


set(tests
    component_bool_serialization
   )

#set(copy_component_PARAMETERS
#   LOCALITIES 2
#    THREADS_PER_LOCALITY 2)

#set(migrate_component_PARAMETERS
#    LOCALITIES 2
#    THREADS_PER_LOCALITY 2)

#set(inheritance_2_classes_abstract_FLAGS
#    DEPENDENCIES iostreams_component)


#add_hpx_pseudo_target(test_vault.unit_test)

foreach(test ${tests})
  set(sources
      ${test}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(${test}_test
                     SOURCES ${sources}
	   				 ${${test}_FLAGS}
                     FOLDER "Test_vault/Unit_test")

    
       #HPX_PREFIX ${HPX_BUILD_PREFIX}
       #EXCLUDE_FROM_ALL
	   #${${test}_FLAGS}

  #add_hpx_unit_test("components" ${test} ${${test}_PARAMETERS})

  # add a custom target for this example
  add_hpx_pseudo_target(test_vault.unit_test.${test})


  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(test_vault.unit_test
                              test_vault.unit_test.${test})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(test_vault.unit_test.${test}
                              ${test}_test_exe)

endforeach()


