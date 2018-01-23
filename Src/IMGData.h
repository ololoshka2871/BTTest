#ifndef IMGDATA_H
#define IMGDATA_H

struct imgdata{
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  unsigned char	 pixel_data[64 * 64 * 2 + 1];
};

extern const imgdata test;

#endif // IMGDATA_H
