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

#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "mbsim/contours/rigid_contour.h"
#include "mbsim/utils/boost_parameters.h"
#include "mbsim/utils/openmbv_utils.h"
#include <openmbvcppinterface/frustum.h>

namespace MBSim {

  /**
   * \brief frustum with axis in direction of second column of contour reference frame
   * \author Martin Foerg
   * \author Thorsten Schindler
   * \date 2009-04-20 some comments (Thorsten Schindler)
   */
  class Frustum : public RigidContour {
    public:
      /**
       * \brief constructor with contact from inside
       * \param name of contour
       */
      Frustum(const std::string &name="", Frame *R=nullptr) : RigidContour(name,R) { }

      Frustum(const std::string &name, const fmatvec::Vec2 &r_, double h_, Frame *R=nullptr) : RigidContour(name,R), r(r_), h(h_) { }

      Frustum(const std::string &name, const fmatvec::Vec2 &r_, double h_, bool outCont_, Frame *R=nullptr) : RigidContour(name,R), r(r_), h(h_), outCont(outCont_) { }

      /* INHERITED INTERFACE OF ELEMENT */
      void init(InitStage stage, const InitConfigSet &config) override;
      /***************************************************/

      /* INHERITED INTERFACE OF CONTOUR */
      fmatvec::Vec2 evalZeta(const fmatvec::Vec3 &WrPoint) override;
      /***************************************************/

      /**
       * \brief constructor
       * \param name of the contour
       * \param contact from outside?
       */
      Frustum(const std::string &name, bool outCont_) : RigidContour(name),  outCont(outCont_) { }

      /* GETTER / SETTER */
      void setRadii(const fmatvec::Vec2 &r_);
      const fmatvec::Vec2& getRadii() const;
      void setHeight(double h_);
      double getHeight() const;
      void setOutCont(bool outCont_);
      bool getOutCont() const;
      /***************************************************/

      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBV, tag, (optional (diffuseColor,(const fmatvec::Vec3&),fmatvec::Vec3(std::vector<double>{-1,1,1}))(transparency,(double),0)(pointSize,(double),0)(lineWidth,(double),0))) {
        OpenMBVColoredBody ombv(diffuseColor,transparency,pointSize,lineWidth);
        openMBVRigidBody=ombv.createOpenMBV<OpenMBV::Frustum>();
      }
      
      void initializeUsingXML(xercesc::DOMElement *element) override;

      private:
      /** 
       * \brief upper r(1) and lower radius r(0) in direction of the axis
       */
      fmatvec::Vec2 r;

      /** 
       * \brief height
       */
      double h{0.};

      /** 
       * \brief contact on outer or inner surface?
       */
      bool outCont{false};
  };

  inline void Frustum::setRadii(const fmatvec::Vec2 &r_) { r = r_; }
  inline const fmatvec::Vec2& Frustum::getRadii() const { return r; }
  inline void Frustum::setHeight(double h_) { h = h_; }
  inline double Frustum::getHeight() const { return h; }
  inline void Frustum::setOutCont(bool outCont_) { outCont = outCont_; }
  inline bool Frustum::getOutCont() const { return outCont; } 
}

#endif /* _FRUSTUM_H_ */
