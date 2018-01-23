#ifndef RAWIMAGELOADER_H
#define RAWIMAGELOADER_H

#include "DisplayController.h"
#include "SEP525_DMA_FreeRTOS.h"

template<int FragmentSize>
class RawImageLoader : public IPipeLine {
public:
    RawImageLoader(DisplayController* controller,
                   const std::shared_ptr<FatFile> &file,
                   const Rectungle& rect,
                   const uint32_t offset,
                   const uint8_t bytesPrePixel = 1)
        : file(file), position(rect), bytesPrePixel(bytesPrePixel)
    {
        this->controller = controller;
        this->offset = offset;
        read = 0;
    }

    bool processFS(SdFat& fs) {
        if (!file->isOpen())
            return false;

        volatile uint32_t points_to_read;
        uint32_t bytes_to_read;

        if (!file->seekSet(offset))
            goto fail;

        points_to_read = position.PixelsRemaning(offset / bytesPrePixel);
        if (points_to_read <= 0) {
            goto fail;
        }
        bytes_to_read = min(
                    min(file->available(), points_to_read * bytesPrePixel),
                    FragmentSize);

        read = file->read(buf, bytes_to_read);
        if (read < 0)
            goto fail;

        points_to_read = position.PixelsRemaning((offset + read) / bytesPrePixel);
        if (points_to_read == 0)
            file->close();

        return true;

fail:
        file->close();
        return false;
    }

    void processDisplay(SEP525_DMA_FreeRTOS& display) {
        uint32_t pixeloffset = offset / display.BytesPrePixel();
        display.drawFragment((const uint16_t*)buf, read, position,
                             position.offset2columnAbs(pixeloffset),
                             position.offset2rowAbs(pixeloffset));
    }

protected:
    DisplayController *controller;
    std::shared_ptr<FatFile> file;
    uint32_t offset, read;
    Rectungle position;
    const uint8_t bytesPrePixel;

    uint8_t buf[FragmentSize];
};

#endif // RAWIMAGELOADER_H
