set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILDIN_STL_SUPPORT YES)

doxygen_add_docs(docs
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/include"
)

