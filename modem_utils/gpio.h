/**
 * [2024] SubSeaPulse SRL 
 * All Rights Reserved.
 * @file gpio.h
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

#ifndef GPIO_MODULE_H
#define GPIO_MODULE_H

#include <string>
#include <gpiod.h>
class GPIO
{
public:
    static const std::string CHIPNAME; /*< name of GPIO*/
    static const int TXRX_PORT_N; /*< port to switch the tx/rx switch*/
    static const int AMP_POWER_PORT_N; /* port to switch on and off the amplifier*/

    /** 
     * Class constructor.
     */
    GPIO();

    /** 
     * Class destructor.
     */
    ~GPIO();

    /**
     * TX/RX switch
     * @param tx_on boolean switch true=TX, false=RX
     * @return true if the switch was successful, false otherwise
     */  
    bool txrxSwitch(bool tx_on);

private:
    struct gpiod_chip *gpiochip = nullptr; /*< objec to open the gpio*/
    struct gpiod_line *amp_power_switch = nullptr; /*< line to enable/disable the power amp*/
    struct gpiod_line *txrx_switch = nullptr; /*< line to switch between tx and rx*/

    /**
     * Turns on the supply for the TX amplifier.
     * @param tx_on boolean switch true=TX, false=RX
     * @return true if the switch was successful, false otherwise
     */  
    bool ampSwitch(bool enable); 

};
#endif /* GPIO_MODULE_H */