import sys
sys.path.append('../../build/bin')
import OpenIGTLinkPython
clientServer=OpenIGTLinkPython.ClientSocket_New()
clientServer.ConnectToServer("127.0.0.1",18944)
transformMSG=OpenIGTLinkPython.TransformMessage_New()
pos=OpenIGTLinkPython.floatArray(3)
pos[0] =3.5
transformMSG.SetPosition(pos)
transformMSG.Pack()
clientServer.Send(transformMSG.GetPackPointer(),transformMSG.GetPackSize())
