//
//  CameraParamsReader.cpp
//  TRVect_From_Figure
//
//  Created by Andrés Prada G. on 30/08/2019.
//  Copyright © 2019 Andrés Prada G. All rights reserved.
//

#include "CameraParamsReader.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

CameraParamsReader::CameraParamsReader(std::string filename){
    _filepath = filename;
}

/*
 * @brief: Function to read parameters from the file
 */
bool CameraParamsReader::read(Mat &camMatrix, Mat &distCoeffs){
    std::ifstream ifs(_filepath);
    if(!ifs.is_open())
        return false;
    json j = json::parse(ifs);
    std::vector<double> camVect = j.at("intrinsics").at("K").get<std::vector<double>>();
    for (int c1 = 0; c1 < 3; c1++){
        for (int c2 = 0; c2 < 3; c2++){
            camMatrix.at<double>(c1, c2) = camVect.at(3*c1+c2);
        }
    }
    std::vector<double> distVect = j.at("intrinsics").at("distortion").get<std::vector<double>>();
    for(int c1 = 0; c1 < 8; c1++){
        distCoeffs.at<double>(0,c1) = distVect.at(c1);
    }
    return true;
}
