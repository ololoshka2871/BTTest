#ifndef SEP525_DMA_FREERTOS_H
#define SEP525_DMA_FREERTOS_H

#include <SEPS525_OLED.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#ifndef DMA_TRESHOLD
#define DMA_TRESHOLD 16
#endif

class Rectungle {
public:
    int x1, x2, y1, y2;
    Rectungle(int  x1 = 0, int y1 = 0, int x2 = 0, int y2 = 0) {
        this->x1 = x1;
        this->x2 = x2;
        this->y1 = y1;
        this->y2 = y2;
    }
    Rectungle(const Rectungle& src) {
        this->x1 = src.x1;
        this->x2 = src.x2;
        this->y1 = src.y1;
        this->y2 = src.y2;
    }

    size_t size() const;
    size_t width() const;
    size_t heigth() const;

    uint32_t offset2column(uint32_t offset) const;
    uint32_t offset2row(uint32_t offset) const;

    uint32_t offset2columnAbs(uint32_t offset) const;
    uint32_t offset2rowAbs(uint32_t offset) const;
};

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

    virtual void drawImage(const uint16_t *data, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    virtual void drawFragment(const uint16_t *data, size_t size_bytes,
                           uint16_t x, uint16_t y,
                           uint16_t w, uint16_t h,
                           uint16_t start_x, uint16_t start_y);
    virtual void drawFragment(const uint16_t *data, size_t size_bytes,
                           const Rectungle& rect, uint16_t start_x, uint16_t start_y);

protected:
    void setup();
    void set_region(int x, int y, int w, int h);
    virtual void set_region(int x, int y, int w, int h, int start_x, int start_y);

    virtual void select_region(const Region& region);

    virtual void set_region(const Region& region);

    virtual void send_fill_color(uint16_t color, uint32_t size);

private:
    Region currentRegion;
};
#endif // SEP525_DMA_FREERTOS_H
