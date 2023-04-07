#include "MirrorTriangles.hpp"
#include "cvglRandom.hpp"

using namespace std;

vector<cvglVertex> MirrorTriangles::genTriangle(int i, float nTriangles, float yrange, float overlap, float x_offset )
{
    vector<cvglVertex> randTri;
    cvglRandom rand;

    float xRatio = 1. / nTriangles;

    float minxrange =  (i == 0 ? 0 : (i-overlap) * xRatio);
    float maxxrange =  ((i + 1 + overlap) * xRatio);

    float rx1 = cvgl::scale( rand.uniformRand(), 0., 1., minxrange, maxxrange) ;
    float ry1 = cvgl::scale( rand.uniformRand(), 0., 1., 0, yrange);

    float minx = rx1;
    float miny = ry1;
    float maxx = rx1;
    float maxy = ry1;

    float rx2 = cvgl::scale( rand.uniformRand(), 0., 1., minxrange, maxxrange);
    float ry2 = cvgl::scale( rand.uniformRand(), 0., 1., 0., yrange);

    minx = rx2 < minx ? rx2 : minx;
    miny = ry2 < miny ? ry2 : miny;
    maxx = rx2 > minx ? rx2 : maxx;
    maxy = ry2 > miny ? ry2 : maxy;

    float rx3 = cvgl::scale( rand.uniformRand(), 0., 1., minxrange, maxxrange);
    float ry3 = cvgl::scale( rand.uniformRand(), 0., 1., 0, yrange);

    minx = rx3 < minx ? rx3 : minx;
    miny = ry3 < miny ? ry3 : miny;
    maxx = rx3 > minx ? rx3 : maxx;
    maxy = ry3 > miny ? ry3 : maxy;

    float w = maxx - minx;
    float h = maxy - miny;

    float xx = cvgl::scale( rx1, 0., 1., -1., 1) + x_offset;
    float yy = cvgl::scale( ry1, 0., 1., -1., 1);
    float x_tex = rx1 - w;
    if( x_tex < 0 )
        x_tex = rx1 + w;

    randTri.emplace_back(cvglVertex({{xx, yy, 0.0f}, {x_tex, 1-ry1}}));

    xx = cvgl::scale( rx2, 0., 1., -1., 1) + x_offset;
    yy = cvgl::scale( ry2, 0., 1., -1., 1);

    x_tex = rx2 - w;
    if( x_tex < 0 )
        x_tex = rx2 + w;

    randTri.emplace_back(cvglVertex({{xx, yy, 0.0f},  {x_tex, 1-ry2}}));

    xx = cvgl::scale( rx3, 0., 1., -1., 1) + x_offset;
    yy = cvgl::scale( ry3, 0., 1., -1., 1);

    x_tex = rx3 - w;
    if( x_tex < 0 )
        x_tex = rx3 + w;

    randTri.emplace_back(cvglVertex({{xx, yy, 0.0f},  {x_tex, 1-ry3}}));

    return randTri;

}


vector<cvglVertex> MirrorTriangles::genTriangle(float x, float y, float xrange, float yrange)
{
    vector<cvglVertex> randTri;
    cvglRandom rand;

    float xMin = x - xrange;
    float xMax = x + xrange;
    float yMin = y - yrange;
    float yMax = y + yrange;

    float rx1 = cvgl::scale( rand.uniformRand(), 0., 1., xMin, xMax);
    float ry1 = cvgl::scale( rand.uniformRand(), 0., 1., yMin, yMax);

    float minx = rx1;
    float miny = ry1;
    float maxx = rx1;
    float maxy = ry1;

    float rx2 = cvgl::scale( rand.uniformRand(), 0., 1., xMin, xMax) ;
    float ry2 = cvgl::scale( rand.uniformRand(), 0., 1., yMin, yMax);

    minx = rx2 < minx ? rx2 : minx;
    miny = ry2 < miny ? ry2 : miny;
    maxx = rx2 > minx ? rx2 : maxx;
    maxy = ry2 > miny ? ry2 : maxy;

    float rx3 = cvgl::scale( rand.uniformRand(), 0., 1., xMin, xMax);
    float ry3 = cvgl::scale( rand.uniformRand(), 0., 1., yMin, yMax);

    minx = rx3 < minx ? rx3 : minx;
    miny = ry3 < miny ? ry3 : miny;
    maxx = rx3 > minx ? rx3 : maxx;
    maxy = ry3 > miny ? ry3 : maxy;

    float w = maxx - minx;
    float h = maxy - miny;

    float xx = cvgl::scale( rx1, 0., 1., -1., 1);
    float yy = cvgl::scale( ry1, 0., 1., -1., 1);
    float x_tex = rx1 - w;
    if( x_tex < 0 )
        x_tex = rx1 + w;

    randTri.emplace_back(cvglVertex({{xx, yy, (float)rand.uniformRand()}, {x_tex, 1-ry1}}));

    xx = cvgl::scale( rx2, 0., 1., -1., 1);
    yy = cvgl::scale( ry2, 0., 1., -1., 1);

    x_tex = rx2 - w;
    if( x_tex < 0 )
        x_tex = rx2 + w;

    randTri.emplace_back(cvglVertex({{xx, yy, (float)rand.uniformRand()},  {x_tex, 1-ry2}}));

    xx = cvgl::scale( rx3, 0., 1., -1., 1);
    yy = cvgl::scale( ry3, 0., 1., -1., 1);

    x_tex = rx3 - w;
    if( x_tex < 0 )
        x_tex = rx3 + w;

    randTri.emplace_back(cvglVertex({{xx, yy, (float)rand.uniformRand()},  {x_tex, 1-ry3}}));

    return randTri;

}


void MirrorTriangles::triCollision(double x, double y, unique_ptr<cvglObject>& triSet)
{

    double tri_step = 0.1;

    double gl_x = cvgl::scale(x, 0., 1., -1., 1.);
    float thresh = 0.01;
    int size = triSet->getSize();
    for( int tri = 0; tri < size; tri += 3)
    {
        cvglVertex vertex1 = triSet->getVertex(tri);
        cvglVertex vertex2 = triSet->getVertex(tri+1);
        cvglVertex vertex3 = triSet->getVertex(tri+2);

        double x1 = vertex1.position[0];
        double avg_x = (vertex1.position[0] + vertex2.position[0] + vertex3.position[0]) / 3.;

        double dx = gl_x - avg_x;
        double dir = dx > 0 ? 1 : -1;
        if( abs(dx) < thresh )
        {
            triSet->setPosition(tri, vertex1.position[0] + (tri_step * dir), vertex1.position[1], 0 );
            triSet->setPosition(tri+1, vertex2.position[0] + (tri_step * dir), vertex2.position[1], 0 );
            triSet->setPosition(tri+2, vertex3.position[0] + (tri_step * dir), vertex3.position[1], 0 );

        }


    }



}
