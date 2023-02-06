# boost dependency
# @copyright See `LICENSE` in the root directory of this project.
# This takes a while to build, consider running cmake with `-DFETCHCONTENT_QUIET=Off`

set(BOOST_INCLUDE_LIBRARIES url)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.81.0/boost-1.81.0.tar.xz
  URL_HASH SHA256=06bc525a392650eb6248f40a13f40112b6c485eec7103b6dcde7196f2a3570e0
)
FetchContent_MakeAvailable(Boost)
