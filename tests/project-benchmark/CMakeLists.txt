add_executable(project-benchmark)
target_link_libraries(project-benchmark benchmark::benchmark)

# this would probably be better using project as a library, but it's an executable
target_include_directories(project-benchmark PRIVATE ${PROJECT_ROOT})

target_sources(project-benchmark
	PRIVATE
		project-bench.cpp
		# project-level files here
		# ${PROJECT_ROOT}/src/<file_path_here>
)
