#include "ros/ros.h"
#include "quad_control/UavState.h"

#include <Eigen/Dense>
#include <tf/LinearMath/Matrix3x3.h>
#include <tf_conversions/tf_eigen.h>
#include <eigen_conversions/eigen_msg.h>

ros::Subscriber traj_sub;
ros::Publisher  traj_pub;

void odom_cb(const quad_control::UavState::ConstPtr& odom_msg) {
    quad_control::UavState msg;
    msg = *odom_msg;

    tf::vectorEigenToMsg(Eigen::Vector3d::Zero(),msg.pose_d.linear);
    tf::vectorEigenToMsg(Eigen::Vector3d::Zero(),msg.twist_d.linear);
    tf::vectorEigenToMsg(Eigen::Vector3d::Zero(),msg.wrench_d.linear);

    if (odom_msg->pose.linear.z >= -1)
        msg.pose_d.linear.z = odom_msg->pose.linear.z - 0.01;
    msg.psi_d.data = 0.0;

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

    ros::Rate rate(1000);

    traj_sub = nh.subscribe("/odometry", 1, odom_cb);
    traj_pub = nh.advertise<quad_control::UavState>("/trajectory", 1);
    
    ros::Duration(0.01).sleep();

    while(ros::ok()) {
        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}