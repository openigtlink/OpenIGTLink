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
  Module:    $RCSfile: itkLightObject.cxx,v $
  Language:  C++
  Date:      $Date: 2009-11-12 23:48:21 -0500 (Thu, 12 Nov 2009) $
  Version:   $Revision: 5332 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igtlLightObject.h"
#include "igtlObjectFactory.h"
#include "igtlFastMutexLock.h"

#include <list>
#include <memory>
#include <exception>

// Better name demanging for gcc
#if __GNUC__ > 3 || ( __GNUC__ == 3 && __GNUC_MINOR__ > 0 )
#define GCC_USEDEMANGLE
#endif

#ifdef GCC_USEDEMANGLE
#include <cstdlib>
#include <cxxabi.h>
#endif 

namespace igtl
{

LightObject::Pointer
LightObject::New()
{
  Pointer smartPtr;
  LightObject *rawPtr = ::igtl::ObjectFactory<LightObject>::Create();
  if(rawPtr == NULL)
    {
    rawPtr = new LightObject;
    }
  smartPtr = rawPtr;
  rawPtr->UnRegister();
  return smartPtr;
}

LightObject::Pointer
LightObject::CreateAnother() const
{
  return LightObject::New();
}

/**
 * Delete a igtl object. This method should always be used to delete an object 
 * when the new operator was used to create it. Using the C++ delete method
 * will not work with reference counting.
 */
void 
LightObject
::Delete() 
{
  this->UnRegister();
}


/**
 * Avoid DLL boundary problems.
 */
#ifdef _WIN32
void*
LightObject
::operator new(size_t n)
{
  return new char[n];
}

void*
LightObject
::operator new[](size_t n)
{
  return new char[n];
}

void
LightObject
::operator delete(void* m)
{
  delete [] (char*)m;
}

void
LightObject
::operator delete[](void* m, size_t)
{
  delete [] (char*)m;
}
#endif 


/**
 * This function will be common to all igtl objects.  It just calls the
 * header/self/trailer virtual print methods, which can be overriden by
 * subclasses (any igtl object).
 */
void 
LightObject
::Print(std::ostream& os) const
{
  this->PrintHeader(os); 
  this->PrintSelf(os);
  this->PrintTrailer(os);
}


/**
 * This method is called when igtlExceptionMacro executes. It allows 
 * the debugger to break on error.
 */
void 
LightObject
::BreakOnError()
{
  ;  
}


/**
 * Increase the reference count (mark as used by another object).
 */
void 
LightObject
::Register() const
{
  m_ReferenceCountLock.Lock();
  m_ReferenceCount++;
  m_ReferenceCountLock.Unlock();
}


/**
 * Decrease the reference count (release by another object).
 */
void 
LightObject
::UnRegister() const
{
  m_ReferenceCountLock.Lock();
  int tmpReferenceCount = --m_ReferenceCount;
  m_ReferenceCountLock.Unlock();
  
  // ReferenceCount in now unlocked.  We may have a race condition
  // to delete the object.
  if ( tmpReferenceCount <= 0)
    {
    delete this;
    }
}


/**
 * Sets the reference count (use with care)
 */
void
LightObject
::SetReferenceCount(int ref)
{
  m_ReferenceCountLock.Lock();
  m_ReferenceCount = ref;
  m_ReferenceCountLock.Unlock();

  if ( ref <= 0)
    {
    delete this;
    }
}

LightObject
::~LightObject()
{
  /**
   * warn user if reference counting is on and the object is being referenced
   * by another object.
   * a call to uncaught_exception is necessary here to avoid throwing an
   * exception if one has been thrown already. This is likely to
   * happen when a subclass constructor (say B) is throwing an exception: at
   * that point, the stack unwinds by calling all superclass destructors back
   * to this method (~LightObject): since the ref count is still 1, an 
   * exception would be thrown again, causing the system to abort()!
   */
  if(m_ReferenceCount > 0 && !std::uncaught_exception())
    {
    // A general exception safety rule is that destructors should
    // never throw.  Something is wrong with a program that reaches
    // this point anyway.  Also this is the least-derived class so the
    // whole object has been destroyed by this point anyway.  Just
    // issue a warning.
    // igtlExceptionMacro(<< "Trying to delete object with non-zero reference count.");
    igtlWarningMacro("Trying to delete object with non-zero reference count.");
    }
}


/**
 * Chaining method to print an object's instance variables, as well as
 * its superclasses.
 */
void
LightObject
::PrintSelf(std::ostream& os) const
{
#ifdef GCC_USEDEMANGLE
  char const * mangledName = typeid(*this).name();
  int status;
  char* unmangled = abi::__cxa_demangle(mangledName, 0, 0, &status);

  const char* indent = "    ";

  os << indent << "RTTI typeinfo:   ";

  if(status == 0)
    {
    os << unmangled;
    free(unmangled);
    }
  else
    {
    os << mangledName;
    }

  os << std::endl;
#else
  char* indent = "    ";  
  os << indent << "RTTI typeinfo:   " << typeid( *this ).name() << std::endl;
#endif
  os << indent << "Reference Count: " << m_ReferenceCount << std::endl;
}


/**
 * Define a default print header for all objects.
 */
void
LightObject
::PrintHeader(std::ostream& os) const
{
  os << "    " << this->GetNameOfClass() << " (" << this << ")\n";
}


/**
 * Define a default print trailer for all objects.
 */
void
LightObject
::PrintTrailer(std::ostream& igtlNotUsed(os)) const
{
}


/**
 * This operator allows all subclasses of LightObject to be printed via <<.
 * It in turn invokes the Print method, which in turn will invoke the
 * PrintSelf method that all objects should define, if they have anything
 * interesting to print out.
 */
std::ostream&
operator<<(std::ostream& os, const LightObject& o)
{
  o.Print(os);
  return os;
}


} // end namespace igtl
