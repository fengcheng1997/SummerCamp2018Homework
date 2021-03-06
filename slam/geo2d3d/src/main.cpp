#include <vector>
#include <fstream>

#include "Geometry.h"
#include <GSLAM/core/Random.h>

using namespace std;
using namespace GSLAM;

using summercamp::Geometry;
using summercamp::GeometryPtr;

bool operator ==(GSLAM::Point2d left,GSLAM::Point2d right){
    return (left-right).norm()<1e-8;
}

bool operator !=(GSLAM::Point2d left,GSLAM::Point2d right){
    return !(left==right);
}


bool operator ==(GSLAM::Point3d left,GSLAM::Point3d right){
    return (left-right).norm()<1e-8;
}

bool operator !=(GSLAM::Point3d left,GSLAM::Point3d right){
    return !(left==right);
}

GSLAM::Point2d to2d(GSLAM::Point3d pt){return Point2d(pt.x/pt.z,pt.y/pt.z);}

bool pointOnLineTest(GeometryPtr geo){
    if(!geo->pointOnLine(Point3d(0,0,0),Point3d(1,2,0)));
    GSLAM::Point3d pt1(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    GSLAM::Point3d pt2(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    GSLAM::Point3d line=geo->line(pt1,pt2);
    if(!geo->pointOnLine(pt1,line)) return false;
    if(!geo->pointOnLine(pt2,line)) return false;
    if(geo->pointOnLine(pt1+line,line)) return false;
    return true;
}

bool intersectTest(GeometryPtr geo){
    if(to2d(geo->intersect(Point3d(1,0,0),Point3d(0,1,0)))!=Point2d(0,0)) return false;

    GSLAM::Point3d pt1(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    GSLAM::Point3d pt2(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    GSLAM::Point3d pt3(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    auto line1=geo->line(pt1,pt2);
    auto line2=geo->line(pt1,pt3);
    auto line3=geo->line(pt2,pt3);

    if(to2d(pt1)!=to2d(geo->intersect(line1,line2)))
        return false;
    if(to2d(pt2)!=to2d(geo->intersect(line1,line3)))
        return false;
    if(to2d(pt3)!=to2d(geo->intersect(line2,line3)))
        return false;
    return true;
}

bool lineTest(GeometryPtr geo){
    return intersectTest(geo);
}

bool distanceTest(GeometryPtr geo)
{
    if(fabs(geo->distance(Point3d(1,0,1),Point3d(1,0,0))-1)>1e-8) return false;
    if(fabs(geo->distance(Point3d(0,1,1),Point3d(0,1,0))-1)>1e-8) return false;

    GSLAM::Point3d pt1(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    GSLAM::Point3d pt2(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    GSLAM::Point3d pt3(GSLAM::Random::RandomValue<double>(0,1),
                       GSLAM::Random::RandomValue<double>(0,1),1);
    auto line1=geo->line(pt1,pt2);
    auto line2=geo->line(pt1,pt3);
    auto line3=geo->line(pt2,pt3);
    if(geo->distance(line1,pt1)>1e-8) return false;
    if(geo->distance(line1,pt2)>1e-8) return false;
    if(geo->distance(line2,pt1)>1e-8) return false;
    if(geo->distance(line2,pt3)>1e-8) return false;
    if(geo->distance(line3,pt2)>1e-8) return false;
    if(geo->distance(line3,pt3)>1e-8) return false;

    auto distance1=geo->distance(pt3,line1);
    auto distance2=geo->distance(pt2,line2);
    auto distance3=geo->distance(pt1,line3);

    double area1=distance1*(pt1-pt2).norm();
    double area2=distance2*(pt1-pt3).norm();
    double area3=distance3*(pt2-pt3).norm();
    if(fabs(area1-area2)>1e-8) return false;
    if(fabs(area1-area3)>1e-8) return false;
    return true;
}

bool similarity2DTest(GeometryPtr geo)
{
    for(int i=0;i<100;i++)
    {
        double scale=Random::RandomValue(1.,1.);
        double theta=Random::RandomValue(0.,10.);
        Point2d t(Random::RandomValue(0.,10.),
                  Random::RandomValue(0.,10.));
        Point2d pt(Random::RandomValue(0.,10.),
                   Random::RandomValue(0.,10.));
        Point2d result=geo->transform(theta,scale,t,pt);
        Point2d untran=geo->transform(-theta,1./scale,Point2d(),result-t);
        if(pt!=untran)
        {
            LOG(ERROR)<<"Pt:"<<pt<<"!="<<untran;
            return false;
        }
    }
    return true;
}

bool transform2DTest(GeometryPtr geo)
{
    for(int i=0;i<100;i++)
    {
        Point2d t(Random::RandomValue(0.,10.),Random::RandomValue(0.,10.));
        Point2d p(Random::RandomValue(0.,10.),Random::RandomValue(0.,10.));
        double H[9]={1.,0.,t.x,
                  0.,2.,t.y,
                  0.,0.,1.};
        double H2[9]={1.,0.,0,
                   0.,0.5,0,
                   0.,0.,1.};
        auto result=geo->transform(H,p);
        if(p!=geo->transform(H2,result-t)) return false;
    }
    return true;
}

bool transform3DTest(GeometryPtr geo)
{
    for(int i=0;i<100;i++)
    {
        Point3d t(Random::RandomValue(0.,10.),
                  Random::RandomValue(0.,10.),
                  Random::RandomValue(0.,10.));
        Point3d p(Random::RandomValue(0.,10.),
                  Random::RandomValue(0.,10.),
                  Random::RandomValue(0.,10.));

        double H[16]={1.,0.,0.,t.x,
                     0.,2.,0.,t.y,
                     0.,0.,4.,t.z,
                     0.,0.,0.,1.};
        double H2[16]={1.,0.,0.,0.,
                       0.,0.5,0.,0.,
                       0.,0.,0.25,0.,
                       0.,0.,0.,1.};
        auto result=geo->transform(H,p);
        if(p!=geo->transform(H2,result-t)) return false;
    }
    return true;
}

bool epipolarLineTest(GeometryPtr geo){
    GSLAM::Camera camera({640,480,400,400,320,240});

    for(int i=0;i<100;i++)
    {
        Point3d r1(Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.));
        Point3d r2(Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.));
        Point3d t1(Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.));
        Point3d t2(Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.),
                   Random::RandomValue(0.,1.));

        GSLAM::SE3    pose1(SO3::exp(r1),t1);
        GSLAM::SE3    pose2(SO3::exp(r2),t2);

        Point3d p1(Random::RandomGaussianValue(0.,1.),
                   Random::RandomGaussianValue(0.,1.),10.);
        Point3d line=geo->epipolarLine(camera,pose1,camera,pose2,camera.Project(p1));
        if(line.norm()<1e-8)
            return false;
        auto    p2=camera.Project(pose2.inverse()*pose1*p1);

        if(!geo->pointOnLine(Point3d(p2.x,p2.y,1.),line))
            return false;

    }

    return true;
}


int Return(int argc,char** argv,int i){
    std::string topic="slam/geo2d3d";
    std::string name ="demo";
    std::string scoreFile="score.txt";
    if(argc>1) topic=argv[1];
    if(argc>2) name =argv[2];
    if(argc>3) scoreFile=argv[3];

    std::vector<std::string> outputInfos={
        "[S]("+topic+"/"+name+"/Geometry.cpp)",
        "[C]("+topic+"/evaluation/create.md)",
        "[C]("+topic+"/evaluation/point_on_line.md)",
        "[C]("+topic+"/evaluation/intersect.md)",
        "[B]("+topic+"/evaluation/line.md)",
        "[B]("+topic+"/evaluation/distance.md)",
        "[B]("+topic+"/evaluation/similarity2d.md)",
        "[A]("+topic+"/evaluation/transform2d.md)",
        "[A]("+topic+"/evaluation/transform3d.md)",
        "[A]("+topic+"/evaluation/epipolar.md)"
    };

    if(argc>3){
        std::ofstream ofs(scoreFile);
        ofs<<outputInfos[i];
    }
    else std::cout<<outputInfos[i];
}

int main(int argc,char** argv){
    GeometryPtr geo=Geometry::create();

    if(!geo)
        return Return(argc,argv,1);

    if(!pointOnLineTest(geo))
        return Return(argc,argv,2);

    if(!intersectTest(geo))
        return Return(argc,argv,3);

    if(!lineTest(geo))
        return Return(argc,argv,4);

    if(!distanceTest(geo))
        return Return(argc,argv,5);

    if(!similarity2DTest(geo))
        return Return(argc,argv,6);

    if(!transform2DTest(geo))
        return Return(argc,argv,7);

    if(!transform3DTest(geo))
        return Return(argc,argv,8);

    if(!epipolarLineTest(geo)){
        return Return(argc,argv,9);
    }

    return Return(argc,argv,0);
}
