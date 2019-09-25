/* Copyright (C) 2004-2019 MBSim Development Team
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

#include <config.h>
#include "bevelgear_planargear.h"
#include "mbsim/frames/contour_frame.h"
#include "mbsim/contours/bevel_gear.h"
#include "mbsim/contours/planar_gear.h"
#include <mbsim/utils/rotarymatrices.h>

using namespace fmatvec;
using namespace std;

namespace MBSim {

  void ContactKinematicsBevelGearPlanarGear::assignContours(const vector<Contour*> &contour) {
    if(dynamic_cast<BevelGear*>(contour[0])) {
      ibevelgear = 0; iplanargear = 1;
      bevelgear = static_cast<BevelGear*>(contour[0]);
      planargear = static_cast<PlanarGear*>(contour[1]);
    }
    else {
      ibevelgear = 1; iplanargear = 0;
      bevelgear = static_cast<BevelGear*>(contour[1]);
      planargear = static_cast<PlanarGear*>(contour[0]);
    }
    beta[0] = bevelgear->getHelixAngle();
    beta[1] = planargear->getHelixAngle();
    m = bevelgear->getModule()/cos(beta[0]);
    al0 = bevelgear->getPressureAngle();
    z[0] = bevelgear->getNumberOfTeeth();
    z[1] = planargear->getNumberOfTeeth();
    delh2 = (M_PI/2-planargear->getBacklash()/m)/z[1];
    delh1 = (M_PI/2-bevelgear->getBacklash()/m)/z[0];
  }

  void ContactKinematicsBevelGearPlanarGear::updateg(SingleContact &contact, int ii) {
    contact.getGeneralizedRelativePosition(false)(0) = 1e10;
    Vec3 ey1 = bevelgear->getFrame()->evalOrientation().T()*planargear->getFrame()->evalOrientation().col(1);
    Vec3 ez2 = planargear->getFrame()->getOrientation().T()*bevelgear->getFrame()->getOrientation().col(2);
    double phi1 = (ey1(0)>=0?1:-1)*acos(ey1(1)/sqrt(pow(ey1(0),2)+pow(ey1(1),2))); 
    double phi2 = (ez2(0)>=0?-1:1)*acos(ez2(2)/sqrt(pow(ez2(0),2)+pow(ez2(2),2))); 
    for(int i=0; i<2; i++) {
      int signi = i?-1:1;
      Vec3 rOP[2];
      vector<int> v[2];
      if(maxNumContacts==1) {
        int k = 0;
        double rsi = 1e10;
        int signk = (phi2 + signi*delh2)>0?-1:1;
        for(int k_=0; k_<z[1]; k_++) {
          double rsi_ = fabs(phi2 + signk*k_*2*M_PI/z[1] + signi*delh2);
          if(rsi_<rsi) {
            rsi = rsi_;
            k = k_;
          }
        }
        v[1].push_back(signk*k);
        double phi1corr = ((phi2 + signk*k*2*M_PI/z[1] - signi*delh2)*z[1])/z[0];

        k = 0;
        rsi = 1e10;
        signk = (phi1 - signi*delh1 - phi1corr)>0?-1:1;
        for(int k_=0; k_<z[0]; k_++) {
          double rsi_ = fabs(phi1 + signk*k_*2*M_PI/z[0] - signi*delh1 - phi1corr);
          if(rsi_<rsi) {
            rsi = rsi_;
            k = k_;
          }
        }
        v[0].push_back(signk*k);
      }
      else {
        throw runtime_error("The maximum number of contacts must be 1 at present");
      }

      double k[2];
      for (auto & i0 : v[0]) {
        for (auto & i1 : v[1]) {
          k[0] = i0;
          k[1] = i1;
          if(ii==0 or not(k[0]==ksave[0][0] and k[1]==ksave[0][1])) {
            Vec2 zeta2(NONINIT);
            double phi2q = phi2+k[1]*2*M_PI/z[1]+signi*delh2;
            zeta2(1) = 0;
            zeta2(0) = sin(phi2q)/cos(phi2q-beta[1])*sin(al0)*m*z[1]/2;
            planargear->setFlank(signi);
            planargear->setTooth(k[1]);
            rOP[1] = planargear->evalPosition(zeta2);

            Vec2 zeta1(NONINIT);
            zeta1(0) = -(phi1+k[0]*2*M_PI/z[0]-signi*delh1);
            zeta1(1) = 0;
            bevelgear->setFlank(signi);
            bevelgear->setTooth(k[0]);
            rOP[0] = bevelgear->evalPosition(zeta1);

            Vec u2 = planargear->evalWs(zeta2);
            Vec v2 = planargear->evalWt(zeta2);
            Vec3 n2 = crossProduct(u2,v2);

            double g = n2.T()*(rOP[0]-rOP[1]);
            if(g>-0.5*M_PI*m and g<contact.getGeneralizedRelativePosition(false)(0)) {
              ksave[ii][0] = k[0];
              ksave[ii][1] = k[1];
              signisave[ii] = signi;

              contact.getContourFrame(iplanargear)->setZeta(zeta2);
              contact.getContourFrame(iplanargear)->setPosition(rOP[1]);
              contact.getContourFrame(iplanargear)->getOrientation(false).set(0,n2);
              contact.getContourFrame(iplanargear)->getOrientation(false).set(1,u2);
              contact.getContourFrame(iplanargear)->getOrientation(false).set(2,v2);

              contact.getContourFrame(ibevelgear)->setZeta(zeta1);
              contact.getContourFrame(ibevelgear)->setPosition(rOP[0]);
              contact.getContourFrame(ibevelgear)->getOrientation(false).set(0,bevelgear->evalWn(zeta1));
              contact.getContourFrame(ibevelgear)->getOrientation(false).set(1,bevelgear->evalWu(zeta1));
              contact.getContourFrame(ibevelgear)->getOrientation(false).set(2,crossProduct(contact.getContourFrame(ibevelgear)->getOrientation(false).col(0),contact.getContourFrame(ibevelgear)->getOrientation(false).col(1)));

              contact.getGeneralizedRelativePosition(false)(0) = g;
            }
          }
        }
      }
    }
  }

  void ContactKinematicsBevelGearPlanarGear::updatewb(SingleContact &contact, int ii) {
    throw runtime_error("ContactKinematicsBevelGearPlanarGear::updatewb not yet implemented!");
  }

}