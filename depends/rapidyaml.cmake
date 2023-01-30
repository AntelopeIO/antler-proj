# rapidyaml (ryml) dependency
# @copyright See `LICENSE` in the root directory of this project.


if(true)

  FetchContent_Declare(
    ryml
    SYSTEM
    GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
    # Always use a full commit hash to ensure a tag change in the source
    # repo doesn't change the behavior of our code.
    GIT_TAG        213b201d264139cd1b887790197e08850af628e3 # v0.4.1

    # Patch that allows for:
    #   `operator std::string_view()`
    #   C++17 standard
    PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/patches/rapidyaml.patch
  )

  FetchContent_MakeAvailable(ryml)

endif()



if(false)

  # For whatever reason, this path doesn't work.
  #
  # The reason to prefer this route in package building is that files
  # are easier to mirror than github repos.

  FetchContent_Declare(
    ryml
    SYSTEM
    URL      https://github.com/biojppm/rapidyaml/archive/refs/tags/v0.4.1.tar.gz
    URL_HASH SHA256=b4ef0300b5899ede26ed529d8a8daa3347810693957707dbe522f69b17250ae2
    DOWNLOAD_NAME rapidyaml-v0.4.1.tar.gz
  )

  FetchContent_Populate(ryml)

  FetchContent_Declare(
    c4core
    SYSTEM
    SOURCE_DIR ${ryml_SOURCE_DIR}/ext/c4core/
    URL        https://github.com/biojppm/c4core/archive/refs/tags/v0.1.9.tar.gz
    URL_HASH   SHA256=12aba7d04e77ce5e7c5edc3165e700f7314b0f67e3706ad285bb7cb2020c52af
    DOWNLOAD_NAME c4core-v0.1.9.tar.gz
  )

  FetchContent_Populate(c4core)


  FetchContent_MakeAvailable(ryml)

endif()
