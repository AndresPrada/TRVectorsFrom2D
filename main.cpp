/* TRA Robotics Technical Task
 * Author: Andrés Prada
 *
 *  Description: This program finds the rotation and translation vectors of a certain
 *               object given in a .svg file in a given image. It finds the object in the image,
 *               extracts it and match it. Finally, it returns the R and T vectors.
 *  Usage:
 *       ./TRVect_From_Figure [YOUR_IMAGE_PATH] [CAMERA_PARAMS_PATH].json [TEMPLATE_PATH].svg (Optional) [PRINT=1|0]
 *
 *  Example:
 *       ./TRVect_From_Figure /images/1.png camera_parameters.json object.svg 1
 *
 *  Notes:
 *      * In order to compile this program, the installation of the JSON parser (https://github.com/nlohmann/json) is required,
 *        as well as the OpenCV module.
 */

#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

#include "src/SVGParser.hpp"
#include "src/MDetector.hpp"
#include "src/CameraParamsReader.hpp"

using namespace std;
using namespace cv;

// Declaration of the function to estimate pose
void estimatePose(InputArrayOfArrays detectedPoints, InputArray objPoints, InputArray cameraMatrix, InputArray distCoeff, OutputArray rvec, OutputArray tvecs);

// Declaration for printing vectors
void printVector(std::vector<Vec3d> vect);

int main(int argc, const char * argv[]) {
    // Parse the arguments
    if(argc < 3) {
        // Print error
        cerr << "Usage: ./program [image_path] [camera_params_path.json] [template_path.svg]" << endl;
        return 0;
    }
    string image_path = argv[1];
    string cameraParams_path = argv[2];
    string templ_path = argv[3];
    bool print = false;
    if (argc == 5){
        if (strncmp(argv[4], "0", 1))
            print = true;
    }
    
    double scale = 10000;
    Mat objPoints;
    
    // Extract points from template
    SVGParser parser(templ_path, scale, objPoints);
    
    // Determine number of edges
    int n_vertices  = objPoints.rows;
    
    // Load image into memory
    Mat image = imread(image_path);
    
    // Detect and extract points from image
    MDetector detector(image, n_vertices);
    std::vector<vector<Point2f>> detectedPoints = detector.getPoints();
    
    // Load Camera Parameters
    Mat cameraMatrix = Mat(3,3,CV_64FC1);
    Mat distCoeffs = Mat(1,8,CV_64FC1);
    CameraParamsReader reader(cameraParams_path);
    reader.read(cameraMatrix, distCoeffs);
    
    // Variables to store Rotation and Translation vectors
    std::vector<Vec3d> rvecs, tvecs;
    estimatePose(detectedPoints, objPoints, cameraMatrix, distCoeffs, rvecs, tvecs);
    
    if (print){
        aruco::drawAxis(image, cameraMatrix, distCoeffs, rvecs, tvecs, 0.01f);
        
        // Image is reduced for visualization purposes
        Size size (image.cols/3, image.rows/3);
        Mat dst;
        cv::resize(image, dst, size);
        cout << "Press Enter on the image to close it" << endl;
        imshow("Projection of the object", dst);
        waitKey(0);
    }
    
    cout << "Rotation vector: " << endl;
    printVector(rvecs);
    cout << "Translation vector: " << endl;
    printVector(tvecs);
    return 0;
}

/*
 * @brief: Function to return Rotation and Translation vectors from object points and detected points
 */
void estimatePose(InputArrayOfArrays detectedPoints, InputArray objPoints, InputArray cameraMatrix, InputArray distCoeff, OutputArray rvecs, OutputArray tvecs){
    rvecs.create(1, 1, CV_64FC3);
    tvecs.create(1, 1, CV_64FC3);
    
    // Get Mats from the variables and calculate projection
    Mat _rvecs = rvecs.getMat(), _tvecs = tvecs.getMat(), _objPoints = objPoints.getMat(), _detectedCorners = detectedPoints.getMat(0);
    solvePnP(_objPoints, _detectedCorners, cameraMatrix, distCoeff, _rvecs.at<Vec3d>(),
             _tvecs.at<Vec3d>());
}

/*
 * @brief: Function to print vectors
 */
void printVector(std::vector<Vec3d> vect){
    for(int c = 0; c < vect.size(); c++){
        cout << vect.at(c) << endl;
    }
}
