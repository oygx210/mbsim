/* Copyright (C) 2004-2015 MBSim Development Team
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
#include "generic_flexible_ffr_body.h"
#include "mbsimFlexibleBody/frames/node_frame.h"
#include "mbsim/contours/rigid_contour.h"
#include "mbsim/dynamic_system.h"
#include "mbsim/links/joint.h"
#include "mbsim/utils/rotarymatrices.h"
#include "mbsim/utils/xmlutils.h"
#include "mbsim/objectfactory.h"
#include "mbsim/environment.h"
#include "mbsim/functions/kinematics/rotation_about_axes_xyz.h"
#include "mbsim/functions/kinematics/rotation_about_axes_zxz.h"
#include "mbsim/functions/kinematics/rotation_about_axes_zyx.h"
#include "mbsim/functions/kinematics/rotation_about_axes_xyz_mapping.h"
#include "mbsim/functions/kinematics/rotation_about_axes_zxz_mapping.h"
#include "mbsim/functions/kinematics/rotation_about_axes_zyx_mapping.h"
#include "mbsim/functions/kinematics/rotation_about_axes_xyz_transformed_mapping.h"
#include "mbsim/functions/kinematics/rotation_about_axes_zxz_transformed_mapping.h"
#include "mbsimFlexibleBody/namespace.h"
#include <openmbvcppinterface/flexiblebody.h>

using namespace std;
using namespace fmatvec;
using namespace MBSim;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSimFlexibleBody {

  Range<Var,Var> i02(0,2);

  GenericFlexibleFfrBody::GenericFlexibleFfrBody(const string &name) : NodeBasedBody(name),  Id(Eye()),  APK(EYE) {

    updKJ[0] = true;
    updKJ[1] = true;

    K=new Frame("K");
    NodeBasedBody::addFrame(K);

    updateJacobians_[0] = &GenericFlexibleFfrBody::updateJacobians0;
    updateJacobians_[1] = &GenericFlexibleFfrBody::updateJacobians1;
    updateKJ_[0] = &GenericFlexibleFfrBody::updateKJ0;
    updateKJ_[1] = &GenericFlexibleFfrBody::updateKJ1;

    evalOMBVColorRepresentation[0] = &GenericFlexibleFfrBody::evalNone;
    evalOMBVColorRepresentation[1] = &GenericFlexibleFfrBody::evalXDisplacement;
    evalOMBVColorRepresentation[2] = &GenericFlexibleFfrBody::evalYDisplacement;
    evalOMBVColorRepresentation[3] = &GenericFlexibleFfrBody::evalZDisplacement;
    evalOMBVColorRepresentation[4] = &GenericFlexibleFfrBody::evalTotalDisplacement;
    evalOMBVColorRepresentation[5] = &GenericFlexibleFfrBody::evalXXStress;
    evalOMBVColorRepresentation[6] = &GenericFlexibleFfrBody::evalYYStress;
    evalOMBVColorRepresentation[7] = &GenericFlexibleFfrBody::evalZZStress;
    evalOMBVColorRepresentation[8] = &GenericFlexibleFfrBody::evalXYStress;
    evalOMBVColorRepresentation[9] = &GenericFlexibleFfrBody::evalYZStress;
    evalOMBVColorRepresentation[10] = &GenericFlexibleFfrBody::evalZXStress;
    evalOMBVColorRepresentation[11] = &GenericFlexibleFfrBody::evalEquivalentStress;
  }

  GenericFlexibleFfrBody::~GenericFlexibleFfrBody() {
    if(fPrPK) { delete fPrPK; fPrPK=nullptr; }
    if(fAPK) { delete fAPK; fAPK=nullptr; }
    if(fTR) { delete fTR; fTR=nullptr; }
  }

  void GenericFlexibleFfrBody::updateh(int index) {
    h[index] += evalKJ(index).T()*(evalhb() - evalMb()*evalKi());
  }

  void GenericFlexibleFfrBody::calcSize() {
    ne = Pdm.cols();

    int nqT=0, nqR=0, nuT=0, nuR=0;
    if(fPrPK) {
      nqT = fPrPK->getArg1Size();
      nuT = fPrPK->getArg1Size(); // TODO fTT->getArg1Size()
    }
    if(fAPK) {
      nqR = fAPK->getArg1Size();
      nuR = fAPK->getArg1Size(); // TODO fTR->getArg1Size()
    }

    if(translationDependentRotation) {
      assert(nqT == nqR);
      assert(nuT == nuR);
      nq = nqT + ne;
      nu = nuT + ne;
      iqT = Range<Var,Var>(0,nqT+nqR-1);
      iqR = Range<Var,Var>(0,nqT+nqR-1);
      iqE = Range<Var,Var>(nqT+nqR,nq-1);
      iuT = Range<Var,Var>(0,nuT+nuR-1);
      iuR = Range<Var,Var>(0,nuT+nuR-1);
      iuE = Range<Var,Var>(nuT+nuR,nu-1);
    }
    else {
      nq = nqT + nqR + ne;
      nu = nuT + nuR + ne;
      iqT = Range<Var,Var>(0,nqT-1);
      iqR = Range<Var,Var>(nqT,nqT+nqR-1);
      iqE = Range<Var,Var>(nqT+nqR,nq-1);
      iuT = Range<Var,Var>(0,nuT-1);
      iuR = Range<Var,Var>(nuT,nqT+nqR-1);
      iuE = Range<Var,Var>(nuT+nuR,nu-1);
    }
    updSize = false;
  }

  void GenericFlexibleFfrBody::calcqSize() {
    NodeBasedBody::calcqSize();
    qSize = nq;
  }

  void GenericFlexibleFfrBody::calcuSize(int j) {
    NodeBasedBody::calcuSize(j);
    if(j==0)
      uSize[j] = nu;
    else
      uSize[j] = 6 + ne;
  }

  void GenericFlexibleFfrBody::determineSID() {
    Cr0.resize(ne,NONINIT);
    Gr0.resize(ne);
    Gr1.resize(ne,vector<SqrMat3>(ne));
    Ge.resize(ne);
    Oe0.resize(ne,NONINIT);
    Oe1.resize(6);
    vector<vector<SqrMatV> > Kom(3,vector<SqrMatV>(3));
    mmi0(0,0) = rrdm(1,1) + rrdm(2,2);
    mmi0(0,1) = -rrdm(1,0);
    mmi0(0,2) = -rrdm(2,0);
    mmi0(1,1) = rrdm(0,0) + rrdm(2,2);
    mmi0(1,2) = -rrdm(2,1);
    mmi0(2,2) = rrdm(0,0) + rrdm(1,1);
    for(int i=0; i<3; i++) {
      for(int j=0; j<3; j++) {
        if(i!=j)
          Kom[i][j].resize() = PPdm[i][j];
      }
    }
    Kom[0][0].resize() = -PPdm[1][1]-PPdm[2][2];
    Kom[1][1].resize() = -PPdm[2][2]-PPdm[0][0];
    Kom[2][2].resize() = -PPdm[0][0]-PPdm[1][1];

    Me.resize(ne,NONINIT);
    mmi1.resize(ne);
    mmi2.resize(ne,vector<SqrMat3>(ne));

    for(int i=0; i<ne; i++) {
      mmi1[i](0,0) = 2.*(rPdm[1](1,i) + rPdm[2](2,i));
      mmi1[i](0,1) = -(rPdm[1](0,i) + rPdm[0](1,i));
      mmi1[i](0,2) = -(rPdm[2](0,i) + rPdm[0](2,i));
      mmi1[i](1,1) = 2.*(rPdm[0](0,i) + rPdm[2](2,i));
      mmi1[i](1,2) = -(rPdm[2](1,i) + rPdm[1](2,i));
      mmi1[i](2,2) = 2.*(rPdm[0](0,i) + rPdm[1](1,i));

      Oe0.e(i,0) = -rPdm[2](2,i) - rPdm[1](1,i);
      Oe0.e(i,1) = -rPdm[2](2,i) - rPdm[0](0,i);
      Oe0.e(i,2) = -rPdm[1](1,i) - rPdm[0](0,i);
      Oe0(i,3) = rPdm[0](1,i) + rPdm[1](0,i);
      Oe0(i,4) = rPdm[1](2,i) + rPdm[2](1,i);
      Oe0(i,5) = rPdm[2](0,i) + rPdm[0](2,i);

      Gr0[i](0,0) = 2.*(rPdm[2](2,i) + rPdm[1](1,i));
      Gr0[i](0,1) = -2.*rPdm[1](0,i);
      Gr0[i](0,2) = -2.*rPdm[2](0,i);
      Gr0[i](1,0) = -2.*rPdm[0](1,i);
      Gr0[i](1,1) = 2.*(rPdm[2](2,i) + rPdm[0](0,i));
      Gr0[i](1,2) = -2.*rPdm[2](1,i);
      Gr0[i](2,0) = -2.*rPdm[0](2,i);
      Gr0[i](2,1) = -2.*rPdm[1](2,i);
      Gr0[i](2,2) = 2.*(rPdm[1](1,i) + rPdm[0](0,i));

      Cr0.e(i,0) = rPdm[1](2,i) - rPdm[2](1,i);
      Cr0.e(i,1) = rPdm[2](0,i) - rPdm[0](2,i);
      Cr0.e(i,2) = rPdm[0](1,i) - rPdm[1](0,i);

      for(int j=i; j<ne; j++) {
        Me.ej(i,j) = PPdm[0][0].e(i,j) + PPdm[1][1].e(i,j) + PPdm[2][2].e(i,j);

        mmi2[i][j].e(0,0) = PPdm[1][1].e(i,j) + PPdm[2][2].e(i,j);
        mmi2[i][j].e(1,1) = PPdm[0][0].e(i,j) + PPdm[2][2].e(i,j);
        mmi2[i][j].e(2,2) = PPdm[0][0].e(i,j) + PPdm[1][1].e(i,j);
        mmi2[i][j].e(0,1) = -PPdm[1][0].e(i,j);
        mmi2[i][j].e(0,2) = -PPdm[2][0].e(i,j);
        mmi2[i][j].e(1,2) = -PPdm[2][1].e(i,j);
        mmi2[i][j].e(1,0) = -PPdm[0][1].e(i,j);
        mmi2[i][j].e(2,0) = -PPdm[0][2].e(i,j);
        mmi2[i][j].e(2,1) = -PPdm[1][2].e(i,j);
        mmi2[j][i] = mmi2[i][j].T();

        Gr1[i][j] = 2.*mmi2[i][j];
        Gr1[j][i] = Gr1[i][j].T();
      }
    }
    Ct0 = Pdm.T();
    for(const auto & i : K0t)
      Ct1.push_back(i);

    vector<SqrMatV> Kr(3);
    Kr[0].resize() = -PPdm[1][2] + PPdm[1][2].T();
    Kr[1].resize() = -PPdm[2][0] + PPdm[2][0].T();
    Kr[2].resize() = -PPdm[0][1] + PPdm[0][1].T();

    for(const auto & i : Kr)
      Cr1.push_back(i);
    for(unsigned int i=0; i<K0r.size(); i++)
      Cr1[i] += K0r[i];

    Ge.resize(3);
    for(int i=0; i<3; i++)
      Ge[i].resize() = 2.*Kr[i];

    for(int i=0; i<3; i++)
      Oe1[i].resize() = Kom[i][i];
    Oe1[3].resize() = Kom[0][1] + Kom[0][1].T();
    Oe1[4].resize() = Kom[1][2] + Kom[1][2].T();
    Oe1[5].resize() = Kom[2][0] + Kom[2][0].T();
    for(unsigned int i=0; i<K0om.size(); i++)
      Oe1[i] += K0om[i];

    if(not(De0.size()))
      De0 = beta.e(0)*Me + beta.e(1)*Ke0;

    if(Knl1.size()) {
      Ke1.resize(Knl1.size());
      if(Knl2.size()) Ke2.resize(Knl2.size(),vector<SqrMatV>(Knl2.size()));
      for(unsigned int i=0; i<Knl1.size(); i++) {
        Ke1[i].resize() = (Knl1[i].T() + 0.5*Knl1[i]);
        for(unsigned int j=0; j<Knl2.size(); j++)
          Ke2[i][j].resize() = 0.5*Knl2[i][j];
      }
    }
 }

  void GenericFlexibleFfrBody::prefillMassMatrix() {
    M_.resize(6+ne,NONINIT);
    for(int i=0; i<3; i++) {
      M_.ej(i,i)=m;
      for(int j=i+1; j<3; j++)
        M_.ej(i,j)=0;
    }
    for(int i=0; i<ne; i++) {
      for(int j=0; j<3; j++)
        M_.e(i+6,j) = Ct0.e(i,j);
      for(int j=i; j<ne; j++)
        M_.ej(i+6,j+6) = Me.ej(i,j);
    }
  }

  void GenericFlexibleFfrBody::init(InitStage stage, const InitConfigSet &config) {
    if(stage==preInit) {
      if(generalizedVelocityOfRotation==unknown)
        throwError("Generalized velocity of rotation unknown");

      for(auto & k : contour) {
        auto *contour_ = dynamic_cast<RigidContour*>(k);
        if(contour_ and not(contour_->getFrameOfReference()))
          contour_->setFrameOfReference(K);
      }

      if(not rPdm.size())
        rPdm.resize(3);
      else if(rPdm.size()!=3)
        throwError("(GenericFlexibleFfrBody::init): size of positionShapeFunctionIntegralArray does not match, is " + to_string(rPdm.size()) + " must be " + to_string(3));
      if(not PPdm.size())
        PPdm.resize(3,vector<SqrMatV>(3));
      else if(PPdm.size()!=3)
        throwError("(GenericFlexibleFfrBody::init): first size of shapeFunctionShapeFunctionIntegralArray does not match, is " + to_string(PPdm.size()) + " must be " + to_string(3));
      else if(PPdm[0].size()!=3)
        throwError("(GenericFlexibleFfrBody::init): second size of shapeFunctionShapeFunctionIntegralArray does not match, is " + to_string(PPdm[0].size()) + " must be " + to_string(3));
      if(rPdm[0].cols()!=Pdm.cols())
        throwError("(GenericFlexibleFfrBody::init): number of columns in element of positionShapeFunctionIntegralArray does not match, is " + to_string(rPdm[0].cols()) + " must be " + to_string(Pdm.cols()));
      if(PPdm[0][0].cols()!=Pdm.cols())
        throwError("(GenericFlexibleFfrBody::init): number of columns in element of shapeFunctionShapeFunctionIntegralArray does not match, is " + to_string(PPdm[0][0].cols()) + " must be " + to_string(Pdm.cols()));

      PJT[0].resize(getGeneralizedVelocitySize());
      PJR[0].resize(nu);

      Ki.resize(6+ne);

      PJT[1].resize(6+ne);
      PJR[1].resize(6+ne);
      for(int i=0; i<3; i++)
	PJT[1](i,i) = 1;
      for(int i=3; i<6; i++)
	PJR[1](i-3,i) = 1;

      qRel.resize(nq);
      uRel.resize(nu);
      qdRel.resize(nq);
      udRel.resize(nu);
      WJTrel.resize(nu);
      WJRrel.resize(nu);

      updNodalPos.resize(KrKP.size(),true);
      updNodalVel.resize(KrKP.size(),true);
      updNodalAcc.resize(KrKP.size(),true);
      updNodalJac[0].resize(KrKP.size(),true);
      updNodalJac[1].resize(KrKP.size(),true);
      updNodalGA.resize(KrKP.size(),true);
      updNodalStress.resize(KrKP.size(),true);
      WrOP.resize(KrKP.size());
      WrRP.resize(KrKP.size());
      disp.resize(KrKP.size());
      Womrel.resize(KrKP.size());
      Wvrel.resize(KrKP.size());
      Wom.resize(KrKP.size());
      WvP.resize(KrKP.size());
      WaP.resize(KrKP.size());
      Wpsi.resize(KrKP.size());
      WjP.resize(KrKP.size());
      WjR.resize(KrKP.size());
      if(not ARP.size())
        ARP.resize(KrKP.size(),SqrMat3(Eye()));
      AWK.resize(ARP.size());
      if(not Psi.size())
        Psi.resize(Phi.size(),Mat3xV(ne));
      if(not sigma0.size())
        sigma0.resize(KrKP.size());
      sigma.resize(KrKP.size());

      frameForJacobianOfRotation = generalizedVelocityOfRotation==coordinatesOfAngularVelocityWrtFrameForKinematics?K:R;
    }
    else if(stage==unknownStage) {
      WJP[0].resize(KrKP.size(),Mat3xV(gethSize(0),NONINIT));
      WJR[0].resize(KrKP.size(),Mat3xV(gethSize(0),NONINIT));
      WJP[1].resize(KrKP.size(),Mat3xV(gethSize(1),NONINIT));
      WJR[1].resize(KrKP.size(),Mat3xV(gethSize(1),NONINIT));

      KJ[0].resize(6+ne,hSize[0]);
      KJ[1].resize(6+ne,hSize[1]);
      for(int i=0; i<ne; i++) {
        KJ[0](6+i,hSize[0]-ne+i) = 1;
        KJ[1](6+i,hSize[1]-ne+i) = 1;
      }

      T.init(Eye());

      if(Me.size()==0)
        determineSID();
      prefillMassMatrix();

      K->getJacobianOfTranslation(1,false) = PJT[1];
      K->getJacobianOfRotation(1,false) = PJR[1];

      auto *Atmp = dynamic_cast<StateDependentFunction<RotMat3>*>(fAPK);
      if(Atmp and generalizedVelocityOfRotation!=derivativeOfGeneralizedPositionOfRotation) {
        RotationAboutThreeAxes<VecV> *A3 = dynamic_cast<RotationAboutThreeAxes<VecV>*>(Atmp->getFunction());
        if(A3) {
          fTR = (generalizedVelocityOfRotation==coordinatesOfAngularVelocityWrtFrameOfReference)?A3->getMappingFunction():A3->getTransformedMappingFunction();
          if(not fTR) throwError("(GenericFlexibleFfrBody::init): coordinate transformation not yet available for current rotation");
          fTR->setParent(this);
          constJR = true;
          constjR = true;
          PJRR = SqrMat3(EYE);
          PJR[0].set(i02,iuR,PJRR);
        }
        else
          throwError("(GenericFlexibleFfrBody::init): coordinate transformation only valid for spatial rotations");
      }

      if(fPrPK) {
        if(fPrPK->constParDer1()) {
          constJT = true;
          PJTT = fPrPK->parDer1(qTRel,0);
          PJT[0].set(i02,iuT,PJTT);
        }
        if(fPrPK->constParDer2()) {
          constjT = true;
          PjhT = fPrPK->parDer2(qTRel,0);
        }
      }
      if(fAPK) {
        if(fAPK->constParDer1()) {
          constJR = true;
          PJRR = fAPK->parDer1(qRRel,0);
          PJR[0].set(i02,iuR,PJRR);
        }
        if(fAPK->constParDer2()) {
          constjR = true;
          PjhR = fAPK->parDer2(qRRel,0);
        }
      }

      T.init(Eye());

      // do not invert generalized mass matrix in case of special parametrisation
      if(dynamic_cast<DynamicSystem*>(R->getParent()) and (not fPrPK or constJT) and not fAPK) {
        nonConstantMassMatrix = false;
        M = m*JTJ(PJT[0]) + PJT[0].T()*Ct0.T() + Ct0*PJT[0];
        M(RangeV(M.size()-ne,M.size()-1)) = Me;
        LLM = facLL(M);
      }
    }
    else if(stage==plotting) {
      if(plotFeature[ref(openMBV)] and openMBVBody) {
        if(not dynamic_pointer_cast<OpenMBV::FlexibleBody>(openMBVBody)->getNumberOfVertexPositions())
          dynamic_pointer_cast<OpenMBV::FlexibleBody>(openMBVBody)->setNumberOfVertexPositions(KrKP.size());
      }
    }
    NodeBasedBody::init(stage, config);
    if(fTR) fTR->init(stage, config);
    if(fPrPK) fPrPK->init(stage, config);
    if(fAPK) fAPK->init(stage, config);
  }

  void GenericFlexibleFfrBody::setUpInverseKinetics() {
    InverseKineticsJoint *joint = new InverseKineticsJoint(string("Joint_")+R->getParent()->getName()+"_"+name);
    static_cast<DynamicSystem*>(parent)->addInverseKineticsLink(joint);
    joint->setForceDirection(Mat3xV(3,EYE));
    joint->setMomentDirection(Mat3xV(3,EYE));
    joint->connect(R,K);
    joint->setBody(this);
  }

  void GenericFlexibleFfrBody::plot() {
    if(plotFeature[ref(openMBV)] and openMBVBody) {
      vector<double> data;
      data.push_back(getTime());
      for(int i=0; i<dynamic_pointer_cast<OpenMBV::FlexibleBody>(openMBVBody)->getNumberOfVertexPositions(); i++) {
        const Vec3 &WrOP = evalNodalPosition(i);
        for(int j=0; j<3; j++)
          data.push_back(WrOP(j));
        data.push_back((this->*evalOMBVColorRepresentation[ombvColorRepresentation])(i));
      }
      dynamic_pointer_cast<OpenMBV::FlexibleBody>(openMBVBody)->append(data);
    }
    NodeBasedBody::plot();
  }

  void GenericFlexibleFfrBody::updateMb() {
    Vec3 mc = rdm + Pdm*evalqERel();
    SqrMat3 mtc = tilde(mc);

    SymMat3 I1;
    SqrMat3 I2;
    for (int i=0; i<ne; i++) {
      I1 += mmi1[i]*getqERel().e(i);
      for (int j=0; j<ne; j++)
        I2 += mmi2[i][j]*(getqERel().e(i)*getqERel().e(j));
    }

    SymMat3 I = mmi0 + I1 + SymMat3(I2);
    for(int i=0; i<3; i++) {
      for(int j=0; j<3; j++) {
        M_.e(i+3,j+3) = I.e(i,j);
        M_.e(i+3,j) = mtc.e(i,j);
      }
    }

    MatVx3 Cr1_(ne,NONINIT);
    for(int i=0; i<3; i++)
      Cr1_.set(i,Cr1[i]*getqERel());
    for(int i=0; i<ne; i++)
      for(int j=0; j<3; j++)
        M_.e(i+6,j+3) = Cr0.e(i,j) + Cr1_.e(i,j);

    MatVx3 Ct_ = Ct0;
    if(Ct1.size()) {
      MatVx3 Ct1_(ne,NONINIT);
      for(int i=0; i<3; i++)
        Ct1_.set(i,Ct1[i]*getqERel());
      Ct_ += Ct1_;
      for(int i=0; i<ne; i++)
        for(int j=0; j<3; j++)
          M_.e(i+6,j) = Ct_.e(i,j);
    }

    Matrix<General,Var,Fixed<6>,double> Oe_(ne,NONINIT), Oe1_(ne,NONINIT);
    for(int i=0; i<6; i++)
      Oe1_.set(i,Oe1[i]*getqERel());

    Oe_ = Oe0 + Oe1_;

    vector<SqrMat3> Gr1_(ne);
    SqrMat3 hom21;
    for(int i=0; i<ne; i++) {
      Gr1_[i].init(0);
      for(int j=0; j<ne; j++)
        Gr1_[i] += Gr1[j][i]*getqERel().e(j);
      hom21 += (Gr0[i]+Gr1_[i])*evaluERel().e(i);
    }
    MatVx3 Ge_(ne,NONINIT);
    for(int i=0; i<3; i++)
      Ge_.set(i,Ge[i]*getuERel());

    Vec3 om = K->evalOrientation().T()*K->evalAngularVelocity();
    Vector<Fixed<6>,double> omq;
    for(int i=0; i<3; i++)
      omq.e(i) = pow(om.e(i),2);
    omq.e(3) = om.e(0)*om.e(1); 
    omq.e(4) = om.e(1)*om.e(2); 
    omq.e(5) = om.e(0)*om.e(2);

    VecV hom(6+ne), hg(6+ne), he(6+ne);

    hom.set(RangeV(0,2),-crossProduct(om,crossProduct(om,mc))-2.*crossProduct(om,Ct_.T()*getuERel()));
    hom.set(RangeV(3,5),-(hom21*om) - crossProduct(om,I*om));
    hom.set(RangeV(6,6+ne-1),-(Ge_*om) - Oe_*omq);

    Vec Kg = K->getOrientation().T()*(MBSimEnvironment::getInstance()->getAccelerationOfGravity());
    hg.set(RangeV(0,2),m*Kg);
    hg.set(RangeV(3,5),mtc*Kg);
    hg.set(RangeV(6,hg.size()-1),Ct_*Kg);

    SqrMatV Ke_ = SqrMatV(Ke0);
    for(unsigned int i=0; i<Ke1.size(); i++) {
      Ke_ += Ke1[i]*getqERel().e(i);
      for(unsigned int j=0; j<Ke2.size(); j++)
        Ke_ += Ke2[i][j]*(getqERel().e(i)*getqERel().e(j));
    }

    VecV ke = Ke_*getqERel() + De0*evalqdERel();

    if(ksigma0.size())
      ke += ksigma0;
    if(ksigma1.size())
      ke += ksigma1*getqERel();

    he.set(RangeV(6,hg.size()-1),ke);

    h_ = hom + hg - he;

    updMb = false;
  }

  void GenericFlexibleFfrBody::updateqd() {
    qd(iqT) = evaluTRel();
    qd(iqR) = fTR ? (*fTR)(evalqRRel())*getuRRel() : getuRRel();
    qd(iqE) = getuERel();
  }

  void GenericFlexibleFfrBody::updateT() {
    if(fTR) T(iqR,iuR) = (*fTR)(evalqRRel());
  }

  void GenericFlexibleFfrBody::updateGeneralizedPositions() {
    qRel = q;
    qTRel = qRel(iqT);
    qRRel = qRel(iqR);
    qERel = qRel(iqE);
    updq = false;
  }

  void GenericFlexibleFfrBody::updateGeneralizedVelocities() {
    uRel = u;
    uTRel = uRel(iuT);
    uRRel = uRel(iuR);
    uERel = uRel(iuE);
    updu = false;
  }

  void GenericFlexibleFfrBody::updateDerivativeOfGeneralizedPositions() {
    qdTRel = evaluTRel();
    qdRRel = fTR ? (*fTR)(evalqRRel())*getuRRel() : getuRRel();
    qdERel = getuERel();
    qdRel.set(iqT,qdTRel);
    qdRel.set(iqR,qdRRel);
    qdRel.set(iqE,qdERel);
    updqd = false;
  }

  void GenericFlexibleFfrBody::updateGeneralizedAccelerations() {
    udRel = evalud();
    udERel = udRel(iuE);
    updud = false;
  }

 void GenericFlexibleFfrBody::updatePositions() {
    if(fPrPK) PrPK = (*fPrPK)(evalqTRel(),getTime());
    if(fAPK) APK = (*fAPK)(evalqRRel(),getTime());
    WrPK = R->evalOrientation()*PrPK;
    updPos = false;
  }

  void GenericFlexibleFfrBody::updateVelocities() {
    if(fPrPK) WvPKrel = R->evalOrientation()*(evalPJTT()*evaluTRel() + evalPjhT());
    if(fAPK) WomPK = frameForJacobianOfRotation->evalOrientation()*(evalPJRR()*evaluRRel() + evalPjhR());
    updVel = false;
  }

  void GenericFlexibleFfrBody::updateJacobians() {
    if(fPrPK) {
      if(!constJT) {
        PJTT = fPrPK->parDer1(evalqTRel(),getTime());
        PJT[0].set(i02,iuT,PJTT);
      }
      if(!constjT)
        PjhT = fPrPK->parDer2(evalqTRel(),getTime());
    }
    if(fAPK) {
      if(!constJR) {
        PJRR = fAPK->parDer1(evalqRRel(),getTime());
        PJR[0].set(i02,iuR,PJRR);
      }
      if(!constjR)
        PjhR = fAPK->parDer2(evalqRRel(),getTime());
    }
    updPJ = false;
  }

  void GenericFlexibleFfrBody::updateGyroscopicAccelerations() {
    if(fPrPK and not(constJT and constjT))
      PjbT = (fPrPK->parDer1DirDer1(evalqdTRel(),evalqTRel(),getTime())+fPrPK->parDer1ParDer2(evalqTRel(),getTime()))*getuTRel() + fPrPK->parDer2DirDer1(evalqdTRel(),evalqTRel(),getTime()) + fPrPK->parDer2ParDer2(evalqTRel(),getTime());
    if(fAPK and not(constJR and constjR))
      PjbR = (fAPK->parDer1DirDer1(evalqdRRel(),evalqRRel(),getTime())+fAPK->parDer1ParDer2(evalqRRel(),getTime()))*getuRRel() + fAPK->parDer2DirDer1(evalqdRRel(),evalqRRel(),getTime()) + fAPK->parDer2ParDer2(evalqRRel(),getTime());
    updPjb = false;
  }

  void GenericFlexibleFfrBody::updateKJ0() {
    KJ[0].set(RangeV(0,2),RangeV(0,KJ[0].cols()-1),K->evalOrientation().T()*K->evalJacobianOfTranslation());
    KJ[0].set(RangeV(3,5),RangeV(0,KJ[0].cols()-1),K->getOrientation().T()*K->getJacobianOfRotation());
    Ki.set(RangeV(0,2),K->getOrientation().T()*K->evalGyroscopicAccelerationOfTranslation());
    Ki.set(RangeV(3,5),K->getOrientation().T()*K->getGyroscopicAccelerationOfRotation());
    updKJ[0] = false;
  }

  void GenericFlexibleFfrBody::updateKJ1() {
    KJ[1].set(RangeV(0,2),RangeV(0,KJ[1].cols()-1),K->evalOrientation().T()*K->evalJacobianOfTranslation(1));
    KJ[1].set(RangeV(3,5),RangeV(0,KJ[1].cols()-1),K->getOrientation().T()*K->getJacobianOfRotation(1));
    updKJ[1] = false;
  }

  void GenericFlexibleFfrBody::updatePositions(Frame *frame) {
    frame->setPosition(R->getPosition() + evalGlobalRelativePosition());
    frame->setOrientation(R->getOrientation()*getRelativeOrientation());
  }

 void GenericFlexibleFfrBody::updateVelocities(Frame *frame) {
    frame->setAngularVelocity(R->evalAngularVelocity() + evalGlobalRelativeAngularVelocity());
    frame->setVelocity(R->getVelocity() + getGlobalRelativeVelocity() + crossProduct(R->getAngularVelocity(),evalGlobalRelativePosition()));
  }

  void GenericFlexibleFfrBody::updateAccelerations(Frame *frame) {
    frame->setAcceleration(K->evalJacobianOfTranslation()*evaludall() + K->evalGyroscopicAccelerationOfTranslation());
    frame->setAngularAcceleration(K->evalJacobianOfRotation()*getudall() + K->evalGyroscopicAccelerationOfRotation());
  }

  void GenericFlexibleFfrBody::updateJacobians0(Frame *frame) {
    frame->getJacobianOfTranslation(0,false).init(0);
    frame->getJacobianOfRotation(0,false).init(0);
    frame->getJacobianOfTranslation(0,false).set(i02,RangeV(0,R->gethSize()-1), R->evalJacobianOfTranslation() - tilde(evalGlobalRelativePosition())*R->evalJacobianOfRotation());
    frame->getJacobianOfRotation(0,false).set(i02,RangeV(0,R->gethSize()-1), R->evalJacobianOfRotation());
    frame->getJacobianOfTranslation(0,false).add(i02,RangeV(gethSize(0)-getuSize(0),gethSize(0)-1), R->evalOrientation()*evalPJT());
    frame->getJacobianOfRotation(0,false).add(i02,RangeV(gethSize(0)-getuSize(0),gethSize(0)-1), frameForJacobianOfRotation->evalOrientation()*getPJR());
  }

  void GenericFlexibleFfrBody::updateGyroscopicAccelerations(Frame *frame) {
    frame->setGyroscopicAccelerationOfTranslation(R->evalGyroscopicAccelerationOfTranslation() + crossProduct(R->evalGyroscopicAccelerationOfRotation(),evalGlobalRelativePosition()) + R->evalOrientation()*evalPjbT() + crossProduct(R->evalAngularVelocity(), 2.*evalGlobalRelativeVelocity()+crossProduct(R->evalAngularVelocity(),evalGlobalRelativePosition())));
    frame->setGyroscopicAccelerationOfRotation(R->evalGyroscopicAccelerationOfRotation() + frameForJacobianOfRotation->evalOrientation()*getPjbR() + crossProduct(R->evalAngularVelocity(), evalGlobalRelativeAngularVelocity()));
  }

  void GenericFlexibleFfrBody::resetUpToDate() {
    NodeBasedBody::resetUpToDate();
    updPjb = true;
    updMb = true;
    updKJ[0] = true;
    updKJ[1] = true;
  }

  void GenericFlexibleFfrBody::updateM() {
    M += JTMJ(evalMb(),evalKJ());
  }

  void GenericFlexibleFfrBody::initializeUsingXML(DOMElement *element) {
    NodeBasedBody::initializeUsingXML(element);

    // frames
    DOMElement *e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"frames")->getFirstElementChild();
    while(e) {
      NodeFrame *f=new NodeFrame(E(e)->getAttribute("name"));
      addFrame(f);
      f->initializeUsingXML(e);
      e=e->getNextElementSibling();
    }

    // contours
    e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"contours")->getFirstElementChild();
    while(e) {
      auto *c=ObjectFactory::createAndInit<Contour>(e);
      addContour(c);
      e=e->getNextElementSibling();
    }

    e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"generalTranslation");
    if(e) {
      auto *trans=ObjectFactory::createAndInit<MBSim::Function<Vec3(VecV,double)> >(e->getFirstElementChild());
      setGeneralTranslation(trans);
    }
    else {
      e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"timeDependentTranslation");
      if(e) {
        auto *trans=ObjectFactory::createAndInit<MBSim::Function<Vec3(double)> >(e->getFirstElementChild());
        setTimeDependentTranslation(trans);
      }
      else {
        e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"stateDependentTranslation");
        if(e) {
          auto *trans=ObjectFactory::createAndInit<MBSim::Function<Vec3(VecV)> >(e->getFirstElementChild());
          setStateDependentTranslation(trans);
        }
      }
    }
    e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"generalRotation");
    if(e) {
      auto *rot=ObjectFactory::createAndInit<MBSim::Function<RotMat3(VecV,double)> >(e->getFirstElementChild());
      setGeneralRotation(rot);
    }
    else {
      e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"timeDependentRotation");
      if(e) {
        auto *rot=ObjectFactory::createAndInit<MBSim::Function<RotMat3(double)> >(e->getFirstElementChild());
        setTimeDependentRotation(rot);
      }
      else {
        e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"stateDependentRotation");
        if(e) {
          auto *rot=ObjectFactory::createAndInit<MBSim::Function<RotMat3(VecV)> >(e->getFirstElementChild());
          setStateDependentRotation(rot);
        }
      }
    }
    e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"translationDependentRotation");
    if(e) translationDependentRotation = E(e)->getText<bool>();
    e=E(element)->getFirstElementChildNamed(MBSIM%"generalizedVelocityOfRotation");
    if(e) {
      string generalizedVelocityOfRotationStr=string(X()%E(e)->getFirstTextChild()->getData()).substr(1,string(X()%E(e)->getFirstTextChild()->getData()).length()-2);
      if(generalizedVelocityOfRotationStr=="derivativeOfGeneralizedPositionOfRotation") generalizedVelocityOfRotation=derivativeOfGeneralizedPositionOfRotation;
      else if(generalizedVelocityOfRotationStr=="coordinatesOfAngularVelocityWrtFrameOfReference") generalizedVelocityOfRotation=coordinatesOfAngularVelocityWrtFrameOfReference;
      else if(generalizedVelocityOfRotationStr=="coordinatesOfAngularVelocityWrtFrameForKinematics") generalizedVelocityOfRotation=coordinatesOfAngularVelocityWrtFrameForKinematics;
      else generalizedVelocityOfRotation=unknown;
    }

    e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"enableOpenMBVFrameK");
    if(e) {
//      if(!openMBVBody) setOpenMBVRigidBody(OpenMBV::ObjectFactory::create<OpenMBV::InvisibleBody>());
      OpenMBVFrame ombv;
      ombv.initializeUsingXML(e);
      K->setOpenMBVFrame(ombv.createOpenMBV());
    }

    e=E(element)->getFirstElementChildNamed(MBSIMFLEX%"plotFeatureFrameK");
    while(e and E(e)->getTagName()==MBSIMFLEX%"plotFeatureFrameK") {
      auto pf=getPlotFeatureFromXML(e);
      K->setPlotFeature(pf.first, pf.second);
      e=e->getNextElementSibling();
    }
  }

  void GenericFlexibleFfrBody::updateStresses(int j) {
    sigma[j] = sigma0[j];
    if(sigmahel[j].cols()) {
      Matrix<General, Fixed<6>, Var, double> sigmahe = sigmahel[j];
      for(unsigned int i=0; i<sigmahen.size(); i++)
        sigmahe += sigmahen[j][i]*evalqERel()(i);
      sigma[j] += sigmahe*getqERel();
    }
    updNodalStress[j] = false;
  }

  void GenericFlexibleFfrBody::updatePositions(int i) {
    AWK[i] = K->evalOrientation()*ARP[i]*(Id+tilde(Psi[i]*evalqERel()));
    disp[i] = Phi[i]*getqERel();
    WrRP[i] = K->getOrientation()*(KrKP[i]+disp[i]);
    WrOP[i] = K->getPosition() + WrRP[i];
    updNodalPos[i] = false;
  }

  void GenericFlexibleFfrBody::updateVelocities(int i) {
    Womrel[i] = K->evalOrientation()*(Psi[i]*evaluERel());
    Wvrel[i] = K->getOrientation()*(Phi[i]*getuERel());
    Wom[i] = K->evalAngularVelocity() + Womrel[i];
    WvP[i] = K->getVelocity() + crossProduct(K->getAngularVelocity(), evalGlobalRelativePosition(i)) + Wvrel[i];
    updNodalVel[i] = false;
  }

  void GenericFlexibleFfrBody::updateAccelerations(int i) {
    Wpsi[i] = K->evalAngularAcceleration() + crossProduct(K->evalAngularVelocity(),evalGlobalRelativeAngularVelocity(i)) + K->evalOrientation()*(Psi[i]*evaludERel());
    WaP[i] = K->getAcceleration() + crossProduct(K->getAngularAcceleration(), evalGlobalRelativePosition(i)) + crossProduct(K->getAngularVelocity(), crossProduct(K->getAngularVelocity(), evalGlobalRelativePosition(i))) + 2.*crossProduct(K->getAngularVelocity(), getNodalRelativeVelocity(i)) + K->getOrientation()*(Phi[i]*getudERel());
    updNodalAcc[i] = false;
  }

  void GenericFlexibleFfrBody::updateJacobians(int i, int j) {
    Mat3xV Phi_ = Phi[i];
    if(K0F.size() and K0F[i].size()) {
      MatVx3 PhigeoT(ne,NONINIT);
      for(int k=0; k<3; k++)
        PhigeoT.set(k,K0F[i][k]*q);
      Phi_ += PhigeoT.T();
    }
    Mat3xV Psi_ = Psi[i];
    if(K0M.size() and K0M[i].size()) {
      MatVx3 PsigeoT(ne,NONINIT);
      for(int k=0; k<3; k++)
        PsigeoT.set(k,K0M[i][k]*q);
      Psi_ += PsigeoT.T();
    }
    WJR[j][i] = K->evalJacobianOfRotation(j);
    WJR[j][i].add(RangeV(0,2),RangeV(gethSize(j)-ne,gethSize(j)-1),K->evalOrientation()*Psi_);
    WJP[j][i] = K->getJacobianOfTranslation(j) - tilde(evalGlobalRelativePosition(i))*K->getJacobianOfRotation(j);
    WJP[j][i].add(RangeV(0,2),RangeV(gethSize(j)-ne,gethSize(j)-1),K->getOrientation()*Phi_);
    updNodalJac[j][i] = false;
  }

  void GenericFlexibleFfrBody::updateGyroscopicAccelerations(int i) {
    WjR[i] = K->evalGyroscopicAccelerationOfRotation() + crossProduct(K->evalAngularVelocity(),evalGlobalRelativeAngularVelocity(i));
    WjP[i] = K->getGyroscopicAccelerationOfTranslation() + crossProduct(K->getGyroscopicAccelerationOfRotation(),evalGlobalRelativePosition(i)) + crossProduct(K->getAngularVelocity(),crossProduct(K->getAngularVelocity(),evalGlobalRelativePosition(i))) + 2.*crossProduct(K->getAngularVelocity(),getNodalRelativeVelocity(i));
    updNodalGA[i] = false;
  }

  void GenericFlexibleFfrBody::updatePositions(NodeFrame* frame) {
    frame->setPosition(evalNodalPosition(frame->getNodeIndex()));
    frame->setOrientation(getNodalOrientation(frame->getNodeIndex()));
 }

  void GenericFlexibleFfrBody::updateVelocities(NodeFrame* frame) {
    frame->setVelocity(evalNodalVelocity(frame->getNodeIndex()));
    frame->setAngularVelocity(getNodalAngularVelocity(frame->getNodeIndex()));
  }

  void GenericFlexibleFfrBody::updateAccelerations(NodeFrame* frame) {
    frame->setAcceleration(evalNodalAcceleration(frame->getNodeIndex()));
    frame->setAngularAcceleration(getNodalAngularAcceleration(frame->getNodeIndex()));
  }

  void GenericFlexibleFfrBody::updateJacobians(NodeFrame* frame, int j) {
    frame->setJacobianOfTranslation(evalNodalJacobianOfTranslation(frame->getNodeIndex(),j));
    frame->setJacobianOfRotation(getNodalJacobianOfRotation(frame->getNodeIndex(),j));
  }

  void GenericFlexibleFfrBody::updateGyroscopicAccelerations(NodeFrame* frame) {
    frame->setGyroscopicAccelerationOfTranslation(evalNodalGyroscopicAccelerationOfTranslation(frame->getNodeIndex()));
    frame->setGyroscopicAccelerationOfRotation(getNodalGyroscopicAccelerationOfRotation(frame->getNodeIndex()));
  }

  double GenericFlexibleFfrBody::evalEquivalentStress(int i) {
    const Vector<Fixed<6>,double> &s = evalNodalStress(i);
    return sqrt(0.5*(pow(s(0)-s(1),2)+pow(s(1)-s(2),2)+pow(s(2)-s(0),2))+3*(pow(s(3),2)+pow(s(4),2)+pow(s(5),2)));
  }

}