if(NOT TARGET mgstringref)
    add_library(mgstringref INTERFACE)
    set_target_properties(mgstringref PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR})
endif()
