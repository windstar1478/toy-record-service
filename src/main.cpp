//main.cpp

#include <iostream>
#include "httplib.h"
using namespace std;

void build_response(httplib::Response& res, int status_code) { //상태 코드에 따라 HTTP 응답 내용을 설정하는 함수
	if (status_code == 200) {
		res.set_content("Server On", "text/plain"); //응답: "Server On" (뒤의 "text/plain": 일반 텍스트)
		res.status = 200; //요청이 정상적으로 인증 조건 통과 
	}
	else if (status_code == 401) {
		res.set_content("Unauthorized", "text/plain");
		res.status = 401; //인증 실패 (Unauthorized)
	}
}

int main() {
	cout << "[BOOT] server start" << endl; //부트 로그 출력

	httplib::Server server; //서버 객체 생성

	server.Get("/health", [](const httplib::Request& req, httplib::Response& res) { //'/health'로 GET 요청이 오면 이 코드 실행
		cout << "[REQ] GET /health ";//GET 요청 로그 출력

		if (req.has_param("token")) { //token이 존재하는지 확인
			string token = req.get_param_value("token"); //사용자가 제공한 token값 조회 후 (sting) token에 저장
			cout << "token = " << token << endl;

			if (token == "abc") { //만약 제공된 token 값이 "abc"와 일치한다면
				cout << "[AUTH] success" << endl;
				build_response(res, 200);
			}
			else { //token 값이 일치하지 않으면
				cout << "[AUTH] unauthorized" << endl;
				build_response(res, 401);
			}
		}
		else { //token이 존재하지 않음
			cout << "[AUTH] no token";
			build_response(res, 401);
		}

	});

	cout << "[LISTEN] waiting for requests on 8080" << endl; //8080번 포트에서 대기할 것이라 선언
	//서버 실행
	server.listen("0.0.0.0", 8080); //8080번 포트를 열고, 요청 올 때까지 대기

	return 0;
}
