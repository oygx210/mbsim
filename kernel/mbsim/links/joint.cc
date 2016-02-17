/* Copyright (C) 2004-2014 MBSim Development Team
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
#include "mbsim/links/joint.h"
#include "mbsim/constitutive_laws/bilateral_impact.h"
#include "mbsim/dynamic_system_solver.h"
#include "mbsim/objectfactory.h"
#include <mbsim/constitutive_laws/generalized_force_law.h>
#include <mbsim/constitutive_laws/friction_force_law.h>
#include <mbsim/constitutive_laws/generalized_impact_law.h>
#include <mbsim/constitutive_laws/friction_impact_law.h>
#include "mbsim/contact_kinematics/contact_kinematics.h"
#include <mbsim/utils/utils.h>
#include <mbsim/objects/rigid_body.h>

using namespace std;
using namespace fmatvec;
using namespace MBXMLUtils;
using namespace xercesc;
using namespace boost;

namespace MBSim {

  MBSIM_OBJECTFACTORY_REGISTERXMLNAME(Joint, MBSIM%"Joint")

  Joint::Joint(const string &name) : FloatingFrameLink(name), ffl(0), fml(0), fifl(0), fiml(0) {
  }

  Joint::~Joint() {
    delete ffl;
    delete fml;
    delete fifl;
    delete fiml;
  }

  void Joint::updatewb(double t) {
    Mat3xV WJT = refFrame->getOrientation(t) * JT;
    VecV sdT = WJT.T() * getGlobalRelativeVelocity(t);

    wb(0, DF.cols() - 1) += getGlobalForceDirection(t).T() * (frame[1]->getGyroscopicAccelerationOfTranslation(t) - C.getGyroscopicAccelerationOfTranslation(t) - crossProduct(C.getAngularVelocity(t), getGlobalRelativeVelocity(t) + WJT * sdT));
    wb(DF.cols(), DM.cols() + DF.cols() - 1) += getGlobalMomentDirection(t).T() * (frame[1]->getGyroscopicAccelerationOfRotation(t) - C.getGyroscopicAccelerationOfRotation(t) - crossProduct(C.getAngularVelocity(t), getGlobalRelativeAngularVelocity(t)));
  }

  void Joint::updatelaFM(double t) {
    for (int i = 0; i < forceDir.cols(); i++)
      lambdaF(i) = la(i);
  }

  void Joint::updatelaFS(double t) {
    for (int i = 0; i < forceDir.cols(); i++)
      lambdaF(i) = (*ffl)(getGeneralizedRelativePosition(t)(i), getGeneralizedRelativeVelocity(t)(i));
  }

  void Joint::updatelaMM(double t) {
    for (int i = forceDir.cols(), j=0; i < forceDir.cols() + momentDir.cols(); i++, j++)
      lambdaM(j) = la(i);
  }

  void Joint::updatelaMS(double t) {
    for (int i = forceDir.cols(), j=0; i < forceDir.cols() + momentDir.cols(); i++, j++)
      lambdaM(j) = (*fml)(getGeneralizedRelativePosition(t)(i), getGeneralizedRelativeVelocity(t)(i));
  }

  void Joint::updatexd(double t) {
    xd = getGeneralizedRelativeVelocity(t)(iM);
  }

  void Joint::updatedx(double t, double dt) {
    xd = getGeneralizedRelativeVelocity(t)(iM) * dt;
  }

  void Joint::updateh(double t, int j) {
    Vec3 F = (ffl and not ffl->isSetValued())?getGlobalForceDirection(t)*getlaF(t):Vec3();
    Vec3 M = (fml and not fml->isSetValued())?getGlobalMomentDirection(t)*getlaM(t):Vec3();

    h[j][0] -= C.getJacobianOfTranslation(t,j).T() * F + C.getJacobianOfRotation(t,j).T() * M;
    h[j][1] += frame[1]->getJacobianOfTranslation(t,j).T() * F + frame[1]->getJacobianOfRotation(t,j).T() * M;
  }

  void Joint::updateW(double t, int j) {
    int nF = (ffl and ffl->isSetValued())?forceDir.cols():0;
    int nM = (fml and fml->isSetValued())?momentDir.cols():0;
    Mat3xV RF(nF+nM);
    Mat3xV RM(RF.cols());
    RF.set(Index(0,2), Index(0,nF-1), getGlobalForceDirection(t)(Index(0,2),Index(0,nF-1)));
    RM.set(Index(0,2), Index(nF,nF+nM-1), getGlobalMomentDirection(t)(Index(0,2),Index(0,nM-1)));

    W[j][0] -= C.getJacobianOfTranslation(t,j).T() * RF + C.getJacobianOfRotation(t,j).T() * RM;
    W[j][1] += frame[1]->getJacobianOfTranslation(t,j).T() * RF + frame[1]->getJacobianOfRotation(t,j).T() * RM;
  }

  void Joint::calcxSize() {
    FloatingFrameLink::calcxSize();
    xSize = momentDir.cols();
  }

  void Joint::init(InitStage stage) {
    if (stage == resize) {
      FloatingFrameLink::init(stage);
      gdd.resize(gdSize);
      gdn.resize(gdSize);
    }
    else if (stage == unknownStage) {
      FloatingFrameLink::init(stage);

      if (ffl) {
        fifl = new BilateralImpact;
        fifl->setParent(this);
      }
      if (fml) {
        fiml = new BilateralImpact;
        fiml->setParent(this);
      }

      JT.resize(3 - forceDir.cols());
      if (forceDir.cols() == 2)
        JT.set(0, crossProduct(forceDir.col(0), forceDir.col(1)));
      else if (forceDir.cols() == 3)
        ;
      else if (forceDir.cols() == 0)
        JT = SqrMat(3, EYE);
      else { // define a coordinate system in the plane perpendicular to the force direction
        JT.set(0, computeTangential(forceDir.col(0)));
        JT.set(1, crossProduct(forceDir.col(0), JT.col(0)));
      }
      if(not ffl)
        updatelaF_ = &Joint::updatelaF0;
      else if(ffl->isSetValued())
        updatelaF_ = &Joint::updatelaFM;
      else
        updatelaF_ = &Joint::updatelaFS;
      if(not fml)
        updatelaM_ = &Joint::updatelaM0;
      else if(fml->isSetValued())
        updatelaM_ = &Joint::updatelaMM;
      else
        updatelaM_ = &Joint::updatelaMS;
    }
    else
      FloatingFrameLink::init(stage);
    if(ffl) ffl->init(stage);
    if(fml) fml->init(stage);
    if(fifl) fifl->init(stage);
    if(fiml) fiml->init(stage);
  }

  bool Joint::isSetValued() const {
    bool flag = false;
    if (ffl)
      flag |= ffl->isSetValued();
    if (fml)
      flag |= fml->isSetValued();

    return flag;
  }

  bool Joint::isSingleValued() const {
    bool flag = false;
    if (ffl)
      if (not ffl->isSetValued())
        flag = true;
    if (fml)
      if (not fml->isSetValued())
        flag = true;

    return flag;
  }

  void Joint::solveImpactsFixpointSingle(double t, double dt) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &LaMBS = ds->getLa();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      La(i) = fifl->project(La(i), gdn(i), gd(i), rFactor(i));
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      La(i) = fiml->project(La(i), gdn(i), gd(i), rFactor(i));
    }
  }

  void Joint::solveConstraintsFixpointSingle(double t) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &laMBS = ds->getla();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      la(i) = ffl->project(la(i), gdd(i), rFactor(i));
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      la(i) = fml->project(la(i), gdd(i), rFactor(i));
    }
  }

  void Joint::solveImpactsGaussSeidel(double t, double dt) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &LaMBS = ds->getLa();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i] + 1; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      La(i) = fifl->solve(a[ia[laInd + i]], gdn(i), gd(i));
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i] + 1; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      La(i) = fiml->solve(a[ia[laInd + i]], gdn(i), gd(i));
    }
  }

  void Joint::solveConstraintsGaussSeidel(double t) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &laMBS = ds->getla();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i] + 1; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      la(i) = ffl->solve(a[ia[laInd + i]], gdd(i));
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i] + 1; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      la(i) = fml->solve(a[ia[laInd + i]], gdd(i));
    }
  }

  void Joint::solveImpactsRootFinding(double t, double dt) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &LaMBS = ds->getLa();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      res(i) = La(i) - fifl->project(La(i), gdn(i), gd(i), rFactor(i));
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      res(i) = La(i) - fiml->project(La(i), gdn(i), gd(i), rFactor(i));
    }
  }

  void Joint::solveConstraintsRootFinding(double t) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &laMBS = ds->getla();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      res(i) = la(i) - ffl->project(la(i), gdd(i), rFactor(i));
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      res(i) = la(i) - fml->project(la(i), gdd(i), rFactor(i));
    }
  }

  void Joint::jacobianConstraints(double t) {

    const SqrMat Jprox = ds->getJprox();
    const SqrMat G = ds->getG(t);

    for (int i = 0; i < forceDir.cols(); i++) {
      RowVec jp1 = Jprox.row(laInd + i);
      RowVec e1(jp1.size());
      e1(laInd + i) = 1;
      Vec diff = ffl->diff(la(i), gdd(i), rFactor(i));

      jp1 = e1 - diff(0) * e1; // -diff(1)*G.row(laInd+i)
      for (int j = 0; j < G.size(); j++)
        jp1(j) -= diff(1) * G(laInd + i, j);
    }

    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {

      RowVec jp1 = Jprox.row(laInd + i);
      RowVec e1(jp1.size());
      e1(laInd + i) = 1;
      Vec diff = fml->diff(la(i), gdd(i), rFactor(i));

      jp1 = e1 - diff(0) * e1; // -diff(1)*G.row(laInd+i)
      for (int j = 0; j < G.size(); j++)
        jp1(j) -= diff(1) * G(laInd + i, j);
    }
  }

  void Joint::jacobianImpacts(double t) {

    const SqrMat Jprox = ds->getJprox();
    const SqrMat G = ds->getG(t);

    for (int i = 0; i < forceDir.cols(); i++) {
      RowVec jp1 = Jprox.row(laInd + i);
      RowVec e1(jp1.size());
      e1(laInd + i) = 1;
      Vec diff = fifl->diff(La(i), gdn(i), gd(i), rFactor(i));

      jp1 = e1 - diff(0) * e1; // -diff(1)*G.row(laInd+i)
      for (int j = 0; j < G.size(); j++)
        jp1(j) -= diff(1) * G(laInd + i, j);
    }

    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      RowVec jp1 = Jprox.row(laInd + i);
      RowVec e1(jp1.size());
      e1(laInd + i) = 1;
      Vec diff = fiml->diff(La(i), gdn(i), gd(i), rFactor(i));

      jp1 = e1 - diff(0) * e1; // -diff(1)*G.row(laInd+i)
      for (int j = 0; j < G.size(); j++)
        jp1(j) -= diff(1) * G(laInd + i, j);
    }
  }

  void Joint::updaterFactors(double t) {
    if (isActive()) {

      const double *a = ds->getGs(t)();
      const int *ia = ds->getGs().Ip();

      for (int i = 0; i < rFactorSize; i++) {
        double sum = 0;
        for (int j = ia[laInd + i] + 1; j < ia[laInd + i + 1]; j++)
          sum += fabs(a[j]);
        double ai = a[ia[laInd + i]];
        if (ai > sum) {
          rFactorUnsure(i) = 0;
          rFactor(i) = 1.0 / ai;
        }
        else {
          rFactorUnsure(i) = 1;
          rFactor(i) = 1.0 / ai;
        }
      }
    }
  }

  void Joint::checkImpactsForTermination(double t, double dt) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &LaMBS = ds->getLa();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      if (!fifl->isFulfilled(la(i), gdn(i), gd(i), LaTol, gdTol)) {
        ds->setTermination(false);
        return;
      }
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdn(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdn(i) += a[j] * LaMBS(ja[j]);

      if (!fiml->isFulfilled(La(i), gdn(i), gd(i), LaTol, gdTol)) {
        ds->setTermination(false);
        return;
      }
    }
  }

  void Joint::checkConstraintsForTermination(double t) {

    const double *a = ds->getGs(t)();
    const int *ia = ds->getGs().Ip();
    const int *ja = ds->getGs().Jp();
    const Vec &laMBS = ds->getla();
    const Vec &b = ds->getb(false);

    for (int i = 0; i < forceDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      if (!ffl->isFulfilled(la(i), gdd(i), laTol, gddTol)) {
        ds->setTermination(false);
        return;
      }
    }
    for (int i = forceDir.cols(); i < forceDir.cols() + momentDir.cols(); i++) {
      gdd(i) = b(laInd + i);
      for (int j = ia[laInd + i]; j < ia[laInd + 1 + i]; j++)
        gdd(i) += a[j] * laMBS(ja[j]);

      if (!fml->isFulfilled(la(i), gdd(i), laTol, gddTol)) {
        ds->setTermination(false);
        return;
      }
    }
  }

  void Joint::setForceLaw(GeneralizedForceLaw * rc) {
    ffl = rc;
    ffl->setParent(this);
  }

  void Joint::setMomentLaw(GeneralizedForceLaw * rc) {
    fml = rc;
    fml->setParent(this);
  }

  void Joint::setForceDirection(const Mat3xV &fd) {

    forceDir = fd;

    for (int i = 0; i < fd.cols(); i++)
      forceDir.set(i, forceDir.col(i) / nrm2(fd.col(i)));
  }

  void Joint::setMomentDirection(const Mat3xV &md) {

    momentDir = md;

    for (int i = 0; i < md.cols(); i++)
      momentDir.set(i, momentDir.col(i) / nrm2(md.col(i)));
  }

  void Joint::initializeUsingXML(DOMElement *element) {
    FloatingFrameLink::initializeUsingXML(element);
    DOMElement *e = E(element)->getFirstElementChildNamed(MBSIM%"forceDirection");
    if(e) setForceDirection(getMat(e,3,0));
    e = E(element)->getFirstElementChildNamed(MBSIM%"forceLaw");
    if(e) setForceLaw(ObjectFactory::createAndInit<GeneralizedForceLaw>(e->getFirstElementChild()));
    e = E(element)->getFirstElementChildNamed(MBSIM%"momentDirection");
    if(e) setMomentDirection(getMat(e,3,0));
    e = E(element)->getFirstElementChildNamed(MBSIM%"momentLaw");
    if(e) setMomentLaw(ObjectFactory::createAndInit<GeneralizedForceLaw>(e->getFirstElementChild()));
  }

  DOMElement* Joint::writeXMLFile(DOMNode *parent) {
    DOMElement *ele0 = FloatingFrameLink::writeXMLFile(parent);
//    if (forceDir.cols()) {
//      addElementText(ele0, MBSIM%"forceDirection", forceDir);
//      DOMElement *ele1 = new DOMElement(MBSIM%"forceLaw");
//      if (ffl)
//        ffl->writeXMLFile(ele1);
//      ele0->LinkEndChild(ele1);
//    }
//    if (momentDir.cols()) {
//      addElementText(ele0, MBSIM%"momentDirection", momentDir);
//      DOMElement *ele1 = new DOMElement(MBSIM%"momentLaw");
//      if (fml)
//        fml->writeXMLFile(ele1);
//      ele0->LinkEndChild(ele1);
//    }
//    DOMElement *ele1 = new DOMElement(MBSIM%"connect");
//    //ele1->SetAttribute("ref1", frame[0]->getXMLPath(frame[0])); // absolute path
//    //ele1->SetAttribute("ref2", frame[1]->getXMLPath(frame[1])); // absolute path
//    ele1->SetAttribute("ref1", frame[0]->getXMLPath(this, true)); // relative path
//    ele1->SetAttribute("ref2", frame[1]->getXMLPath(this, true)); // relative path
//    ele0->LinkEndChild(ele1);
    return ele0;
  }

  InverseKineticsJoint::InverseKineticsJoint(const string &name) : Joint(name), body(0) {
  }

  void InverseKineticsJoint::calcbSize() {
    if(body) bSize = body->getPJT(0,false).cols();
  }

  void InverseKineticsJoint::updateb(double t) {
    if(body) {
      b(Index(0, bSize - 1), Index(0, 2)) = body->getPJT(t).T();
      b(Index(0, bSize - 1), Index(3, 5)) = body->getPJR(t).T();
    }
  }

  void InverseKineticsJoint::init(InitStage stage) {
    if (stage == resize) {
      Joint::init(stage);
      x.resize(momentDir.cols());
    }
    else
      Joint::init(stage);
  }

}
