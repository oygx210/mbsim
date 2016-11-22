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

#ifndef _OPENMBV_UTILS_H_
#define _OPENMBV_UTILS_H_

#include <fmatvec/fmatvec.h>
#include <openmbvcppinterface/arrow.h>
#include <openmbvcppinterface/frame.h>
#include <openmbvcppinterface/sphere.h>
#include <openmbvcppinterface/cuboid.h>
#include <openmbvcppinterface/frustum.h>
#include <openmbvcppinterface/extrusion.h>
#include <openmbvcppinterface/coilspring.h>
#include <openmbvcppinterface/indexedfaceset.h>

namespace MBSim {

  class OpenMBVBody {
    protected:
      std::string id;
    public:
      OpenMBVBody() { }
      void initializeObject(const std::shared_ptr<OpenMBV::Body> &object);
      void initializeUsingXML(xercesc::DOMElement *element);
  };

  class OpenMBVDynamicColoredBody : public OpenMBVBody {
    protected:
      fmatvec::Vec3 dc;
      double tp;
    public:
      OpenMBVDynamicColoredBody(const fmatvec::Vec3 &dc_="[-1;1;1]", double tp_=0) : dc(dc_), tp(tp_) { }
      void initializeObject(const std::shared_ptr<OpenMBV::DynamicColoredBody> &object);
      void initializeUsingXML(xercesc::DOMElement *element);
  };

  class OpenMBVArrow : public OpenMBVDynamicColoredBody {
    protected:
      OpenMBV::Arrow::Type type;
      OpenMBV::Arrow::ReferencePoint refPoint;
      double sL, sS;
    public:
      OpenMBVArrow(const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0, const OpenMBV::Arrow::Type &type_=OpenMBV::Arrow::toHead, const OpenMBV::Arrow::ReferencePoint &refPoint_=OpenMBV::Arrow::fromPoint, double sL_=1, double sS_=1) : OpenMBVDynamicColoredBody(dc,tp), type(type_), refPoint(refPoint_), sL(sL_), sS(sS_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Arrow> &object);
      std::shared_ptr<OpenMBV::Arrow> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVFrame : public OpenMBVDynamicColoredBody {
    protected:
      double size, offset;
    public:
      OpenMBVFrame(double size_=1, double offset_=1, const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), size(size_), offset(offset_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Frame> &object);
      std::shared_ptr<OpenMBV::Frame> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVSphere : public OpenMBVDynamicColoredBody {
    protected:
      double r;
      std::string xml;
    public:
      OpenMBVSphere(double r_=1, const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0, std::string xml_="radius") : OpenMBVDynamicColoredBody(dc,tp), r(r_), xml(xml_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Sphere> &object);
      std::shared_ptr<OpenMBV::Sphere> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVLine : public OpenMBVDynamicColoredBody {
    protected:
      double l;
    public:
      OpenMBVLine(double l_=1, const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), l(l_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Cuboid> &object);
      std::shared_ptr<OpenMBV::Cuboid> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVPlane : public OpenMBVDynamicColoredBody {
    protected:
      fmatvec::Vec2 l;
    public:
      OpenMBVPlane(const fmatvec::Vec2 &l_=fmatvec::Vec2(fmatvec::INIT,1.), const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), l(l_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Cuboid> &object);
      std::shared_ptr<OpenMBV::Cuboid> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVCuboid : public OpenMBVDynamicColoredBody {
    protected:
      fmatvec::Vec3 l;
    public:
      OpenMBVCuboid(const fmatvec::Vec3 &l_=fmatvec::Vec3(fmatvec::INIT,1.), const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), l(l_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Cuboid> &object);
      std::shared_ptr<OpenMBV::Cuboid> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVCircle : public OpenMBVDynamicColoredBody {
    protected:
      double r;
    public:
      OpenMBVCircle(double r_=1, const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), r(r_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Frustum> &object);
      std::shared_ptr<OpenMBV::Frustum> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVFrustum : public OpenMBVDynamicColoredBody {
    protected:
      double t, b, h;
    public:
      OpenMBVFrustum(double t_=1, double b_=1, double h_=1, const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), t(t_), b(b_), h(h_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Frustum> &object);
      std::shared_ptr<OpenMBV::Frustum> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVExtrusion : public OpenMBVDynamicColoredBody {
    protected:
      double h;
    public:
      OpenMBVExtrusion(double h_=1, const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), h(h_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::Extrusion> &object);
      std::shared_ptr<OpenMBV::Extrusion> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVCoilSpring : public OpenMBVDynamicColoredBody {
    protected:
      double r, cr, sf, n, l;
      OpenMBV::CoilSpring::Type type;
    public:
      OpenMBVCoilSpring(double r_=1, double cr_=-1, double sf_=1, double n_=3, double l_=-1, OpenMBV::CoilSpring::Type type_=OpenMBV::CoilSpring::tube, const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp), r(r_), cr(cr_), sf(sf_), n(n_), l(l_), type(type_) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void initializeObject(const std::shared_ptr<OpenMBV::CoilSpring> &object);
      std::shared_ptr<OpenMBV::CoilSpring> createOpenMBV(xercesc::DOMElement* e=0);
  };

  class OpenMBVIndexedFaceSet : public OpenMBVDynamicColoredBody {
    public:
      OpenMBVIndexedFaceSet(const fmatvec::Vec3 &dc="[-1;1;1]", double tp=0) : OpenMBVDynamicColoredBody(dc,tp) { }
      std::shared_ptr<OpenMBV::IndexedFaceSet> createOpenMBV(xercesc::DOMElement* e=0);
  };

}


#endif
