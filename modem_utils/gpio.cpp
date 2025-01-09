/**
 * [2024] SubSeaPulse SRL 
 * All Rights Reserved.
 * @file gpio.cpp
 * @author Filippo Campagnaro
 * @version 1.0.0
 * @brief Module for GPIO control.
 */

// This is free software: you can redistribute it and/or modify it        *
// under the terms of the GNU General Public License version 3 as         *
// published by the Free Software Foundation.                             *
//                                                                        *
// This program is distributed in the hope that it will be useful, but    *
// WITHOUT ANY WARRANTY; without even the implied warranty of FITNESS     *
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for       *
// more details.                                                          *
//                                                                        *
// You should have received a copy of the GNU General Public License      *
// along with this program. If not, see <http://www.gnu.org/licenses/>.   *

#include "gpio.h"
#include <iostream>

const std::string GPIO::CHIPNAME = "gpiochip0"; 
const int GPIO::TXRX_PORT_N = 27; 
const int GPIO::AMP_POWER_PORT_N = 22; 

GPIO::~GPIO() 
{
    if (txrx_switch) 
    {
        txrxSwitch(false);
        gpiod_line_release(txrx_switch);
    } 
    if (amp_power_switch) 
    {
        ampSwitch(false);
        gpiod_line_release(amp_power_switch);
    }
    if(gpiochip) {
    	gpiod_chip_close(gpiochip);
    }
}

GPIO::GPIO()
{
    gpiochip = gpiod_chip_open_by_name(CHIPNAME.c_str());
    if (!gpiochip)
    {
        std::cout << "GPIO: cannot open chip: "  << CHIPNAME << std::endl;
        return;
    }
    txrx_switch = gpiod_chip_get_line(gpiochip, TXRX_PORT_N);
    if (txrx_switch)
    {
        if (gpiod_line_request_output(txrx_switch, "txrx_switch", 0)) //start in RX state
        {
            std::cout << "GPIO: unable to reserve txrx_switch line: " << TXRX_PORT_N  << std::endl; 
        };
    }
    else
    {
        std::cout << "GPIO: cannot get txrx_switch line: " << TXRX_PORT_N << std::endl;
    }

    amp_power_switch = gpiod_chip_get_line(gpiochip, AMP_POWER_PORT_N); 
    if (amp_power_switch)
    {
        if (gpiod_line_request_output(amp_power_switch, "amp_power_switch", 1))
        {
            std::cout << "GPIO: unable to reserve amp_power_switch line: " << AMP_POWER_PORT_N << std::endl;
        };
    }
    else
    {
        std::cout << "GPIO: cannot get amp_power_switch line: " << AMP_POWER_PORT_N << std::endl;
    }
    ampSwitch(true); 
}

bool GPIO::ampSwitch(bool enable)
{
    if (amp_power_switch)
    {
        if (gpiod_line_set_value(amp_power_switch, enable))
        {
            std::cout << "GPIO: unable to set amp_power_switch line: " << AMP_POWER_PORT_N << std::endl;
            return false;
        }
        return true;
    }
    return false;
}



bool GPIO::txrxSwitch(bool tx_on)
{
    if (txrx_switch)
    {
        if (gpiod_line_set_value(txrx_switch, tx_on))
        {
            std::cout << "GPIO: unable to set txrx_switch line: " << TXRX_PORT_N << std::endl;
            return false;
        }
        return true;
    }
    return false;
}