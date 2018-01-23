CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)

SET(IMG2RGB565_CONVERTER ${CMAKE_CURRENT_SOURCE_DIR}/imgTo565.py)

find_program (ImageMagick_convert_EXECUTABLE convert)

if (EXISTS ${ImageMagick_convert_EXECUTABLE})

function(Prepare_Images FILES)
    set(products)
    foreach(f ${FILES})
        get_filename_component(fn ${f} NAME_WE)
        set(png_file    ${CMAKE_CURRENT_BINARY_DIR}/${fn}.jpg)
        set(rgb565_file ${CMAKE_CURRENT_BINARY_DIR}/${fn}.565)

        add_custom_command(
            OUTPUT ${png_file}
            COMMAND
                 ${ImageMagick_convert_EXECUTABLE} ${f} ${png_file}
            DEPENDS
                ${f}
            VERBATIM
            )

        add_custom_command(
            OUTPUT ${rgb565_file}
            COMMAND
                 ${IMG2RGB565_CONVERTER} ${png_file} > ${rgb565_file}
            DEPENDS
                ${png_file}
            VERBATIM
            )
        list(APPEND products ${rgb565_file})
    endforeach()

    add_custom_target(IMG565
        SOURCES ${products}
        )
endfunction()

else()
    function(Prepare_Images FILES)
        add_custom_target(IMG565
            echo "No IMAGEMAGICK package installed"
            )
    endfunction()
endif()


SET(IMG2RGB565_CONVERTER    ${IMG2RGB565_CONVERTER}     PARENT_SCOPE)
