import http.client

GREEN = '\033[1;92m'
YELLOW = '\033[93m'
RED = '\033[1;91m'
RESET = '\033[0m'

HTTP_STATUS_CODES = {
	100: "Continue",
	101: "Switching Protocol",
	200: "OK",
	201: "Created",
	202: "Accepted",
	203: "Non-Authoritative Information",
	204: "No Content",
	205: "Reset Content",
	206: "Partial Content",
	300: "Multiple Choice",
	301: "Moved Permanently",
	302: "Moved Temporarily",
	303: "See Other",
	304: "Not Modified",
	307: "Temporary Redirect",
	308: "Permanent Redirect",
	400: "Bad Request",
	401: "Unauthorized",
	403: "Forbidden",
	404: "Not Found",
	405: "Method Not Allowed",
	406: "Not Acceptable",
	407: "Proxy Authentication Required",
	408: "Request Timeout",
	409: "Conflict",
	410: "Gone",
	411: "Length Required",
	412: "Precondition Failed",
	413: "Payload Too Large",
	414: "URI Too Long",
	415: "Unsupported Media Type",
	416: "Requested Range Not Satisfiable",
	417: "Expectation Failed",
	418: "I'm a teapot",
	421: "Misdirected Request",
	425: "Too Early",
	426: "Upgrade Required",
	428: "Precondition Required",
	429: "Too Many Requests",
	431: "Request Header Fields Too Large",
	451: "Unavailable for Legal Reasons",
	500: "Internal Server Error",
	501: "Not Implemented",
	502: "Bad Gateway",
	503: "Service Unavailable",
	504: "Gateway Timeout",
	505: "HTTP Version Not Supported",
	506: "Variant Also Negotiates",
	507: "Insufficient Storage",
	510: "Not Extended",
	511: "Network Authentication Required"
}

def test_request(host, port, http_method, uri, payload, expected_status, expected_body):
	test_name = f"Test {http_method} http://{host}:{port}{uri}"

	conn = http.client.HTTPConnection(host, port)

	request_headers = {
		"Accept": "*/*",
		"User-Agent": "Python Test Script",
		"content-type": "text/plain"
	}

	conn.request(http_method, uri, payload, request_headers)
	response = conn.getresponse()

	status = response.status
	headers = response.getheaders()
	body = response.read().decode("utf-8")

	if status == expected_status and (len(expected_body) == 0 or body == expected_body):
		print(f"{test_name} {GREEN}OK! :){RESET}")
	else:
		print(f"{test_name} {RED}FAIL! :({RESET}")
		print("\tExpected values:")
		print(f"\t\t- Status: {expected_status}")
		if (len(expected_body) > 0):
			print(f"\t\t- Body:\n{expected_body}")
		print("\tResponse values:")
		print(f"\t\t- Status: {status}")
		if (len(expected_body) > 0):
			print(f"\t\t- Body:\n{body}")
		print("")

def get_file_content(file_path):
	try:
		with open(file_path, 'r', encoding='utf-8') as file:
			content = file.read()
		return content
	except Exception as e:
		print(f"{YELLOW}WARNING: File could not be opened, you might want to repeat the following test.{RESET}")
		return ""

def get_default_error_page(error_code):
	return f"<html lang=\"es\">\n<head>\n    <meta charset=\"UTF-8\">\n    <title>{error_code} {HTTP_STATUS_CODES[error_code]}</title>\n</head>\n<body>\n    <center><b style=\"font-size:24px;\">{error_code} {HTTP_STATUS_CODES[error_code]}</b></center>\n</body>\n</html>\n"

# GET REQUESTS:
test_request("localhost", 8080, "GET", "/hello-world.html", "", 200, get_file_content("./docs/fusion_web/hello-world.html"))
test_request("localhost", 8080, "GET", "/index.html", "", 200, get_file_content("./docs/fusion_web/index.html"))

# POST REQUESTS:
test_request("localhost", 8080, "POST", "/cgi-bin/echo.py", "Payload", 200, "Payload")

# ERROR RESPONSES:
test_request("localhost", 8080, "POST", "/cgi-bin/no-exec-permission.py", "", 403, get_file_content("./docs/fusion_web/error_pages/403.html"))
test_request("localhost", 8081, "POST", "/cgi-bin/no-exec-permission.py", "", 403, get_default_error_page(403))
test_request("localhost", 8080, "POST", "/this-does-not-exist", "", 404, get_file_content("./docs/fusion_web/error_pages/404.html"))
test_request("localhost", 8081, "POST", "/this-does-not-exist", "", 404, get_default_error_page(404))
test_request("localhost", 8080, "GET", "/abc/hehe.html", "", 405, get_file_content("./docs/fusion_web/error_pages/405.html"))
test_request("localhost", 8082, "GET", "/index.html", "", 405, get_default_error_page(405))
test_request("localhost", 8081, "POST", "/cgi-bin/echo.py", "Payload", 413, get_default_error_page(413))
test_request("localhost", 8080, "POST", "/cgi-bin/exception.py", "", 500, get_file_content("./docs/fusion_web/error_pages/500.html"))
test_request("localhost", 8081, "POST", "/cgi-bin/exception.py", "", 500, get_default_error_page(500))
test_request("localhost", 8081, "POST", "/cgi-bin/loop.py", "", 504, get_default_error_page(504))
