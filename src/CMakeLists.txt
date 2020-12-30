include_directories(${friasApp_SOURCE_DIR}/spdlog)
include_directories(${friasApp_SOURCE_DIR}/drogon)

file(GLOB SCP_LIST ${CMAKE_CURRENT_SOURCE_DIR}/views/*.csp)
foreach(cspFile ${SCP_LIST})
    message(STATUS "cspFile:" ${cspFile})
    get_filename_component(classname ${cspFile} NAME_WE)
    message(STATUS "view classname:" ${classname})
    add_custom_command(OUTPUT ${classname}.h ${classname}.cc
                    COMMAND drogon_ctl
                            ARGS
                            create
                            view
                            ${cspFile}
                    DEPENDS ${cspFile}
                    VERBATIM)
    set(VIEWSRC ${VIEWSRC} ${classname}.cc)
endforeach()

message( STATUS "Generate models")
execute_process(COMMAND "drogon_ctl" 
                        "create" 
                        "model" 
                        "-f" 
                        "." 
                WORKING_DIRECTORY ${friasApp_SOURCE_DIR}/src/models/
                RESULT_VARIABLE rv)

file(GLOB SOURCES "*.cc" "filters/*.cc" "models/*.cc")
add_executable(friasApp ${SOURCES} ${VIEWSRC})
target_link_libraries(friasApp spdlog drogon)