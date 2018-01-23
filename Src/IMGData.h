#ifndef IMGDATA_H
#define IMGDATA_H

struct imgdata{
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */
  const char	 *pixel_data;
};

extern const imgdata test;
extern const imgdata arrow ;

#endif // IMGDATA_H
