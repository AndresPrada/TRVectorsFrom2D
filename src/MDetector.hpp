//
//  MDetector.hpp
//  TRVect_From_Figure
//
//  Created by Andrés Prada G. on 30/08/2019.
//  Copyright © 2019 Andrés Prada G. All rights reserved.
//

#ifndef MDetector_hpp
#define MDetector_hpp

#include <opencv2/opencv.hpp>
using namespace cv;

class MDetector{
public:
    MDetector(cv::Mat image, int vertices);
    std::vector<std::vector<Point2f>> getPoints();
private:
    // Variables
    cv::Mat _image;
    int _vertices;
    std::vector<std::vector<Point2f>> _corners;
    
    // Private functions
    std::vector<Point2f> extractPointsFromImage();
    int findIndexExtrema(std::vector<Point2f> unorderedContour, Point2f center);
    
    // Helper functions
    int indexofBiggestElement(double array[], int size);
    double dist(double x1, double y1, double x2, double y2);
    Point2f lineLineIntersection(Point2f A, Point2f B, Point2f C, Point2f D);
    void rotateVectToMatch(std::vector<Point2f> vector, int index);
};

#endif /* MDetector_hpp */
