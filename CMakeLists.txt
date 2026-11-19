
cmake_minimum_required(VERSION 3.20)

project(MarketDataFeedHandler
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(feed_handler
    src/FeedProtocol.cpp
    src/OrderTracker.cpp
    src/UdpPublisher.cpp
    src/UdpReceiver.cpp
)
target_compile_features(feed_handler PUBLIC cxx_std_17)

target_include_directories(feed_handler
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

add_executable(test_protocol tests/test_protocol.cpp)
target_link_libraries(test_protocol PRIVATE feed_handler)
add_test(NAME protocol COMMAND test_protocol)

add_executable(test_event tests/test_event.cpp)
target_link_libraries(test_event PRIVATE feed_handler)
add_test(NAME event COMMAND test_event)

add_executable(test_udp_receiver tests/test_udp_receiver.cpp)
target_link_libraries(test_udp_receiver PRIVATE feed_handler)
add_test(NAME udp_receiver COMMAND test_udp_receiver)

add_executable(test_udp_pipeline tests/test_udp_pipeline.cpp)
target_link_libraries(test_udp_pipeline PRIVATE feed_handler)
add_test(NAME udp_pipeline COMMAND test_udp_pipeline)