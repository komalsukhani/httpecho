The server will reverse the string sent in request body on path
	hostname:postnumber/reverse


Compile 
		g++ webserver.cpp -o server

Run
		server IP_Add Port_number

Example

	Request:
						POST /reverse HTTP/1.1
						Host: 127.0.0.1:8080

						Hello World!

	Response:
					HTTP/1.1 200 OK
          Content-Type: text/plain
	
					!dlroW olleH



