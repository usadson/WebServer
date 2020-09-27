/**
 * Copyright (C) 2020 Tristan. All Rights Reserved.
 * This file is licensed under the BSD 2-Clause license.
 * See the COPYING file for licensing information.
 */

#include "strings.hpp"

namespace Strings {

const base::String DefaultWebPage = "<!doctype html><html lang=\"en\"><head><meta name=\"author\" content=\"Tristan\"><meta charset=\"utf-8\"><meta name=\"description\" content=\"It works! This is a default webpage.\"><title>Wizard Web Server - Default Webpage</title><style>*{font-family:\"Noto Sans\",\"Calibri\",\"Roboto\",sans-serif}html{background-color:#ddd;margin:0;height:100%}body{height:100%;margin:0}main{margin:0 10%;height:100%;width:80%;background-color:#fff}h1{padding-top:10%;margin:0;text-align:center}div{margin:0 6%}</style></head><body><main><h1>Wizard Web Server</h1><div><h3>It works!</h3><p>The fact that you can read this means that the web server has been configured correctly!</p><p>Now go ahead and change this page to your personal homepage :)</p></div><div><h3>About</h3><p>This is a default page, served to you by the <a href=\"https://github.com/usadson/WebServer\">Wizard Web Server</a>, a free BSD-3-Clause licensed webserver.</p></div></main></body></html>";
const base::String ForbiddenPage =
	"<!doctype html>"
	"<html>"
	"<head><title>Forbidden</title></head>"
	"<body><h1>Forbidden</h1></body>"
	"</html>";
const base::String NotFoundPage =
	"<!doctype html>"
	"<html>"
	"<head><title>File Not Found</title></head>"
	"<body><h1>File Not Found</h1></body>"
	"</html>";
const base::String TooManyRequestsPage =
	"<!doctype html>"
	"<html>"
	"<head><title>Too Many Requests</title></head>"
	"<body>"
	"<h1>Too Many Requests!</h1>"
	"<hr>"
	"<p>The automatic abuse system has detected that you're sending too many requests. Calm down for a few seconds, then try again :)</p>"
	"</body>"
	"</html>";
const base::String FileSystemOverloadPage =
	"<!doctype html>"
	"<html>"
	"<head>"
	"<title>Service Unavailable</title>"
	"</head>"
	"<body>"
	"<h1>Service Unavailable</h1>"
	"<hr>"
	"<p>Unfortunately, we're currently experiencing temporary outages. Sorry for the inconvenience!</p>"
	"</body>"
	"</html>";

namespace BadRequestMessages {
const base::String EmptyMethod =
	"A method cannot be empty.\n"
	"RFC 7230 section 3.1.1 specifies a method as follows:\n"
	"     method = token\n"
	"\n"
	"Where 'token' is as follows:\n"
	"     token          = 1*tchar\n"
	"\n"
	"At least one tchar is expected, and 'tchar' is specified as follows:\n"
	"     tchar          = \"!\" / \"#\" / \"$\" / \"%\" / \"&\" / \"'\" / \"*\"\n"
	"                    / \"+\" / \"-\" / \".\" / \"^\" / \"_\" / \"`\" / \"|\" / \"~\"\n"
	"                    / DIGIT / ALPHA\n"
	"                    ; any VCHAR, except delimiters\n";
const base::String HeaderFieldNameTooLong = "header field-name too long";
const base::String HeaderFieldValueTooLong = "header field-value too long";
const base::String MethodTooLong = "method too long";
const base::String RequestTargetTooLong = "request-target too long";
const base::String TooManyOWSs = "too many ows's";
} // namespace BadRequests

namespace StatusLines {
const base::String BadRequest = "HTTP/1.1 400 Bad Request";
const base::String Forbidden = "HTTP/1.1 403 Forbidden";
const base::String HTTPVersionNotSupported = "HTTP/1.1 505 HTTP Version Not Supported";
const base::String MovedPermanently = "HTTP/1.1 301 Moved Permanently";
const base::String NotFound = "HTTP/1.1 404 Not Found";
const base::String OK = "HTTP/1.1 200 OK";
const base::String PayloadTooLarge = "HTTP/1.1 413 Payload Too Large";
const base::String ServiceUnavailable = "HTTP/1.1 503 Service Unavailable";
const base::String TooManyRequests = "HTTP/1.1 429 Too Many Requests";
const base::String URITooLong = "HTTP/1.1 414 URI Too Long";
} // namespace StatusLines

} // namespace Strings
