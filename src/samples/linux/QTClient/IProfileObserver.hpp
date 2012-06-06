/* 
 *   This file is part of iviLINK 0.9 Technology Preview Samples Set.          
 *                                                                             
 *   iviLINK 0.9 Technology Preview Samples Set is free software:              
 *   you can redistribute it and/or modify                                     
 *   it under the terms of the GNU General Public License as published by      
 *   the Free Software Foundation, version 3 of the License.                   
 *                                                                             
 *   iviLINK 0.9 Technology Preview Samples Set is distributed in the          
 *   hope that it will be useful, but WITHOUT ANY WARRANTY; without            
 *   even the implied warranty of MERCHANTABILITY or FITNESS FOR A             
 *   PARTICULAR PURPOSE.  See the GNU General Public License for more details. 
 *                                                                              
 *   You should have received a copy of the GNU General Public License         
 *   along with iviLINK 0.9 Technology Preview.                                
 *   If not, see <http://www.gnu.org/licenses/>.                               
 * 
 */




#ifndef IPROFILE_OBSERVER_HPP
#define IPROFILE_OBSERVER_HPP

#include <string>

class IProfileObserver
{
    public:

    virtual void messageReceived(std::string profile_id, std::string message) = 0;
};
#endif
