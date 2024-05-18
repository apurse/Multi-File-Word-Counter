

add_executable( project-unit )
target_link_libraries( project-unit GTest::GTest )

# this would probably be better using project as a library, but it's an executable
target_include_directories( project-unit PRIVATE ${PROJECT_ROOT} )

target_sources( project-unit
	PRIVATE
		project-unit.cpp
		# cpp files from project here
		#		${PROJECT_ROOT}
)

add_test( NAME unit-tests
	  COMMAND project-unit
	)
