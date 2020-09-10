# Conformance
This software is built to be 100% RFC 7230 compliant. The quest to do this in
conjunction with supporting all clients can be a problem, though. This document
reports which parts of the specification are difficult.

## CRLF line endings
The specification explicitely states that the line endings should be CRLF, not
LF or CR singlely. Some clients (especially newer ones) use LF as line endings,
which is incorrect but handled by most server software.

## CRLF after POST message data
Section 3.5 isn't correctly implemented.

## Transfer encoding
This isn't implemented correctly.
