import socket
import ssl
import time

def send_post_request_in_chunks(host, port, path, headers, body_parts, use_https=False, verify=False):
    context = ssl.create_default_context()
    if not verify:
        context.check_hostname = False
        context.verify_mode = ssl.CERT_NONE
    
    if use_https:
        conn = context.wrap_socket(socket.socket(socket.AF_INET), server_hostname=host)
    else:
        conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    conn.connect((host, port))

    # Construct the initial part of the request
    request_line = f"POST {path} HTTP/1.1\r\n"
    headers_lines = ''.join(f"{header}: {value}\r\n" for header, value in headers.items())

    # Send request line and headers
    conn.sendall(request_line.encode())
    time.sleep(0.5)
    conn.sendall(headers_lines.encode())
    time.sleep(0.5)
    conn.sendall(b"\r\n")  # End of headers

    # Send body in parts
    for part in body_parts:
        conn.sendall(part.encode())

    # Close the socket to indicate the end of the request
    conn.shutdown(socket.SHUT_WR)

    # Receive and print the response
    response = b""
    while True:
        chunk = conn.recv(4096)
        if not chunk:
            break
        response += chunk
    
    conn.close()
    
    print("Response:")
    print(response.decode())

# Example usage
host = "localhost"
port = 8080
path = "/cgi-bin/echo.py"
headers = {
    "Host": host,
    "User-Agent": "python-script",
    "Content-Type": "application/x-www-form-urlencoded",
    "Content-Length": str(len("field1=value1&field2=value2"))
}
body_parts = ["field1=value1&", "field2=value2"]

send_post_request_in_chunks(host, port, path, headers, body_parts, use_https=False, verify=False)
