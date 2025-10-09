set(ARCH_SUBDIR "x86_64-w64-mingw32")
if (MSVC)
  set(ARCH_SUBDIR "")
endif ()

function(fetch_sdl3 pkg version)
  CPMAddPackage(
          NAME SDL3${pkg}
          URL "https://github.com/libsdl-org/SDL${pkg}/releases/download/release-${version}/SDL3${pkg}-devel-${version}-mingw.zip"
          DOWNLOAD_ONLY YES
  )
  set(SDL3${pkg}_DIR ${SDL3${pkg}_SOURCE_DIR}/${ARCH_SUBDIR} PARENT_SCOPE)
endfunction()

fetch_sdl3("" 3.2.24)
fetch_sdl3(_ttf 3.2.2)
fetch_sdl3(_image 3.2.4)