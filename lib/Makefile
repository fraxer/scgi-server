all:
	g++  example.cpp -o server -Wall -g  -L.  -L/usr/local/lib -levent -lscgi

lib:
	g++ -c  scgiServer.cpp  -I/usr/local/include -O0 -march=armv7ve -std=c++14
	ar rcs libscgi.a *o

project:
	g++ ../scgi_server.cpp -o ../scgi_server -Wall -g -L. -lscgi -L/usr/local/lib -levent  -lmysqlclient -I/usr/local/include -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lcrypto -O0 -march=armv7ve -std=c++14
	killall -v scgi_server
	../scgi_server