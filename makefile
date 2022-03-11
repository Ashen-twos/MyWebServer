server:
	g++ -g Package/util.cpp Package/Epoll.cpp Package/InetAddress.cpp Package/Socket.cpp \
	http/HttpData.cpp http/HttpRequest.cpp http/HttpMsg.cpp  \
	Timer/Timer.cpp Log/Log.cpp MySQL/MySQLPool.cpp MySQL/SQLRAII.cpp \
	pthread/PthreadSync.cpp pthread/PthreadPool.cpp Server.cpp main.cpp -o server -pthread -lmysqlclient
clean:
	rm server
test:
	g++ -g Package/util.cpp Package/Epoll.cpp Package/InetAddress.cpp Package/Socket.cpp \
	http/HttpData.cpp http/HttpRequest.cpp http/HttpMsg.cpp  \
	Timer/Timer.cpp Log/Log.cpp \
	pthread/PthreadSync.cpp Server.cpp Log/test.cpp -o test -pthread --std=c++14

demo2:
	g++ -g pthread/PthreadSync.cpp Log/Log.cpp MySQL/MySQLPool.cpp MySQL/SQLRAII.cpp MySQL/demo.cpp -o demo2 -pthread -lmysqlclient