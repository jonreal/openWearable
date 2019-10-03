#include "ros/ros.h"
#include "std_msgs/String.h"

#include <sstream>

// C++ ros publisher
//
// standard string publisher
class RosPub {
  public:
    RosPub(void);
    ~RosPub();
    void _RosPubPublish(char* buffer);

  private:
    ros::Publisher _pub;
    ros::NodeHandle _n;
    std_msgs::String _msg;
};

RosPub::RosPub(void) {
  _pub = _n.advertise<std_msgs::String>("openwearable", 10);
}

RosPub::~RosPub(void) {
  ros::shutdown();
}

void RosPub::_RosPubPublish(char* buffer) {
  std::string buf(buffer);
  _msg.data = buf;
  if (ros::ok()) {
  //  ROS_INFO("%s", _msg.data.c_str());
    _pub.publish(_msg);
    ros::spinOnce();
  }
}

// C wrapper
extern "C" {

  typedef struct RosPub rospub_t;

  rospub_t* RosPubInit() {
    int argc = 0;
    ros::init(argc,NULL,"talker",ros::init_options::NoSigintHandler);
    return new RosPub();
  }

  void RosPubPublish(rospub_t* _s, char* buffer) {
    _s->_RosPubPublish(buffer);
  }

  void RosPubCleanup(rospub_t* _s) {
    free(_s);
  }

}
