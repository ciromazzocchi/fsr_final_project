#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "quad_control/UavState.h"
#include "../Utils/utility.hpp"

#include <Eigen/Dense>
#include <tf/LinearMath/Matrix3x3.h>
#include <tf_conversions/tf_eigen.h>
#include <eigen_conversions/eigen_msg.h>

ros::Subscriber traj_sub;
ros::Publisher  traj_pub;

void odom_cb(const nav_msgs::Odometry::ConstPtr& odom_msg) {
    quad_control::UavState msg;
    msg.header = odom_msg->header;

    Eigen::Quaterniond q = Eigen::Quaterniond( odom_msg->pose.pose.orientation.w,
        odom_msg->pose.pose.orientation.x, odom_msg->pose.pose.orientation.y,
        odom_msg->pose.pose.orientation.z);
    Eigen::Vector3d eta = getEta(q);

    tf::vectorEigenToMsg(Eigen::Vector3d::Zero(),msg.mu_hat);
    tf::twistEigenToMsg(Eigen::Matrix<double,6,1>::Zero(),msg.error_pose);
    tf::twistEigenToMsg(Eigen::Matrix<double,6,1>::Zero(),msg.error_twist);

    tf::vectorEigenToMsg(Eigen::Vector3d::Zero(),msg.pose_d.linear);
    tf::vectorEigenToMsg(Eigen::Vector3d::Zero(),msg.twist_d.linear);
    tf::vectorEigenToMsg(Eigen::Vector3d::Zero(),msg.wrench_d.linear);

    msg.pose.linear.x = odom_msg->pose.pose.position.x;
    msg.pose.linear.y = odom_msg->pose.pose.position.y;
    msg.pose.linear.z = odom_msg->pose.pose.position.z;

    tf::vectorEigenToMsg(eta,msg.pose.angular);

    msg.twist.linear.x = odom_msg->twist.twist.linear.x;
    msg.twist.linear.y = odom_msg->twist.twist.linear.y;
    msg.twist.linear.z = odom_msg->twist.twist.linear.z;
    msg.twist.angular.x = odom_msg->twist.twist.angular.x;
    msg.twist.angular.y = odom_msg->twist.twist.angular.y;
    msg.twist.angular.z = odom_msg->twist.twist.angular.z;

    if (odom_msg->pose.pose.position.z >= -1)
        msg.pose_d.linear.z = odom_msg->pose.pose.position.z - 0.01;
    else
        msg.pose_d.linear.z = -1;

    msg.psi_d = 0.0;

    Eigen::Vector3d e = Eigen::Vector3d( msg.pose.linear.x - msg.pose_d.linear.x,
        msg.pose.linear.y - msg.pose_d.linear.y, msg.pose.linear.z - msg.pose_d.linear.z
    );

    Eigen::Vector3d eDot = Eigen::Vector3d( msg.twist.linear.x - msg.twist_d.linear.x,
        msg.twist.linear.y - msg.twist_d.linear.y, msg.twist.linear.z - msg.twist_d.linear.z
    );

    tf::vectorEigenToMsg(e,msg.error_pose.linear);
    tf::vectorEigenToMsg(eDot,msg.error_twist.linear);

    traj_pub.publish(msg);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "trajectory_node");
    ros::NodeHandle nh("~");

    traj_sub = nh.subscribe("/sub_topic", 1, odom_cb);
    traj_pub = nh.advertise<quad_control::UavState>("/pub_topic", 1);
    
    ros::spin();

    return 0;
}