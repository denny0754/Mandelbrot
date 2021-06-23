set(ALL_SOURCES 
	Main.cpp
	MandelbrotGui.cpp
	MandelbrotUtils.cpp
)

include_directories(
	../vendors/include/
	../include/
)

add_executable(Mandelbrot ${ALL_SOURCES})

# Add `libs` folder
target_link_directories(Mandelbrot PUBLIC "../libs/")

set(ALL_LIBS sfml-system sfml-graphics sfml-window pthread)

# Links the program and all libraries requested
target_link_libraries(
  Mandelbrot
  PRIVATE project_options
        #   project_warnings
          CONAN_PKG::docopt.cpp
          CONAN_PKG::fmt
          CONAN_PKG::spdlog
		  ${ALL_LIBS}
)