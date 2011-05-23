// __BEGIN_LICENSE__
// Copyright (C) 2006-2011 United States Government as represented by
// the Administrator of the National Aeronautics and Space Administration.
// All Rights Reserved.
// __END_LICENSE__


/// \file TestNURBS.h
///

#include <cxxtest/TestSuite.h>
#include "stereo.h"
#include "SurfaceNURBS.h"
#include "vw/vw.h"
#include "vw/FileIO.h"
#include "vw/stereo/DisparityMap.h"

using namespace std;
using namespace vw;
using namespace vw::stereo;

class TestNURBS : public CxxTest::TestSuite {
 public:

  void testNURBS() {

    printf("\n\n");
    
    std::string out_prefix = "/irg/home/mbroxton/arabia/out";

    

    vcl_cout << "\nLoading image " << out_prefix + "-H-preNURBS.exr" << ":\n";
    vil_image_view<double> Limg = read_hdr_image(out_prefix + "-H-preNURBS.exr");
    vcl_cout << "\nLoading image " << out_prefix + "-V-preNURBS.exr" << ":\n";
    vil_image_view<double> Rimg = read_hdr_image(out_prefix + "-V-preNURBS.exr");

    write_image(Limg, out_prefix + "-H-before.pgm");
    write_image(Rimg, out_prefix + "-V-before.pgm");

    //    DisparityMap disparities(Limg, Rimg);

    vil_image_view<double> Lafter;
    //    vil_image_view<double> Rafter;

    FitSurfaceNURBS(Limg, Lafter, 10, vw_TRUE); 
    //    FitNURBSurface(Rimg, Rafter, 3, vw_TRUE); 
      
    write_image(Lafter, out_prefix + "-H-after.pgm");
    //    write_image(Rafter, out_prefix + "-V-after.pgm");


  }
  
};

