/* Copyright (C) 2004-2009 MBSim Development Team
 *
 * This library is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU Lesser General Public 
 * License as published by the Free Software Foundation; either 
 * version 2.1 of the License, or (at your option) any later version. 
 *  
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * Lesser General Public License for more details. 
 *  
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * Contact: martin.o.foerg@googlemail.com
 */

#include<config.h>
#include "mbsim/contours/room.h"
#include <mbsim/contours/plate.h>
#include <mbsim/frames/fixed_relative_frame.h>
#include <mbsim/utils/rotarymatrices.h>

#include <openmbvcppinterface/compoundrigidbody.h>

using namespace std;
using namespace fmatvec;

namespace MBSim {

  void Room::init(InitStage stage, const InitConfigSet &config) {
    if (stage == preInit) {
      Vec3 Kr[6];
      Vec2 limit[6];
      SqrMat3 AIK[6];

      //X-Axis
      Kr[0](0) = l / 2.;
      Kr[0](1) = 0;
      Kr[0](2) = 0;
      limit[0](0) = d;
      limit[0](1) = h;
      AIK[0] = BasicRotAIKy(M_PI);

      Kr[1](0) = -l / 2.;
      Kr[1](1) = 0;
      Kr[1](2) = 0;
      limit[1](0) = d;
      limit[1](1) = h;
      AIK[1] = BasicRotAIKy(0);

      //Y-Axis
      Kr[2](0) = 0;
      Kr[2](1) = d / 2.;
      Kr[2](2) = 0;
      limit[2](0) = l;
      limit[2](1) = h;
      AIK[2] = BasicRotAIKz(-M_PI_2);

      Kr[3](0) = 0;
      Kr[3](1) = -d / 2.;
      Kr[3](2) = 0;
      limit[3](0) = l;
      limit[3](1) = h;
      AIK[3] = BasicRotAIKz(M_PI_2);

      //Z-Axis
      Kr[4](0) = 0;
      Kr[4](1) = 0;
      Kr[4](2) = h / 2.;
      limit[4](0) = d;
      limit[4](1) = l;
      AIK[4] = BasicRotAIKy(M_PI_2);

      Kr[5](0) = 0;
      Kr[5](1) = 0;
      Kr[5](2) = -h / 2.;
      limit[5](0) = d;
      limit[5](1) = l;
      AIK[5] = BasicRotAIKy(-M_PI_2);

      for (int i = 0; i < 6; i++) {
        stringstream s;
        s << "Face_" <<  i + 1;

        FixedRelativeFrame *frame = new FixedRelativeFrame("R");
        frame->setRelativePosition(Kr[i]);
        frame->setRelativeOrientation(AIK[i]);
        frame->setFrameOfReference(R);
        addFrame(frame);

        Plate * plate = new Plate(s.str());
        plate->setYLength(limit[i](0));
        plate->setZLength(limit[i](1));
        plate->setFrameOfReference(frame);
        addContour(plate);
      }
    }
    else if (stage == plotting) {
      if(openMBVRigidBody)
        static_pointer_cast<OpenMBV::Cuboid>(openMBVRigidBody)->setLength(l,d,h);
    }
    CompoundContour::init(stage, config);
  }

}
