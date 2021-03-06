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

#ifndef _GROUP_H_
#define _GROUP_H_

#include <mbsim/dynamic_system.h>

namespace MBSim {

  /**
   * \brief group ingredients do not depend on each other
   * \author Martin Foerg
   * \date 2009-03-26 some comments (Thorsten Schindler)
   * \date 2009-06-14 OpenMP (Thorsten Schindler)
   * \date 2009-07-08 relative dynamic system location (Thorsten Schindler)
   * \todo OpenMP only static scheduling with intelligent reordering of vectors by dynamic test runs
   */
  class Group : public DynamicSystem {
    public:
      /**
       * \brief constructor
       * \param name of group
       */
      Group(const std::string &name="") : DynamicSystem(name) { }

      /**
       * \brief destructor
       */
      ~Group() override = default;

      using DynamicSystem::addObject;

      void initializeUsingXML(xercesc::DOMElement *element) override;

    protected:
      fmatvec::Vec RrRD;
      fmatvec::SqrMat ARD;
  };
}

#endif
