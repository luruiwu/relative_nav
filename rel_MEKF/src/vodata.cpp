 /* \copyright This work was completed by Robert Leishman while performing official duties as 
  * a federal government employee with the Air Force Research Laboratory and is therefore in the 
  * public domain (see 17 USC § 105). Public domain software can be used by anyone for any purpose,
  * and cannot be released under a copyright license
  */

/*!
 *  \file vodata.cpp
 *  \author Robert Leishman
 *  \date July 2012
*/


#include "rel_estimator/vodata.h"

using namespace Eigen;

//
// Default Constructor
//
VOData::VOData()
{
  total_covariance_.setZero();
  parent_frame_id_ = "";
  child_frame_id_ = "";
  translation_.setZero();
}



//
// Typcial Constructor
//
VOData::VOData(const k_message &vo_message)
{
  SetData(vo_message);
}



//
// Function to set data using a message
//
void VOData::SetData( k_message vo_message)
{
  timestamp_ = vo_message.header.stamp;
  parent_frame_id_ = vo_message.header.frame_id;
  image_number_ = vo_message.header.seq;
  child_frame_id_ = vo_message.child_frame_id;
  new_reference_ = vo_message.newReference;
  inliers_ = vo_message.inliers;
  corresponding_ = vo_message.corresponding;
  translation_ << vo_message.transform.translation.x, vo_message.transform.translation.y, vo_message.transform.translation.z;
  rotation_.x() = vo_message.transform.rotation.x;
  rotation_.y() = vo_message.transform.rotation.y;
  rotation_.z() = vo_message.transform.rotation.z;
  rotation_.w() = vo_message.transform.rotation.w;

  Map<RMatrixXd> cov(vo_message.covariance.data(),7,7);
  Matrix<double,7,7> cov2;
  cov2 = cov; //this step is just to be sure I don't get a data corruption issue...
  total_covariance_ = cov2;
}
