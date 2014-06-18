/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMacro.h,v $
  Language:  C++
  Date:      $Date: 2009-05-22 15:29:17 -0400 (Fri, 22 May 2009) $
  Version:   $Revision: 4248 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/**
 * igtlMacro.h defines standard system-wide macros, constants, and other
 * parameters. One of its most important functions is to define macros used
 * to interface to instance variables in a standard fashion. For example,
 * these macros manage modified time, debugging information, and provide a
 * standard interface to set and get instance variables.  Macros are
 * available for built-in types; for string classe; vector arrays;
 * object pointers; and debug, warning, and error printout information. 
 */
  
#ifndef __igtlMacro_h
#define __igtlMacro_h

#include "igtlWin32Header.h"
//#include "igtlConfigure.h"

#include <string>

// Determine type of string stream to use.
#if !defined(CMAKE_NO_ANSI_STRING_STREAM)
#  include <sstream>
#elif !defined(CMAKE_NO_ANSI_STREAM_HEADERS)
#  include <strstream>
#  define IGTL_NO_ANSI_STRING_STREAM
#else
#  include <strstream.h>
#  define IGTL_NO_ANSI_STRING_STREAM
#endif

/** \namespace igtl
 * \brief The "igtl" namespace contains all OpenIGTLink classes. There are several nested namespaces
 * within the igtl:: namespace. */
namespace igtl
{
} // end namespace igtl - this is here for documentation purposes

/** A convenience macro marks variables as not being used by a method,
 * avoiding compile-time warnings. */
#define igtlNotUsed(x)

/** Macro to initialize static constants.  This is used frequently to replace
 * the use of enum's within a class.  Some compilers do not allow an enum of
 * one class to be passed as template argument to another class. Other
 * uses of this macro as possible.
 *
 * This is based (verbatim) on the BOOST_STATIC_CONSTANT macro. The original
 * Boost documentation is below.
 *
 * BOOST_STATIC_CONSTANT workaround --------------------------------------- //
 * On compilers which don't allow in-class initialization of static integral
 * constant members, we must use enums as a workaround if we want the constants
 * to be available at compile-time. This macro gives us a convenient way to
 * declare such constants.
 */
#if defined(_MSC_VER) && (_MSC_VER <= 1300) 
#   define IGTL_NO_INCLASS_MEMBER_INITIALIZATION
#endif
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x540)
#   define IGTL_NO_INCLASS_MEMBER_INITIALIZATION
#endif
#if defined(__SVR4) && !defined(__SUNPRO_CC)
#   define IGTL_NO_INCLASS_MEMBER_INITIALIZATION
#endif

// A class template like this will not instantiate on GCC 2.95:
//   template<class T> struct A
//   {
//     static const int N = 1;
//     enum { S = sizeof(A::N) };
//   };
// We need to use enum for static constants instead.
#if defined(__GNUC__)
# define IGTL_NO_SIZEOF_CONSTANT_LOOKUP
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1300) 
#define IGTL_NO_SELF_AS_TRAIT_IN_TEMPLATE_ARGUMENTS
#endif

#if defined(IGTL_NO_INCLASS_MEMBER_INITIALIZATION) || \
    defined(IGTL_NO_SIZEOF_CONSTANT_LOOKUP)
#   define igtlStaticConstMacro(name,type,value) enum { name = value }
#else
#   define igtlStaticConstMacro(name,type,value) static const type name = value
#endif

#ifdef IGTL_NO_SELF_AS_TRAIT_IN_TEMPLATE_ARGUMENTS
#   define igtlGetStaticConstMacro(name) name
#else
#   define igtlGetStaticConstMacro(name) (Self::name)
#endif

/** Set an input. This defines the Set"name"Input() method */
#define igtlSetInputMacro(name, type, number) \
  virtual void Set##name##Input(const type *_arg) \
  { \
    igtlDebugMacro("setting input " #name " to " << _arg); \
    if (_arg != static_cast<type *>(this->ProcessObject::GetInput( number ))) \
      { \
      this->ProcessObject::SetNthInput( number, const_cast<type *>(_arg) ); \
      } \
  } \
  virtual void SetInput##number(const type *_arg) \
  { \
    igtlDebugMacro("setting input " #number " to " << _arg); \
    if (_arg != static_cast<type *>(this->ProcessObject::GetInput( number ))) \
      { \
      this->ProcessObject::SetNthInput( number, const_cast<type *>(_arg) ); \
      } \
  } 

/** Macro used to redefine a type from the superclass. */
#define igtlSuperclassTraitMacro(traitnameType) \
  typedef typename Superclass::traitnameType traitnameType;

/** Get an input. This defines the Get"name"Input() method */
#define igtlGetInputMacro(name, type, number) \
  virtual const type * Get##name##Input() const \
  { \
    igtlDebugMacro("returning input " << #name " of " << static_cast<const type *>(this->ProcessObject::GetInput( number )) ); \
    return static_cast<const type *>(this->ProcessObject::GetInput( number )); \
  } \
  virtual const type * GetInput##number() const \
  { \
    igtlDebugMacro("returning input " << #number " of " << static_cast<const type *>(this->ProcessObject::GetInput( number )) ); \
    return static_cast<const type *>(this->ProcessObject::GetInput( number )); \
  } 

/** Set a decorated input. This defines the Set"name"() method.
 * It invokes SetInputMacro() and GetInputMacro() for the decorated object */
#define igtlSetDecoratedInputMacro(name, type, number) \
  igtlSetInputMacro(name, SimpleDataObjectDecorator<type>, number); \
  igtlGetInputMacro(name, SimpleDataObjectDecorator<type>, number); \
  virtual void Set##name(const type &_arg) \
  { \
    typedef SimpleDataObjectDecorator< type > DecoratorType; \
    igtlDebugMacro("setting input " #name " to " << _arg); \
    const DecoratorType * oldInput = \
      static_cast< const DecoratorType * >( \
        this->ProcessObject::GetInput(number) ); \
    if( oldInput && oldInput->Get() == _arg ) \
      { \
      return; \
      } \
    typename DecoratorType::Pointer newInput = DecoratorType::New(); \
    newInput->Set( _arg ); \
    this->Set##name##Input( newInput ); \
  }

/** Set a decorated input that derives from igtl::Object, but not from
 * igtl::DataObject. This defines the Set"name"() method.  It invokes
 * SetInputMacro() and GetInputMacro() for the decorated object */
#define igtlSetDecoratedObjectInputMacro(name, type, number) \
  igtlSetInputMacro(name, DataObjectDecorator<type>, number); \
  igtlGetInputMacro(name, DataObjectDecorator<type>, number); \
  virtual void Set##name(const type *_arg) \
  { \
    typedef DataObjectDecorator< type > DecoratorType; \
    igtlDebugMacro("setting input " #name " to " << _arg); \
    const DecoratorType * oldInput = \
      static_cast< const DecoratorType * >( \
        this->ProcessObject::GetInput(number) ); \
    if( oldInput && oldInput->Get() == _arg ) \
      { \
      return; \
      } \
    typename DecoratorType::Pointer newInput = DecoratorType::New(); \
    newInput->Set( _arg ); \
    this->Set##name##Input( newInput ); \
  }


/** Set built-in type.  Creates member Set"name"() (e.g., SetVisibility()); */
#define igtlSetMacro(name,type) \
  virtual void Set##name (const type _arg) \
  { \
    igtlDebugMacro("setting " #name " to " << _arg); \
    if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      } \
  } 

/** Get built-in type.  Creates member Get"name"() (e.g., GetVisibility()); */
#define igtlGetMacro(name,type) \
  virtual type Get##name () \
  { \
    igtlDebugMacro("returning " << #name " of " << this->m_##name ); \
    return this->m_##name; \
  }

/** Get built-in type.  Creates member Get"name"() (e.g., GetVisibility());
 * This is the "const" form of the igtlGetMacro.  It should be used unless
 * the member can be changed through the "Get" access routine. */
#define igtlGetConstMacro(name,type) \
  virtual type Get##name () const \
  { \
    igtlDebugMacro("returning " << #name " of " << this->m_##name ); \
    return this->m_##name; \
  }

/** Get built-in type.  Creates member Get"name"() (e.g., GetVisibility());
 * This is the "const" form of the igtlGetMacro.  It should be used unless
 * the member can be changed through the "Get" access routine.
 * This versions returns a const reference to the variable. */
#define igtlGetConstReferenceMacro(name,type) \
  virtual const type & Get##name () const \
  { \
    igtlDebugMacro("returning " << #name " of " << this->m_##name ); \
    return this->m_##name; \
  }
  
  /** Set built-in type.  Creates member Set"name"() (e.g., SetVisibility());
  * This should be use when the type is an enum. It is use to avoid warnings on
  * some compilers with non specified enum types passed to igtlDebugMacro.*/
#define igtlSetEnumMacro(name,type) \
  virtual void Set##name (const type _arg) \
  { \
    igtlDebugMacro("setting " #name " to " << static_cast<long>(_arg)); \
    if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      } \
  } 

/** Get built-in type.  Creates member Get"name"() (e.g., GetVisibility()); 
  * This should be use when the type is an enum. It is use to avoid warnings on
  * some compilers with non specified enum types passed to igtlDebugMacro.*/
#define igtlGetEnumMacro(name,type) \
  virtual type Get##name () const \
  { \
    igtlDebugMacro("returning " << #name " of " << static_cast<long>(this->m_##name) ); \
    return this->m_##name; \
  }

/** Set character string.  Creates member Set"name"() 
 * (e.g., SetFilename(char *)). The macro assumes that
 * the class member (name) is declared a type std::string. */
#define igtlSetStringMacro(name) \
  virtual void Set##name (const char* _arg) \
  { \
    if ( _arg && (_arg == this->m_##name) ) { return;} \
    if (_arg) \
      { \
      this->m_##name = _arg;\
      } \
     else \
      { \
      this->m_##name = ""; \
      } \
  } \
  virtual void Set##name (const std::string & _arg) \
  { \
    this->Set##name( _arg.c_str() ); \
  } \


/** Get character string.  Creates member Get"name"() 
 * (e.g., SetFilename(char *)). The macro assumes that
 * the class member (name) is declared as a type std::string. */
#define igtlGetStringMacro(name) \
  virtual const char* Get##name () const \
  { \
    return this->m_##name.c_str(); \
  }

/** Set built-in type where value is constrained between min/max limits.
 * Create member Set"name"() (e.q., SetRadius()). #defines are 
 * convienience for clamping open-ended values. */
#define igtlSetClampMacro(name,type,min,max) \
  virtual void Set##name (type _arg) \
  { \
    igtlDebugMacro("setting " << #name " to " << _arg ); \
    if (this->m_##name != (_arg<min?min:(_arg>max?max:_arg))) \
      { \
      this->m_##name = (_arg<min?min:(_arg>max?max:_arg)); \
      } \
  } 

/** Set pointer to object; uses Object reference counting methodology.
 * Creates method Set"name"() (e.g., SetPoints()). Note that using
 * smart pointers requires using real pointers when setting input,
 * but returning smart pointers on output. */
#define igtlSetObjectMacro(name,type) \
  virtual void Set##name (type* _arg) \
  { \
    igtlDebugMacro("setting " << #name " to " << _arg ); \
    if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      } \
  } 

/** Get a smart pointer to an object.  Creates the member 
 * Get"name"() (e.g., GetPoints()). */
#define igtlGetObjectMacro(name,type) \
  virtual type * Get##name () \
  { \
    igtlDebugMacro("returning " #name " address " << this->m_##name ); \
    return this->m_##name.GetPointer(); \
  } 

/** Set const pointer to object; uses Object reference counting methodology.
 * Creates method Set"name"() (e.g., SetPoints()). Note that using
 * smart pointers requires using real pointers when setting input,
 * but returning smart pointers on output. */
#define igtlSetConstObjectMacro(name,type) \
  virtual void Set##name (const type* _arg) \
  { \
    igtlDebugMacro("setting " << #name " to " << _arg ); \
    if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      } \
  } 


/** Get a smart const pointer to an object.  Creates the member 
 * Get"name"() (e.g., GetPoints()). */
#define igtlGetConstObjectMacro(name,type) \
  virtual const type * Get##name () const \
  { \
    igtlDebugMacro("returning " #name " address " << this->m_##name ); \
    return this->m_##name.GetPointer(); \
  } 

/** Get a const reference to a smart pointer to an object.  
 * Creates the member Get"name"() (e.g., GetPoints()). */
#define igtlGetConstReferenceObjectMacro(name,type) \
  virtual const typename type::Pointer & Get##name () const \
  { \
    igtlDebugMacro("returning " #name " address " << this->m_##name ); \
    return this->m_##name; \
  } 

/** Create members "name"On() and "name"Off() (e.g., DebugOn() DebugOff()).
 * Set method must be defined to use this macro. */
#define igtlBooleanMacro(name) \
  virtual void name##On () { this->Set##name(true);} \
  virtual void name##Off () { this->Set##name(false);}

/** General set vector macro creates a single method that copies specified
 * number of values into object.
 * Examples: void SetColor(c,3) */
#define igtlSetVectorMacro(name,type,count) \
  virtual void Set##name(type data[]) \
  { \
    unsigned int i; \
    for (i=0; i<count; i++) { if ( data[i] != this->m_##name[i] ) { break; }} \
    if ( i < count ) \
      { \
      for (i=0; i<count; i++) { this->m_##name[i] = data[i]; }\
      } \
  }

/** Get vector macro. Returns pointer to type (i.e., array of type).
 * This is for efficiency. */
#define igtlGetVectorMacro(name,type,count) \
  virtual type *Get##name () const \
  { \
    return this->m_##name; \
  } 

/** Define two object creation methods.  The first method, New(),
 * creates an object from a class, potentially deferring to a factory.
 * The second method, CreateAnother(), creates an object from an
 * instance, potentially deferring to a factory.  This second method
 * allows you to create an instance of an object that is exactly the
 * same type as the referring object.  This is useful in cases where
 * an object has been cast back to a base class.
 *
 * These creation methods first try asking the object factory to create
 * an instance, and then default to the standard "new" operator if the
 * factory fails.
 *
 * These routines assigns the raw pointer to a smart pointer and then call
 * UnRegister() on the rawPtr to compensate for LightObject's constructor
 * initializing an object's reference count to 1 (needed for proper
 * initialization of process objects and data objects cycles). */
#define igtlNewMacro(x) \
static Pointer New(void) \
{ \
  Pointer smartPtr = ::igtl::ObjectFactory<x>::Create(); \
  if(smartPtr.GetPointer() == NULL) \
    { \
    smartPtr = new x; \
    } \
  smartPtr->UnRegister(); \
  return smartPtr; \
} \
virtual ::igtl::LightObject::Pointer CreateAnother(void) const \
{ \
  ::igtl::LightObject::Pointer smartPtr; \
  smartPtr = x::New().GetPointer(); \
  return smartPtr; \
}


/** Define two object creation methods.  The first method, New(),
 * creates an object from a class but does not defer to a factory.
 * The second method, CreateAnother(), creates an object from an
 * instance, again without deferring to a factory.  This second method
 * allows you to create an instance of an object that is exactly the
 * same type as the referring object.  This is useful in cases where
 * an object has been cast back to a base class.
 *
 * These creation methods first try asking the object factory to create
 * an instance, and then default to the standard "new" operator if the
 * factory fails.
 *
 * These routines assigns the raw pointer to a smart pointer and then call
 * UnRegister() on the rawPtr to compensate for LightObject's constructor
 * initializing an object's reference count to 1 (needed for proper
 * initialization of process objects and data objects cycles). */
#define igtlFactorylessNewMacro(x) \
static Pointer New(void) \
{ \
  Pointer smartPtr; \
  x *rawPtr = new x; \
  smartPtr = rawPtr; \
  rawPtr->UnRegister(); \
  return smartPtr; \
} \
  virtual ::igtl::LightObject::Pointer CreateAnother(void) const \
{ \
  ::igtl::LightObject::Pointer smartPtr;         \
  smartPtr = x::New().GetPointer(); \
  return smartPtr; \
}

/** Macro used to add standard methods to all classes, mainly type
 * information. */
#define igtlTypeMacro(thisClass,superclass) \
    virtual const char *GetNameOfClass() const \
        {return #thisClass;} 


//namespace igtl
//{
///**
// * The following is used to output debug, warning, and error messages.
// * Use a global function which actually calls:
// * OutputWindow::GetInstance()->DisplayText();
// * This is to avoid Object #include of OutputWindow
// * while OutputWindow #includes Object. */
//extern IGTLCommon_EXPORT void OutputWindowDisplayText(const char*);
//extern IGTLCommon_EXPORT void OutputWindowDisplayErrorText(const char*);
//extern IGTLCommon_EXPORT void OutputWindowDisplayWarningText(const char*);
//extern IGTLCommon_EXPORT void OutputWindowDisplayGenericOutputText(const char*);
//extern IGTLCommon_EXPORT void OutputWindowDisplayDebugText(const char*);
//} // end namespace igtl

/** This macro is used to print debug (or other information). They are
 * also used to catch errors, etc. Example usage looks like:
 * igtlDebugMacro(<< "this is debug info" << this->SomeVariable); */
#if defined(IGTL_LEAN_AND_MEAN) || defined(__BORLANDC__)
#define igtlDebugMacro(x)
#else
#define igtlDebugMacro(x) \
  { if (this->GetDebug() /*&& ::igtl::Object::GetGlobalWarningDisplay()*/)   \
    { ::igtl::OStringStream igtlmsg; \
      igtlmsg << "Debug: In " __FILE__ ", line " << __LINE__ << "\n" \
             << this->GetNameOfClass() << " (" << this << "): " x  \
             << "\n\n"; \
      std::cerr << igtlmsg.str(); /*::igtl::OutputWindowDisplayDebugText(igtlmsg.str().c_str());*/} \
}
#endif


/** This macro is used to print warning information (i.e., unusual circumstance
 * but not necessarily fatal.) Example usage looks like:
 * igtlWarningMacro(<< "this is warning info" << this->SomeVariable); */
#ifdef IGTL_LEAN_AND_MEAN
#define igtlWarningMacro(x)
#else
#define igtlWarningMacro(x) \
  { if ( 1/*::igtl::Object::GetGlobalWarningDisplay()*/)  \
    { ::igtl::OStringStream igtlmsg; \
      igtlmsg << "WARNING: In " __FILE__ ", line " << __LINE__ << "\n" \
             << this->GetNameOfClass() << " (" << this << "): " x  \
             << "\n\n"; \
      std::cerr << igtlmsg.str(); /*::igtl::OutputWindowDisplayWarningText(igtlmsg.str().c_str());*/} \
}
#endif

namespace igtl
{

/**
 * igtl::OStringStream wrapper to hide differences between
 * std::ostringstream and the old ostrstream.  Necessary for
 * portability.
 */
#if !defined(IGTL_NO_ANSI_STRING_STREAM)
class OStringStream: public std::ostringstream
{
public:
  OStringStream() {}
private:
  OStringStream(const OStringStream&);
  void operator=(const OStringStream&);
};
#else
namespace OStringStreamDetail
{
  class Cleanup
  {
  public:
    Cleanup(std::ostrstream& ostr): m_OStrStream(ostr) {}
    ~Cleanup() { m_OStrStream.rdbuf()->freeze(0); }
    static void IgnoreUnusedVariable(const Cleanup&) {}
  protected:
    std::ostrstream& m_OStrStream;
  };
}//namespace OStringStreamDetail

class OStringStream: public std::ostrstream
{
public:
  typedef std::ostrstream Superclass;
  OStringStream() {}
  std::string str()
    {
      OStringStreamDetail::Cleanup cleanup(*this);
      OStringStreamDetail::Cleanup::IgnoreUnusedVariable(cleanup);
      int pcount = this->pcount();
      const char* ptr = this->Superclass::str();
      return std::string(ptr?ptr:"", pcount);
    }
private:
  OStringStream(const OStringStream&);
  void operator=(const OStringStream&);
};
#endif

}//namespace igtl

#if defined(IGTL_CPP_FUNCTION)
  #if defined(__BORLANDC__)
    #define IGTL_LOCATION __FUNC__
  #elif defined(_WIN32) && !defined(__MINGW32__) && !defined(__CYGWIN__) && !defined(CABLE_CONFIGURATION) && !defined(CSWIG)
    #define IGTL_LOCATION __FUNCSIG__
  #elif defined(__GNUC__)
    #define IGTL_LOCATION __PRETTY_FUNCTION__
  #else
    #define IGTL_LOCATION __FUNCTION__
  #endif
#else
  #define IGTL_LOCATION "unknown"
#endif

#define igtlExceptionMacro(x) \
  { \
  ::igtl::OStringStream igtlmsg;                                 \
  igtlmsg << "Debug: In " __FILE__ ", line " << __LINE__ << "\n" \
          << this->GetNameOfClass() << " (" << this << "): " x   \
          << "\n\n";                                             \
  std::cerr << igtlmsg.str(); /*::igtl::OutputWindowDisplayDebugText(igtlmsg.str().c_str());*/ \
}

#define igtlErrorMacro(x) \
  { \
  ::igtl::OStringStream igtlmsg;                                 \
  igtlmsg << "Debug: In " __FILE__ ", line " << __LINE__ << "\n" \
          << this->GetNameOfClass() << " (" << this << "): " x   \
          << "\n\n";                                             \
  std::cerr << igtlmsg.str(); /*::igtl::OutputWindowDisplayDebugText(igtlmsg.str().c_str());*/ \
}


#ifdef IGTL_LEAN_AND_MEAN
#define igtlGenericOutputMacro(x)
#else
#define igtlGenericOutputMacro(x) \
  { if (1/*::igtl::Object::GetGlobalWarningDisplay()*/) \
    { ::igtl::OStringStream igtlmsg; \
      igtlmsg << "WARNING: In " __FILE__ ", line " << __LINE__ << "\n" \
             x << "\n\n"; \
      std::cerr << igtlmsg.str();/*::igtl::OutputWindowDisplayGenericOutputText(igtlmsg.str().c_str());*/} \
}
#endif



//----------------------------------------------------------------------------
// Macros for simplifying the use of logging
//
#define igtlLogMacro( x, y)  \
{         \
  if (this->GetLogger() ) \
    {  \
    this->GetLogger()->Write(::igtl::LoggerBase::x, y); \
    }  \
}


#define igtlLogMacroStatic( obj, x, y)  \
{         \
  if (obj->GetLogger() ) \
    {  \
    obj->GetLogger()->Write(::igtl::LoggerBase::x, y); \
    }  \
}


//----------------------------------------------------------------------------
// Setup legacy code policy.
//
// CMake options IGTL_LEGACY_REMOVE and IGTL_LEGACY_SILENT are converted
// to definitions (or non-defs) in igtlConfigure.h and tested below.
// They may be used to completely remove legacy code or silence the
// warnings.  The default is to warn about their use.
//
// Source files that test the legacy code may define IGTL_LEGACY_TEST
// like this:
//
//  #define IGTL_LEGACY_TEST
//  #include "igtlClassWithDeprecatedMethod.h"
//
// in order to silence the warnings for calling deprecated methods.
// No other source files in IGTL should call the methods since they are
// provided only for compatibility with older user code.

// Define igtlLegacyMacro to mark legacy methods where they are
// declared in their class.  Example usage:
//
//   // @deprecated Replaced by MyOtherMethod() as of IGTL 2.0.
//   igtlLegacyMacro(void MyMethod());
#if defined(IGTL_LEGACY_REMOVE)
// Remove legacy methods completely.  Put a bogus declaration in
// place to avoid stray semicolons because this is an error for some
// compilers.  Using a class forward declaration allows any number
// of repeats in any context without generating unique names.
# define igtlLegacyMacro(method) class igtlLegacyMethodRemoved /* no ';' */
#elif defined(IGTL_LEGACY_SILENT) || defined(IGTL_LEGACY_TEST) || defined(CSWIG)
  // Provide legacy methods with no warnings.
# define igtlLegacyMacro(method) method
#else
  // Setup compile-time warnings for uses of deprecated methods if
  // possible on this compiler.
# if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#  define igtlLegacyMacro(method) method __attribute__((deprecated))
# elif defined(_MSC_VER) && _MSC_VER >= 1300
#  define igtlLegacyMacro(method) __declspec(deprecated) method
# else
#  define igtlLegacyMacro(method) method
# endif
#endif

// Macros to create runtime deprecation warning messages in function
// bodies.  Example usage:
//
//   void igtlMyClass::MyOldMethod()
//     {
//     igtlLegacyBodyMacro(igtlMyClass::MyOldMethod, 2.0);
//     }
//
//   void igtlMyClass::MyMethod()
//     {
//     igtlLegacyReplaceBodyMacro(igtlMyClass::MyMethod, 2.0,
//                               igtlMyClass::MyOtherMethod);
//     }
#if defined(IGTL_LEGACY_REMOVE) || defined(IGTL_LEGACY_SILENT)
# define igtlLegacyBodyMacro(method, version)
# define igtlLegacyReplaceBodyMacro(method, version, replace)
#else
# define igtlLegacyBodyMacro(method, version) \
  igtlWarningMacro(#method " was deprecated for IGTL " #version " and will be removed in a future version.")
# define igtlLegacyReplaceBodyMacro(method, version, replace) \
  igtlWarningMacro(#method " was deprecated for IGTL " #version " and will be removed in a future version.  Use " #replace " instead.")
#endif

#if defined(__INTEL_COMPILER)
# pragma warning (disable: 193) /* #if testing undefined identifier */
#endif

//=============================================================================
/* Choose a way to prevent template instantiation on this platform.
  - IGTL_TEMPLATE_DO_NOT_INSTANTIATE = use #pragma do_not_instantiate to
                                      prevent instantiation
  - IGTL_TEMPLATE_EXTERN = use extern template to prevent instantiation

   Note that VS 6 supports extern template instantiation but it is
   hard to block the resulting warning because its stream headers
   re-enable it.  Therefore we just disable support for now.
*/
#if defined(__sgi) && defined(_COMPILER_VERSION)
# define IGTL_TEMPLATE_DO_NOT_INSTANTIATE 1
#elif defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 700
# define IGTL_TEMPLATE_EXTERN 1
#elif defined(__GNUC__) && __GNUC__ >= 3
# define IGTL_TEMPLATE_EXTERN 1
#elif defined(_MSC_VER) && _MSC_VER >= 1300
# define IGTL_TEMPLATE_EXTERN 1
#endif
#if !defined(IGTL_TEMPLATE_DO_NOT_INSTANTIATE)
# define IGTL_TEMPLATE_DO_NOT_INSTANTIATE 0
#endif
#if !defined(IGTL_TEMPLATE_EXTERN)
# define IGTL_TEMPLATE_EXTERN 0
#endif

/* Define a macro to explicitly instantiate a template.
  - IGTL_TEMPLATE_EXPORT(X) =
      Explicitly instantiate X, where X is of the form N(a1[,a2...,aN]).
      examples: IGTL_TEMPLATE_EXPORT(1(class Foo<int>))
                IGTL_TEMPLATE_EXPORT(2(class Bar<int, char>))
      Use one level of expansion delay to allow user code to have
      a macro determining the number of arguments. */
#define IGTL_TEMPLATE_EXPORT(x) IGTL_TEMPLATE_EXPORT_DELAY(x)
#define IGTL_TEMPLATE_EXPORT_DELAY(x) template IGTL_TEMPLATE_##x;

/* Define a macro to prevent template instantiations.
  - IGTL_TEMPLATE_IMPORT(X) =
      Prevent instantiation of X, where X is of the form N(a1[,a2...,aN]).
      examples: IGTL_TEMPLATE_IMPORT(1(class Foo<int>))
                IGTL_TEMPLATE_IMPORT(2(class Bar<int, char>))
      Use one level of expansion delay to allow user code to have
      a macro determining the number of arguments.
*/
#if IGTL_TEMPLATE_EXTERN
# define IGTL_TEMPLATE_IMPORT_DELAY(x) extern template IGTL_TEMPLATE_##x;
# if defined(_MSC_VER)
#  pragma warning (disable: 4231) /* extern template extension */
# endif
#elif IGTL_TEMPLATE_DO_NOT_INSTANTIATE
# define IGTL_TEMPLATE_IMPORT_DELAY(x) \
         IGTL_TEMPLATE_IMPORT_IMPL(do_not_instantiate IGTL_TEMPLATE_##x)
# define IGTL_TEMPLATE_IMPORT_IMPL(x) _Pragma(#x)
#endif
#if defined(IGTL_TEMPLATE_IMPORT_DELAY)
# define IGTL_TEMPLATE_IMPORT(x) IGTL_TEMPLATE_IMPORT_DELAY(x)
# define IGTL_TEMPLATE_IMPORT_WORKS 1
#else
# define IGTL_TEMPLATE_IMPORT(x)
# define IGTL_TEMPLATE_IMPORT_WORKS 0
#endif

/* Define macros to export and import template instantiations.  These
   depend on each class providing a macro defining the instantiations
   given template arguments in X.  The argument X is of the form
   N(a1[,a2...,aN]).  The argument Y is a valid preprocessing token
   unique to the template arguments given in X.  Typical usage is

     IGTL_EXPORT_TEMPLATE(igtlfoo_EXPORT, Foo, (int), I)
     IGTL_EXPORT_TEMPLATE(igtlfoo_EXPORT, Bar, (int, char), IC)

   The IGTL_TEMPLATE_<name> macro should be defined in igtl<name>.h and
   is of the following form:

     #define IGTL_TEMPLATE_<name>(_, EXPORT, x, y) namespace igtl { \
       _(<n>(class EXPORT <name>< IGTL_TEMPLATE_<n> x >)) \
       namespace Templates { typedef <name>< IGTL_TEMPLATE_<n> x > <name>##y; }\
     }

   The argument "_" will be replaced by another macro such as
   IGTL_TEMPLATE_EXPORT or IGTL_TEMPLATE_IMPORT, so it should be used as
   if calling one of these macros.  The argument "EXPORT" will be
   replaced by a dllexport/dllimport macro such as IGTLCommon_EXPORT.
   The argument "x" is a paren-enclosed list of template arguments.
   The argument "y" is a preprocessing token corresponding to the
   given template arguments and should be used to construct typedef
   names for the instantiations.

   Note the use of IGTL_TEMPLATE_<n>, where <n> is the number of
   template arguments for the class template.  Note also that the
   number of template arguments is usually the length of the list
   nested within the inner parentheses, so the instantiation is listed
   with the form <n>(...).  Example definitions:

     #define IGTL_TEMPLATE_Foo(_, EXPORT, x, y) namespace igtl { \
       _(1(class EXPORT Foo< IGTL_TEMPLATE_1 x >)) \
       _(1(EXPORT std::ostream& operator<<(std::ostream&, \
                                           const Foo< IGTL_TEMPLATE_1 x >&))) \
       namespace Templates { typedef Foo< IGTL_TEMPLATE_1 x > Foo##y; }\
     }

     #define IGTL_TEMPLATE_Bar(_, EXPORT, x, y) namespace igtl { \
       _(2(class EXPORT Bar< IGTL_TEMPLATE_2 x >)) \
       _(1(EXPORT std::ostream& operator<<(std::ostream&, \
                                           const Bar< IGTL_TEMPLATE_2 x >&))) \
       namespace Templates { typedef Bar< IGTL_TEMPLATE_2 x > Bar##y; }\
     }

   Note that in the stream operator for template Bar there is a "1" at
   the beginning even though two arguments are taken.  This is because
   the expression "IGTL_TEMPLATE_2 x" is contained inside the
   parentheses of the function signature which protects the resulting
   comma from separating macro arguments.  Therefore the nested
   parentheses contain a list of only one macro argument.

   The IGTL_EMPTY macro used in these definitions is a hack to work
   around a VS 6.0 preprocessor bug when EXPORT is empty.
*/
#define IGTL_EXPORT_TEMPLATE(EXPORT, c, x, y) \
        IGTL_TEMPLATE_##c(IGTL_TEMPLATE_EXPORT, EXPORT IGTL_EMPTY, x, y)
#define IGTL_IMPORT_TEMPLATE(EXPORT, c, x, y) \
        IGTL_TEMPLATE_##c(IGTL_TEMPLATE_IMPORT, EXPORT IGTL_EMPTY, x, y)
#define IGTL_EMPTY

/* Define macros to support passing a variable number of arguments
   throug other macros.  This is used by IGTL_TEMPLATE_EXPORT,
   IGTL_TEMPLATE_IMPORT, and by each template's instantiation
   macro.  */
#define IGTL_TEMPLATE_1(x1)                         x1
#define IGTL_TEMPLATE_2(x1,x2)                      x1,x2
#define IGTL_TEMPLATE_3(x1,x2,x3)                   x1,x2,x3
#define IGTL_TEMPLATE_4(x1,x2,x3,x4)                x1,x2,x3,x4
#define IGTL_TEMPLATE_5(x1,x2,x3,x4,x5)             x1,x2,x3,x4,x5
#define IGTL_TEMPLATE_6(x1,x2,x3,x4,x5,x6)          x1,x2,x3,x4,x5,x6
#define IGTL_TEMPLATE_7(x1,x2,x3,x4,x5,x6,x7)       x1,x2,x3,x4,x5,x6,x7
#define IGTL_TEMPLATE_8(x1,x2,x3,x4,x5,x6,x7,x8)    x1,x2,x3,x4,x5,x6,x7,x8
#define IGTL_TEMPLATE_9(x1,x2,x3,x4,x5,x6,x7,x8,x9) x1,x2,x3,x4,x5,x6,x7,x8,x9

/* In order to support both implicit and explicit instantation a .h
   file needs to know whether it should include its .txx file
   containing the template definitions.  Define a macro to tell
   it.  Typical usage in igtlFoo.h:
     #if IGTL_TEMPLATE_TXX
     # include "igtlFoo.txx"
     #endif
*/
#if defined(IGTL_MANUAL_INSTANTIATION)
# define IGTL_TEMPLATE_TXX 0
#else
# define IGTL_TEMPLATE_TXX !(IGTL_TEMPLATE_CXX || IGTL_TEMPLATE_TYPE)
#endif

/* All explicit instantiation source files define IGTL_TEMPLATE_CXX.
   Define IGTL_MANUAL_INSTANTIATION to tell .h files that have not been
   converted to this explicit instantiation scheme to not include
   their .txx files.  Also disable warnings that commonly occur in
   these files but are not useful.  */
#if IGTL_TEMPLATE_CXX
# undef IGTL_MANUAL_INSTANTIATION
# define IGTL_MANUAL_INSTANTIATION
# if defined(_MSC_VER)
#  pragma warning (disable: 4275) /* non dll-interface base */
#  pragma warning (disable: 4661) /* no definition available */
# endif
#endif
//=============================================================================

/* Define macros to export and import template instantiations for each
   library in IGTL.  */
#define IGTL_EXPORT_IGTLCommon(c, x, n) \
        IGTL_EXPORT_TEMPLATE(IGTLCommon_EXPORT, c, x, n)
#define IGTL_IMPORT_IGTLCommon(c, x, n) \
        IGTL_IMPORT_TEMPLATE(IGTLCommon_EXPORT, c, x, n)

/* Define a macro to decide whether to block instantiation of IGTL
   templates.  They should be blocked only if the platform supports
   blocking template instantiation and the explicit instantiations are
   available.

   - IGTL_TEMPLATE_EXPLICIT =
      Whether to include "XXX+-.h" from "XXX.h" to prevent implicit
      instantiations of templates explicitly instantiated elsewhere.
      Typical usage in igtlFoo.h:
        #if IGTL_TEMPLATE_EXPLICIT
        # include "igtlFoo+-.h"
        #endif
*/
#if IGTL_TEMPLATE_IMPORT_WORKS && defined(IGTL_EXPLICIT_INSTANTIATION)
# define IGTL_TEMPLATE_EXPLICIT !IGTL_TEMPLATE_CXX
#else
# define IGTL_TEMPLATE_EXPLICIT 0
#endif


//----------------------------------------------------------------------------
// Macro to declare that a function does not return. __attribute__((noreturn))
//    On some compiler, functions that do not return (ex: exit(0)) must
//    have the noreturn attribute. Otherwise, a warning is raised. Use
//    that macro to avoid those warnings. GCC defines the attribute
//    noreturn for versions 2.5 and higher.
#if defined(__GNUC__)
#  if (((__GNUC__ == 2) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ >= 3))
#    define IGTL_NO_RETURN \
       __attribute__ ((noreturn))
#  endif
#else
#  define IGTL_NO_RETURN
#endif


#ifdef IGTL_USE_TEMPLATE_META_PROGRAMMING_LOOP_UNROLLING
//--------------------------------------------------------------------------------
//  Helper macros for Template Meta-Programming techniques of for-loops unrolling
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// Macro that generates an unrolled for loop for assigning elements of one array
// to elements of another array The array are assumed to be of same length
// (dimension), and this is also assumed to be the value of NumberOfIterations.
// No verification of size is performed. Casting is perfomed as part of the
// assignment, by using the DestinationElementType as the casting type. 
// Source and destination array types must have defined opearator[] in their API.
#define igtlFoorLoopAssignmentMacro(DestinationType,SourceType,DestinationElementType,DestinationArray,SourceArray,NumberOfIterations) \
    for(unsigned int i=0;i < NumberOfIterations; ++i) \
      { \
      DestinationArray[i] = static_cast< DestinationElementType >( SourceArray[i] ); \
      }

//--------------------------------------------------------------------------------
// Macro that generates an unrolled for loop for rounding and assigning
// elements of one array to elements of another array The array are assumed to
// be of same length (dimension), and this is also assumed to be the value of
// NumberOfIterations.  No verification of size is performed. Casting is
// perfomed as part of the assignment, by using the DestinationElementType as
// the casting type. 
// Source and destination array types must have defined opearator[] in their API.
#define igtlFoorLoopRoundingAndAssignmentMacro(DestinationType,SourceType,DestinationElementType,DestinationArray,SourceArray,NumberOfIterations) \
    for(unsigned int i=0;i < NumberOfIterations; ++i) \
      { \
      DestinationArray[i] = static_cast< DestinationElementType >( vnl_math_rnd( SourceArray[i] ) ); \
      }

#endif
// end of Template Meta Programming helper macros


#endif //end of igtlMacro.h
