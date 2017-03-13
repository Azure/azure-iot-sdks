
set(iothub_client_cpp_sample_amqp_cpp_files
    iothub_client_sample_amqp.cpp
)

set(iothub_client_cpp_sample_amqp_h_files
)

include_directories(.)

add_executable(iothub_client_cpp_sample_amqp
    ${iothub_client_cpp_sample_amqp_cpp_files}
    ${iothub_client_cpp_sample_amqp_h_files}
)

set_property(TARGET iothub_client_cpp_sample_amqp 
    PROPERTY CXX_STANDARD 11)

target_link_libraries(iothub_client_cpp_sample_amqp
    iothub_client_cpp
    iothub_client_amqp_transport)

linkSharedUtil(iothub_client_cpp_sample_amqp)
linkUAMQP(iothub_client_cpp_sample_amqp)