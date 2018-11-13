add_library(hokuyo_urg STATIC Urg_driver.cpp 
                              Connection_information.cpp 
                              Urg_func.cpp 
                              urg_debug.c 
                              urg_serial_linux.c 
                              urg_serial_utils.c 
                              urg_serial_utils_linux.c 
                              urg_ring_buffer.c 
                              urg_tcpclient.c 
                              urg_sensor.c 
                              urg_utils.c 
                              urg_connection.c  
                              urg_serial.c 
                              ticks.cpp 
                              MR2_scan2point.cpp
                              )
