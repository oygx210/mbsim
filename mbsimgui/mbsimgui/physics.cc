/*
   MBSimGUI - A fronted for MBSim.
   Copyright (C) 2019 Martin Förg

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

#include <config.h>
#include "physics.h"
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMProcessingInstruction.hpp>

using namespace std;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSimGUI {

  DOMElement* UniversalGravitation::processIDAndHref(DOMElement *element) {
    element = Link::processIDAndHref(element);

    DOMElement *ELE=E(element)->getFirstElementChildNamed(MBSIMPHYSICS%"enableOpenMBV");
    if(ELE) {
      DOMDocument *doc=element->getOwnerDocument();
      DOMProcessingInstruction *id=doc->createProcessingInstruction(X()%"OPENMBV_ID", X()%getID().toStdString());
      ELE->insertBefore(id, nullptr);
    }

    return element;
  }

  DOMElement* Weight::processIDAndHref(DOMElement *element) {
    element = Link::processIDAndHref(element);

    DOMElement *ELE=E(element)->getFirstElementChildNamed(MBSIMPHYSICS%"enableOpenMBV");
    if(ELE) {
      DOMDocument *doc=element->getOwnerDocument();
      DOMProcessingInstruction *id=doc->createProcessingInstruction(X()%"OPENMBV_ID", X()%getID().toStdString());
      ELE->insertBefore(id, nullptr);
    }

    return element;
  }

  DOMElement* Buoyancy::processIDAndHref(DOMElement *element) {
    element = Link::processIDAndHref(element);

    DOMElement *ELE=E(element)->getFirstElementChildNamed(MBSIMPHYSICS%"enableOpenMBV");
    if(ELE) {
      DOMDocument *doc=element->getOwnerDocument();
      DOMProcessingInstruction *id=doc->createProcessingInstruction(X()%"OPENMBV_ID", X()%getID().toStdString());
      ELE->insertBefore(id, nullptr);
    }

    return element;
  }

  DOMElement* Drag::processIDAndHref(DOMElement *element) {
    element = Link::processIDAndHref(element);

    DOMElement *ELE=E(element)->getFirstElementChildNamed(MBSIMPHYSICS%"enableOpenMBV");
    if(ELE) {
      DOMDocument *doc=element->getOwnerDocument();
      DOMProcessingInstruction *id=doc->createProcessingInstruction(X()%"OPENMBV_ID", X()%getID().toStdString());
      ELE->insertBefore(id, nullptr);
    }

    return element;
  }

  DOMElement* Aerodynamics::processIDAndHref(DOMElement *element) {
    element = Link::processIDAndHref(element);

    DOMElement *ELE=E(element)->getFirstElementChildNamed(MBSIMPHYSICS%"enableOpenMBV");
    if(ELE) {
      DOMDocument *doc=element->getOwnerDocument();
      DOMProcessingInstruction *id=doc->createProcessingInstruction(X()%"OPENMBV_ID", X()%getID().toStdString());
      ELE->insertBefore(id, nullptr);
    }

    return element;
  }

}