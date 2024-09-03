import http.client
import time

def send_chunked_request(host, port, path, chunks):
    conn = http.client.HTTPConnection(host, port)
    
    # Headers for chunked transfer
    headers = {
        "Transfer-Encoding": "chunked",
        "Content-Type": "text/plain"
    }
    
    # Send the request
    conn.putrequest("POST", path)
    for header, value in headers.items():
        conn.putheader(header, value)
    conn.endheaders()
    
    # Send chunks
    for chunk in chunks:
        conn.send(f"{len(chunk):X}\r\n".encode())
        time.sleep(0.5);
        conn.send(chunk.encode())
        time.sleep(0.5);
        conn.send(b"\r\n")
        time.sleep(0.5);
    
    # Send the final zero-length chunk
    conn.send(b"0\r\n\r\n")
    
    # Get the response
    response = conn.getresponse()
    print("Response status:", response.status)
    print("Response reason:", response.reason)
    print("Response headers:", response.getheaders())
    print("Response body:", response.read().decode())
    
    conn.close()

# Example usage
host = "localhost"
port = 8080
path = "/cgi-bin/echo.py"
chunks = ["This is the first chunk.", "This is the second chunk.", "This is the third chunk."]

send_chunked_request(host, port, path, chunks)
