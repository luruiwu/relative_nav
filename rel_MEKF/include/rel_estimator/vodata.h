 /* \copyright This work was completed by Robert Leishman while performing official duties as 
  * a federal government employee with the Air Force Research Laboratory and is therefore in the 
  * public domain (see 17 USC § 105). Public domain software can be used by anyone for any purpose,
  * and cannot be released under a copyright license
  */

/*!
 *  \file vodata.h
 *  \brief Contains the class VOData, which is used to hold visual odometry data.
 *  \author Robert Leishman
 *  \date July 2012
*/

#ifndef VODATA_H
#define VODATA_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>
#include <Eigen/StdDeque>
#include <ros/time.h>
#include "kinect_vo/kinect_vo_message.h"


/*!
 *  \typedef kinect_visual_odometry::kinect_visual_odometry is replaced with k_message
*/
typedef kinect_vo::kinect_vo_message k_message;





/*!
 *  \typedef To make a Row-Major eigen matrix:
*/
typedef Eigen::Matrix<double,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> RMatrixXd;


/*!
 *  \class VOData vodata.h "include/rel_estimator/vodata.h"
 *  \brief This class is used to hold all the elements of the visual odometry data.
 *  I have found that continued use of the ros message for VO data is too combursome, especially dealing with covariance
 *  data.
*/
class VOData
{
public:
  /// Eigen macro used when there are fixed-sized class member variables and you dynamically create an instance of the
  /// class.  (See: http://eigen.tuxfamily.org/dox/TopicStructHavingEigenMembers.html)
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  /*!
   *  \brief Default constructor, zeros everything
  */
  VOData();

  /*!
   *  \brief This constructor initializes everything using the ROS message
   *  \param vo_message is the current VO message
  */
  VOData(const k_message &vo_message);


  /*!
   *  \brief allows initialization for an insance set by default constructor
  */
  void SetData(k_message vo_message);


  /*!
   *  \brief Allows the VOData to be copied
  */
  void Copy(VOData *other_vo_data)
  {
    this->timestamp_ = other_vo_data->Timestamp();
    this->new_reference_ = other_vo_data->NewReference();
    this->inliers_ = other_vo_data->Inliers();
    this->corresponding_ = other_vo_data->Corresponding();
    this->image_number_ = other_vo_data->ImageNumber();
    this->total_covariance_ = other_vo_data->Covariance();
    this->translation_ = other_vo_data->Translation();
    this->rotation_ = other_vo_data->Rotation();
    this->child_frame_id_ = other_vo_data->ChildFrameID();
    this->parent_frame_id_ = other_vo_data->ParentFrameID();
  }



  /// The following functions simply provide read-only access to the message:

  ros::Time Timestamp(){return timestamp_;}

  bool NewReference(){return new_reference_;}

  int Inliers(){return inliers_;}

  int Corresponding(){return corresponding_;}

  int ImageNumber(){return image_number_;}

  Eigen::Matrix<double,7,7> Covariance(){return total_covariance_;}

  Eigen::Vector3d Translation(){return translation_;}

  Eigen::Quaterniond Rotation(){return rotation_;}

  std::string ChildFrameID(){return child_frame_id_;}

  std::string ParentFrameID(){return parent_frame_id_;}


  /// The following allow write access to some members:
  void setCovariance(Eigen::Matrix<double,7,7> &new_covariance){total_covariance_ = new_covariance;}

  void setTranslation(Eigen::Vector3d &new_translation){translation_ = new_translation;}

  void setRotation(Eigen::Quaterniond &new_quaternion){rotation_ = new_quaternion;}


protected:
  ros::Time timestamp_; //!< the ROS timestamp with the current image was taken
  bool new_reference_; //!< bool denoting if the image was made the next reference image
  int inliers_; //!< the number of inliers for the particular vo message
  int corresponding_; //!< the number of corresponding features found
  int image_number_; //!< the image number (or sequence from the header)
  Eigen::Matrix<double,7,7> total_covariance_; //!< the covariance on the transformation
  Eigen::Vector3d translation_; //!< the translation portion of the transformation between images
  Eigen::Quaterniond rotation_; //!< the rotation portion of the transformation
  std::string child_frame_id_; //!< the name of the child reference frame i.e. the current camera reference frame
  std::string parent_frame_id_; //!< the name of the reference frame that is the basis for the transformation
};

#endif // VODATA_H
