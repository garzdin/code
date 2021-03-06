 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * Diagnostics.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: Edwin Schreuder
 */

#include "int/Diagnostics.hpp"

#include <iostream>
#include <thread>

#include <cDiagnostics.hpp>
#include <cDiagnosticsEvents.hpp>

using namespace std;

Diagnostics::Diagnostics(PeripheralsInterfaceData& piData, bool ballhandlersAvailable) :
	piData(piData), diagSender(diagnostics::DIAG_HALMW, 0) {
	TRACE(">");

	started = false;

	if (ballhandlersAvailable) {
		desiredNumberOfDevices = 5;
	}
	else {
		desiredNumberOfDevices = 3;
	}

	previousNumberOfDevices = desiredNumberOfDevices;

	numberOfOnlineBoards = desiredNumberOfDevices;
	previousLeftMotionBoardOnline = true;
	previousRightMotionBoardOnline = true;
	previousRearMotionBoardOnline = true;
	previousLeftBallhandlerBoard = true;
	previousRightBallhandlerBoard = true;

	TRACE("<");
}

void Diagnostics::start() {
	TRACE(">");

	started = true;
	diagnosticsThread = thread(&Diagnostics::timer, this);

	TRACE("<");
}

void Diagnostics::stop() {
	TRACE(">");

	started = true;
	diagnosticsThread = thread(&Diagnostics::timer, this);

	TRACE("<");
}

void Diagnostics::timer() {
	TRACE(">");

	cout << "INFO    : Starting diagnostics." << endl;

	while (started) {
		addSensorData();
		addStatus();

		this_thread::sleep_for(chrono::milliseconds(100));
	}

	TRACE("<");
}

void Diagnostics::addSensorData() {
	rosMsgs::t_diag_halmw msg;

	piVelAcc v = piData.getVelocityInput();
	msg.speed_vx = v.vel.x;
	msg.speed_vy = v.vel.y;
	msg.speed_vphi = v.vel.phi;

	v = piData.getVelocityOutput();
	msg.feedback_vx = v.vel.x;
	msg.feedback_vy = v.vel.y;
	msg.feedback_vphi = v.vel.phi;
	
	// check if driving
	bool isDriving = false;
	if (fabs(v.vel.x) > 0.1) isDriving = true;
	if (fabs(v.vel.y) > 0.1) isDriving = true;
	if (fabs(v.vel.phi) > 0.1) isDriving = true;

	msg.hasball = piData.getHasBall();

	MotionBoardDataOutput rearMotionBoardData = piData.getRearMotionBoard().getDataOutput();
	MotionBoardDataOutput rightMotionBoardData = piData.getRightMotionBoard().getDataOutput();
	MotionBoardDataOutput leftMotionBoardData = piData.getLeftMotionBoard().getDataOutput();

	msg.motion_rear_temperature = rearMotionBoardData.motion.motorTemperature;
	msg.motion_left_temperature = leftMotionBoardData.motion.motorTemperature;
	msg.motion_right_temperature = rightMotionBoardData.motion.motorTemperature;

	// voltage 
	voltageMonitor.feed(rearMotionBoardData.motorController.voltage);
	voltageMonitor.feed(leftMotionBoardData.motorController.voltage);
	voltageMonitor.feed(rightMotionBoardData.motorController.voltage);
	msg.voltage = voltageMonitor.get();
	
	// check voltage, but do not generate warnings while driving
	if (!isDriving)
	{
		voltageMonitor.check();
	}

	// get ballhandler angles
	msg.bh_left_angle = piData.getLeftBallhandlerBoard().getDataOutput().ballhandler.angle * 0.01;
	msg.bh_right_angle = piData.getRightBallhandlerBoard().getDataOutput().ballhandler.angle * 0.01;

	diagSender.set(msg);
}

void Diagnostics::addStatus() {

	size_t numberOfDevices = piData.getNumberOfConnectedDevices();

	if (numberOfDevices != previousNumberOfDevices) {
		logDeviceStatus(numberOfDevices);
		previousNumberOfDevices = numberOfDevices;
	}

	if (numberOfDevices > 0) {
		addBoardStatus();
	}
}

void Diagnostics::logDeviceStatus(size_t numberOfDevices) {

	if (numberOfDevices == 0) {
			// No USB converters connected.
			cerr << "ERROR   : No USB to Serial converters connected." << endl;
			TRACE_ERROR("No USB to Serial converters connected; USB problem?");
	}
	else if (numberOfDevices < desiredNumberOfDevices) {
		if ((previousNumberOfDevices >= numberOfDevices) || (previousNumberOfDevices == 0)) {
			cerr << "ERROR   : Not all (" << numberOfDevices << "/" << desiredNumberOfDevices << ") USB to Serial converters connected." << endl;
			TRACE_ERROR("Not all (%d/%d) USB to Serial converters connected; USB cable loose?",
					numberOfDevices, desiredNumberOfDevices);
		}
	}
	else {
		if (numberOfDevices >= desiredNumberOfDevices) {
			cout << "ERROR   : All USB to Serial converters connected." << endl;
			TRACE_INFO("All USB to Serial converters connected.");
		}
	}
}

void Diagnostics::addBoardStatus() {
	size_t previousNumberOfOnlineBoards = numberOfOnlineBoards;
	numberOfOnlineBoards = 0;

	bool leftMotionBoardOnline = updateBoardStatus(piData.getLeftMotionBoard().isOnline());
	bool rightMotionBoardOnline = updateBoardStatus(piData.getRightMotionBoard().isOnline());
	bool rearMotionBoardOnline = updateBoardStatus(piData.getRearMotionBoard().isOnline());

	bool leftBallhandlerBoardOnline = updateBoardStatus(piData.getLeftBallhandlerBoard().isOnline());;
	bool rightBallhandlerBoardOnline = updateBoardStatus(piData.getRightBallhandlerBoard().isOnline());;

	if (numberOfOnlineBoards > 0) {
		if ((previousNumberOfOnlineBoards == 0) && (numberOfOnlineBoards >= desiredNumberOfDevices)) {
			cout << "INFO    : EMO unpressed." << endl;
			TRACE_INFO("EMO unpressed.");
		}
		else {
			logBoardStatus(previousLeftMotionBoardOnline, leftMotionBoardOnline, "Left motion");
			logBoardStatus(previousRightMotionBoardOnline, rightMotionBoardOnline, "Right motion");
			logBoardStatus(previousRearMotionBoardOnline, rearMotionBoardOnline, "Rear motion");
			logBoardStatus(previousLeftBallhandlerBoard, leftBallhandlerBoardOnline, "Left ballhandler");
			logBoardStatus(previousRightBallhandlerBoard, rightBallhandlerBoardOnline, "Right ballhandler");
		}
	}
	else {
		if (previousNumberOfOnlineBoards > 0) {
			cerr << "ERROR   : EMO pressed." << endl;
			TRACE_ERROR("EMO pressed.");
		}
	}

	previousLeftMotionBoardOnline = leftMotionBoardOnline;
	previousRightMotionBoardOnline = rightMotionBoardOnline;
	previousRearMotionBoardOnline = rearMotionBoardOnline;
	previousLeftBallhandlerBoard = leftBallhandlerBoardOnline;
	previousRightBallhandlerBoard = rightBallhandlerBoardOnline;
}

bool Diagnostics::updateBoardStatus(bool boardOnline) {
	if (boardOnline) {
		numberOfOnlineBoards++;
	}

	return boardOnline;
}

void Diagnostics::logBoardStatus(bool previousBoardOnline, bool boardOnline, string name) {
	if (boardOnline && !previousBoardOnline) {
		cout << "INFO    : " << name << " board is online again!" << endl;
		TRACE_INFO("%s board is online again!", name.c_str());
	}
	if (!boardOnline && previousBoardOnline) {
		cerr << "ERROR   : " << name << " board is not available." << endl;
		TRACE_ERROR("%s board is not available.", name.c_str());
	}
}
