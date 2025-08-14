To enable interoperability, this Recommendation assumes a minimum set of capabilities from the virtual filestore as follows:

[ ] create file

[ ] delete file

[ ] rename file

[ ] append file

[ ] replace file

[ ] create directory

[ ] remove directory

CCSDS 720.2-G-4 Page 3-2 May 2021
Table 3-1: User Operations Message Types
Msg Type
(hex) Interpretation
00 Proxy Put Request
01 Proxy Message to User
02 Proxy Filestore Request
03 Proxy Fault Handler Override
04 Proxy Transmission Mode
05 Proxy Flow Label
06 Proxy Segmentation Control
07 Proxy Put Response
08 Proxy Filestore Response
09 Proxy Put Cancel
0B Proxy Closure Request
10 Directory Listing Request
11 Directory Listing Response
20 Remote Status Report Request
21 Remote Status Report Response
30 Remote Suspend Request
31 Remote Suspend Response
38 Remote Resume Request
39 Remote Resume Response