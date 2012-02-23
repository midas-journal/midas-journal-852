cmake_minimum_required(VERSION 2.8)
#Change PROJECT_NAME to the name of your project
PROJECT(perimeterEstimation)

# set the name of the input image used to run the test
SET(INPUT_IMAGE ${CMAKE_SOURCE_DIR}/images/cthead1.png)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)
FIND_PROGRAM(ITK_TEST_DRIVER itkTestDriver)
SET(TEST_COMMAND ${ITK_TEST_DRIVER} --add-before-env PATH ${CMAKE_BINARY_DIR})

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
  ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  ${ITK_LIBRARIES}
)


# install devel files
OPTION(INSTALL_DEVEL_FILES "Install C++ headers" ON)
IF(INSTALL_DEVEL_FILES)
FILE(GLOB develFiles *.h *.txx) 
FOREACH(f ${develFiles})
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${f})
ENDFOREACH(f)
ENDIF(INSTALL_DEVEL_FILES)



# option for wrapping
OPTION(BUILD_WRAPPERS "Wrap library" OFF)
IF(BUILD_WRAPPERS)
   SUBDIRS(Wrapping)
ENDIF(BUILD_WRAPPERS)
   
   

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project

IF(BUILD_TESTING)

SET(CurrentExe "check2")
ADD_EXECUTABLE(check2 check.cxx)
TARGET_LINK_LIBRARIES(check2 ${Libraries})
set_target_properties(check2 PROPERTIES COMPILE_FLAGS -DDIM=2)

SET(CurrentExe "label_check2")
ADD_EXECUTABLE(label_check2 label_check.cxx)
TARGET_LINK_LIBRARIES(label_check2 ${Libraries})
set_target_properties(label_check2 PROPERTIES COMPILE_FLAGS -DDIM=2)

SET(CurrentExe "check3")
ADD_EXECUTABLE(check3 check.cxx)
TARGET_LINK_LIBRARIES(check3 ${Libraries})

SET(CurrentExe "label_check3")
ADD_EXECUTABLE(label_check3 label_check.cxx)
TARGET_LINK_LIBRARIES(label_check3 ${Libraries})


SET(CurrentExe "lm2")
ADD_EXECUTABLE(lm2 lm.cxx itkGeometryUtilities.cxx)
TARGET_LINK_LIBRARIES(lm2 ${Libraries})
set_target_properties(lm2 PROPERTIES COMPILE_FLAGS -DDIM=2)

SET(CurrentExe "glm2")
ADD_EXECUTABLE(glm2 lm.cxx itkGeometryUtilities.cxx)
TARGET_LINK_LIBRARIES(glm2 ${Libraries})
set_target_properties(glm2 PROPERTIES COMPILE_FLAGS "-DDIM=2 -DITK_DO_NOT_USE_PERIMETER_SPECIALIZATION")

SET(CurrentExe "lm3")
ADD_EXECUTABLE(lm3 lm.cxx itkGeometryUtilities.cxx)
TARGET_LINK_LIBRARIES(lm3 ${Libraries})

SET(CurrentExe "glm3")
ADD_EXECUTABLE(glm3 lm.cxx itkGeometryUtilities.cxx)
TARGET_LINK_LIBRARIES(glm3 ${Libraries})
set_target_properties(glm3 PROPERTIES COMPILE_FLAGS "-DITK_DO_NOT_USE_PERIMETER_SPECIALIZATION")

FIND_PACKAGE(VTK)
if(VTK_DIR)
  INCLUDE(${VTK_USE_FILE})
  
  ADD_EXECUTABLE(perf3D perf3D.cxx)
  TARGET_LINK_LIBRARIES(perf3D vtkHybrid ${Libraries})
endif()

ENDIF(BUILD_TESTING)

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
# ADD_TEST(Testname ExecutableToRun arg1 arg2 arg3)

ADD_TEST(Test ${TEST_COMMAND}
   check ${INPUT_IMAGE} out.png
   --compare out.png ${CMAKE_SOURCE_DIR}/images/test.png
)
