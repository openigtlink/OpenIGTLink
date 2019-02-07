import sys
sys.path.append('~/Desktop/Work/OpenIGTLink/build/bin') # add the share library path and OpenIGTLinkPython.py path here
import OpenIGTLinkPython
serverSocket=OpenIGTLinkPython.ServerSocket_New()
serverSocket.CreateServer(18944)
#Client need to connect to the server within 10 second. you can modify the wait time
socket=serverSocket.WaitForConnection(10000)

header=OpenIGTLinkPython.MessageBase_New()
header.InitPack()
socket.Receive(header.GetPackPointer(),header.GetPackSize())
header.Unpack()
transformMSG=OpenIGTLinkPython.TransformMessage_New()
transformMSG.SetMessageHeader(header.GetPointer())
transformMSG.AllocatePack()
socket.Receive(transformMSG.GetPackBodyPointer(),transformMSG.GetPackBodySize())
transformMSG.Unpack(1)
pos=OpenIGTLinkPython.floatArray(3)
transformMSG.GetPosition(pos)
pos[0] #should show 3.5

header=OpenIGTLinkPython.MessageBase_New()
header.InitPack()
socket.Receive(header.GetPackPointer(),header.GetPackSize())
header.Unpack()
imageMSG=OpenIGTLinkPython.ImageMessage_New()
imageMSG.SetMessageHeader(header.GetPointer())
imageMSG.AllocatePack()
socket.Receive(imageMSG.GetPackBodyPointer(),imageMSG.GetPackBodySize())
imageMSG.Unpack(1)
spacing=OpenIGTLinkPython.floatArray(3)
imageMSG.GetSpacing(spacing)
dimension=OpenIGTLinkPython.intArray(3)
imageMSG.GetDimensions(dimension)
imageData=OpenIGTLinkPython.charArray(imageMSG.GetImageSize())
imageMSG.GetImageData(imageData)
imageData[2] # should return 3
