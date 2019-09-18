# TRVectorsFrom2D
Description: This program finds the rotation and translation vectors of a certain object given in a .svg file in a given image. It finds the object in the image, extracts it and match it. Finally, it returns the R and T vectors. 

## Params
* Input:   
    *  image_path.png :: the image should contain just one element of the .svg in the 3D space.
    *  camera_params :: this file should contain the `K` and `dist` camera parameters.
    *  template_path :: the template should be stored as a .svg as shown in the example.

* Output:
    * T and R vectors.

In order to compile this program, the following libraries are needed:
* `opencv`
* `https://github.com/nlohmann/json`

## Improvements
* This program can be improved by finding the exact points on the figure by performing an Least-Square regression with the obtained points.
* More improvements will be performed on this code in the near future ...

Author: Andrés Prada


