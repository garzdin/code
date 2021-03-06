 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * robotMeasurementType.cpp
 *
 *  Created on: Aug 16, 2016
 *      Author: Tim Kouters
 */

#include "int/types/robot/robotMeasurementType.hpp"

robotMeasurementClass_t::robotMeasurementClass_t()
{
	_coordinate = coordinateType::ROBOT_COORDS;
	_timestamp = 0.0;
	_confidence = 0.0;
	_x = 0.0;
	_y = 0.0;
	_theta = 0.0;
}

robotMeasurementClass_t::~robotMeasurementClass_t()
/*
 * Chuck Norris can knit the softest sweater using electrical pylons
 */
{

}

void robotMeasurementClass_t::setID(const uniqueWorldModelID identifier)
{
	_identifier = identifier;
}

void robotMeasurementClass_t::setCoordinateType(const coordinateType coordinates)
{
	_coordinate = coordinates;
}

void robotMeasurementClass_t::setTimestamp(const double timestamp)
{
	_timestamp = timestamp;
}

void robotMeasurementClass_t::setConfidence(const float confidence)
{
	_confidence = confidence;
}

void robotMeasurementClass_t::setPosition(const float x, const float y, const float theta)
{
	_x = x;
	_y = y;
	_theta = theta;
}


uniqueWorldModelID robotMeasurementClass_t::getID() const
{
	return _identifier;
}

coordinateType robotMeasurementClass_t::getCoordindateType() const
{
	return _coordinate;
}

double robotMeasurementClass_t::getTimestamp() const
{
	return _timestamp;
}

float robotMeasurementClass_t::getConfidence() const
{
	return _confidence;
}

float robotMeasurementClass_t::getX() const
{
	return _x;
}

float robotMeasurementClass_t::getY() const
{
	return _y;
}

float robotMeasurementClass_t::getTheta() const
{
	return _theta;
}

Position2D robotMeasurementClass_t::getPosition() const
{
    return Position2D(_x, _y, _theta);
}

