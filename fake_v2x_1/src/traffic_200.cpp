#define movement_cnt 8
#include <ros/ros.h>
#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <unistd.h> /* close() */
#include "fake_v2x_1/v2x_spat.h"
//#include <asn_application.h>
/* Including external dependencies */
/*#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>*/
//신호등 8개 //각각의 신호등은 빨, 초, 노 event를 가짐. 
//신호등마다 각자의 주기로 신호등의 event가 변화
//빨 - 680, 노 - 30, 초 - 320

/*int32 spat_id_region
int32 spat_movement_cnt
string[] spat_movement_name # assume that movement state contains only one movement event
int32[] spat_eventstate #0 : unavaliable/ 3: stop and remain/ 5 : permissive_movement_allowed
int32[] spat_minendtime*/
using namespace std;

enum{
    unavailable=0, stop_and_remain=3, permissive_movement_allowed=5, permissive_clearance=7
};

typedef long MovementPhaseState_t;

struct Intersection;

typedef struct Intersection{
    int region=200;
    int id=200;
    long signalGroup;
    string movement_name;
    MovementPhaseState_t eventState;
    int minEndTime;
    //MovementList_t states;
} Intersection_t;

class Traffic_light2{
    public:
    Traffic_light2(){
        for(int i = 0; i < movement_cnt; ++i){
            if(i%2==0){
                v.movement_name = "STR";
            }
            else{
                v.movement_name = "LEFT";
            }
            v.signalGroup = i+1;
            initial_light();//event_state와 min_endtime지정.
            list.push_back(v);
        }
        traffic_light2_pub = nh.advertise<fake_v2x_1::v2x_spat>("traffic_200",1000);
        change_light();
    }
    void initial_light(){
        if(v.signalGroup==1||v.signalGroup==2){
            v.eventState = 5;
            v.minEndTime = 50;
        }
        if(v.signalGroup==3||v.signalGroup==4){
            v.eventState = 3;
            v.minEndTime = 580;
        }
        if(v.signalGroup==5||v.signalGroup==6){
            v.eventState = 3;
            v.minEndTime = 80;
        }
        if(v.signalGroup==7||v.signalGroup==8){
            v.eventState = 3;
            v.minEndTime = 230;
        }
    }
    void change_light(){
        ros::Rate loop_rate(10);
        while(ros::ok()){
            int movement_list_cnt = list.size();
            for(int i = 0; i<movement_list_cnt; ++i){
                list[i].minEndTime-=10;
                if(list[i].minEndTime==0){
                    switch(list[i].eventState){
                        case 3:
                            list[i].eventState = 5;
                            list[i].minEndTime = 320;
                            break;
                        case 5:
                            list[i].eventState = 7;
                            list[i].minEndTime = 30;
                            break;     
                        case 7:
                            list[i].eventState = 3;
                            list[i].minEndTime = 680;
                            break;     
                        default:
                            break;                       
                    }
                }
            }
            publish();
            loop_rate.sleep();
        }
        
    }
    void publish(){
        msg.msg_type = fake_v2x_1::v2x_spat::SPAT_MSG_TYPE;
        msg.spat_id_region = 0;
        msg.spat_movement_name.resize(0);
	    msg.spat_eventstate.resize(0);
	    msg.spat_minendtime.resize(0);
        msg.spat_signalgroup.resize(0);

        msg.spat_id_region = 200;
        msg.spat_movement_cnt = movement_cnt;
        for(int i = 0; i < msg.spat_movement_cnt; ++i){
            msg.spat_movement_name.push_back(list[i].movement_name);
            msg.spat_signalgroup.push_back(list[i].signalGroup);
            msg.spat_eventstate.push_back(list[i].eventState);
            msg.spat_minendtime.push_back(list[i].minEndTime);
        }
        traffic_light2_pub.publish(msg);
    }
    private:
    vector<Intersection_t> list;
    fake_v2x_1::v2x_spat msg;
    ros::NodeHandle nh;
    ros::Publisher traffic_light2_pub;
    Intersection_t v;
};



int main(int argc, char *argv[])
{
    ros::init(argc, argv, "traffic_200");
    Traffic_light2 v;
    ros::spin();
    return 0;
}