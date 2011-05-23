// __BEGIN_LICENSE__
// Copyright (C) 2006-2011 United States Government as represented by
// the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
// __END_LICENSE__


/// \file DEM.h
///

#ifndef __DEM_H__
#define __DEM_H__

#include <vw/Core/Exception.h>
#include <vw/Image/ImageView.h>
#include <vw/Image/PixelTypes.h>
#include <vw/Math/BBox.h>
#include <vw/Cartography/GeoReference.h>
#include <string>
#include <iostream>
#include <algorithm>

void write_GMT_script(std::string const prefix,
                      int width, int height, double min_value, double max_value,
                      double scale_factor,
                      vw::cartography::GeoReference const& georef);

// Some template type computation machinery to help us write ENVI header files
template <class ChannelT> struct vw_to_envi_channel_type {};

template<> struct vw_to_envi_channel_type<vw::uint8> { static int value() { return 1; } };
template<> struct vw_to_envi_channel_type<vw::int16> { static int value() { return 2; } };
template<> struct vw_to_envi_channel_type<vw::int32> { static int value() { return 3; } };
template<> struct vw_to_envi_channel_type<vw::float32> { static int value() { return 4; } };
template<> struct vw_to_envi_channel_type<vw::float64> { static int value() { return 5; } };
template<> struct vw_to_envi_channel_type<vw::uint16> { static int value() { return 12; } };
template<> struct vw_to_envi_channel_type<vw::uint32> { static int value() { return 13; } };

// Write an ENVI compatible DEM header
template <class ViewT>
void write_ENVI_header(std::string header_name, 
                       double default_z_value,
                       vw::ImageViewBase<ViewT> const& image,
                       vw::cartography::GeoReference georef) {

  FILE *headerFile = fopen(header_name.c_str(), "w");
  
  fprintf(headerFile, "ENVI\n");
  fprintf(headerFile, "description = { \n");
  fprintf(headerFile, "   -- Digital Elevation Map generated by the NASA Ames Stereo Pipeline --\n");
  fprintf(headerFile, "   \n");
  fprintf(headerFile, "   The Ames Stereo Pipeline generates 3D coordinates in a planetocentric \n");
  fprintf(headerFile, "   coordinate system with the origin at the planetary center of mass.\n");
  fprintf(headerFile, "   Elevations are referenced relative to standards set by the International\n");
  fprintf(headerFile, "   Astronomical Union (IAU)\n");
  fprintf(headerFile, "   \n");
  fprintf(headerFile, "   This DEM was generated using an area based correlation technique followed by several \n");
  fprintf(headerFile, "   several stages of outlier detection.  This produced a map of the disparities for each \n");
  fprintf(headerFile, "   pixel which was then interpolated (hole filled) using a 2D non-uniform b-spline fitting\n");
  fprintf(headerFile, "   process.  You should find a interpolation map file included in this archive that shows\n");
  fprintf(headerFile, "   which pixels represent interpolated data.\n");
  fprintf(headerFile, "   \n");
  fprintf(headerFile, "   Projection Information:\n");

  std::ostringstream temp;
  temp << georef;
  fprintf(headerFile, "   %s\n", temp.str().c_str());
  
  vw::BBox2 bounding_box = georef.bounding_box(image.impl().cols(), image.impl().rows());
  double min_value = *(std::min_element(image.begin(), image.end()));
  double max_value = *(std::max_element(image.begin(), image.end()));
  
  fprintf(headerFile, "   \n");
  fprintf(headerFile, "   Bounding box:\n");
  fprintf(headerFile, "     Minimum X (left)    = %f\n", bounding_box.min().x());
  fprintf(headerFile, "     Minimum Y (top)     = %f\n", bounding_box.min().y());
  fprintf(headerFile, "     Maximum X (right)   = %f\n", bounding_box.max().x());
  fprintf(headerFile, "     Maximum Y (bottom)  = %f\n", bounding_box.max().y());
  fprintf(headerFile, "     Minimum Z           = %f\n", min_value);
  fprintf(headerFile, "     Maximum Z           = %f\n", max_value);
  fprintf(headerFile, "     Default Z           = %f\n", default_z_value);
  fprintf(headerFile, "}\n");
  fprintf(headerFile, "samples = %d\n", image.impl().cols());
  fprintf(headerFile, "lines   = %d\n", image.impl().rows());
  fprintf(headerFile, "bands   = %d\n", image.impl().planes());
  fprintf(headerFile, "header offset = 0\n");

  double x_pixel_scaling = georef.transform()(0,0);
  double y_pixel_scaling = georef.transform()(1,1);
  fprintf(headerFile, "map info = { Geographic Lat/Lon, 1.5, 1.5, %f, %f, %f, %f, %s, units=Degrees}\n",
          bounding_box.min().x(), bounding_box.max().y(), x_pixel_scaling, y_pixel_scaling, georef.datum().name().c_str());
  fprintf(headerFile, "file type = ENVI Standard\n");

  typedef typename vw::PixelChannelType<typename ViewT::pixel_type>::type channel_type;
  fprintf(headerFile, "data type = %d\n", vw_to_envi_channel_type<channel_type>::value());	   // Floating point id == 4

  // refers to whether the data are band sequential (BSQ), band
  // interleaved by pixel (BIP), or band interleaved by line (BIL).  I
  // think that GDAL will write BSQ images, but we should change this
  // to BIP if we use the default Vision Wokrbench TIFF driver, for
  // example. - mbroxton
  fprintf(headerFile, "interleave = bsq\n");
  // Endian check...
  union { short number; char bytes[2]; } short_endian_check;
  short_endian_check.number = 1;
  // IEEE byte order: 0 = little endian, 1 = big endian
  unsigned int byte_order = (char(short_endian_check.number) ==
			     short_endian_check.bytes[1]);
  fprintf(headerFile, "byte order = %d\n", byte_order);
  fprintf(headerFile, "\n");
  fclose(headerFile);
}

#endif // __DEM_H__ 
