#include "rospublisher.hpp"


RosPublisher::RosPublisher(void) {
  pub = n.advertise<std_msgs::String>("chatter", 1000);
}

void RosPublisher::RosPublisherUpdateMsg(char* s)
{
  msg.data = std::string s(ss);
}

void RosPublisher::RosPublisherPublish(void) {
  pub.publish(msg);
  ros::spinOnce();
}
