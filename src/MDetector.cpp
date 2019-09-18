//
//  MDetector.cpp
//  TRVect_From_Figure
//
//  Created by Andrés Prada G. on 30/08/2019.
//  Copyright © 2019 Andrés Prada G. All rights reserved.
//

#include "MDetector.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

MDetector::MDetector(cv::Mat image, int vertices){
    // Copy to local variables
    image.copyTo(_image);
    _vertices = vertices;
    
    // Process the image and extract detected points
    std::vector<Point2f> unorderedContour(extractPointsFromImage());
    
    // Find center of the points
    Point2f center = minAreaRect(unorderedContour).center;
    
    // Find the index of the extrema point of the polygon to perform the match
    int index = findIndexExtrema(unorderedContour, center);
    
    // Rotate the vector accordingly, to set the (0,0) to the lower-left point
    if(index<=2)
        rotate(unorderedContour.begin(), unorderedContour.begin()+unorderedContour.size() - 2 + index, unorderedContour.end());
    else
        rotate(unorderedContour.begin(), unorderedContour.begin() - 2 + index, unorderedContour.end());
    
    // Copy variable to internal variable
    _corners.push_back(unorderedContour);
}

/*
 * @brief: Return value of the points
 */
std::vector<vector<Point2f>> MDetector::getPoints(){
    return _corners;
}

std::vector<Point2f> MDetector::extractPointsFromImage(){
    int cannyThres = 180;
    
    // Convert image to grayscale
    cv::Mat processedImg;
    cvtColor(_image, processedImg, CV_BGR2GRAY);
    
    // Increase the contrast of the image
    processedImg.convertTo(processedImg, -1, 2, 0);
    
    // Apply Gaussian blur to avoid noise
    GaussianBlur(processedImg, processedImg, Size(3,3), 1);
    
    // Apply Canny edge detection to detect edges
    Canny( processedImg, processedImg, cannyThres, cannyThres*2, 3);
    
    // Define structing element and dilate the image with this element
    int dilate_factor = 5;
    cv::Mat structElem = getStructuringElement(MORPH_RECT, Size(2*dilate_factor+1,2*dilate_factor+1), Point(dilate_factor,dilate_factor));
    dilate(processedImg, processedImg, structElem);
    
    // Find contours
    std::vector<std::vector<Point>> contours;
    std::vector<Vec4i> hierarchy;
    findContours(processedImg, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    
    // Extract blobs
    int n_blobs = 0;
    double ratio[contours.size()];
    std::vector<Point2f> approx_list[contours.size()];
    std::vector<Point2f> approx;
    
    // Iterate over every contour detected
    for( int i = 0; i< contours.size(); i++ ){
        // Approximate the contour to a polygon
        approxPolyDP(contours[i], approx, 5, true);
        if (approx.size() == _vertices){
            approx_list[n_blobs] = approx;
            ratio[n_blobs] = contourArea(approx);
            n_blobs++;
        }
    }
    
    // Find the index of the element with higest area and load it into variable
    int k = indexofBiggestElement(ratio, n_blobs);
    return approx_list[k];
}

/*
 * Auxiliary functions
 */

/*
 * @brief: Locates the external point /\ P, to rotate the points and match the two shapes
 *                                   |  |
 *                                   |__|
 */
int MDetector::findIndexExtrema(vector<Point2f> unorderedContour, Point2f center){
    std::vector<Point2f> input(unorderedContour);
    int index;
    double distance = -1;
    double dist_aux;
    Point2f intersection;
    for (int it = 4; it > -1; it--){
        intersection = lineLineIntersection(input[0],input[1], input[2], input[3]);
        dist_aux = dist(center.x, center.y, intersection.x, intersection.y);
        if(dist_aux > distance){
            distance = dist_aux;
            index = it;
        }
        rotate(input.begin(), input.begin()+input.size()-1, input.end());
    }
    return index;
}

/*
 * @brief: Find index of the smallest element
 */
int MDetector::indexofBiggestElement(double array[], int size){
    int index = 0;
    for(int i = 1; i < size; i++){
        if(array[i] > array[index])
            index = i;
    }
    return index;
}

/*
 * @brief: Find distance among two points
 */
double MDetector::dist(double x1, double y1, double x2, double y2)
{
    double distance;
    double distance_x = (x2 - x1);
    double distance_y = (y2 - y1);
    distance = sqrt((distance_x * distance_x) + (distance_y * distance_y));
    return distance;
}

/*
 * @brief: Find intersection of two lines given by points AB - CD
 */
Point2f MDetector::lineLineIntersection(Point2f A, Point2f B, Point2f C, Point2f D)
{
    // Line AB represented as a1x + b1y = c1
    double a1 = B.y - A.y;
    double b1 = A.x - B.x;
    double c1 = a1*(A.x) + b1*(A.y);
    
    // Line CD represented as a2x + b2y = c2
    double a2 = D.y - C.y;
    double b2 = C.x - D.x;
    double c2 = a2*(C.x)+ b2*(C.y);
    
    double determinant = a1*b2 - a2*b1;
    
    if (determinant == 0)
    {
        // The lines are parallel. This is simplified
        // by returning a pair of FLT_MAX
        return Point2f(FLT_MAX, FLT_MAX);
    }
    else
    {
        double x = (b2*c1 - b1*c2)/determinant;
        double y = (a1*c2 - a2*c1)/determinant;
        return Point2f(x, y);
    }
}
