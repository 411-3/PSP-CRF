project(psp_crf)

find_package(OpenCV REQUIRED)

include_directories( include )

add_executable(psp_crf main.cpp)

target_link_libraries(psp_crf /home/xduser/lihuan/superpixel_crf/lib/libSLIC.a /home/xduser/lihuan/superpixel_crf/lib/libopencv_lbp.a)

target_link_libraries(psp_crf ${OpenCV_LIBS} )

target_link_libraries(psp_crf /home/xduser/lihuan/superpixel_crf/lib/libcnpy.a)

target_link_libraries(psp_crf /home/xduser/lihuan/superpixel_crf/lib/libdensecrf.a)





