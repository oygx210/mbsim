#ifndef NURBS_SURFACE_FMATVEC_H_
#define NURBS_SURFACE_FMATVEC_H_

#include <mbsim/numerics/nurbs/nurbs_defs.h>
#include "nurbs_curve.h"

namespace fmatvec {
  typedef Matrix<General, Fixed<4>, Var, double> Mat4xV;

  typedef Matrix<General, Var, Fixed<4>, double> MatVx4;
}

namespace MBSim {

//  enum Direction { u_direction=1, v_direction=2, both_direction=3} ;
//
//  template <class T, int N> class NurbsSurfaceArray ;
//
//  template <class T, int N> void gordonSurface(NurbsCurveArray<T,N>& , NurbsCurveArray<T,N>& , const Matrix< Point_nD<T,N>>& , NurbsSurface<T,N>& );
  int surfMeshParams(const fmatvec::GeneralMatrix<fmatvec::Vec3>& Q, fmatvec::VecV& uk, fmatvec::VecV& vl);
  int surfMeshParamsH(const fmatvec::GeneralMatrix<fmatvec::Vec4>& Qw, fmatvec::VecV& uk, fmatvec::VecV& vl);
//  template <class T, int N> int surfMeshParamsClosedU(const Matrix< Point_nD<T,N>>& Qw, Vector<T>& uk, Vector<T>& vl, int degU );
  int surfMeshParamsClosedU(const fmatvec::GeneralMatrix<fmatvec::Vec3>& Q, fmatvec::VecV& uk, fmatvec::VecV& vl, int degU);
  int surfMeshParamsClosedUH(const fmatvec::GeneralMatrix<fmatvec::Vec4>& Qw, fmatvec::VecV& uk, fmatvec::VecV& vl, int degU);
//
//  template <class T, int N> void globalSurfInterpXY(const Matrix< Point_nD<T,N>>& , int , int , NurbsSurface<T,N>& );
//  template <class T, int N> void globalSurfInterpXY(const Matrix< Point_nD<T,N>>& , int , int , NurbsSurface<T,N>& , const Vector<T>& , const Vector<T>& );
//  template <class T, int N> void globalSurfApprox(const Matrix< Point_nD<T,N>>& , int , int , NurbsSurface<T,N>& , double=0);
//  template <class T, int N> void wrapPointMatrix(const Matrix< Point_nD<T,N>>& Q, int , int, Matrix< Point_nD<T,N>>& Qw);

  /*!
   \class NurbsSurface nurbsS.h
   \brief A class to represent a NURBS surface

   The NURBS surface is composed of points in homogenous space. It can have
   any degree in both the \a u and the \a v direction.
   */
//template <class T, int N>
  class NurbsSurface {
    public:
      enum Method {
        equallySpaced = 0,
        chordLength,
      };

      /**
       * \brief default constructor
       */
      NurbsSurface() : U(1), V(1), P(1, 1) { }
      /**
       * \brief copy constructor
       */
      NurbsSurface(const NurbsSurface& nS) : U(nS.U), V(nS.V), P(nS.P), degU(nS.degU), degV(nS.degV) { }
      /**
       * \brief constructor
       */
      NurbsSurface(int DegU, int DegV, const fmatvec::VecV& Uk, const fmatvec::VecV& Vk, fmatvec::GeneralMatrix<fmatvec::Vec4>& Q) : U(Uk), V(Vk), P(Q), degU(DegU), degV(DegV) { }
//  NurbsSurface(int DegU, int DegV, Vector<T>& Uk, Vector<T>& Vk, Matrix< Point_nD<T,N>>& Cp, Matrix<T>& W) ;
      //! Empty desctructor
      virtual ~NurbsSurface() = default;

      // Reference to internal data
      //! A reference to the U knot vector
      const fmatvec::VecV& knotU() const { return U; }
      //! A reference to the V knot vector
      const fmatvec::VecV& knotV() const { return V; }
      //! Returns the i-th knot from U
      double knotU(int i) const { return U(i); }
      //! Returns the i-th knot from V
      double knotV(int i) const { return V(i); }
      //! A reference to the control points
      const fmatvec::GeneralMatrix<fmatvec::Vec4>& ctrlPnts() const { return P; }
      //! A reference to the control point at (i,j)
      const fmatvec::Vec4 ctrlPnts(int i, int j) const { return P(i, j); }
      //! A reference to the degree in U of the surface
      int degreeU() const { return degU; }
      //! A reference to the degree in V of the surface
      int degreeV() const { return degV; }

      void setKnotU(const fmatvec::VecV& U_) { U = U_; }
      void setKnotV(const fmatvec::VecV& V_) { V = V_; }
      void setCtrlPnts(const fmatvec::GeneralMatrix<fmatvec::Vec4>& P_) { P = P_; }
      void setDegreeU(int degU_) { degU = degU_; }
      void setDegreeV(int degV_) { degV = degV_; }

      // Basic operators
      virtual NurbsSurface& operator=(const NurbsSurface&);
      void resize(int Pu, int Pv, int DegU, int DegV);
//  virtual void resizeKeep(int Pu, int Pv, int DegU, int DegV) ;
//  int ok();

      // Basis functions
      fmatvec::HPoint<3> operator()(double u, double v) const;
      //! rojects the point in the normal space
      fmatvec::Point<3> pointAt(double u, double v) const;
//  void basisFuns(T u, T v, int spanU, int spanV, Vector<T>& Nu, Vector<T>& Nv) const ;
//  void basisFunsU(T u, int span, Vector<T>& M) const ;
//  void basisFunsV(T u, int span, Vector<T>& M) const ;
//  void dersBasisFuns(T u, T v, int dU, int dV,int uspan, int vspan,Matrix<T> & Niku, Matrix<T>& Njkv ) const ;

      // Derivative functions
      void deriveAt(double u, double v, int d, fmatvec::GeneralMatrix<fmatvec::Vec3> &skl) const;
      void deriveAtH(double u, double v, int d, fmatvec::GeneralMatrix<fmatvec::Vec4> &skl) const;
      fmatvec::Vec3 normal(double u, double v) const;

      // Surface fitting functions
      void globalInterp(const fmatvec::GeneralMatrix<fmatvec::Vec3>& Q, int DegU, int DegV);
      void globalInterp(const fmatvec::GeneralMatrix<fmatvec::Vec3>& Q, const fmatvec::VecV& uk, const fmatvec::VecV& vk, int DegU, int DegV);
      void globalInterpClosedU(const fmatvec::GeneralMatrix<fmatvec::Vec3>& Q, int DegU, int DegV);
      void globalInterpClosedU(const fmatvec::GeneralMatrix<fmatvec::Vec3>& Q, const fmatvec::VecV& uk, const fmatvec::VecV& vk, int DegU, int DegV);

      void globalInterpH(const fmatvec::GeneralMatrix<fmatvec::Vec4>& Q, int DegU, int DegV, Method method=chordLength);
      void globalInterpClosedUH(const fmatvec::GeneralMatrix<fmatvec::Vec4>& Q, int DegU, int DegV, Method method=chordLength);
//  void globalInterpClosedUH(const Matrix< HPoint_nD<T,N>>& Q,const Vector<T> &Uk, const Vector<T> &Vk, const Vector<T> &uk, const Vector<T> &vk, int pU, int pV);//testing
//  void leastSquares(const Matrix< Point_nD<T,N>>& Q, int pU, int pV, int nU, int nV) ;
//  void leastSquaresClosedU(const Matrix< Point_nD<T,N>>& Q, int pU, int pV, int nU, int nV) ;
//
//#ifndef HAVE_ISO_FRIEND_DECL
//  friend void gordonSurface (NurbsCurveArray<T,N>& lU, NurbsCurveArray<T,N>& lV, const Matrix< Point_nD<T,N>>& intersections, NurbsSurface<T,N>& gS);
//  friend void globalSurfInterpXY (const Matrix< Point_nD<T,N>>& Q, int pU, int pV, NurbsSurface<T,N>& S);
//  friend void globalSurfInterpXY (const Matrix< Point_nD<T,N>>& Q, int pU, int pV, NurbsSurface<T,N>& S, const Vector<T>& uk, const Vector<T>& vk);
//  friend void globalSurfApprox (const Matrix< Point_nD<T,N>>& Q, int pU, int pV, NurbsSurface<T,N>& S, double error);
//#else
//  friend void gordonSurface <> (NurbsCurveArray<T,N>& lU, NurbsCurveArray<T,N>& lV, const Matrix< Point_nD<T,N>>& intersections, NurbsSurface<T,N>& gS);
//  friend void globalSurfInterpXY <> (const Matrix< Point_nD<T,N>>& Q, int pU, int pV, NurbsSurface<T,N>& S);
//  friend void globalSurfInterpXY <> (const Matrix< Point_nD<T,N>>& Q, int pU, int pV, NurbsSurface<T,N>& S, const Vector<T>& uk, const Vector<T>& vk);
//  friend void globalSurfApprox <> (const Matrix< Point_nD<T,N>>& Q, int pU, int pV, NurbsSurface<T,N>& S, double error);
//#endif
//
//  // Surface generation function
//  int skinV(NurbsCurveArray<T,N>& ca, int degV);
//  int skinU(NurbsCurveArray<T,N>& ca, int degU);
//  void sweep(const NurbsCurve<T,N>& t, const NurbsCurve<T,N>& C, const NurbsCurve<T,N>& Sv, int K,int useAy=0, int invAz=0) ;
//  void sweep(const NurbsCurve<T,N>& t, const NurbsCurve<T,N>& C, int K,int useAy=0, int invAz=0) ;
//  void makeFromRevolution(const NurbsCurve<T,N>& profile, const Point_nD<T,N>& S, const Point_nD<T,N>& Z, double theta) ;
//  void makeFromRevolution(const NurbsCurve<T,N>& profile, const Point_nD<T,N>& S, const Point_nD<T,N>& Z) ;
//  void makeFromRevolution(const NurbsCurve<T,N>& profile) ;
//
//  void makeSphere(const Point_nD<T,N>& O, T r) ;
//  void makeTorus(const Point_nD<T,N>& O, T R, T r);
//
//
//  void degreeElevate(int tU, int tV) ;
//  virtual void degreeElevateU(int tU) ;
//  virtual void degreeElevateV(int tV) ;
//
//
//  int decompose(NurbsSurfaceArray<T,N>& Sa) const ;
//
//
//
//  virtual void refineKnots(const Vector<T>& nU, const Vector<T>& nV) ;
//  virtual void refineKnotU(const Vector<T>& X);
//  virtual void refineKnotV(const Vector<T>& X);
//
//  virtual void mergeKnots(const Vector<T>& nU, const Vector<T>& nV) ;
//  virtual void mergeKnotU(const Vector<T>& X);
//  virtual void mergeKnotV(const Vector<T>& X);
//
//  // Measuring functions
//  T area(T eps=0.001,int n=100) const ;
//  T areaIn(T us, T ue, T vs, T ve, T eps, int n) const ;
//  T areaF(T u, T v) const ;
//
//  // special functions
//  void isoCurveU(T u, NurbsCurve<T,N>& c) const ;
//  void isoCurveV(T v, NurbsCurve<T,N>& c) const ;
//
//  // I/O functions
//  int read(const char* filename);
//  int write(const char* filename) const;
//  virtual int read(ifstream &fin) ;
//  int write(ofstream &fout) const ;
//  int writeVRML(const char* filename,const Color& color,int Nu,int Nv, T u_s, T u_e, T v_s, T v_e) const { return ParaSurface<T,N>::writeVRML(filename,color,Nu,Nv,u_s,u_e,v_s,v_e);}
//  int writeVRML(ostream &fout,const Color& color,int Nu,int Nv, T u_s, T u_e, T v_s, T v_e) const { return ParaSurface<T,N>::writeVRML(fout,color,Nu,Nv,u_s,u_e,v_s,v_e);}
//  int writeVRML97(const char* filename,const Color& color,int Nu,int Nv, T u_s, T u_e, T v_s, T v_e) const { return ParaSurface<T,N>::writeVRML97(filename,color,Nu,Nv,u_s,u_e,v_s,v_e);}
//  int writeVRML97(ostream &fout,const Color& color,int Nu,int Nv, T u_s, T u_e, T v_s, T v_e) const { return ParaSurface<T,N>::writeVRML97(fout,color,Nu,Nv,u_s,u_e,v_s,v_e);}
//  ostream& print(ostream& os) const ;
//
//  //! Calls the ParaSurface routine with proper values
//  int writeVRML(const char* filename,const Color& color=whiteColor,int Nu=20,int Nv=20) const
//    { return ParaSurface<T,N>::writeVRML(filename,color,Nu,Nv,U[0],U[U.n()-1],V[0],V[V.n()-1]); }
//  //! Calls the ParaSurface routine with proper values
//  int writeVRML(ostream& fout,const Color& color=whiteColor,int Nu=20,int Nv=20) const
//    { return ParaSurface<T,N>::writeVRML(fout,color,Nu,Nv,U[0],U[U.n()-1],V[0],V[V.n()-1]); }
//
//  //! Calls the ParaSurface routine with proper values
//  int writeVRML97(const char* filename,const Color& color=whiteColor,int Nu=20,int Nv=20) const
//    { return ParaSurface<T,N>::writeVRML97(filename,color,Nu,Nv,U[0],U[U.n()-1],V[0],V[V.n()-1]); }
//  //! Calls the ParaSurface routine with proper values
//  int writeVRML97(ostream& fout,const Color& color=whiteColor,int Nu=20,int Nv=20) const
//    { return ParaSurface<T,N>::writeVRML97(fout,color,Nu,Nv,U[0],U[U.n()-1],V[0],V[V.n()-1]); }
//
//  int writePOVRAY(ostream& povray, int patch_type=1, double flatness=0.01, int num_u_steps=8, int num_v_steps=8) const ;
//  int writePOVRAY(T, ostream& povray, const Color& color=Color(250,250,250),int smooth=0 , T ambient=0.2, T diffuse=0.6) const ;
//  int writePOVRAY(const char *filename, const Color& color, const Point_nD<T,N>& view, const Point_nD<T,N>& up, int patch_type=1, double flatness=0.01, int num_u_steps=8, int num_v_steps=8) const ;
//  int writePOVRAY(T tolerance, const char *filename, const Color& color, const Point_nD<T,N>& view, const Point_nD<T,N>& up, int smooth=0, T ambient=0.2, T diffuse=0.6) const ;
//
//  int writeRIB(ostream& rib) const ;
//  int writeRIB(char* filename, const Color& color, const Point_nD<T,N>& view) const ;
//
//  // tesselate is deprecated...
//  void tesselate(T tolerance, BasicList<Point_nD<T,N>> &points, BasicList<int> &connect, BasicList<Point_nD<T,N>> *normal=0) const ;
//
//  int writePS(const char*, int nu, int nv, const Point_nD<T,N>& camera, const Point_nD<T,N>& lookAt, int cp=0,T magFact=T(-1),T dash=T(5)) const ;
//  int writePSp(const char*, int nu, int nv, const Point_nD<T,N>& camera, const Point_nD<T,N>& lookAt, const Vector< Point_nD<T,N>>&,const Vector< Point_nD<T,N>>&, int cp=0,T magFact=0.0,T dash=5.0) const ;
//  int writeOOGL(const char* filename, T fDu, T fDv,T fBu=0.0, T fBv=0.0, T fEu=1.0, T fEv=1.0, bool bDRawCP=false) const ;
//  int writeOOGL(const char* filename) const ;
//
//  int writeDisplayQUADMESH(const char* filename, int iNu=100,int iNv=100,const Color& color=blueColor,T fA=.25, T  fO=0.2) const;
//
//
//  // Modifies the shape of the surface
//  void transform(const MatrixRT<T>& A) ;
//  //! Modifies a control point
//  void modCP(int i, int j, const HPoint_nD<T,N>& p)
//    { P(i,j) = p ; }
//  //! Modifies a control point
//  void modCPby(int i, int j, const HPoint_nD<T,N>& p)
//    { P(i,j) += p ; }
//
//  T& modU(int i) { return U[i] ; }
//  //! modifies a knot
//  T modU(int i) const { return U[i]; }
//  //! modifies a knot
//  T& modV(int i) { return V[i] ; }
//  //! modifies a knot
//  T modV(int i) const { return V[i]; }
//
//  //! modifies the U knot vector if uKnot is of a proper size
//  void modKnotU(const Vector<T>& uKnot) { if(P.rows()+degU+1==uKnot.n()) U=uKnot ; }
//  //! modifies the U knot vector if uKnot is of a proper size
//  void modKnotV(const Vector<T>& vKnot) { if(P.cols()+degV+1==vKnot.n()) V=vKnot ; }
//
//  int movePoint(T u, T v, const Point_nD<T,N>& delta);
//  int movePoint(const Vector<T>& ur, const Vector<T>& vr, const Vector< Point_nD<T,N>>& D, const Vector_INT& Du, const Vector_INT& Dv) ;
//  int movePoint(const Vector<T>& ur, const Vector<T>& vr, const Vector< Point_nD<T,N>>& D, const Vector_INT& Du, const Vector_INT& Dv, const Vector_INT& Dk, const Vector_INT& Dl) ;
//  int movePoint(const Vector<T>& ur, const Vector<T>& vr, const Vector< Point_nD<T,N>>& D, const Vector_INT& Du, const Vector_INT& Dv, const Vector_INT& Dk, const Vector_INT& Dl, const BasicArray<Coordinate>& fixCP) ;
//
//  NurbsSurface<T,N>& transpose(void) ;

    protected:
      fmatvec::VecV U; //!< the U knot vector
      fmatvec::VecV V; //!< the V knot vector
      fmatvec::GeneralMatrix<fmatvec::Vec4> P; //!< The matrix of control points
      int degU{0}; //!< the degree of the surface in U
      int degV{0}; //!< the degree of the surface in V

      //changed
      fmatvec::Mat InverseU; //!< The matrix for the computation of the control points in U-direction
      fmatvec::Mat InverseV; //!< The matrix for the computation of the control points in V-direction

      // Knot functions
      void findSpan(double u, double v, int spanU, int spanV) const;
      int findSpanU(double u) const;
      int findSpanV(double v) const;

      int findMultU(int r) const;
      int findMultV(int r) const;

  };

} // end namespace

///*!
// */
//namespace PLib {
//
///*!
//  \class  NurbsSurfaceArray nurbsS.h
//  \brief An array of NurbsSurface
//
//  This class represents an array of NurbsSurface.
//
//  \author Philippe Lavoie
//  \date 4 Oct. 1996
//*/
//template <class T, int N>
//class NurbsSurfaceArray {
//public:
//  //! a reference to the size of the array
//  int n() const
//    { return sze ; }
//  NurbsSurfaceArray(NurbsSurface<T,N>* Sa, int size) ;
//  NurbsSurfaceArray() { S = 0 ; sze = 0 ; rsize = 0 ;} //<! Default constructor
//  virtual ~NurbsSurfaceArray(){ if(S){ for(int i=0;i<rsize;i++) delete S[i];  delete []S ; }}
//
//  //! the ith surface
//  virtual NurbsSurface<T,N>& operator[](int i)
//    { return *(S[i]) ; }
//  //! the ith surface
//  virtual NurbsSurface<T,N> operator[](int i) const
//    { return *(S[i]) ; }
//
//  virtual void resize(int s) ;
//  void init(NurbsSurface<T,N>* Sa, int size) ;
//
//  NurbsSurfaceArray<T,N>& operator=(const NurbsSurfaceArray<T,N>& Sa) ;
//
//protected:
//  int sze ; //!< the number of NURBS curves in the array
//  int rsize ; //!< the number of space allocated for the array
//  NurbsSurface<T,N>** S ; //!< An array of pointers to NURBS curves
//};
//
//}//end namespace
//
//// In case someone wants the old behavior for degree elevation
//template <class T, int N>
//inline void degreeElevate(const PLib::NurbsSurface<T,N>& S, int tU, int tV, PLib::NurbsSurface<T,N>& nS) { nS = S ; nS.degreeElevate(tU,tV) ; }
//template <class T, int N>
//inline void degreeElevateU(const PLib::NurbsSurface<T,N>& S, int tU, PLib::NurbsSurface<T,N>& nS) { nS = S ; nS.degreeElevateU(tU) ; }
//template <class T, int N>
//inline void degreeElevateV(const PLib::NurbsSurface<T,N>& S, int tV, PLib::NurbsSurface<T,N>& nS) { nS = S ; nS.degreeElevateV(tV) ; }
//
//
//#ifdef INCLUDE_TEMPLATE_SOURCE
//#include "nurbsS.cpp"
//#endif

#endif 
