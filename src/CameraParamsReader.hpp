//
//  CameraParamsReader.hpp
//  TRVect_From_Figure
//
//  Created by Andrés Prada G. on 30/08/2019.
//  Copyright © 2019 Andrés Prada G. All rights reserved.
//

#ifndef CameraParamsReader_hpp
#define CameraParamsReader_hpp

#include <opencv2/opencv.hpp>

using namespace cv;

class CameraParamsReader{
public:
    CameraParamsReader(std::string filename);
    bool read(Mat &camMatrix, Mat &distCoeffs);
private:
    std::string _filepath;
};
#endif /* CameraParamsReader_hpp */
