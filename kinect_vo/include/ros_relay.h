 /* \copyright This work was completed by Robert Leishman while performing official duties as 
  * a federal government employee with the Air Force Research Laboratory and is therefore in the 
  * public domain (see 17 USC § 105). Public domain software can be used by anyone for any purpose,
  * and cannot be released under a copyright license
  */

/*! \file ros_relay.h
  * \author Robert Leishman
  * \date March 2012
  *
  * \brief The ros_relay.h file is the header for the ros_relay class.
  *
  *
*/

// guard against multiple inclusion
#ifndef ROS_RELAY_H
#define ROS_RELAY_H

#include "ros/ros.h"
#include <string.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/TransformStamped.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>
#include <eigen3/Eigen/Dense>
#include <sys/stat.h>  // Provides the mkdir() function
#include <boost/array.hpp>

#include "pose_estimator.h"
//#include "image_display.h"
#include "kinect_vo/kinect_vo_message.h"
#include "kinect_vo/request_new_reference.h"
#include "evart_bridge/transform_plus.h"



extern pthread_mutex_t mutex_; //!< mutex to change "set_next_as_ref_"


/*!
 * \typedef kinectSyncePolicy
 * \brief From RGBDSlam, this typedef brings kinect messages with almost the same timestamp into one callback
 * It combines two image messages and two camera information messages into a single message.
 * Used withing the ROSRelay class, by the function kinectCallback
*/
typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image,
                                                        sensor_msgs::Image,
                                                        sensor_msgs::CameraInfo,
                                                        sensor_msgs::CameraInfo> kinectSyncPolicy; //

/*!
 *  \class ROSRelay ros_relay.h "include/ros_relay.h"
 *  \brief The ROS relay class provides the communication functionality for the visual odometery.  It listens for the
 *  appropriate messages from ROS, brings in the images and transfers them over to OpenCV for processing, and it
 *  publishes the 6 Degree of Freedom (DOF) change in pose determined by the software back to those listening.
 *
*/
class ROSRelay
{

public:

  /*!
   *  \brief The constructor expects the ROS Node Handle, to arrage the sychronization policy.
   *
   *  The relavent topics are advertised and subscribed to and
   *  \todo the processing class is instatiated!
   *
   *  \param nh the handle for the ROS node started by main
  */
  ROSRelay(ros::NodeHandle &nh);

  /*!
   *  \brief The destructor, it destroys things...
   *  \todo write what the destructor destroys!
  */
  ~ROSRelay();


  /*!
   *  \brief This function is called when the image syncronizer recieves a packet from openni_camera.  It takes the
   *  images in and if there is a trigger to process, it sends the images off to process.
   *
   *  This function is called through the callback, set up in the constructor of ROSRelay.  It will be called as often as
   *  data is recieved on the topics for the color image, depth image, and camera information.  To subsample, images will
   *  only be processed with the flag "process_images_" is true.  The callback provides the images to this function.
   *
   *  \param rbg_image is the color image from the kinect
   *  \param depth_image is the depth image from the kinect
   *  \param depth_info contains the camera information (calibration parameters) for the kinect depth camera
   *  \param rgb_info contains the RGB camera calibration params, which are used to create the 3D points later
  */
  void kinectCallback(const sensor_msgs::ImageConstPtr& rbg_image,
                      const sensor_msgs::ImageConstPtr& depth_image,
                      const sensor_msgs::CameraInfoConstPtr& depth_info,
                      const sensor_msgs::CameraInfoConstPtr& rgb_info); //

  /*!
   *  \brief The callback for when motion capture truth is available.
   *
   *  Once the data is recieved, it is logged in a file.  <b> This function is only called when logging is enabled. </b>
   *
   *  \param pose is the stamped transform as given by the topic from the motion capture
   *
  */
  void motionCaptureCallback(const evart_bridge::transform_plus &pose);


  /*!
   *  \brief This callback is used to supply the visual odometry with a transformation to seed the search for
   *  correspondences.
   *  \param transformation takes points in the reference camera frame and expresses them in the current camera frame
  */
  void rotationMatrixCallback(const geometry_msgs::TransformStamped &transformation);


  /*!
   *  \brief Callback used when the service for resetting the reference image is called.  When called, the next image
   *  will become the reference image.
   *
  */
  inline bool newReferenceCallback(kinect_vo::request_new_reference::Request &request,
                                   kinect_vo::request_new_reference::Response &response)
  {
    pthread_mutex_lock(&mutex_);
      set_next_as_ref_ = true;
    pthread_mutex_unlock(&mutex_);
    response.succeed = true;
    return true;
  }

protected:
  //Variables:

  message_filters::Synchronizer<kinectSyncPolicy>* kinect_sync_; /*!< This is a message synchronizer to get all the data
                                                                 from the kinect at the same time. */
  message_filters::Subscriber<sensor_msgs::Image> *visual_sub_; //!< kinect rbg image for the subscriber
  message_filters::Subscriber<sensor_msgs::Image> *depth_sub_;  //!< kinect depth image for the subscriber
  message_filters::Subscriber<sensor_msgs::CameraInfo> *depth_info_sub_;  //!< kinect camera info for the subscriber
  message_filters::Subscriber<sensor_msgs::CameraInfo> *cam_info_sub_;  //!< RGB camera infor for the subscriber

  ros::Publisher pose_publisher_; //!< ROS publisher for publishing the pose changes found by the VO algorithm.
  ros::Publisher alt_pose_publisher_; //!< Publish the alternate way of doing the covariance in a different message
  ros::Publisher rgb_keyframe_pub_; //!< publish keyframe RGB images
  ros::Publisher depth_keyframe_pub_; //!< publishe the keyframe depth images
  ros::Publisher rgb_camera_info_pub_; //!< republishes the RGB camera info
  ros::Subscriber mocap_subscribe_; //!< ROS subscriber for bringing in motion capture data.
  ros::Subscriber rotation_subscriber_; //!< ROS subscriber for bringing in an estimate of the rotation and translation
  //! between images.
  ros::ServiceServer newReferenceServer_; //!< ROS Service server for requesting that the next image be a reference image

  std::string mocap_topic_;
  std::string rotation_topic_; //!< topic for rotation subscriber
  bool optimize_; //!< flag for enabling optimization
  bool save_show_images_; //!< flag for enabling the display and saving of images
  bool enable_logging_; //!< flag for enabling logs (truth expressed in relative sense and VO logs)
  bool publish_keyframes_; //!< flag for enabling republishing keyframe images
  bool set_next_as_ref_; //!< this bool sets the next current image as the reference image.
  bool set_as_reference_; //!< if the current is set as reference
  bool set_mocap_as_ref_; //!< to tell the motion capture function to set the next value as reference

  evart_bridge::transform_plus ref_pose_; //!< the reference pose using motion capture data

  cv::Mat depth_mono8_image_; //!< the depth image 8 bit image for masking
  cv::Mat visual_image_; //!< the color image

  bool process_images_; //!< flag for whether or not the images should be processed             (NECESSARY????)

  PoseEstimator *pose_estimator_; //!< instance of the pose estimator to calculate the change in pose between two images

  cv::Mat rotation_estimate_; //!< The current rotation estimate between the reference camera and the current camera

  int dropped_frames_; //!< Counter for any frames that were unable to be processed (not enough matches is the underlying reason)
  int keyframe_index_; //!< counter for the keyframe number

  //for debug only, to make sure things are working so far...
  std::string VISUAL_WINDOW;
  std::string DEPTH_WINDOW;
  bool first_mocap_;


  //File Stuff (should be temporary, once Bag files are implemented)
  std::ofstream log_file_;
  std::string file_name_;
  //2nd file for cortex:
  std::ofstream cortex_file_;
  std::string cortex_name_;

  //ImageDisplay *display;


  //Methods:
  /*!
   *  \brief This function, taken from the RGBDSLAM package, makes the depth image into a mono,
   *  so that it can be used as a mask.
   *
   *  \param float_img is the float version of the depth image (recieved over ROS)
   *  \param mono8_img is an image of type CV_8UC1
   *  \returns the mono8_img by reference
  */
  inline void depthToCV8UC1(const cv::Mat& float_img, cv::Mat& mono8_img)
  {
    if(mono8_img.rows != float_img.rows || mono8_img.cols != float_img.cols)
    {
      mono8_img = cv::Mat(float_img.size(), CV_8UC1);
    }
    cv::convertScaleAbs(float_img, mono8_img, 100, 0.0);
  }



  /*!
   *  \brief Brings essential elements from a time structure into a string for a filename.
   *
   *  \param time_struct is the time structure containing the time
   *  \returns a string for the file name (without spaces for ease of matlab import)
  */
  std::string timeStructFilename(struct tm *time_struct);


  /*!
   * \brief Quick conversion between an Eigen Matrix and a Boost::array (and ensures positive covariance)
   * \param matrix is the Eigen matrix (comes in full)
   * \param matptr is the boost::array which is returned as a copy of matrix
  */
  inline void eigenToMatrixPtr(const Eigen::Matrix<double,7,7> &matrix, boost::array<double,49> &matptr)
  {
    for( size_t row = 0; row < 7; row++ )
      for( size_t col = 0; col < 7; col++ )
      {
        matptr[ row * 7 + col ] = matrix(row,col);
      }
  }


//  /*!
//   *  \brief Rotation matricies are changed to quaternions using this function
//   *
//   *  No assumption is made regarding the nature of the rotation matrix here (i.e. which Euler sequence is used).
//   *
//   *  \param R is a direction-cosines based rotation matrix (could be from an Euler sequence)
//   *  \param q is a vector of doubles that returns the quaternion.
//  */
//  void convertRToQuaternion(cv::Mat R, std::vector<double> *q);


//  /*!
//   *  \brief Change a quaternion to a rotation matrix
//   *
//   *  \param R is the returned direction-cosines based rotation matrix (could be from an Euler sequence)
//   *  \param transform contains the supplied quaternion.
//  */
//  void convertQuaterniontoR(cv::Mat *R, geometry_msgs::TransformStamped transform);


};
#endif
