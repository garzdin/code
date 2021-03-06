 /*** 
 2014 - 2017 ASML Holding N.V. All Rights Reserved. 
 
 NOTICE: 
 
 IP OWNERSHIP All information contained herein is, and remains the property of ASML Holding N.V. The intellectual and technical concepts contained herein are proprietary to ASML Holding N.V. and may be covered by patents or patent applications and are protected by trade secret or copyright law. NON-COMMERCIAL USE Except for non-commercial purposes and with inclusion of this Notice, redistribution and use in source or binary forms, with or without modification, is strictly forbidden, unless prior written permission is obtained from ASML Holding N.V. 
 
 NO WARRANTY ASML EXPRESSLY DISCLAIMS ALL WARRANTIES WHETHER WRITTEN OR ORAL, OR WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED, ANY IMPLIED WARRANTIES OR CONDITIONS OF MERCHANTABILITY, NON-INFRINGEMENT, TITLE OR FITNESS FOR A PARTICULAR PURPOSE. 
 
 NO LIABILITY IN NO EVENT SHALL ASML HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING WITHOUT LIMITATION ANY LOST DATA, LOST PROFITS OR COSTS OF PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES), HOWEVER CAUSED AND UNDER ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE OR THE EXERCISE OF ANY RIGHTS GRANTED HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 
 ***/ 
 /*
 * VoltageMonitor.cpp
 *
 *  Created on: Apr 25, 2017
 *      Author: Jan Feitsma
 */

#include "int/VoltageMonitor.hpp"
#include "cDiagnosticsEvents.hpp"

VoltageMonitor::VoltageMonitor()
{
    _idx = 0;
}

VoltageMonitor::~VoltageMonitor()
{
}

void VoltageMonitor::feed(float voltage)
{
    _buffer[_idx] = voltage;
    ++_idx;
    if (_idx == BUFFERSIZE)
    {
        _idx = 0;
    }
}

float VoltageMonitor::get()
{
    // crude filtering: simply take the max over buffer
    float result = 0;
    for (int idx = 0; idx < BUFFERSIZE; ++idx)
    {
        result = std::max(result, _buffer[idx]);
    }
    return result;
}

void VoltageMonitor::check()
{
    float voltage = get();
    // generate warning or error events every 0.1 volt??
    if (voltage < ERROR_THRESHOLD)
    {
        TRACE_ERROR_TIMEOUT(EVENT_TIMEOUT, "voltage dropped to %.1fV", voltage);
    }
    else if (voltage < WARNING_THRESHOLD)
    {
        TRACE_WARNING_TIMEOUT(EVENT_TIMEOUT, "voltage dropped to %.1fV", voltage);
    }
}

