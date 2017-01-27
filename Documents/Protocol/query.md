[Back to Index](/Documents/Protocol/index.md)

Query Message
=============

- Protocol Version: 3.0
- Release Date: January 20, 2017

Simple Querying Mechanism
=========================

In OpenIGTLink version 2, special prefixes GET_, STT_, STP_, and RTS_ are used
in the message type field in the header for messages to query and control data
flow. Those messages with those special prefix should be defined along with
primary message types (for example, STT_TDATA, STP_TDATA and RTS_TDATA should
be defined with TDATA).

GET_ prefix: Query a single message
===================================

"GET_&lt;datatype&gt;" query message is used to request for a single message
with type &lt;datatype&gt;. The receiver of "GET_&lt;datatype&gt;" message must
return a message with type &lt;datatype&gt; and the same name as the query
message. A "GET_&lt;datatype&gt;" message without device name requests any
available data. If data is not available, a returned message must be null body
(data size = 0). A format of "GET_&lt;datatype&gt;" may be defined per data type,
if necessary.

STT_ and STP_ prefixes: Control data flow
=========================================

"STT_&lt;datatype&gt;" and "STP_&lt;datatype&gt;" query message is used to
request to start and stop sending a series of messages. The receiver of
"STT_&lt;datatype&gt;" or "STP_&lt;datatype&gt;" message must return
"RTS_&lt;datatype&gt;" message with the same name as the query message
to notify that the receiver receives the query. Formats of "STT_&lt;datatype&gt;",
"STP_&lt;datatype&gt;" and "RTS_&lt;datatype&gt;" may be defined per data type,
if necessary.

Available Message Type (Including sub-types)
============================================

Type name | GET query   | STT query   | STP query   | RTS message  | Description
----------|-------------|-------------|-------------|--------------|--------------------------------------
IMAGE     | GET_IMAGE   | STT_IMAGE   | STP_IMAGE   | RTS_IMAGE    | 2D/3D image data
TRANSFORM | GET_TRANSFOR| STT_TRANSFOR| STP_TRANSFOR| RTS_TRANSFOR | Affine transform data.
POSITION  | GET_POSITION| STT_POSITION| STP_POSITION| RTS_POSITION | Position and orientation (quaternion)
CAPABILITY| GET_CAPABIL | --          | --          | RTS_CAPABIL  | Points or fiducials.
STATUS    | GET_STATUS  | --          | --          | RTS_STATUS   | Device status
TDATA     | GET_TDATA   | STT_TDATA   | STP_TDATA   | RTS_TDATA    | Tracking data
IMGMETA   | GET_IMGMETA | --          | --          | RTS_IMGMETA  | List of image meta data including patient name, ID (medical record number), size, etc.
LBMETA    | GET_LBMETA  | --          | --          | RTS_LBMETA   | List of label meta data.
POINT     | GET_POINT   | --          | --          | RTS_POINT    | Points or fiducials.
TRAJ      | GET_TRAJ    | --          | --          | RTS_TRAJ     | Trajectory data (needle path etc.)
NDARRAY   | GET_NDARRAY | STT_NDARRAY | STP_NDARRAY | RTS_NDARRAY  | Associative array to transfer a set of values with key names.
COMMAND   | --          | --          | --          | RTS_COMMAND  | Command


