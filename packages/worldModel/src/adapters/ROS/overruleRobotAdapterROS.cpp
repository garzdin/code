 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * overruleRobotAdapterROS.cpp
 *
 *  Created on: Nov 15, 2016
 *      Author: Tim Kouters
 */

#include "int/adapters/ROS/overruleRobotAdapterROS.hpp"

#include "ext/WorldModelNames.h"

#include "cDiagnosticsEvents.hpp"
#include "FalconsCommon.h"
#include "timeConvert.hpp"

overruleRobotAdapterROS::overruleRobotAdapterROS()
{

}

overruleRobotAdapterROS::~overruleRobotAdapterROS()
/*
 * Chuck Norris makes fire by rubbing 2 ice cubes together
 */
{

}

void overruleRobotAdapterROS::InitializeROS()
{
	try
	{
		_hROS.reset(new ros::NodeHandle());

		_srvOverruleRobot = _hROS->advertiseService(
                WorldModelInterface::s_set_overrule_own_location,
                &overruleRobotAdapterROS::overrule_own_location_cb, this);
	}
	catch(std::exception &e)
	{
		TRACE_ERROR("Caught exception: %s", e.what());
		std::cout << "Caught exception: " << e.what() << std::endl;
		throw std::runtime_error(std::string("Linked to: ") + e.what());
	}
}

bool overruleRobotAdapterROS::overrule_own_location_cb(
        		worldModel::overrule_own_location::Request &req,
        		worldModel::overrule_own_location::Response &resp)
{
	try
	{
		robotClass_t robot;

		robot.setCoordinateType(coordinateType::FIELD_COORDS);
		robot.setCoordinates(
				req.x,
				req.y,
				req.theta);
		robot.setVelocities(
				req.vx,
				req.vy,
				req.vtheta);
		robot.setRobotID(getRobotNumber());
		robot.setTimestamp(getTimeNow() + req.overruleTimeInSeconds);

		notify(robot);

		return true;
	}
	catch(std::exception &e)
	{
		TRACE_ERROR("Caught exception: %s", e.what());
		std::cout << "Caught exception: " << e.what() << std::endl;
		throw std::runtime_error(std::string("Linked to: ") + e.what());
	}
}
