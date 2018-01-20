#ifndef SEP525_DMA_FREERTOS_H
#define SEP525_DMA_FREERTOS_H

#include <SEPS525_OLED.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

class SEP525_DMA_FreeRTOS : public SEPS525_OLED {
private:
    class Region {
    public:
        Region(int size_x, int size_y) {
            this->size_x = size_x;
            this->size_y = size_y;

            x = y = xs = ys = 0;
            all_ocupied = false;
        }

        Region& operator=(const Region& src) {
            x = src.x;
            y = src.y;
            xs = src.xs;
            ys = src.ys;
            all_ocupied = src.all_ocupied;
            return *this;
        }

        bool operator ==(const Region& other) const {
            return (x == other.x) && (y == other.y) && (xs == other.xs) && (ys == other.ys);
        }

        bool compare(int x, int y, int w, int h) const {
            return (this->x == x) && (this->y == y) && (xs == w) && (ys == h);
        }

        Region& update(int x, int y, int xs, int ys) {
            this->x = x;
            this->y = y;
            this->xs = xs;
            this->ys = ys;

            all_ocupied = isCocupied();

            return *this;
        }

        Region& extendTo(int x, int y) {
            this->x = min(this->x, x);
            this->y = min(this->y, y);

            this->xs = max(this->xs, x);
            this->ys = max(this->ys, y);

            all_ocupied = isCocupied();

            return *this;
        }

        bool contains(int x, int y) {
            return all_ocupied || ( (x >= this->x) && (y >= this->y) &&
                    (x <= this->xs) && (y <= this->ys));
        }

        bool contains(int x, int y, int xs, int ys) {
            return all_ocupied || ((x >= this->x) && (y >= this->y) &&
                    (xs <= this->xs) && (ys <= this->ys));
        }

        bool isCocupied() const {
            return (x == 0) && (y == 0) && (xs == size_x) && (ys == size_y);
        }

        Region& fill() {
            all_ocupied = true;
            this->x = 0;
            this->y = 0;

            this->xs = size_x;
            this->ys = size_y;
            return *this;
        }

        int size_x, size_y;
        int x, y, xs, ys;
        bool all_ocupied;
    };

    SEP525_DMA_FreeRTOS();

    static SEP525_DMA_FreeRTOS *inst;

    static SemaphoreHandle_t mutex;

    static void DMA_callback();

public:
    static SEP525_DMA_FreeRTOS *instance();

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

protected:
    void setup();
    void set_region(int x, int y, int w, int h);

    virtual void select_region(const Region& region);

    virtual void set_region(const Region& region);

private:
    Region currentRegion;
};
#endif // SEP525_DMA_FREERTOS_H
