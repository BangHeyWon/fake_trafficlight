Header header

int32 msg_type
int32 BSM_MSG_TYPE = 1
int32 SPAT_MSG_TYPE = 2
int32 TIM_MSG_TYPE = 3
int32 MAP_MSG_TYPE = 4

# SPaT
int32 spat_id_region
int32 spat_movement_cnt
int32[] spat_signalgroup
string[] spat_movement_name # assume that movement state contains only one movement event
int32[] spat_eventstate #0 : unavaliable/ 3: stop and remain/ 5 : permissive_movement_allowed
int32[] spat_minendtime
