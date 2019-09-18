//
//  SVGParser.cpp
//  TRVect_From_Figure
//
//  Created by Andrés Prada G. on 30/08/2019.
//  Copyright © 2019 Andrés Prada G. All rights reserved.
//

#include "SVGParser.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

SVGParser::SVGParser(std::string filepath, double scale, OutputArray objPoints){
    
    // Copy variables to local variables
    _lengthConv = scale;
    _filepath = filepath;
    
    // Extract all the points from the SVG
    Mat points = extractPointsFromSVG(filepath);
    
    /* The points need to be ordered clockwise in the following scheme:
     *      /\ 3.
     *  2. |  | 4.
     *  1. |__| 5.
     */
    reorderClockwise(points);
    
    // Finally, add the 3rd Dimension, and scalate from milimeters to meters
    add3DimensionAndScale(points, _objPoints);
    _objPoints.copyTo(objPoints);
}

cv::Mat SVGParser::extractPointsFromSVG(std::string filepath){
    Mat points;
    string line;
    ifstream file(filepath);
    if (file.is_open()){
        while (getline(file,line)){
            if (line.find("rect") != string::npos)
                extractPointsInRect(line, points);
            else if (line.find("polygon") != string::npos)
                extractPointsInPolygon(line, points);
        }
        file.close();
    }
    return points;
}

/*
 * @brief extract 4 points in the rectangle line
 */
void SVGParser::extractPointsInRect(string line, Mat &points){
    float x = stoi(line.substr(line.find("x=")+3, 1))*1.f;
    float y = stoi(line.substr(line.find("y=")+3, 1))*1.f;
    float width = stoi(line.substr(line.find("width=")+7, 3))*1.f;
    float height = stoi(line.substr(line.find("height=")+8, 3))*1.f;
    if (!isDuplicated(Point2f(x,y), points))
        points.push_back(Point2f(x,y));
    if (!isDuplicated(Point2f(x+width,y), points))
        points.push_back(Point2f(x+width,y));
    if (!isDuplicated(Point2f(x,y+height), points))
        points.push_back(Point2f(x,y+height));
    if (!isDuplicated(Point2f(x+width,y+height), points))
        points.push_back(Point2f(x+width,y+height));
}

/*
 * @brief extract all the points in the polygon line
 */
void SVGParser::extractPointsInPolygon(string line, Mat &points){
    string l_points = line.substr(line.find("points=")+8, line.length() - line.find("points=")-11);
    string delimiter_pts = " ";
    string delimiter_coords = ",";
    vector<string> points_str = split(l_points, delimiter_pts);
    for (int n_points = 0; n_points < points_str.size(); n_points++){
        vector<string> point_x_y = split(points_str[n_points], delimiter_coords);
        if (!isDuplicated(Point2f(stoi(point_x_y[0])*1.f, stoi(point_x_y[1])*1.f), points))
            points.push_back(Point2f(stoi(point_x_y[0])*1.f, stoi(point_x_y[1])*1.f));
    }
}

/*
 * @brief Helper function: Splits strings by the delimeter
 */
vector<string> SVGParser::split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

/*
 * @brief Helper function: Get Clockwise angle of two points
 */
double SVGParser::getClockwiseAngle(Point2f p) {
    double angle = 0.0;
    
    angle = -1 * atan2(p.x, -1 * p.y);
    return angle;
}

/*
 * @brief Helper function: Compare angles to order two points clockwise
 */
bool SVGParser::comparePoints(Point2f p1, Point2f p2) {
    return getClockwiseAngle(p1) < getClockwiseAngle(p2);
}

/*
 * @brief Helper function: reorder every point in the figure clockwise
 */
void SVGParser::reorderClockwise(Mat points){
    for(int c1 = 0; c1 < points.rows; c1++){
        for(int c2 = 0; c2 < points.rows; c2++){
            if (comparePoints(points.at<Point2f>(c1), points.at<Point2f>(c2)) && (c1 != c2))
                swap(points.at<Point2f>(c1), points.at<Point2f>(c2));
        }
    }
}


/*
 * @brief Helper function: Add the 3rd dimension to the object, to calculate the RT vectors, and scalate according to the scale.
 */
void SVGParser::add3DimensionAndScale(Mat points, OutputArray _objPoints){
    _objPoints.create(points.rows, 1, CV_32FC3);
    Mat objPoints = _objPoints.getMat();
    for(int c1 = 0; c1 < points.rows; c1++)
        objPoints.ptr< Vec3f >(0)[c1] = Vec3f(points.at<Point2f>(c1).x/_lengthConv, points.at<Point2f>(c1).y/_lengthConv, 0);
}

/*
 * @brief Helper function: Check if a point already exists in the points array
 */
bool SVGParser::isDuplicated(Point2f point, Mat points){
    for(int c1 = 0; c1 <points.rows; c1++){
        if(point == points.at<Point2f>(c1))
            return true;
    }
    return false;
}
