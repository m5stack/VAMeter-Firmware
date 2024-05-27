
file(GLOB_RECURSE APP_LAYER_SRCS
    ../../../app/*.c
    ../../../app/*.cpp
)

set(APP_LAYER_INCS
    ../../../app
)

file(GLOB_RECURSE HAL_VAMETER_SRCS
    ./hal_vameter/*.c
    ./hal_vameter/*.cpp
)

idf_component_register(SRCS "vameter_user_demo.cpp" ${APP_LAYER_SRCS} ${HAL_VAMETER_SRCS}
                    INCLUDE_DIRS "." ${APP_LAYER_INCS})
