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

set( GEN_MODELS_CMD "./drogon_ctl")
set( GEN_MODELS_CMD_ARG1 "create")
set( GEN_MODELS_CMD_ARG2 "model")
set( GEN_MODELS_CMD_ARG4 "-f")
set( GEN_MODELS_CMD_ARG5 "src/models/")
execute_process(COMMAND ${GEN_MODELS_CMD} 
                        ${GEN_MODELS_CMD_ARG1} 
                        ${GEN_MODELS_CMD_ARG2} 
                        ${GEN_MODELS_CMD_ARG3} 
                        ${GEN_MODELS_CMD_ARG4} 
                        ${GEN_MODELS_CMD_ARG5} 
                WORKING_DIRECTORY ${friasApp_SOURCE_DIR} 
                RESULT_VARIABLE rv)
message( STATUS "Generate models")

file(GLOB SOURCES "*.cpp" "*.cc" "models/*.cc")
add_executable(friasApp ${SOURCES} ${VIEWSRC})
target_link_libraries(friasApp spdlog drogon)