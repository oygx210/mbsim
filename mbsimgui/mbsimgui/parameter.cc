/*
    MBSimGUI - A fronted for MBSim.
    Copyright (C) 2012 Martin Förg

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
#include "parameter.h"
#include "objectfactory.h"
#include "utils.h"
#include "fileitemdata.h"
#include "mainwindow.h"

using namespace std;
using namespace MBXMLUtils;
using namespace xercesc;

namespace MBSimGUI {

  extern MainWindow *mw;

  void ParameterItem::removeXMLElements() {
    DOMNode *e = element->getFirstChild();
    while(e) {
      DOMNode *en=e->getNextSibling();
      element->removeChild(e);
      e = en;
    }
  }

  vector<Parameter*> ParameterItem::createParameters(DOMElement *element) {
    vector<Parameter*> param;
    DOMElement *e=element->getFirstElementChild();
    while(e) {
      Parameter *parameter=ObjectFactory::getInstance()->createParameter(e);
      parameter->setXMLElement(e);
      param.push_back(parameter);
      e=e->getNextElementSibling();
    }
    return param;
  }

  DOMElement* Parameter::createXMLElement(DOMNode *parent) {
    xercesc::DOMDocument *doc=parent->getOwnerDocument();
    element=D(doc)->createElement(getXMLType());
    E(element)->setAttribute("name", getXMLType().second);
    parent->insertBefore(element, nullptr);
    return element;
  }

  StringParameter::StringParameter() {
    icon = Utils::QIconCached(QString::fromStdString((mw->getInstallPath()/"share"/"mbsimgui"/"icons"/"string.svg").string()));
  }

  ScalarParameter::ScalarParameter() {
    icon = Utils::QIconCached(QString::fromStdString((mw->getInstallPath()/"share"/"mbsimgui"/"icons"/"scalar.svg").string()));
  }

  VectorParameter::VectorParameter() {
    icon = Utils::QIconCached(QString::fromStdString((mw->getInstallPath()/"share"/"mbsimgui"/"icons"/"vector.svg").string()));
  }

  QString VectorParameter::getValue() const {
    DOMElement *ele=element->getFirstElementChild();
    if(ele and E(ele)->getTagName() == PV%"xmlVector")
      return "xmlVector";
    else
      return Parameter::getValue();
  }

  MatrixParameter::MatrixParameter() {
    icon = Utils::QIconCached(QString::fromStdString((mw->getInstallPath()/"share"/"mbsimgui"/"icons"/"matrix.svg").string()));
  }

  QString MatrixParameter::getValue() const {
    DOMElement *ele=element->getFirstElementChild();
    if(ele and E(ele)->getTagName() == PV%"xmlMatrix")
      return "xmlMatrix";
    else
      return Parameter::getValue();
  }

  DOMElement* ImportParameter::createXMLElement(DOMNode *parent) {
    xercesc::DOMDocument *doc=parent->getOwnerDocument();
    element=D(doc)->createElement(getXMLType());
    parent->insertBefore(element, nullptr);
    return element;
  }

  Parameters::Parameters(EmbedItemData *parent) : ParameterItem(parent) {
    icon = QIcon(new OverlayIconEngine((mw->getInstallPath()/"share"/"mbsimgui"/"icons"/"container.svg").string(),
                                       (mw->getInstallPath()/"share"/"mbsimgui"/"icons"/"matrix.svg").string()));
  }

  QString Parameters::getReference() const {
    return parent->getDedicatedParameterFileItem()?parent->getDedicatedParameterFileItem()->getName():"";
  }

}
