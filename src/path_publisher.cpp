#include <nav_msgs/Path.h>
#include <ros/console.h>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <tf/tf.h>
#include <fstream>

class PathPublisher
{
public:
  PathPublisher();

private:
  ros::NodeHandle nh_, p_nh_;
  ros::Subscriber path_filename_sub_;
  ros::Publisher path_pub_;

  std::string paths_directory_;
  std::string path_frame_id_;
  std::string curr_path_filename_;

  void PublishPathFromFile(const std_msgs::String& path_filename_msg);
};

PathPublisher::PathPublisher() : p_nh_("~")
{
  p_nh_.param<std::string>("paths_directory", paths_directory_,
                           std::string("/home/dragoon/dragoon_system/src/dragoon_bringup/paths/"));
  p_nh_.param<std::string>("path_frame_id", path_frame_id_, std::string("map"));

  path_filename_sub_ = nh_.subscribe("/user_input_path", 1, &PathPublisher::PublishPathFromFile, this);
  path_pub_ = nh_.advertise<nav_msgs::Path>("trajectory", 1, true);
}

void PathPublisher::PublishPathFromFile(const std_msgs::String& path_filename_msg)
{
  std::ifstream in_file;
  std::string file_path = paths_directory_ + path_filename_msg.data;
  in_file.open(file_path);
  if (!in_file)
  {
    ROS_ERROR("[Path Publisher]: Could not open path file!!!");
    return;
  }

  ROS_INFO("[Path Publisher]: publishing a path file.");
  nav_msgs::Path current_path;
  current_path.header.frame_id = path_frame_id_;
  current_path.header.stamp = ros::Time::now();
  double x, y, yaw;
  while ((in_file >> x) && (in_file >> y) && (in_file >> yaw))
  {
    geometry_msgs::PoseStamped pose;
    pose.header.frame_id = path_frame_id_;
    pose.pose.position.x = x;
    pose.pose.position.y = y;

    if (yaw < -M_PI || yaw > M_PI) {
      ROS_ERROR("[Path Publisher]: yaw input is out of bound, stop publishing path");
      return;
    }

    geometry_msgs::Quaternion pose_quat = tf::createQuaternionMsgFromYaw(yaw);
    pose.pose.orientation = pose_quat;
    current_path.poses.emplace_back(pose);
  }

  path_pub_.publish(current_path);

  in_file.close();
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "path_publisher_node");
  PathPublisher path_publisher_obj;
  ros::spin();
  return 0;
}