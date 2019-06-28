#include "ros/ros.h"

class RosPublisher
{
  public:
    RosPublisher(void);
    void RosPublisherUpdateMsg(std::stringstream ss);
    void RosPublisherPublish(void);

  private:
    ros::NodeHandle n;
    ros::Publisher pub;
    std_msgs::String msg;
}
