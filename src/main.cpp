//main.cpp

#include <iostream>
#include "httplib.h"
using namespace std;

int main() {
	cout << "[BOOT] server start" << endl; //부트 로그 출력

	httplib::Server server; //서버 객체 생성

	server.Get("/health", [](const httplib::Request& req, httplib::Response& res) { //'/health'로 GET 요청이 오면 이 코드 실행
		cout << "[REQ] GET /health" << endl; //GET 요청 로그 출력

		res.status = 200; //서버 정상 작동 중

		res.set_content("Server On", "text/plain"); //응답: "Server On" (뒤의 "text/plain": 일반 텍스트)
	});

	cout << "[LISTEN] waiting for requests on 8080" << endl; //8080번 포트에서 대기할 것이라 선언

	//서버 실행
	server.listen("0.0.0.0", 8080); //8080번 포트를 열고, 요청 올 때까지 대기

	return 0;
}