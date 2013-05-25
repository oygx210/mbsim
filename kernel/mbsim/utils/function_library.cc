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
 * Contact: markus.ms.schneider@gmail.com
 */

#include "mbsim/utils/function_library.h"
#include "mbsim/objectfactory.h"

using namespace std;
using namespace MBXMLUtils;
using namespace fmatvec;

namespace MBSim {

  void Function1_SS_from_VS::initializeUsingXML(TiXmlElement * element) {
    Function1<double,double>::initializeUsingXML(element);
    TiXmlElement * e;
    e=element;
    Function1<fmatvec::Vec, double> * f=ObjectFactory<Function>::create<Function1<Vec,double> >(e->FirstChildElement());
    f->initializeUsingXML(e->FirstChildElement());
    setFunction(f);
  }


  Vec PositiveSinusFunction1_VS::operator()(const double& tVal, const void *) {
    Vec y=SinusFunction1_VS<fmatvec::Ref>::operator()(tVal);
    for (int i=0; i<ySize; i++)
      if (y(i)<0)
        y(i)=0;
    return y;
  }


  Vec StepFunction1_VS::operator()(const double& tVal, const void *) {
    Vec y(ySize, INIT, 0);
    for (int i=0; i<ySize; i++)
      if (tVal>=stepTime(i))
        y(i)=stepSize(i);
    return y;
  }


  void StepFunction1_VS::initializeUsingXML(TiXmlElement * element) {
    Function1<Vec,double>::initializeUsingXML(element);
    TiXmlElement *e=element->FirstChildElement(MBSIMNS"time");
    Vec stepTime_=Element::getVec(e);
    stepTime=stepTime_;
    e=element->FirstChildElement(MBSIMNS"size");
    Vec stepSize_=Element::getVec(e);
    stepSize=stepSize_;
    check();
  }

  void StepFunction1_VS::check() {
        ySize=stepTime.size();
        assert(stepSize.size()==ySize);
  }



  Vec PeriodicTabularFunction1_VS::operator()(const double& xVal, const void *) {
    double xValTmp=xVal;
    while (xValTmp<xMin)
      xValTmp+=xDelta;
    while (xValTmp>xMax)
      xValTmp-=xDelta;
    return TabularFunction1_VS<fmatvec::Ref,fmatvec::Ref>::operator()(xValTmp);
  }


  void SummationFunction1_VS::initializeUsingXML(TiXmlElement * element) {
    Function1<Vec,double>::initializeUsingXML(element);
    TiXmlElement * e;
    e=element->FirstChildElement(MBSIMNS"function");
    while (e && e->ValueStr()==MBSIMNS"function") {
      TiXmlElement * ee = e->FirstChildElement();
      Function1<Vec,double> *f=ObjectFactory<Function>::create<Function1<Vec,double> >(ee);
      f->initializeUsingXML(ee);
      ee=e->FirstChildElement(MBSIMNS"factor");
      double factor=Element::getDouble(ee);
      addFunction(f, factor);
      e=e->NextSiblingElement();
    }
  }


  TabularFunction2_SSS::TabularFunction2_SSS() : xVec(Vec(0)), yVec(Vec(0)), XY(Mat(0,0)), xSize(0), ySize(0), x0Index(0), x1Index(0), y0Index(0), y1Index(0), func_value(Vec(1,INIT,0)), xy(Vec(4,INIT,1)), XYval(Vec(4,INIT,0)), XYfac(Mat(4,4,INIT,0)) {
  }

  void TabularFunction2_SSS::calcIndex(const double * x, Vec X, int * xSize, int * xIndexMinus, int * xIndexPlus) {
    if (*x<=X(0)) {
      *xIndexPlus=1;
      *xIndexMinus=0;
      cerr << "TabularFunction2_SSS: Value (" << *x << ") is smaller than the smallest table value(" << X(0) << ")!" << endl;
    }
    else if (*x>=X(*xSize-1)) {
      *xIndexPlus=*xSize-1;
      *xIndexMinus=*xSize-2;
      cerr << "TabularFunction2_SSS: Value (" << *x << ") is greater than the greatest table value(" << X(*xSize-1) << ")!" << endl;
    }
    else {
      if (*x<X(*xIndexPlus))
        while(*x<X(*xIndexPlus-1)&&*xIndexPlus>1)
          (*xIndexPlus)--;
      else if (*x>X(*xIndexPlus))
        while(*x>X(*xIndexPlus)&&*xIndexPlus<*xSize-1)
          (*xIndexPlus)++;
      *xIndexMinus=*xIndexPlus-1;
    }
  }

  void TabularFunction2_SSS::setXValues(Vec xVec_) {
    xVec << xVec_;
    xSize=xVec.size();

    for (int i=1; i<xVec.size(); i++)
      if (xVec(i-1)>=xVec(i))
        throw MBSimError("xVec must be strictly monotonic increasing!");
    xSize=xVec.size();
  }

  void TabularFunction2_SSS::setYValues(Vec yVec_) {
    yVec << yVec_;
    ySize=yVec.size();

    for (int i=1; i<yVec.size(); i++)
      if (yVec(i-1)>=yVec(i))
        throw MBSimError("yVec must be strictly monotonic increasing!");
  }

  void TabularFunction2_SSS::setXYMat(Mat XY_) {
    XY << XY_;

    if(xSize==0)
      cout << "It is strongly recommended to set x file first! Continuing anyway." << endl;
    else if(ySize==0)
      cout << "It is strongly recommended to set y file first! Continuing anyway." << endl;
    else {
      if(XY.cols()!=xSize)
        throw MBSimError("Dimension missmatch in xSize");
      else if(XY.rows()!=ySize)
        throw MBSimError("Dimension missmatch in ySize");
    }
  }

  double TabularFunction2_SSS::operator()(const double& x, const double& y, const void * ) {
    calcIndex(&x, xVec, &xSize, &x0Index, &x1Index);
    calcIndex(&y, yVec, &ySize, &y0Index, &y1Index);

    xy(1)=x;
    xy(2)=y;
    xy(3)=x*y;
    const double x0=xVec(x0Index);
    const double x1=xVec(x1Index);
    const double y0=yVec(y0Index);
    const double y1=yVec(y1Index);
    const double nenner=(x0-x1)*(y0-y1);
    XYval(0)=XY(y0Index, x0Index);
    XYval(1)=XY(y0Index, x1Index);
    XYval(2)=XY(y1Index, x0Index);
    XYval(3)=XY(y1Index, x1Index);
    XYfac(0,0)=x1*y1;
    XYfac(0,1)=-x0*y1;
    XYfac(0,2)=-x1*y0;
    XYfac(0,3)=x0*y0;
    XYfac(1,0)=-y1;   
    XYfac(1,1)=y1;   
    XYfac(1,2)=y0;    
    XYfac(1,3)=-y0;
    XYfac(2,0)=-x1;   
    XYfac(2,1)=x0;    
    XYfac(2,2)=x1;    
    XYfac(2,3)=-x0;
    XYfac(3,0)=1.;    
    XYfac(3,1)=-1.;   
    XYfac(3,2)=-1.;  
    XYfac(3,3)=1.;

    return trans(1./nenner*XYfac*XYval)*xy;
  }

  void TabularFunction2_SSS::initializeUsingXML(TiXmlElement * element) {
    Function2<double,double,double>::initializeUsingXML(element);
    TiXmlElement * e;
    e = element->FirstChildElement(MBSIMNS"xValues");
    Vec x_=Element::getVec(e);
    setXValues(x_);
    e = element->FirstChildElement(MBSIMNS"yValues");
    Vec y_=Element::getVec(e);
    setYValues(y_);
    e = element->FirstChildElement(MBSIMNS"xyValues");
    Mat xy_=Element::getMat(e, y_.size(), x_.size());
    setXYMat(xy_);
  }


  void Polynom1_SS::setCoefficients(Vec a) {
    for (int i=0; i<a.size(); i++) {
      addDerivative(new Polynom1_SS::Polynom1_SSEvaluation(a.copy()));
      Vec b(a.size()-1, INIT, 0);
      for (int j=0; j<b.size(); j++)
        b(j)=a(j+1)*(j+1.);
      a.resize(a.size()-1);
      a=b.copy();
    }
    addDerivative(new Polynom1_SS::Polynom1_SSEvaluation(a.copy()));
  }

  void Polynom1_SS::initializeUsingXML(TiXmlElement * element) {
    Function1<double,double>::initializeUsingXML(element);
    MBSim::DifferentiableFunction1<double>::initializeUsingXML(element);
    TiXmlElement * e=element->FirstChildElement(MBSIMNS"coefficients");
    setCoefficients(MBSim::Element::getVec(e));
  }

  double Polynom1_SS::Polynom1_SSEvaluation::operator()(const double& tVal, const void *) {
    double value=a(a.size()-1);
    for (int i=a.size()-2; i>-1; i--)
      value=value*tVal+a(i);
    return value;
  }


}
