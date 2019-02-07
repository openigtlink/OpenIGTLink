import sys
sys.path.append('~/Desktop/Work/OpenIGTLink/build/bin') # add the share library path and OpenIGTLinkPython.py path here
import OpenIGTLinkPython
clientServer=OpenIGTLinkPython.ClientSocket_New()
clientServer.ConnectToServer("127.0.0.1",18944)
#Excute the following commands only when it is connected to the server

transformMSG=OpenIGTLinkPython.TransformMessage_New()
pos=OpenIGTLinkPython.floatArray(3)
pos[0] =3.5
transformMSG.SetPosition(pos)
transformMSG.Pack()
clientServer.Send(transformMSG.GetPackPointer(),transformMSG.GetPackSize())

imageMSG=OpenIGTLinkPython.ImageMessage_New()
space=OpenIGTLinkPython.floatArray(3)
space[0]=1.0
space[1]=1.0
space[2]=1.0
dimension=OpenIGTLinkPython.intArray(3)
dimension[0]=2
dimension[1]=2
dimension[2]=1
imageMSG.SetSpacing(space)
imageMSG.SetDimensions(dimension)
imageMSG.AllocateScalars()
image="1234"
imageMSG.SetScalarPointer(image,4)
imageMSG.Pack()
clientServer.Send(imageMSG.GetPackPointer(),imageMSG.GetPackSize())
