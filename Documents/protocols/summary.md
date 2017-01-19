---
layout: page
title: Specification > Version 2 Summary
header: Pages
---
{% include JB/setup %}

## Overview

The OpenIGTLink Protocol is ideal for building a research prototype system, because of its simple and standardized specification that requires less effort on design and development of communication mechanism for systems integration. By using OpenIGTLink, developers can save their time for more essential part of research. The OpenIGTLink's simple specification has been well accepted by the community, resulting a number of applications in the IGT research community, since it's initial release in early 2008.

While we keep the protocol as simple as possible, it is also important to support as many IGT devices as possible to maximize the connectivity among software and devices, which potentially frees the developers from tedious coding just for system integration. There have been a number of feature requests that would benefit many IGT research projects, such as connectivity with commercial products, and connectivity with other major research tools, such as MATLAB. To respond to those request, and to increase the number of potential applications, we decided review the requests and release version 2 protocol during NA-MIC Summer Project Week 2010. The new protocol defines a set of new message types with new querying scheme, while maintaining a backward compatibility with version 1. This page summarizes the features in version 2.

## Simple Query Scheme

OpenIGTLink V.2 defines a simple querying scheme on top of the existing message format, by introducing a few prefix to the device type field.

## Request a single message

A client can request to send data contained in a single message, by issuing a query message with a device type string starting with "GET_". If specified data is not available, a message with a device type string starting with "RTS_" is returned. For example, when a client requests an IMAGE message to a server, it sends GET_IMAGE message to the server as a query. If the image exist, an IMAGE message is returned from the server to the client. Otherwise, a RTS_IMAGE message with error code is returned.

## Request a stream of messages (introduced in version 2)

A client can start and stop data streaming (sent as a series of messages) from a server, by issuing a query message with a device type starting with "STT_" and "STP_" respectively. If data requested by a "STT_" message is not available, the server returns a message with a device type string starting with "RTS_" prefix. A STP_ message is also acknowledged by a "RTS_" message.

This is useful to start and stop position tracking or real-time imaging remotely from the client. For example, once the server receives "STT_TDATA" message from the client, it start sending "TDATA" messages to the client. The server keep sending "TDATA" messages until it receives "STP_TDATA" from the client.

## New message types

The following tables show lists of message types available in version 2.

### New Messages for image-guided systems

<table border="1" cellpadding="1">
<tr>
<td style="width:10%; background:#88A8FF">Type name
</td><td style="width:10%; background:#88A8FF">GET query
</td><td style="width:10%; background:#88A8FF">STT query
</td><td style="width:10%; background:#88A8FF">STP query
</td><td style="width:10%; background:#88A8FF">RTS message
</td><td style="width:60%; background:#88A8FF">Description
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> TDATA
</td><td>GET_TDATA
</td><td>STT_TDATA
</td><td>STP_TDATA
</td><td>RTS_TDATA
</td><td align="left">Tracking data
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> IMGMETA
</td><td>GET_IMGMETA
</td><td>--
</td><td>--
</td><td>RTS_IMGMETA
</td><td align="left">List of image meta data including patient name, ID (medical record number), size, etc.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> LBMETA
</td><td>GET_LBMETA
</td><td>--
</td><td>--
</td><td>RTS_LBMETA
</td><td align="left">List of label meta data.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> POINT
</td><td>GET_POINT
</td><td>--
</td><td>--
</td><td>RTS_POINT
</td><td align="left">Points or fiducials.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> TRAJ
</td><td>GET_TRAJ
</td><td>--
</td><td>--
</td><td>RTS_TRAJ
</td><td align="left">Trajectory data (needle path etc.)
</td></tr>
</table>


### New Messages for general applications
<table border="1" cellpadding="1">
<tr>
<td style="width:10%; background:#88A8FF">Type name
</td><td style="width:10%; background:#88A8FF">GET query
</td><td style="width:10%; background:#88A8FF">STT query
</td><td style="width:10%; background:#88A8FF">STP query
</td><td style="width:10%; background:#88A8FF">RTS message
</td><td style="width:60%; background:#88A8FF">Description
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> NDARRAY
</td><td>GET_NDARRAY
</td><td>STT_NDARRAY
</td><td>STP_NDARRAY
</td><td>RTS_NDARRAY
</td><td align="left">Associative array to transfer a set of values with key names.
</td></tr>
</table>


### Messages from version 1
<table border="1" cellpadding="1">
<tr>
<td style="width:10%; background:#88A8FF">Type name
</td><td style="width:10%; background:#88A8FF">GET query
</td><td style="width:10%; background:#88A8FF">STT query
</td><td style="width:10%; background:#88A8FF">STP query
</td><td style="width:10%; background:#88A8FF">RTS message
</td><td style="width:60%; background:#88A8FF">Description
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> IMAGE
</td><td>GET_IMAGE
</td><td>STT_IMAGE
</td><td>STP_IMAGE
</td><td>RTS_IMAGE
</td><td align="left">2D/3D image data
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> TRANSFORM
</td><td>GET_TRANSFOR
</td><td>STT_TRANSFOR
</td><td>STP_TRANSFOR
</td><td>RTS_TRANSFOR
</td><td align="left">Affine transform data.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> POSITION
</td><td>GET_POSITION
</td><td>STT_POSITION
</td><td>STP_POSITION
</td><td>RTS_POSITION
</td><td align="left">Position and orientation (quaternion)
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> CAPABILITY
</td><td>GET_CAPABIL
</td><td>--
</td><td>--
</td><td>RTS_CAPABIL
</td><td align="left">Points or fiducials.
</td></tr>
<tr>
<td style="width:25%; background:#F0F8FF"> STATUS
</td><td>GET_STATUS
</td><td>--
</td><td>--
</td><td>RTS_STATUS
</td><td align="left">Device status
</td></tr>
</table>








