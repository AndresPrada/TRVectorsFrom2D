//
//  SVGParser.hpp
//  TRVect_From_Figure
//
//  Created by Andrés Prada G. on 30/08/2019.
//  Copyright © 2019 Andrés Prada G. All rights reserved.
//

#ifndef SVGParser_hpp
#define SVGParser_hpp

#include <opencv2/opencv.hpp>

using namespace cv;
// Declaration of SVGParser class
class SVGParser{
public:
    // Constructor
    SVGParser(std::string filepath, double scale, OutputArray objPoints);
private:
    // Private variables
    cv::Mat _objPoints;
    std::string _filepath;
    double _lengthConv;
    
    // Private functions
    void extractPointsInRect(std::string line, cv::Mat &points);
    void extractPointsInPolygon(std::string line, cv::Mat &points);
    cv::Mat extractPointsFromSVG(std::string filepath);
    
    // Helper functions
    std::vector<std::string> split(const std::string& str, const std::string& delim);
    double getClockwiseAngle(cv::Point2f point);
    bool comparePoints(cv::Point2f p1, cv::Point2f p2);
    void reorderClockwise(cv::Mat points);
    void add3DimensionAndScale(cv::Mat points, OutputArray _objPoints);
    bool isDuplicated(cv::Point2f point, cv::Mat points);
};

#endif /* SVGParser_hpp */
