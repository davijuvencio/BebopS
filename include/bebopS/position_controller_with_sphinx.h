/*
 * Copyright 2019 Giuseppe Silano, University of Sannio in Benevento, Italy
 * Copytight 2019 Peter Griggs, MIT, USA
 * Copyright 2019 Pasquale Oppido, University of Sannio in Benevento, Italy
 * Copyright 2019 Luigi Iannelli, University of Sannio in Benevento, Italy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BEBOP_CONTROL_POSITION_WITH_SPHINX_CONTROLLER_H
#define BEBOP_CONTROL_POSITION_WITH_SPHINX_CONTROLLER_H

#include <mav_msgs/conversions.h>
#include <mav_msgs/eigen_mav_msgs.h>

#include <string>

#include <ros/time.h>

#include <std_msgs/Empty.h>

#include "extendedKalmanFilter.h"
#include "waypoint_filter.h"
#include "waypointfilter_parameters.h"
#include "filter_parameters.h"
#include "stabilizer_types.h"
#include "parameters.h"
#include "common.h"

using namespace std;

namespace bebopS {

// Default values for the Parrot Bebop controller. For more information about the control architecture, please take a look
// at the publications page into the Wiki section.
static const Eigen::Vector2d kPDefaultXYController = Eigen::Vector2d(-26.4259, -26.3627);
static const double kPDefaultAltitudeController = -27.2277;

static const double kPDefaultRollController = -1.7514;
static const double kPDefaultPitchController = -1.7513;
static const double kPDefaultYawRateController = -14.3431;

static const Eigen::Vector2d MuDefaultXYController = Eigen::Vector2d(1, 1);
static const double MuDefaultAltitudeController = 1;

static const double MuDefaultRollController = 0.0544;
static const double MuDefaultPitchController = 0.0543;
static const double MuDefaultYawRateController = 0.44;

static const Eigen::Vector3d UqDefaultXYZ = Eigen::Vector3d(1.1810, 1.1810, 4.6697);


class PositionControllerParameters {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  PositionControllerParameters()
      : beta_xy_(kPDefaultXYController), 
        beta_z_(kPDefaultAltitudeController), 
        beta_phi_(kPDefaultRollController), 
        beta_theta_(kPDefaultPitchController),  
        beta_psi_(kPDefaultYawRateController),
        mu_xy_(MuDefaultXYController),
        mu_z_(MuDefaultAltitudeController),
        mu_theta_(MuDefaultPitchController),
        mu_phi_(MuDefaultRollController),
        mu_psi_(MuDefaultYawRateController),
	    U_q_(UqDefaultXYZ){
  }

  Eigen::Vector2d beta_xy_;
  double beta_z_;

  double beta_phi_;
  double beta_theta_;
  double beta_psi_;

  Eigen::Vector2d mu_xy_;
  double mu_z_;

  double mu_phi_;
  double mu_theta_;
  double mu_psi_;
  
  Eigen::Vector3d U_q_;
};
    
    class PositionControllerWithSphinx{
        public:
            PositionControllerWithSphinx();
            ~PositionControllerWithSphinx();
            void CalculateCommandSignals(geometry_msgs::Twist* ref_command_signals);

            void SetOdom(const EigenOdometry& odometry);
            void SetOdomFromLogger(const EigenOdometry& odometry_from_logger, const EigenOdometry& attitude_from_logger);
            void SetTrajectoryPoint(const mav_msgs::EigenTrajectoryPoint& command_trajectory_positionControllerNode);
            void SetControllerGains();
            void SetVehicleParameters();
            void SetWaypointFilterParameters();
            void SetFilterParameters();
            void SetLaunchFileParameters();
            void GetOdometry(nav_msgs::Odometry* odometry_filtered);
            void GetReferenceAngles(nav_msgs::Odometry* reference_angles);
            void GetTrajectory(nav_msgs::Odometry* smoothed_trajectory);
            void GetUTerrComponents(nav_msgs::Odometry* uTerrComponents);
            void GetVelocityAlongZComponents(nav_msgs::Odometry* zVelocity_components);
            void GetPositionAndVelocityErrors(nav_msgs::Odometry* positionAndVelocityErrors);
            void GetAngularAndAngularVelocityErrors(nav_msgs::Odometry* angularAndAngularVelocityErrors);
            
            PositionControllerParameters controller_parameters_;
            ExtendedKalmanFilter extended_kalman_filter_bebop_;
            VehicleParameters vehicle_parameters_;
            FilterParameters filter_parameters_;
            WaypointFilterParameters waypoint_filter_parameters_;
            WaypointFilter waypoint_filter_;

            //Launch file parameters
            std::string user_;
            double dataStoringTime_;
            bool dataStoring_active_;
            bool waypointFilter_active_;
            bool EKF_active_;

            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        private:
            //Boolean variables to active/inactive the controller and the data storage
            bool controller_active_;
            bool stateEmergency_;

            //publisher
            ros::Publisher land_pub_;
            ros::Publisher reset_pub_;

            //Sting vectors used to stare data
            std::vector<string> listControlSignals_;
            std::vector<string> listReferenceAngles_;
            std::vector<string> listVelocityErrors_;
            std::vector<string> listDroneAttitude_;
            std::vector<string> listTrajectoryErrors_;
            std::vector<string> listAttitudeErrors_;
            std::vector<string> listDerivativeAttitudeErrors_;
            std::vector<string> listDroneAngularVelocitiesABC_;
            std::vector<string> listDroneTrajectoryReference_;
	        std::vector<string> listDroneLinearVelocities_;
	        std::vector<string> listDronePosition_;
            std::vector<string> listCommandSinglasBefore_;
            std::vector<string> listCommandSinglasAfter_;
            std::vector<string> listOdometryFromBebopAutonomyPackage_;
          
            //Controller gains
            double beta_x_, beta_y_, beta_z_;
            double beta_phi_, beta_theta_, beta_psi_;

            double alpha_x_, alpha_y_, alpha_z_;
            double alpha_phi_, alpha_theta_, alpha_psi_;

            double mu_x_, mu_y_, mu_z_;
            double mu_phi_, mu_theta_, mu_psi_;
			
	        double lambda_x_, lambda_y_, lambda_z_;
	        double K_x_1_, K_x_2_;
	        double K_y_1_, K_y_2_;
	        double K_z_1_, K_z_2_;

            //Position and linear velocity errors
            double e_x_;
            double e_y_;
            double e_z_;
            double dot_e_x_;
            double dot_e_y_; 
            double dot_e_z_;
 
            //Attitude and angular velocity errors
            double e_phi_;
            double e_theta_;
            double e_psi_;
            double dot_e_phi_;
            double dot_e_theta_; 
            double dot_e_psi_;

            //Vehicle parameters
            double bf_, m_, g_;
            double l_, bm_;
            double Ix_, Iy_, Iz_;

            //Controller interface with Bebop paramters
            double u_z_sum_, vel_command_;
            double e_psi_sum_, yawRate_command_;            

            ros::NodeHandle n1_;
            ros::NodeHandle n2_;
            ros::NodeHandle n3_;
            ros::NodeHandle n4_;
            ros::Timer timer1_;
            ros::Timer timer2_;
            ros::Timer timer3_;

            //Callback functions to compute the errors among axis and angles
            void CallbackAttitude(const ros::TimerEvent& event);
            void CallbackPosition(const ros::TimerEvent& event);
            void CallbackSaveData(const ros::TimerEvent& event);
            void CallbackLand(const ros::TimerEvent& event);

            nav_msgs::Odometry odometry_filtered_private_;

	    state_t state_;
            control_t control_;
            mav_msgs::EigenTrajectoryPoint command_trajectory_;
            EigenOdometry odometry_;
            EigenOdometry odometry_from_logger_;
            EigenOdometry attitude_from_logger_;

            void Emergency();
            void LandEmergency();
            void SetOdometryEstimated();
            void CommandVelocity(double u_z, double* vel_command);
            void CommandYawRate(double* yawRate_command);
            void Quaternion2Euler(double* roll, double* pitch, double* yaw) const;
            void AttitudeController(double* u_phi, double* u_theta, double* u_psi);
            void AngularVelocityErrors(double* dot_e_phi, double* dot_e_theta, double* dot_e_psi);
            void AttitudeErrors(double* e_phi, double* e_theta, double* e_psi);
            void PosController(double* u_T, double* phi_r, double* theta_r, double* u_x, double* u_y, double* u_z, double* u_Terr);
            void PositionErrors(double* e_x, double* e_y, double* e_z);
            void VelocityErrors(double* dot_e_x, double* dot_e_y, double* dot_e_z);
            void ReferenceAngles(double* phi_r, double* theta_r);

    };

}
#endif // BEBOP_CONTROL_POSITION_WITH_SPHINX_CONTROLLER_H
