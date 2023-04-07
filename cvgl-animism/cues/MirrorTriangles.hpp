#pragma once

#include "cvglObject.hpp"
#include "cvglHelperFunctions.hpp"

class MirrorTriangles
{
public:


    void setTriangleTexcords(std::unique_ptr<cvglObject> & obj )
    {
        for( size_t i = 0; i < obj->getSize(); i++  )
        {
            cvglVertex vert = obj->getVertex(i);

            obj->setTexCord(i, cvgl::scale( vert.position[0], -1., 1., 0., 1.), cvgl::scale( vert.position[1], -1., 1., 0., 1.));
        }
    }

    std::vector<cvglVertex> genTriangle(int i, float nTriangles, float yrange, float overlap, float x_offset );

    std::vector<cvglVertex> genTriangle(float x, float y, float xrange, float yrange);

    void triCollision(double x, double y, std::unique_ptr<cvglObject>& triSet);


};

