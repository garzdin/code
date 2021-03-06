 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * objectMeasurementType.hpp
 *
 *  Created on: Jan 10, 2017
 *      Author: Jan Feitsma
 *
 * Capturing the commonality of ball- and obstacle measurements.
 */

#ifndef OBJECTMEASUREMENTTYPE_HPP_
#define OBJECTMEASUREMENTTYPE_HPP_

#include "int/types/uniqueWorldModelIDtype.hpp"
#include "int/types/cameraType.hpp"

class objectMeasurementType
{
	public:
		objectMeasurementType();
		~objectMeasurementType();

		void setID(const uniqueWorldModelID id);
		void setTimestamp(const double stamp);
		void setCameraType(const cameraType type);
		void setConfidence(const float confidence);
		void setSphericalCoords(const float azimuth, const float elevation, const float radius);
		void setCameraOffset(const float camX, const float camY, const float camZ, const float camPhi);

		uniqueWorldModelID getID() const;
		double getTimestamp() const;
		cameraType getCameraType() const;
		float getConfidence() const;
		float getAzimuth() const;
		float getElevation() const;
		float getRadius() const;
		float getCameraX() const;
		float getCameraY() const;
		float getCameraZ() const;
		float getCameraPhi() const;

	private:
		uniqueWorldModelID _identifier;
		double _timestamp; // instead of timeval, for performance and ease of computation
		cameraType _cameraType;
		float _confidence;
		float _azimuth;
		float _elevation;
		float _radius;
		float _cameraX;
		float _cameraY;
		float _cameraZ;
		float _cameraPhi;
};

#endif /* OBJECTMEASUREMENTTYPE_HPP_ */
