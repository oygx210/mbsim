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

#ifndef _SIGNAL_MANIPULATION_H_
#define _SIGNAL_MANIPULATION_H_

#include "mbsimControl/signal_.h"
#include <mbsim/functions/function.h>

namespace MBSim {
  typedef int Index;
}

namespace MBSimControl {

  /*!
   * \brief Multiplexer
   * \author Markus Schneider
   */
  class Multiplexer : public Signal {
    public:
      Multiplexer(const std::string &name="") : Signal(name) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void init(InitStage stage);
      void addInputSignal(Signal * signal_) { signal.push_back(signal_); }
      void updateSignal();
      int getSignalSize() const;
    private:
      std::vector<Signal*> signal;
      std::vector<std::string> signalString;
  };

  /*!
   * \brief Demultiplexer
   * \author Markus Schneider
   */
  class Demultiplexer : public Signal {
    public:
      Demultiplexer(const std::string &name="") : Signal(name), signal(NULL) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void init(InitStage stage);
      void setInputSignal(Signal *signal_) { signal = signal_; }
      void setIndices(const std::vector<MBSim::Index> &index_) { index = index_; }
      void updateSignal();
      int getSignalSize() const { return index.size(); }
    private:
      Signal *signal;
      std::vector<MBSim::Index> index;
      std::string signalString;
  };

  /*!
   * \brief SignalTimeDiscretization
   * \author Markus Schneider
   */
  class SignalTimeDiscretization : public Signal {  
    public:
      SignalTimeDiscretization(const std::string &name="") : Signal(name), s(NULL), tOld(-99e99) { }
      void initializeUsingXML(xercesc::DOMElement *element);
      void init(InitStage stage);
      void setInputSignal(Signal *signal_) { s=signal_; }
      void updateSignal();
      int getSignalSize() const { return s->getSignalSize(); }
    private:
      Signal *s;
      double tOld;
      std::string signalString;
  };

  /*!
   * \brief PID controller
   * \author Martin Foerg
   */
  class PIDController : public Signal {

    public:   
      PIDController(const std::string& name="") : Signal(name), s(NULL), sd(NULL) {}
      void initializeUsingXML(xercesc::DOMElement * element);
      
      void calcxSize() {xSize=updateSignalMethod==&PIDController::updateSignalPD?0:1;}
      
      void init(InitStage stage);

      void updatedx();
      void updatexd();
      
      void setPID(double P_, double I_, double D_);
      void setInputSignal(Signal *inputSignal_) {s=inputSignal_; }
      void setDerivativeOfInputSignal(Signal *inputSignal_) {sd=inputSignal_; }

      void updateSignal();
      int getSignalSize() const { return s->getSignalSize(); }

    protected:
      double P,I,D;
      Signal *s, *sd;
      std::string sString, sdString;
      void (PIDController::*updateSignalMethod)();
      void updateSignalPD();
      void updateSignalPID();
  };

  /*!
   * \brief UnarySignalOperation
   * \author Martin Foerg
   */
  class UnarySignalOperation : public Signal {  
    public:
      UnarySignalOperation(const std::string &name="") : Signal(name), s(NULL), signalString(""), f(0) { }
      ~UnarySignalOperation() { delete f; }
      void initializeUsingXML(xercesc::DOMElement *element);
      void init(InitStage stage);
      void setInputSignal(Signal *signal_) {s=signal_; }
      void setFunction(MBSim::Function<fmatvec::VecV(fmatvec::VecV)> *f_) {
        f=f_;
        f->setParent(this);
        f->setName("Function");
      };
      void updateSignal();
      int getSignalSize() const { return s->getSignalSize(); }
    private:
      Signal *s;
      std::string signalString;
      MBSim::Function<fmatvec::VecV(fmatvec::VecV)> *f;
  };

  /*!
   * \brief BinarySignalOperation
   * \author Martin Foerg
   */
  class BinarySignalOperation : public Signal {  
    public:
      BinarySignalOperation(const std::string &name="") : Signal(name), s1(NULL), s2(NULL), signal1String(""), signal2String(""), f(0) { }
      ~BinarySignalOperation() { delete f; }
      void initializeUsingXML(xercesc::DOMElement *element);
      void init(InitStage stage);
      void setFirstInputSignal(Signal *signal_) {s1=signal_; }
      void setSecondInputSignal(Signal *signal_) {s2=signal_; }
      void setFunction(MBSim::Function<fmatvec::VecV(fmatvec::VecV,fmatvec::VecV)> *f_) {
        f=f_;
        f->setParent(this);
        f->setName("Function");
      };
      void updateSignal();
      int getSignalSize() const { return s1->getSignalSize(); }
    private:
      Signal *s1, *s2;
      std::string signal1String, signal2String;
      MBSim::Function<fmatvec::VecV(fmatvec::VecV,fmatvec::VecV)> *f;
  };

}

#endif /* _SIGNAL_MANIPULATION_H_ */

