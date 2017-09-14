# Look for Pangolin; note the difference in the directory specifications!
FIND_PATH(Pangolin_INCLUDE_DIR NAMES pangolin/pangolin.h
        PATHS
        /usr/include
        /opt/local/include
        /usr/local/include
        /sw/include
        /usr/local/Pangolin/include
        )

FIND_LIBRARY(Pangolin_LIBRARY NAMES Pangolin
        PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        /usr/local/Pangolin/lib
        )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Pangolin DEFAULT_MSG
        Pangolin_INCLUDE_DIR Pangolin_LIBRARY)
