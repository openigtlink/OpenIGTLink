%module OpenIGTLinkPython
%include <windows.i>
%include "std_string.i"
%include "std_list.i"
%include "std_vector.i"
%include "carrays.i"

%array_class(float, floatArray);
%array_class(char, charArray);
%array_class(int, intArray);

%begin %{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif
%}


%{
#define SWIG_FILE_WITH_INIT

#include "igtlConfigure.h"
#include "igtlWin32Header.h"
#ifdef OpenIGTLink_PLATFORM_WIN32
    // For smartpointer class export
    #define IGTL_EXPORT __declspec(dllexport)
#else
    // For smartpointer class export
    #define IGTL_EXPORT
#endif
#pragma comment(lib, "Ws2_32.lib")


#include "igtlMacro.h"
#include "igtlSocket.h"
#ifdef OpenIGTLink_PLATFORM_WIN32
    #include <windows.h>
    #include <winsock2.h>
#endif
#include <iostream>
#include "igtlTypes.h"
#include "igtlSimpleFastMutexLock.h"
#include "igtlSmartPointer.h"
#include "igtlObject.h"
#include "igtlLightObject.h"
#include "igtlMessageHeader.h"
#include "igtlMessageBase.h"
#include "igtlMessageFactory.h"
#include "igtlSocket.h"
#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "igtlTransformMessage.h"
#include "igtlStringMessage.h"
#include "igtlStatusMessage.h"
#include "igtlPointMessage.h"
#include "igtlImageMessage.h"
#include "igtlImageMessage2.h"
#include "igtlPolyDataMessage.h"
#include "igtlPositionMessage.h"
#include "igtlBindMessage.h"
#include "igtlTrajectoryMessage.h"
#include "igtlSensorMessage.h"
#include "igtlQueryMessage.h"
#include "igtlNDArrayMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlLabelMetaMessage.h"
#include "igtlImageMetaMessage.h"
#include "igtlCapabilityMessage.h"
#include "igtlColorTableMessage.h"
#include "igtlCommandMessage.h"
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
#include "VideoStreaming/igtlVideoMessage.h"
#endif
using namespace igtl;

#ifdef OpenIGTLink_PLATFORM_WIN32
    // Window visual studio wounld't able to link validEncoding. This is a workaround
    #define NUM_ENCODINGS 257
    const int CommandMessage::validEncodings[NUM_ENCODINGS] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,109,110,111,112,113,114,115,116,117,118,119,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2043,2011,2044,2045,2010,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055,2056,2057,2058,2059,2060,2061,2062,2063,2064,2065,2066,2067,2068,2069,2070,2071,2072,2073,2074,2075,2076,2077,2078,2079,2080,2081,2082,2083,2084,2085,2086,2087,2088,2089,2090,2091,2092,2093,2094,2095,2096,2097,2098,2099,2100,2101,2102,2103,2104,2105,2106,2107,2108,2109,2250,2251,2252,2253,2254,2255,2256,2257,2258,2259,2260};
#endif
%}

%include "Source/igtlConfigure.h"
%include "Source/igtlWin32Header.h"
%include "Source/igtlMacro.h"
%include "Source/igtlSmartPointer.h"
%import "Source/igtlLightObject.h"
%import "Source/igtlObject.h"

%typedef float Matrix4x4[4][4];

typedef unsigned char igtlUint8;
typedef char          igtlInt8;
typedef unsigned short igtlUint16;
typedef signed short   igtlInt16;
typedef unsigned int   igtlUint32;
typedef signed int     igtlInt32;
typedef unsigned long long igtlUint64;
typedef signed long long   igtlInt64;
typedef float         igtlFloat32;
typedef double             igtlFloat64;
typedef double igtlComplex[2];

using namespace igtl;
%template(SmartPointerMessageBase) igtl::SmartPointer<igtl::MessageBase>;
%template(SmartPointerMessageHeader) igtl::SmartPointer<igtl::MessageHeader>;
%template(SmartPointerSocket) igtl::SmartPointer<igtl::Socket>;
%template(SmartPointerServerSocket) igtl::SmartPointer<igtl::ServerSocket>;
%template(SmartPointerClientSocket) igtl::SmartPointer<igtl::ClientSocket>;
%template(SmartPointerTransformMessage) igtl::SmartPointer<igtl::TransformMessage>;
%template(SmartPointerStringMessage) igtl::SmartPointer<igtl::StringMessage>;
%template(SmartPointerStatusMessage) igtl::SmartPointer<igtl::StatusMessage>;
%template(SmartPointerPointMessage) igtl::SmartPointer<igtl::PointMessage>;
%template(SmartPointerImageMessage) igtl::SmartPointer<igtl::ImageMessage>;
%template(SmartPointerImageMessage2) igtl::SmartPointer<igtl::ImageMessage2>;
%template(SmartPointerPolyDataMessage) igtl::SmartPointer<igtl::PolyDataMessage>;
%template(SmartPointerPositionMessage) igtl::SmartPointer<igtl::PositionMessage>;
%template(SmartPointerBindMessage) igtl::SmartPointer<igtl::BindMessage>;
%template(SmartPointerTrajectoryMessage) igtl::SmartPointer<igtl::TrackingDataMessage>;
%template(SmartPointerSensorMessage) igtl::SmartPointer<igtl::SensorMessage>;
%template(SmartPointerQueryMessage) igtl::SmartPointer<igtl::QueryMessage>;
%template(SmartPointerNDArrayMessage) igtl::SmartPointer<igtl::NDArrayMessage>;
%template(SmartPointerTrackingDataMessage) igtl::SmartPointer<igtl::TrackingDataMessage>;
%template(SmartPointerQuaternionTrackingDataMessage) igtl::SmartPointer<igtl::QuaternionTrackingDataMessage>;
%template(SmartPointerLabelMetaMessage) igtl::SmartPointer<igtl::LabelMetaMessage>;
%template(SmartPointerImageMetaMessage) igtl::SmartPointer<igtl::ImageMetaMessage>;
%template(SmartPointerCapabilityMessage) igtl::SmartPointer<igtl::CapabilityMessage>;
%template(SmartPointerColorTableMessage) igtl::SmartPointer<igtl::ColorTableMessage>;
%template(SmartPointerCommandMessage) igtl::SmartPointer<igtl::CommandMessage>;
%template(SmartPointerGetTransformMessage) igtl::SmartPointer<igtl::GetTransformMessage>;
%template(SmartPointerGetStatusMessage) igtl::SmartPointer<igtl::GetStatusMessage>;
%template(SmartPointerGetImageMessage) igtl::SmartPointer<igtl::GetImageMessage>;
%template(SmartPointerGetPointMessage) igtl::SmartPointer<igtl::GetPointMessage>;
%template(SmartPointerGetTrajectoryMessage) igtl::SmartPointer<igtl::GetTrajectoryMessage>;
%template(SmartPointerGetPolyDataMessage) igtl::SmartPointer<igtl::GetPolyDataMessage>;
%template(SmartPointerRTSPolyDataMessage) igtl::SmartPointer<igtl::RTSPolyDataMessage>;
%template(SmartPointerStartPolyDataMessage) igtl::SmartPointer<igtl::StartPolyDataMessage>;
%template(SmartPointerStopPolyDataMessage) igtl::SmartPointer<igtl::StopPolyDataMessage>;
%template(SmartPointerRTSTrackingDataMessage) igtl::SmartPointer<igtl::RTSTrackingDataMessage>;
%template(SmartPointerStartTrackingDataMessage) igtl::SmartPointer<igtl::StartTrackingDataMessage>;
%template(SmartPointerStopTrackingDataMessage) igtl::SmartPointer<igtl::StopTrackingDataMessage>;
%template(SmartPointerRTSTrackingDataMessage) igtl::SmartPointer<igtl::RTSTrackingDataMessage>;
%template(SmartPointerRTSQuaternionTrackingDataMessage) igtl::SmartPointer<igtl::RTSQuaternionTrackingDataMessage>;
%template(SmartPointerStartQuaternionTrackingDataMessage) igtl::SmartPointer<igtl::StartQuaternionTrackingDataMessage>;
%template(SmartPointerStopQuaternionTrackingDataMessage) igtl::SmartPointer<igtl::StopQuaternionTrackingDataMessage>;
%template(SmartPointerGetImageMetaMessage) igtl::SmartPointer<igtl::GetImageMetaMessage>;
%template(SmartPointerGetLabelMetaMessage) igtl::SmartPointer<igtl::GetLabelMetaMessage>;
%template(SmartPointerRTSCommandMessage) igtl::SmartPointer<igtl::RTSCommandMessage>;
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
%template(SmartPointerVideoMessage) igtl::SmartPointer<igtl::VideoMessage>;
#endif
// Classes to be wrapped:
%include "Source/igtlSocket.h"
%include "Source/igtlServerSocket.h"
%include "Source/igtlClientSocket.h"
%include "Source/igtlMessageHeader.h"
%include "Source/igtlMessageBase.h"
%include "Source/igtlTransformMessage.h"
%include "Source/igtlStringMessage.h"
%include "Source/igtlStatusMessage.h"
%include "Source/igtlPointMessage.h"
%include "Source/igtlImageMessage.h"
%include "Source/igtlImageMessage2.h"
%include "Source/igtlPolyDataMessage.h"
%include "Source/igtlPositionMessage.h"
%include "Source/igtlBindMessage.h"
%include "Source/igtlTrajectoryMessage.h"
%include "Source/igtlSensorMessage.h"
%include "Source/igtlQueryMessage.h"
%include "Source/igtlNDArrayMessage.h"
%include "Source/igtlTrackingDataMessage.h"
%include "Source/igtlQuaternionTrackingDataMessage.h"
%include "Source/igtlLabelMetaMessage.h"
%include "Source/igtlImageMetaMessage.h"
%include "Source/igtlCapabilityMessage.h"
%include "Source/igtlColorTableMessage.h"
%include "Source/igtlCommandMessage.h"
#if defined(OpenIGTLink_ENABLE_VIDEOSTREAMING)
%include "Source/VideoStreaming/igtlVideoMessage.h"
#endif
