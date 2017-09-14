# Look for sophus; note the difference in the directory specifications!
FIND_PATH(Sophus_INCLUDE_DIR NAMES sophus/scso3.h
  PATHS
  /usr/include
  /opt/local/include
  /usr/local/include
  /sw/include
  /usr/local/Sophus/include
  )

FIND_LIBRARY(Sophus_LIBRARY NAMES Sophus
  PATHS
  /usr/lib
  /usr/local/lib
  /opt/local/lib
  /sw/lib
  /usr/local/Sophus/lib
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sophus DEFAULT_MSG
  Sophus_INCLUDE_DIR Sophus_LIBRARY)
