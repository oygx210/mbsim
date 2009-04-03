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
 * Contact: mfoerg@users.berlios.de
 *          thschindler@users.berlios.de
 *          rzander@users.berlios.de
 */

#ifndef _INTERFACES_H_
#define _INTERFACES_H_

#include <fmatvec.h>
#include <mbsim/element.h>

namespace H5 {
  class CommonFG;
  class Group;
}

#ifdef HAVE_AMVISCPPINTERFACE
namespace AMVis {
  class Group;
}
#endif

namespace MBSim {

  class ContourPointData;
  class DynamicSystem;

  /*!
   * \brief interface for objects for usage in tree structures
   * \author Martin Foerg
   * \date 2009-03-09 some comments (Thorsten Schindler)
   * \date 2009-03-19 element.h added (Thorsten Schindler)
   * \date 2009-03-26 some comments (Thorsten Schindler)
   */
  class ObjectInterface {
    public:
      /**
       * \brief constructor
       */
      ObjectInterface() {}

      /**
       * \brief destructor
       */
      virtual ~ObjectInterface() {}

      /**
       * \brief update linear transformation between differentiated positions and generalised velocities
       * \param simulation time
       */
      virtual void updateT(double t) = 0;

      /**
       * \brief update smooth right hand side
       * \param simulation time
       */
      virtual void updateh(double t) = 0;

      /**
       * \brief update mass matrix
       * \param simulation time
       */
      virtual void updateM(double t) = 0;

      /**
       * \brief update kinematics of frames, contours and bodies
       * \param simulation time
       */
      virtual void updateKinematics(double t) = 0;

      /**
       * \brief update acceleration description of frames, contours and bodies
       * \param simulation time
       */
      virtual void updateJacobians(double t) = 0;

      /**
       * \brief update position increment
       * \param simulation time
       * \param simulation time step size
       */
      virtual void updatedq(double t, double dt) = 0;
      
      /**
       * \brief update velocity increment
       * \param simulation time 
       * \param simulation time step size
       */
      virtual void updatedu(double t, double dt) = 0;

      /**
       * \brief update differentiated velocity
       * \param simulation time 
       */
      virtual void updateud(double t) = 0;

      /**
       * \brief update differentiated positions
       * \param simulation time
       */
      virtual void updateqd(double t) = 0;

      /**
       * \brief update differentiated state
       * \param simulation time
       */
      virtual void updatezd(double t) = 0;

      /**
       * \param size of right hand side
       * \param index for normal usage and inverse kinetics 
       */
      virtual void sethSize(int hSize, int i=0) = 0;
      
      /**
       * \param index for normal usage and inverse kinetics 
       * \return size of right hand side
       */
      virtual int gethSize(int i=0) const = 0;

      /**
       * \return size of positions
       */
      virtual int getqSize() const = 0;

      /**
       * \param index for normal usage and inverse kinetics 
       * \return size of velocities
       */
      virtual int getuSize(int i=0) const = 0;
      
      /**
       * \brief calculates size of positions
       */
      virtual void calcqSize() = 0;
      
      /**
       * \brief calculates size of velocities
       * \param index for normal usage and inverse kinetics 
       */
      virtual void calcuSize(int j=0) = 0;

      /**
       * \param index of positions
       */
      virtual void setqInd(int ind) = 0;

      /**
       * \param index of velocities
       * \param index for normal usage and inverse kinetics 
       */
      virtual void setuInd(int ind, int i=0) = 0;

      /**
       * \param dynamic system
       * \param index for normal usage and inverse kinetics 
       * \return index of right hand side
       */
      virtual int gethInd(DynamicSystem* sys, int i=0) = 0;

      /**
       * \brief update JACOBIAN for inverse kinetics TODO
       * \param simulation time
       */
      virtual void updateSecondJacobians(double t) = 0;
      
      /**
       * \return associated plot group
       */
      virtual H5::Group *getPlotGroup() = 0;

      /**
       * \return plot feature
       */
      virtual PlotFeatureStatus getPlotFeature(PlotFeature fp) = 0;
      
      /**
       * \return plot feature for derived classes
       */
      virtual PlotFeatureStatus getPlotFeatureForChildren(PlotFeature fp) = 0;

#ifdef HAVE_AMVISCPPINTERFACE
      virtual AMVis::Group* getAMVisGrp()=0;
#endif
  };

  /*!
   * all Link%s use this common interface regardless whether single- or set-valued
   * / smooth or non-smooth interactions are defined
   * \brief interface for links
   * \author Martin Foerg
   * \date 2009-03-09 some comments (Thorsten Schindler)
   * \date 2009-03-26 enhanced comments (Roland Zander)
   */
  class LinkInterface {
    public:
      /*!
       * \brief constructor 
       */
      LinkInterface() {}

      /*!
       * \brief destructor
       */
      virtual ~LinkInterface() {}

      /*!
	   * for links holding non-smooth contributions \f$\dd\vLambda\f$, the respective forces are projected
	   * into the minimal coordinate representation of the associated Body%s using the
	   * JACOBIAN matrices \f$\vJ\f$.
	   * \f[ \vr = \vJ\dd\vLambda \f]
	   * The JACOBIAN is provided by the connected Frame%s (which might be
	   * user-defined for Joint%s or internally defined for Contact%s).
       * \brief update smooth link force law
       * \param simulation time
       */
      virtual void updater(double t) = 0;

      /*!
       * \brief updates nonlinear relative acceleration term
       * \param simulation time
       */
      virtual void updatewb(double t) = 0;

      /*!
       * \brief updates JACOBIAN matrix \f$\vW\f$ between Lagrangian multipliers and generalised velocities
       * \param simulation time
       */
      virtual void updateW(double t) = 0;

      /*!
	   * for event driven integration, \f$\vW\f$ can be condensed regarding dependent
	   * tangential contacts, where \f$\Lambda_N\f$ also occures in evaluation of tangential
	   * force when sliding; in this case, \f$\vV\f$ might hold less columns than \f$\vW\f$
       * \brief updates condensed JACOBIAN matrix between condensed Lagrangian multipliers and generalised velocities
       * \param simulation time
       */
      virtual void updateV(double t) = 0;

      /*!
	   * for links holding smooth contributions \f$\vF\f$, the respective forces are projected
	   * into the minimal coordinate representation of the associated Body%s using the
	   * JACOBIAN matrices \f$\vJ\f$.
	   * \f[ \vh = \vJ\vF \f]
	   * The JACOBIAN is provided by the connected Frame%s (which might be
	   * user-defined for Joint%s or internally defined for Contact%s).
       * \brief update smooth link force law
       * \param simulation time
       */
      virtual void updateh(double t) = 0;

      /*!
       * \brief update relative distance
       * \param simulation time
       */
      virtual void updateg(double t) = 0;
      
      /*!
       * \brief update relative velocity
       * \param simulation time
       *
       * compute normal and tangential relative velocities, velocity and angular velocity of possible contact point if necessary
       */
      virtual void updategd(double t) = 0;

      /*!
       * \brief update TODO
       * \param simulation time
       */
      virtual void updatedx(double t, double dt) = 0;
      
      /*!
       * \brief update TODO
       * \param simulation time
       */
      virtual void updatexd(double t) = 0;

      /*!
       * \brief update stop vector (root functions for event driven integration)
       * \param simulation time
       */
      virtual void updateStopVector(double t) = 0;

      /*!
       * \brief update acceleration description of frames and contours
       * \param simulation time
       */
      virtual void updateJacobians(double t) = 0;
  };

  /*!
   * \brief discretization interface for flexible systems
   * \author Thorsten Schindler
   * \author Roland Zander
   * \date 2009-03-09 initial commit in kernel_dev (Thorsten Schindler)
   * 
   * interface for the desription of flexible systems using global and FE ansatz functions
   */
  class DiscretizationInterface {
    public:
      /*!
       * \brief constructor
       */
      DiscretizationInterface() {}    

      /*! 
       * \brief destructor
       */
      virtual ~DiscretizationInterface() {}    

      /*!
       * \return mass matrix of discretization
       */
      virtual fmatvec::SymMat getMassMatrix() const = 0;    

      /*!
       * \return smooth right hand side of discretization
       */
      virtual fmatvec::Vec getGeneralizedForceVector() const = 0;

      /*!
       * \return Jacobian of implicit integration regarding position
       */
      virtual fmatvec::SqrMat getJacobianForImplicitIntegrationRegardingPosition() const = 0;    

      /*!
       * \return Jacobian of implicit integration regarding velocity
       */
      virtual fmatvec::SqrMat getJacobianForImplicitIntegrationRegardingVelocity() const = 0;

      /*!
       * \return dimension of positions
       */
      virtual int getSizeOfPositions() const = 0;

      /*!
       * \return dimension of velocities
       */
      virtual int getSizeOfVelocities() const = 0;

      /*!
       * \brief compute equations of motion
       * \param generalised positions
       * \param generalised velocities
       */
      virtual void computeEquationsOfMotion(const fmatvec::Vec& q,const fmatvec::Vec& u) = 0;

      /*!
       * \brief compute kinetic energy
       * \param generalised positions
       * \param generalised velocities
       */
      virtual double computeKineticEnergy(const fmatvec::Vec& q,const fmatvec::Vec& u) = 0;

      /*! 
       * \brief compute gravitational energy
       * \param generalised positions
       */
      virtual double computeGravitationalEnergy(const fmatvec::Vec& q) = 0;    

      /*!
       * \brief compute elastic energy
       * \param generalised positions
       */
      virtual double computeElasticEnergy(const fmatvec::Vec& q) = 0;

      /*! 
       * \brief compute position of contour in physical representation
       * \param generalised positions
       * \param contour location
       */
      virtual fmatvec::Vec computeTranslation(const fmatvec::Vec& q, const ContourPointData &data) = 0;

      /*!
       * \brief compute orientation of contour in physical representation
       * \param generalised coordiantes
       * \param contour location
       */
      virtual fmatvec::SqrMat computeOrientation(const fmatvec::Vec& q, const ContourPointData &data) = 0;

      /*!
       * \brief compute translational velocity of contour in physical representation
       * \param generalised positions
       * \param generalised velocities
       * \param contour location
       */
      virtual fmatvec::Vec computeTranslationalVelocity(const fmatvec::Vec& q, const fmatvec::Vec& u, const ContourPointData &data) = 0;

      /*!
       * \brief compute angular velocity of contour in physical representation
       * \param generalised positions
       * \param generalised velocities
       * \param contour location
       */
      virtual fmatvec::Vec computeAngularVelocity(const fmatvec::Vec& q, const fmatvec::Vec& u, const ContourPointData &data) = 0;

      /*!
       * \brief compute Jacobian of minimal representation regarding physical representation
       * \param generalised positions
       * \param contour location
       */
      virtual fmatvec::Mat computeJacobianOfMinimalRepresentationRegardingPhysics(const fmatvec::Vec& q,const ContourPointData &data) = 0;
  };


}

#endif

