/* Copyright (C) 2004-2018 MBSim Development Team
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
#include "mbsim/contours/spatial_nurbs_contour.h"
#include "mbsim/utils/contact_utils.h"

using namespace std;
using namespace fmatvec;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSim {

  MBSIM_OBJECTFACTORY_REGISTERCLASS(MBSIM, SpatialNurbsContour)

  Vec3 SpatialNurbsContour::evalKrPS(const Vec2 &zeta) {
    return srf.pointAt(zeta(0),zeta(1));
  }

  Vec3 SpatialNurbsContour::evalKs(const Vec2 &zeta) {
    GeneralMatrix<fmatvec::Vec3> jac;
    srf.deriveAt(zeta(0),zeta(1),1,jac);
    return jac(1,0);
  }

  Vec3 SpatialNurbsContour::evalKt(const Vec2 &zeta) {
    GeneralMatrix<fmatvec::Vec3> jac;
    srf.deriveAt(zeta(0),zeta(1),1,jac);
    return jac(0,1);
  }

  Vec3 SpatialNurbsContour::evalParDer1Ks(const Vec2 &zeta) {
    GeneralMatrix<fmatvec::Vec3> jac;
    srf.deriveAt(zeta(0),zeta(1),2,jac);
    return jac(2,0);
  }

  Vec3 SpatialNurbsContour::evalParDer2Ks(const Vec2 &zeta) {
    GeneralMatrix<fmatvec::Vec3> jac;
    srf.deriveAt(zeta(0),zeta(1),2,jac);
    return jac(1,1);
  }

  Vec3 SpatialNurbsContour::evalParDer1Kt(const Vec2 &zeta) {
    GeneralMatrix<fmatvec::Vec3> jac;
    srf.deriveAt(zeta(0),zeta(1),2,jac);
    return jac(1,1);
  }

  Vec3 SpatialNurbsContour::evalParDer2Kt(const Vec2 &zeta) {
    GeneralMatrix<fmatvec::Vec3> jac;
    srf.deriveAt(zeta(0),zeta(1),2,jac);
    return jac(0,2);
  }

  void SpatialNurbsContour::init(InitStage stage, const InitConfigSet &config) {
    if (stage == preInit) {
//      if (etaNodes.size() < 2)
//        throwError("(SpatialNurbsContour::init): Size of etaNodes must be greater than 1.");
//      if (xiNodes.size() < 2)
//        throwError("(SpatialNurbsContour::init): Size of xiNodes must be greater than 1.");
      srf.resize(cp.rows(),cp.cols(),uKnot.size()-cp.rows()-1,vKnot.size()-cp.cols()-1);
      srf.setDegreeU(uKnot.size()-cp.rows()-1);
      srf.setDegreeV(vKnot.size()-cp.cols()-1);
      srf.setKnotU(uKnot);
      srf.setKnotV(vKnot);
      srf.setCtrlPnts(cp);
    }
    else if(stage==plotting) {
      if(plotFeature[openMBV] && openMBVRigidBody) {
        vector<vector<double> > cp_(cp.rows()*cp.cols(),vector<double>(4));
        for(int i=0; i<cp.rows(); i++) {
          for(int j=0; j<cp.cols(); j++) {
            for(int k=0; k<4; k++)
              cp_[j*cp.rows()+i][k] = cp(i,j)(k);
          }
        }
        static_pointer_cast<OpenMBV::NurbsSurface>(openMBVRigidBody)->setControlPoints(cp_);
        static_pointer_cast<OpenMBV::NurbsSurface>(openMBVRigidBody)->setUKnotVector(uKnot);
        static_pointer_cast<OpenMBV::NurbsSurface>(openMBVRigidBody)->setVKnotVector(vKnot);
        static_pointer_cast<OpenMBV::NurbsSurface>(openMBVRigidBody)->setNumberOfUControlPoints(cp.rows());
        static_pointer_cast<OpenMBV::NurbsSurface>(openMBVRigidBody)->setNumberOfVControlPoints(cp.cols());
      }
    }
    RigidContour::init(stage, config);
  }

  double SpatialNurbsContour::getCurvature(const Vec2 &zeta) {
    throw;
  }

  void SpatialNurbsContour::initializeUsingXML(DOMElement * element) {
    RigidContour::initializeUsingXML(element);
    DOMElement * e;
//    e=E(element)->getFirstElementChildNamed(MBSIM%"etaNodes");
//    etaNodes=E(e)->getText<Vec>();
//    e=E(element)->getFirstElementChildNamed(MBSIM%"xiNodes");
//    xiNodes=E(e)->getText<Vec>();
    e=E(element)->getFirstElementChildNamed(MBSIM%"controlPoints");
    MatVx4 pts=E(e)->getText<MatVx4>();
    e=E(element)->getFirstElementChildNamed(MBSIM%"numberOfEtaControlPoints");
    int nu = E(e)->getText<int>();
    e=E(element)->getFirstElementChildNamed(MBSIM%"numberOfXiControlPoints");
    int nv = E(e)->getText<int>();
    e=E(element)->getFirstElementChildNamed(MBSIM%"etaKnotVector");
    setEtaKnotVector(E(e)->getText<Vec>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"xiKnotVector");
    setXiKnotVector(E(e)->getText<Vec>());
    cp.resize(nu,nv);
    for(int i=0; i<nu; i++) {
      for(int j=0; j<nv; j++)
        cp(i,j) = pts.row(j*nu+i).T();
    }
    e=E(element)->getFirstElementChildNamed(MBSIM%"open");
    if(e) setOpen(E(e)->getText<bool>());
    e=E(element)->getFirstElementChildNamed(MBSIM%"enableOpenMBV");
    if(e) {
      OpenMBVNurbsSurface ombv;
      ombv.initializeUsingXML(e);
      openMBVRigidBody=ombv.createOpenMBV();
    }
  }

}
