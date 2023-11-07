#ifndef _ROS_mower_msgs_Perimeter_h
#define _ROS_mower_msgs_Perimeter_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"

namespace mower_msgs
{

  class Perimeter : public ros::Msg
  {
    public:
      float left,center,right;

    Perimeter():
      left(0),
      center(0),
      right(0)
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } tmp;
      tmp.real = this->left;
      *(outbuffer + offset + 0) = (tmp.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (tmp.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (tmp.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (tmp.base >> (8 * 3)) & 0xFF;
      offset += sizeof(tmp.base);
      tmp.real = this->center;
      *(outbuffer + offset + 0) = (tmp.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (tmp.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (tmp.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (tmp.base >> (8 * 3)) & 0xFF;
      offset += sizeof(tmp.base);
      tmp.real = this->right;
      *(outbuffer + offset + 0) = (tmp.base >> (8 * 0)) & 0xFF;
      *(outbuffer + offset + 1) = (tmp.base >> (8 * 1)) & 0xFF;
      *(outbuffer + offset + 2) = (tmp.base >> (8 * 2)) & 0xFF;
      *(outbuffer + offset + 3) = (tmp.base >> (8 * 3)) & 0xFF;
      offset += sizeof(tmp.base);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      union {
        float real;
        uint32_t base;
      } tmp;
      tmp.base = 0;
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->left = tmp.real;
      offset += sizeof(tmp.base);
      tmp.base = 0;
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->center = tmp.real;
      offset += sizeof(tmp.base);
      tmp.base = 0;
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 0))) << (8 * 0);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 1))) << (8 * 1);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 2))) << (8 * 2);
      tmp.base |= ((uint32_t) (*(inbuffer + offset + 3))) << (8 * 3);
      this->right = tmp.real;
      offset += sizeof(tmp.base);
     return offset;
    }

    virtual const char * getType() override { return "mower_msgs/Perimeter"; };
    virtual const char * getMD5() override { return "1eef3cb68ced22453a42748a6794a55e"; };

  };

}
#endif
