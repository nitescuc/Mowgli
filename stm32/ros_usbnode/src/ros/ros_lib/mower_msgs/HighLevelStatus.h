#ifndef _ROS_mower_msgs_HighLevelStatus_h
#define _ROS_mower_msgs_HighLevelStatus_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace mower_msgs
{

  class HighLevelStatus : public ros::Msg
  {
    public:
      typedef uint8_t _state_type;
      _state_type state;
      typedef const char* _state_name_type;
      _state_name_type state_name;
      typedef const char* _sub_state_name_type;
      _sub_state_name_type sub_state_name;
      typedef float _gps_quality_percent_type;
      _gps_quality_percent_type gps_quality_percent;
      typedef float _battery_percent_type;
      _battery_percent_type battery_percent;
      typedef bool _is_charging_type;
      _is_charging_type is_charging;
      typedef bool _emergency_type;
      _emergency_type emergency;
      enum { HIGH_LEVEL_STATE_NULL = 0 };
      enum { HIGH_LEVEL_STATE_IDLE = 1 };
      enum { HIGH_LEVEL_STATE_AUTONOMOUS = 2 };
      enum { HIGH_LEVEL_STATE_RECORDING = 3 };
      enum { SUBSTATE_1 = 0 };
      enum { SUBSTATE_2 = 1 };
      enum { SUBSTATE_3 = 2 };
      enum { SUBSTATE_4 = 3 };
      enum { SUBSTATE_SHIFT = 6 };

    HighLevelStatus():
      state(0),
      state_name(""),
      sub_state_name(""),
      gps_quality_percent(0),
      battery_percent(0),
      is_charging(0),
      emergency(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      *(outbuffer + offset + 0) = (this->state >> (8 * 0)) & 0xFF;
      offset += sizeof(this->state);
      uint32_t length_state_name = strlen(this->state_name);
      varToArr(outbuffer + offset, length_state_name);
      offset += 4;
      memcpy(outbuffer + offset, this->state_name, length_state_name);
      offset += length_state_name;
      uint32_t length_sub_state_name = strlen(this->sub_state_name);
      varToArr(outbuffer + offset, length_sub_state_name);
      offset += 4;
      memcpy(outbuffer + offset, this->sub_state_name, length_sub_state_name);
      offset += length_sub_state_name;
      union {
        float real;
        uint32_t base;
      } u_gps_quality_percent;
      u_gps_quality_percent.real = this->gps_quality_percent;
      *(outbuffer + offset + 0) = (u_gps_quality_percent.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_gps_quality_percent.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_gps_quality_percent.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_gps_quality_percent.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->gps_quality_percent);
      union {
        float real;
        uint32_t base;
      } u_battery_percent;
      u_battery_percent.real = this->battery_percent;
      *(outbuffer + offset + 0) = (u_battery_percent.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (u_battery_percent.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (u_battery_percent.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (u_battery_percent.base >> (8 * 3)) & 0xFF;
      offset += sizeof(this->battery_percent);
      union {
        bool real;
        uint8_t base;
      } u_is_charging;
      u_is_charging.real = this->is_charging;
      *(outbuffer + offset + 0) = (u_is_charging.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->is_charging);
      union {
        bool real;
        uint8_t base;
      } u_emergency;
      u_emergency.real = this->emergency;
      *(outbuffer + offset + 0) = (u_emergency.base >> (8 * 0)) & 0xFF;
      offset += sizeof(this->emergency);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      this->state =  ((uint8_t) (*(inbuffer + offset)));
      offset += sizeof(this->state);
      uint32_t length_state_name;
      arrToVar(length_state_name, (inbuffer + offset));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_state_name; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_state_name-1]=0;
      this->state_name = (char *)(inbuffer + offset-1);
      offset += length_state_name;
      uint32_t length_sub_state_name;
      arrToVar(length_sub_state_name, (inbuffer + offset));
      offset += 4;
      for(unsigned int k= offset; k< offset+length_sub_state_name; ++k){
          inbuffer[k-1]=inbuffer[k];
      }
      inbuffer[offset+length_sub_state_name-1]=0;
      this->sub_state_name = (char *)(inbuffer + offset-1);
      offset += length_sub_state_name;
      union {
        float real;
        uint32_t base;
      } u_gps_quality_percent;
      u_gps_quality_percent.base = 0;
      u_gps_quality_percent.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_gps_quality_percent.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_gps_quality_percent.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_gps_quality_percent.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->gps_quality_percent = u_gps_quality_percent.real;
      offset += sizeof(this->gps_quality_percent);
      union {
        float real;
        uint32_t base;
      } u_battery_percent;
      u_battery_percent.base = 0;
      u_battery_percent.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      u_battery_percent.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      u_battery_percent.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      u_battery_percent.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->battery_percent = u_battery_percent.real;
      offset += sizeof(this->battery_percent);
      union {
        bool real;
        uint8_t base;
      } u_is_charging;
      u_is_charging.base = 0;
      u_is_charging.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->is_charging = u_is_charging.real;
      offset += sizeof(this->is_charging);
      union {
        bool real;
        uint8_t base;
      } u_emergency;
      u_emergency.base = 0;
      u_emergency.base |= ((uint8_t) (*(inbuffer + offset + 0))) << (8 * 0);
      this->emergency = u_emergency.real;
      offset += sizeof(this->emergency);
     return offset;
    }

    virtual const char * getType() override { return "mower_msgs/HighLevelStatus"; };
    virtual const char * getMD5() override { return "db5c3531ba581d4c594cc4da6f89eb3f"; };

  };

}
#endif