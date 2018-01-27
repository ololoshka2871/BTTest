CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)

file(GLOB_RECURSE
    IMG_SRC
    FOLLOW_SYMLINKS
    "*.xcf"
    "*.jpg"
    )

add_custom_target(img_prj SOURCES ${IMG_SRC})

Prepare_Images("${IMG_SRC}")
