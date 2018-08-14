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
  Module:    $RCSfile: itkCreateObjectFunction.h,v $
  Language:  C++
  Date:      $Date: 2008-12-22 19:05:42 -0500 (Mon, 22 Dec 2008) $
  Version:   $Revision: 3460 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igtlCreateObjectFunction_h
#define __igtlCreateObjectFunction_h

#include "igtlObject.h"

namespace igtl
{

/** \class CreateObjectFunctionBase
 * \brief Define API for object creation callback functions.
 *
 * \ingroup IGTLSystemObjects
 */
class CreateObjectFunctionBase: public Object
{
public:
  igtlTypeMacro(CreateObjectFunctionBase, Object);
  /** Create an object and return a pointer to it as an
   * igtl::LightObject. */
  virtual SmartPointer<LightObject> CreateObject() = 0;

protected:
  CreateObjectFunctionBase() {}
  ~CreateObjectFunctionBase() {}
  
private:
  CreateObjectFunctionBase(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented  
};


/** \class CreateObjectFunction
 * \brief CreateObjectFunction is used to create callback functions that
 * create IGTL Objects for use with the igtl::ObjectFactory.
 * 
 * \ingroup IGTLSystemObjects
 */
template <class T>
class CreateObjectFunction : public CreateObjectFunctionBase
{
public:

  igtlTypeMacro(CreateObjectFunction, CreateObjectFunctionBase);

  /** Methods from igtl:LightObject. */
  igtlFactorylessNewMacro(Self);
  LightObject::Pointer CreateObject() { return T::New().GetPointer(); }

protected:
  CreateObjectFunction() {}
  ~CreateObjectFunction() {}
  
private:
  CreateObjectFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace igtl

#endif
