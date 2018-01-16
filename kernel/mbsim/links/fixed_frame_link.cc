/* Copyright (C) 2004-2014 MBSim Development Team
 * 
 * This library is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either 
 * version 2.1 of the License, or (at your option) any later version. 
 * 
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * Contact: martin.o.foerg@googlemail.com
 */

#include <config.h>
#include "mbsim/links/fixed_frame_link.h"
#include "mbsim/frames/frame.h"
#include "mbsim/utils/eps.h"

using namespace std;
using namespace fmatvec;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSim {

  FixedFrameLink::FixedFrameLink(const std::string &name) : FrameLink(name) {
  }

  void FixedFrameLink::calcSize() {
    ng = 1;
    ngd = 1;
    nla = 1;
    updSize = false;
  }

  void FixedFrameLink::updateVelocities() {
    WvP0P1 = frame[1]->evalVelocity() - frame[0]->evalVelocity();
    WomK0K1 = frame[1]->evalAngularVelocity() - frame[0]->evalAngularVelocity();
    updVel = false;
  }

  void FixedFrameLink::updateh(int j) {
    for(unsigned int i=0; i<h[j].size(); i++)
      h[j][i] += frame[i]->evalJacobianOfTranslation(j).T() * evalForce(i) + frame[i]->evalJacobianOfRotation(j).T() * evalMoment(i);
  }

  void FixedFrameLink::init(InitStage stage, const InitConfigSet &config) {
    if(stage==unknownStage) {
      P[0] = frame[0];
      P[1] = frame[1];
    }
    FrameLink::init(stage, config);
  }

}
