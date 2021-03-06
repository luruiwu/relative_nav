 /* \copyright This work was completed by Robert Leishman while performing official duties as 
  * a federal government employee with the Air Force Research Laboratory and is therefore in the 
  * public domain (see 17 USC § 105). Public domain software can be used by anyone for any purpose,
  * and cannot be released under a copyright license
  */

/*! \file statepacket.h
  * \author Robert Leishman
  * \date June 2012
  *
  * \brief The statepacket.h file is the header for the StatePacket class.
*/

#ifndef STATEPACKET_H
#define STATEPACKET_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <ros/time.h>
#include "rel_estimator/constants.h"

/*!
 *  \class StatePacket statepacket.h "include/rel_estimator/statepacket.h"
 *  \brief This class is a container for the state and covariance.  To accomplish delayed updates for visual data
 *  we must save the state when an image is taken, and continue on.  Once the visual data is ready to be put into the
 *  filter, we have to restore the state and covariance to what they were when the image was taken, apply the update
 *  and then repropogate the IMU and altimeter measurements up to the current time.  This class stores the state and
 *  covariance
*/
class StatePacket
{
public:
  /*!
   *  \brief Constructor initializes the state and covariance to zeros
  */
  StatePacket(){x_.setZero(); P_.setZero(); ros::Time temp(0.d); time_ = temp;}

  /*!
   *  \brief Constructor initializes the state and covariance
   *  \param x is the state
   *  \param P is the covariance
   *  \param t is the timestamp of the state from ROS
  */
  StatePacket(Eigen::Matrix<double, STATE_LENGTH, 1> &x, Eigen::Matrix<double, COVAR_LENGTH, COVAR_LENGTH> &P, ros::Time &t)
  {x_ = x; P_ = P; time_ = t;}

  /*!
   *  \brief Returns the state vector
  */
  Eigen::Matrix<double,STATE_LENGTH,1> getState(){return x_;}

  /*!
   *  \brief Replace the state with a new version
  */
  void setState(Eigen::Matrix<double, STATE_LENGTH, 1> &state){x_ = state;}

  /*!
   *  \brief Return the covariance
  */
  Eigen::Matrix<double,COVAR_LENGTH,COVAR_LENGTH> getCovariance(){return P_;}

  /*!
   *  \brief Replace the covariance
  */
  void setCovariance(Eigen::Matrix<double, COVAR_LENGTH, COVAR_LENGTH> &covar){P_ = covar;}

  /*!
   *  \brief Return the timestamp
  */
  ros::Time getTime(){return time_;}

  /*!
   *  \brief Replace the timestamp with t.
  */void SetTime(ros::Time &t){time_ = t;}

  //~StatePacket(){}

protected:
  Eigen::Matrix<double, STATE_LENGTH, 1> x_;  //!< the state
  Eigen::Matrix<double, COVAR_LENGTH, COVAR_LENGTH> P_; //!< the covariance
  ros::Time time_; //!< the timestamp
};
#endif // STATEPACKET_H
