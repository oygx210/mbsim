/* Copyright (C) 2007  Martin Förg, Roland Zander

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
 * Contact:
 *   mfoerg@users.berlios.de
 *   rzander@users.berlios.de
 *
 */

#ifndef _CONTACT_KINEMATICS_H_
#define _CONTACT_KINEMATICS_H_

#include "fmatvec.h"
#include <vector>

using namespace fmatvec;

namespace MBSim {

  class CoordinateSystem;
  class Contour;
  class ContourPointData;

  /*! Computes a perpendicular vector \result t to the normal \param n */	
  Vec computeTangential(const Vec &n);

  /*! 
   * \brief Basis class for contact kinematical calculations
   *  The generalised position to the contact point including normal and tangential directions need to be up-to-date latest at the end of stage2()
   *  Per default no data is stored or managed within this class
   */
  class ContactKinematics {

    protected:

      int numberOfPotentialContactPoints;

    public:

      ContactKinematics() : numberOfPotentialContactPoints(1) {}
      virtual ~ContactKinematics() {}

      /*! compute \f$\boldsymbol{g}_N\f$, which MUST be provided, others optional$
       * \param contour  vector of Contour holding both contours
       * \param i1       index of first contour within all data-vectors
       * \param i2       index of second contour within all data-vectors
       * \param g        normal distance
       * \param cpData   vector of generalised position vectors(ContourPointData) for both contours
       */
      //virtual void stage1(Vec &g) = 0;
      virtual void updateg(Vec &g, ContourPointData *cpData) {cout << "updateg not implemented" << endl; throw 5;}
      //virtual double computeg() = 0;

      /*! compute \f$\dot{\boldsymbol{g}}\f$, force directions, ... must be up-to-date at end of method
       * \param contour  vector of Contour holding both contours
       * \param i1       index of first contour within all data-vectors
       * \param i2       index of second contour within all data-vectors
       * \param g        normal distance
       * \param gd       contact velocities
       * \param cpData   vector of generalised position vectors(ContourPointData) for both contours
       */
      //virtual void stage2(const Vec& g, Vec &gd) = 0;
      virtual void updategd(const Vec& g, Vec &gd, ContourPointData* cpData) {cout << "updategd not implemented" << endl; throw 5;}
      //virtual Vec computegd(const Vec& g) = 0;

      virtual void updatewb(Vec &wb, const Vec &g, ContourPointData* cpData) {cout << "updatewb not implemented" << endl; throw 5;};

      /*! Treats ordering of contours \param contour */
      virtual void assignContours(const vector<Contour*> &contour) = 0;
      /*! Treats ordering of contours \param contour1 and \param contour2 */
      void assignContours(Contour *contour1, Contour *contour2) {vector<Contour*> c; c.push_back(contour1);c.push_back(contour2); assignContours(c);}

      virtual void updateg(vector<Vec> &g, vector<ContourPointData*> &cpData) {updateg(g[0],cpData[0]);}
      virtual void updategd(vector<Vec> &g, vector<Vec> &gd, vector<ContourPointData*> &cpData) {updategd(g[0],gd[0],cpData[0]);}
      virtual void updatewb(vector<Vec> &wb, vector<Vec> &g, vector<ContourPointData*> &cpData) {updatewb(wb[0],g[0],cpData[0]);}

      int getNumberOfPotentialContactPoints() const {return numberOfPotentialContactPoints;}
  };

}

#endif
