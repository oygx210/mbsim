/*
    MBSimGUI - A fronted for MBSim.
    Copyright (C) 2017 Martin Förg

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_

#include <xercesc/util/XercesDefs.hpp>
#include <mbxmlutilshelper/dom.h>

namespace MBSimGUI {

  const MBXMLUtils::NamespaceURI MBSIM("http://www.mbsim-env.de/MBSim");
  const MBXMLUtils::NamespaceURI OPENMBV("http://www.mbsim-env.de/OpenMBV");
  const MBXMLUtils::NamespaceURI MBSIMINT("http://www.mbsim-env.de/MBSimIntegrator");
  const MBXMLUtils::NamespaceURI MBSIMANALYSER("http://www.mbsim-env.de/MBSimAnalyser");
  const MBXMLUtils::NamespaceURI MBSIMCONTROL("http://www.mbsim-env.de/MBSimControl");
  const MBXMLUtils::NamespaceURI MBSIMPOWERTRAIN("http://www.mbsim-env.de/MBSimPowertrain");
  const MBXMLUtils::NamespaceURI MBSIMFLEX("http://www.mbsim-env.de/MBSimFlexibleBody");
  const MBXMLUtils::NamespaceURI MBSIMXML("http://www.mbsim-env.de/MBSimXML");

}

#endif