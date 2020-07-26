#define movement_cnt 8
#define VISUALIZATION_WIDTH 20 
#include <ros/ros.h>
#include <stdio.h>
#include "fake_v2x_1/v2x_spat.h"
#include <tf/transform_broadcaster.h>
#include <visualization_msgs/MarkerArray.h>
#include <visualization_msgs/Marker.h>
#include <autoware_msgs/DetectedObjectArray.h>
#include <jsk_recognition_msgs/BoundingBoxArray.h>
#include <object_manager_msgs/combined.h>

using namespace std;
struct SPaTCoordinate{
    virtual int* getX() = 0;
    virtual int* getY() = 0;
};

struct SPaTBoundingBoxCoordinate1: public SPaTCoordinate {
    int x[movement_cnt] = {-1, 1, 3, 3, 1, -1, -3, -3};
    int y[movement_cnt] = {-3, -3, -1, 1, 3, 3, 1,-1};
    int* getX() {return x;}
    int* getY() {return y;}
};

class Traffic_sub{
    public:
    Traffic_sub(){
        spat_pub = nh.advertise<visualization_msgs::MarkerArray>("v2x_debug", 10);
        traffic_light2_sub = nh.subscribe("traffic_200",1000,&Traffic_sub::v2xsub,this);
    }
    void v2xsub(const fake_v2x_1::v2x_spatConstPtr& ptr){
        static SPaTCoordinate *coor_ptr = &S1;  
        marker.markers.resize(movement_cnt);
        for(int i = 0; i < movement_cnt; ++i ){
            marker.markers[i].header.frame_id = "base_link";
            marker.markers[i].header.stamp = ros::Time();
            marker.markers[i].ns = "v2x_debug";
            marker.markers[i].id = i+10;
            marker.markers[i].type = visualization_msgs::Marker::SPHERE;
            marker.markers[i].action = visualization_msgs::Marker::ADD;
            
            marker.markers[i].pose.position.x = coor_ptr->getX()[i];
            marker.markers[i].pose.position.y = coor_ptr->getY()[i];
            marker.markers[i].pose.position.z = 1;
            marker.markers[i].pose.orientation.x = 0.0;
            marker.markers[i].pose.orientation.y = 0.0;
            marker.markers[i].pose.orientation.z = 0.0;
            marker.markers[i].pose.orientation.w = 1.0;

            marker.markers[i].scale.x = 0.5;
            marker.markers[i].scale.y = 0.5;
            marker.markers[i].scale.z = 0.5;

            double r=0, g=0, b=0, a=0;
            switch(ptr->spat_eventstate[i]){
                case 3: r=1; g=0; b=0; a=1; break;//red light
                case 5: r=0; g=1; b=0; a=1; break;
                default: a=0; break;
            }
            marker.markers[i].color.a = a; // Don't forget to set the alpha!
            marker.markers[i].color.r = r;
            marker.markers[i].color.g = g;
            marker.markers[i].color.b = b;
        }
        spat_pub.publish(marker);
        //spat_pub.publish(marker);
    }

    private:
    ros::Subscriber traffic_light2_sub;
    ros::Publisher spat_pub;
    ros::NodeHandle nh, private_nh;
    visualization_msgs::MarkerArray marker;
    SPaTBoundingBoxCoordinate1 S1;

};
int main(int argc, char *argv[]){
    ros::init(argc, argv, "traffic_sub_200");
    Traffic_sub t;
    ros::spin();
}