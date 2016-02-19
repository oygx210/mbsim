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

#ifndef _PLANAR_CONTOUR_H_
#define _PLANAR_CONTOUR_H_

#include "mbsim/contours/rigid_contour.h"

#ifdef HAVE_OPENMBVCPPINTERFACE
#include "mbsim/utils/boost_parameters.h"
#include <mbsim/utils/openmbv_utils.h>
#endif

namespace MBSim {

  class ContactKinematics;
  template <class Sig> class Function;

  /** 
   * \brief analytical description of contours with one contour parameter
   * \author Robert Huber
   * \date 2009-04-20 some comments (Thorsten Schindler)
   * \date 2009-06-04 new file (Thorsten Schindler)
   */
  class PlanarContour : public RigidContour {
    public:
      /**
       * \brief constructor
       * \param name of contour
       */
      PlanarContour(const std::string &name="", Frame *R=NULL) : RigidContour(name,R) { }

      /**
       * \brief destructor
       */
      virtual ~PlanarContour();

      /* INHERITED INTERFACE OF ELEMENT */
      std::string getType() const { return "PlanarContour"; }
      void plot(double t, double dt = 1);
      /***************************************************/

      /* INHERITED INTERFACE OF CONTOUR */
      void init(InitStage stage);
      double getCurvature(const fmatvec::Vec2 &zeta);
      fmatvec::Vec3 getKrPS(const fmatvec::Vec2 &zeta);
      fmatvec::Vec3 getKs(const fmatvec::Vec2 &zeta);
      fmatvec::Vec3 getKt(const fmatvec::Vec2 &zeta);
      fmatvec::Vec3 getParDer1Ks(const fmatvec::Vec2 &zeta);
      /***************************************************/

      /* GETTER / SETTER */
      void setContourFunction(Function<fmatvec::Vec3(double)> *f) { funcCrPC = f; }
      Function<fmatvec::Vec3(double)>* getContourFunction() { return funcCrPC; }
      /***************************************************/

#ifdef HAVE_OPENMBVCPPINTERFACE
      BOOST_PARAMETER_MEMBER_FUNCTION( (void), enableOpenMBV, tag, (optional (openMBVNodes,(const std::vector<double>&),std::vector<double>())(diffuseColor,(const fmatvec::Vec3&),"[-1;1;1]")(transparency,(double),0))) {
        OpenMBVExtrusion ombv(1,diffuseColor,transparency);
        openMBVRigidBody=ombv.createOpenMBV(); 
        ombvNodes = openMBVNodes;
      }
#endif
      
      virtual void initializeUsingXML(xercesc::DOMElement *element);
      virtual xercesc::DOMElement* writeXMLFile(xercesc::DOMNode *element);

      /**
       * \brief contact search for RigidContours
       * \author Markus Schneider
       * \date 2010-11-05 initial commit (Markus Schneider)
       */
      ContactKinematics * findContactPairingWith(std::string type0, std::string type1);

      void setNodes(const std::vector<double> &nodes_) { etaNodes = nodes_; }

    protected:
      Function<fmatvec::Vec3(double)> * funcCrPC;

#ifdef HAVE_OPENMBVCPPINTERFACE
      boost::shared_ptr<OpenMBV::RigidBody> openMBVRigidBody;
      std::vector<double> ombvNodes;
#endif
  };

}

#endif

