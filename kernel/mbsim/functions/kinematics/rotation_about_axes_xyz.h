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
 * Contact: martin.o.foerg@gmail.com
 */

#ifndef _ROTATION_ABOUT_AXES_XYZ_H_
#define _ROTATION_ABOUT_AXES_XYZ_H_

#include "mbsim/functions/kinematics/rotation_about_three_axes.h"
#include "mbsim/functions/kinematics/rotation_about_axes_xyz_mapping.h"
#include "mbsim/functions/kinematics/rotation_about_axes_xyz_transformed_mapping.h"

namespace MBSim {

  /*!
   * \brief rotation class for rotation about all three axes using the cardan description
   */
  template<class Arg> 
  class RotationAboutAxesXYZ : public RotationAboutThreeAxes<Arg> {
    using B = fmatvec::Function<fmatvec::RotMat3(Arg)>; 
    using RotationAboutThreeAxes<Arg>::A;
    using RotationAboutThreeAxes<Arg>::J;
    using RotationAboutThreeAxes<Arg>::Jd;
    public:
      RotationAboutAxesXYZ() { J.e(0,0) = 1; }
      fmatvec::RotMat3 operator()(const Arg &q) override {
        double a=q.e(0);
        double b=q.e(1);
        double g=q.e(2);
        double cosa = cos(a);
        double sina = sin(a);
        double cosb = cos(b);
        double sinb = sin(b);
        double cosg = cos(g);
        double sing = sin(g);
        A.e(0,0) = cosb*cosg;
        A.e(1,0) = sina*sinb*cosg+cosa*sing;
        A.e(2,0) = -cosa*sinb*cosg+sina*sing;
        A.e(0,1) = -cosb*sing;
        A.e(1,1) = -sing*sinb*sina+cosa*cosg;
        A.e(2,1) = cosa*sinb*sing+sina*cosg;
        A.e(0,2) = sinb;
        A.e(1,2) = -sina*cosb;
        A.e(2,2) = cosa*cosb;
        return A;
      }
      typename B::DRetDArg parDer(const Arg &q) override {
        double a = q.e(0);
        double b = q.e(1);
        double cosa = cos(a);
        double sina = sin(a);
        double cosb = cos(b);
        if(fabs(cosb)<=1e-13)
          Element::throwError("Singularity in rotation.");
        //J.e(0,0) = 1;
        //J.e(0,1) = 0;
        J.e(0,2) = sin(b);
        //J.e(1,0) = 0;
        J.e(1,1) = cosa;
        J.e(1,2) = -sina*cosb;
        //J.e(2,0) = 0;
        J.e(2,1) = sina;
        J.e(2,2) = cosa*cosb;
        return J;
      }
      typename B::DRetDArg parDerDirDer(const Arg &qd, const Arg &q) override {
        double a = q.e(0);
        double b = q.e(1);
        double ad = qd.e(0);
        double bd = qd.e(1);
        double cosa = cos(a);
        double sina = sin(a);
        double cosb = cos(b);
        double sinb = sin(b);
        //Jd.e(0,0) = 0;
        //Jd.e(0,1) = 0;
        Jd.e(0,2) = cosb*bd;
        //Jd.e(1,0) = 0;
        Jd.e(1,1) = -sina*ad;
        Jd.e(1,2) = -cosa*cosb*ad + sina*sinb*bd;
        //Jd.e(2,0) = 0;
        Jd.e(2,1) = cosa*ad;
        Jd.e(2,2) = -sina*cosb*ad - cosa*sinb*bd;
        return Jd;
      }
      Function<fmatvec::MatV(Arg)>* getMappingFunction() const override { return new RotationAboutAxesXYZMapping<Arg>; }
      Function<fmatvec::MatV(Arg)>* getTransformedMappingFunction() const override { return new RotationAboutAxesXYZTransformedMapping<Arg>; }
  };

}

#endif
