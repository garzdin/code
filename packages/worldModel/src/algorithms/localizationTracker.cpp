 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * localizationTracker.cpp
 *
 *  Created on: Jun 10, 2017
 *      Author: Jan Feitsma
 */

#include "int/algorithms/localizationTracker.hpp"
#include "int/configurators/localizationConfigurator.hpp"

#include "FalconsCommon.h"

#include <algorithm>


localizationTracker::localizationTracker()
{
    _id = 0;
}

localizationTracker::~localizationTracker()
{
    // Chuck Norris stares down the code to make its own destructor joke.
}

void localizationTracker::updateExpectedPosition(Position2D const &deltaDisplacementPos)
{
    _position += deltaDisplacementPos;
}

void localizationTracker::feedMeasurement(robotMeasurementClass_t const &measurement)
{
    // store best of both mirror positions
    Position2D pos1 = measurement.getPosition();
    Position2D pos2 = mirror(measurement.getPosition());
    if (getPositionScore(pos1, _position) < getPositionScore(pos2, _position))
    {
        _position = pos1;
    }
    else
    {
        _position = pos2;
    }
    _visionConfidence = measurement.getConfidence();
    // store timestamp
    _lastUpdateTimestamp = measurement.getTimestamp();
    if (_recentTimestamps.size() == 0)
    {
        _creationTimestamp = _lastUpdateTimestamp;
    }
    _recentTimestamps.push_front(_lastUpdateTimestamp);
}

void localizationTracker::setId(int id)
{
    _id = id;
}

Position2D localizationTracker::mirror(Position2D const &position) const
{
    return Position2D(-position.x, -position.y, M_PI + position.phi);
}

// TODO consider merging getPosition and matchPosition
void localizationTracker::matchPosition(Position2D const &referencePosition)
{
    // choose best of current and mirror position w.r.t. reference
    Position2D pos1 = _position;
    Position2D pos2 = mirror(_position);
    float score1 = getPositionScore(pos1, referencePosition);
    float score2 = getPositionScore(pos2, referencePosition);
    if (score1 > score2)
    {
        // need a flip
        _position = mirror(_position);
    }
    //TRACE("score1=%6.2f, score2=%6.2f", score1, score2);
}

float localizationTracker::getPositionScore(Position2D const &positionA, Position2D const &positionB) const
{
    float dx = positionA.x - positionB.x;
    float dy = positionA.y - positionB.y;
    float dtheta = project_angle_mpi_pi(positionA.phi - positionB.phi);
    float score = fabs(dx) + fabs(dy) + fabs(dtheta) * localizationConfigurator::getInstance().get(localizationConfiguratorFloats::errorRatioRadianToMeter);
    return score;
}

float localizationTracker::getCameraMeasurementScore(robotMeasurementClass_t const &measurement) const
{
    // calculate score for both mirror variants
    float score1 = getPositionScore(measurement.getPosition(), _position);
    float score2 = getPositionScore(mirror(measurement.getPosition()), _position);
    float result = std::min(score1, score2);
    return result;
}

double localizationTracker::getLastUpdateTimestamp() const
{
    return _lastUpdateTimestamp;
}

float localizationTracker::getTrackerScore(double timestampNow)
{
    // get configurables
    float scoreAgeScale = localizationConfigurator::getInstance().get(localizationConfiguratorFloats::scoreAgeScale);
    float scoreActivityScale = localizationConfigurator::getInstance().get(localizationConfiguratorFloats::scoreActivityScale);
    float scoreFreshScale = localizationConfigurator::getInstance().get(localizationConfiguratorFloats::scoreFreshScale);
    // calculate partial scores, each in [0, 1], higher is better
    float ageScore = std::min(1.0, (_lastUpdateTimestamp - _creationTimestamp) / scoreAgeScale);
    float freshScore = 1.0 - std::min(1.0, (timestampNow - _lastUpdateTimestamp) / scoreFreshScale);
    cleanup(timestampNow);
    float activityScore = std::min(1.0f, _recentTimestamps.size() / scoreActivityScale);
    // total score then simply is the product of all partial scores
    float score = ageScore * freshScore * activityScore;
    TRACE("tracker id=%d has score age=%5.2f fresh=%5.2f activity=%5.2f total=%5.2f", _id, ageScore, freshScore, activityScore, score);
    return score;
}

Position2D localizationTracker::getPosition() const
{
    return _position;
}

float localizationTracker::getVisionConfidence() const
{
    return _visionConfidence;
}

bool localizationTracker::isTimedOut(double timestampNow) const
{
    return (timestampNow - _lastUpdateTimestamp) >= localizationConfigurator::getInstance().get(localizationConfiguratorFloats::trackerTimeout);
}

int localizationTracker::getId()
{
    return _id;
}

int localizationTracker::requestId()
{
    static int s_id = 0;
    return ++s_id;
}

void localizationTracker::cleanup(double timestampNow)
{
    float timeout = localizationConfigurator::getInstance().get(localizationConfiguratorFloats::trackerTimeout);
    _recentTimestamps.erase(std::remove_if(_recentTimestamps.begin(),
                                           _recentTimestamps.end(),
                            [=](double t) { return (timestampNow - t) > timeout; }),
                            _recentTimestamps.end());    
}

