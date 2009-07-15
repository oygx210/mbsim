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
 */

#include <config.h>
#include "integrator.h"

namespace MBSim {

  DynamicSystemSolver * Integrator::system = 0;

  Integrator::Integrator() : tStart(0.), tEnd(1.), dtPlot(1e-4), warnLevel(0), output(true), name("Integrator") {}

  void Integrator::initializeUsingXML(TiXmlElement *element) {
    TiXmlElement *e;
    e=element->FirstChildElement(MBSIMINTNS"startTime");
    setStartTime(atof(e->GetText()));
    e=element->FirstChildElement(MBSIMINTNS"endTime");
    setEndTime(atof(e->GetText()));
    e=element->FirstChildElement(MBSIMINTNS"plotStepSize");
    setPlotStepSize(atof(e->GetText()));
    e=element->FirstChildElement(MBSIMINTNS"initialState");
    if(e) setInitialState(fmatvec::Vec(e->GetText()));
  }

}

