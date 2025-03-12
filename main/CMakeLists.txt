set(srcs "main.c" 
    "PSensor/PSensorDrv.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "PSensor" "Common")
