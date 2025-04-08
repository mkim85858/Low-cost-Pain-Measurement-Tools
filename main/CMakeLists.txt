set(srcs "main.c" 
    "Common/I2CDrv.c"
    "PSensor/PSensorDrv.c"
    "LCD/LCDDrv.c"
    "Bluetooth/Bluetooth.c"
    )

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "PSensor" "Common" "Button" "Bluetooth" "LCD")
