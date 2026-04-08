//main.cpp
#include <iostream>
#include <vector>
#include <string>
#include "httplib.h"
using namespace std;


struct AlbumRecord
{
	int id;
	string date;
	string album;
	string artist;

	AlbumRecord(int id, const string& date, const string& album, const string &artist) 
		: id(id), date(date), album(album), artist(artist) {}
};
vector<AlbumRecord> AlbumRecords;	
int next_id = 1; //다음으로 저장될 id 번호

void build_response(httplib::Response& res, int status_code, 
	const string& log_line, const string& body) { //상태 코드에 따라 HTTP 응답 내용을 설정하는 함수
	cout << log_line << endl;

	res.status = status_code;
	res.set_content(body, "text/plain");
}

int main() {
	cout << "[BOOT] server start" << endl; //부트 로그 출력

	httplib::Server server; //서버 객체 생성


	server.Get("/health", [](const httplib::Request& req, httplib::Response& res) {
		cout << "[REQ] GET /health" << endl;//GET 요청 로그 출력

		if (req.has_param("token")) { //token이 존재하는지 확인
			string token = req.get_param_value("token"); //사용자가 제공한 token값 조회 후 (sting) token에 저장
			cout << "token: " << token << endl;

			if (token == "abc") { //만약 제공된 token 값이 "abc"와 일치한다면
				cout << "token success, server on" << endl;
				build_response(res, 200, "[RES] 200 ok", "token success, server on");
			}
			else { //token 값이 일치하지 않으면
				build_response(res, 401, "[AUTH] unauthorized", "unauthorized");
			}
		}
		else { //token이 존재하지 않음
			build_response(res, 401, "[AUTH] missing token", "no token");
		}

		});


	server.Post("/album-records", [](const httplib::Request& req, httplib::Response& res) {
		cout << "[REQ] POST /album-records" << endl;
		string body = req.body;

		//파싱
		size_t p1 = body.find('/'); //'/'를 기준으로 파싱해 위치함
		size_t p2 = body.find('/', p1 + 1);

		//잘못된 입력처리
		if (p1 == string::npos || p2 == string::npos) {
			build_response(res, 400, "[RES] 400 Bad Request", "bad request");
			return;
		}

		//제공된 정보의 임시 문자열
		string date = body.substr(0, p1); //처음부터 '/' 바로 전까지
		string album = body.substr(p1 + 1, p2 - p1 - 1);
		string artist = body.substr(p2 + 1);

		//중복 검사 (date, album, artist)
		for (auto& r : AlbumRecords) {
			if (r.date == date && r.album == album && r.artist == artist) { //모두 중복 시
				cout << "This album already exists" << endl;
				build_response(res, 409, "[RES] 409 conflict", "conflict");
				return;
			}
		}

		int id = next_id; //임시 id 생성 후 다음 id 번호 지정
		next_id++;
		AlbumRecords.emplace_back(id, date, album, artist); //중복이 아니면 데이터 추가
		build_response(res, 201, "[RES] 201 created", "created");

		});


	server.Get("/album-records", [](const httplib::Request& req, httplib::Response& res) {
		//쿼리에 id가 존재하는지 확인
		if (req.has_param("id")) { //쿼리 파라미터로 존재 시
			string Query_id = req.get_param_value("id"); //Query id 임시 생성 후 파라미터로 받은 id 값 저장
			int id; //올바른 파라미터가 전달되었는지 확인용 변수 선언

			try
			{
				//string으로 선언된 Query_id를 int로 변환
				id = stoi(Query_id); //올바른 형식으로 전달됨
			}
			catch (const exception& invalid_type)
			{
				build_response(res, 400, "[RES] 400 Bad request (invalid type)", "Bad Request");
				return;
			}

			for (const auto& r : AlbumRecords) { //특정 id 존재 확인
				if (r.id == id) { //id 존재
					cout << "id is found" << endl;

					string result; //저장할 임시 문자열 생성

					//해당 records의 정보 저장
					result = to_string(r.id) + " | " + r.date + " | " + r.album + " | " + r.artist + "\n";

					build_response(res, 200, "[RES] 200 ok (one record)", result);

					return;
				}
			}

			//찾고자 하는 id가 존재하지 않는다면
			build_response(res, 404, "[RES] 404 Not Found", "Not Found");
		}
		else { //id가 쿼리 파라미터로 존재하지 않을 시
			cout << "[REQ] GET /album-records" << endl;
			//기록된 게 없는지(비어 있는지) 확인
			if (AlbumRecords.empty()) {	//비어 있을 시
				cout << "no records" << endl;
				build_response(res, 200, "[RES] 200 ok(empty)", "no records");
				return;
			}

			string result; //누적시킬 임시 문자열 생성

			//모든 records들의 정보 저장
			for (auto& record : AlbumRecords) {
				result += to_string(record.id) + " | "
					+ record.date + " | "
					+ record.album + " | "
					+ record.artist + "\n";
			}

			build_response(res, 200, "[RES] 200 ok", result);
		}

		});


	server.Delete("/album-records", [](const httplib::Request& req, httplib::Response& res) {
		if (req.has_param("id")) { //id가 쿼리 파라미터로 존재 시
			string Query_id = req.get_param_value("id"); //Query id 임시 생성 후 파라미터로 받은 id 값 저장
			int id; //올바른 파라미터가 전달되었는지 확인용 변수 선언
			try
			{ //string으로 선언된 Query_id를 int로 변환
				id = stoi(Query_id); //올바른 형식으로 전달됨
			}
			catch (const exception& invalid_type)
			{
				build_response(res, 400, "[RES] 400 Bad request (invalid type)", "Bad Request");
				return;
			}
			for (int i = 0; i < AlbumRecords.size(); i++) {
				if (AlbumRecords[i].id == id) {
					AlbumRecords.erase(AlbumRecords.begin() + i);
					build_response(res, 200, "[RES] 200 ok(erased)", "erased");
					return;
				}
			}

			//찾고자 하는 id가 존재하지 않는다면
			build_response(res, 404, "[RES] 404 Not Found", "Not Found");

		}
		else { //id가 쿼리 파라미터로 존재 않을 시 (요청이 잘못됨)
			build_response(res, 400, "[RES] 400 Bad Request(no id)", "Bad Request");
		}


		});




	cout << "[LISTEN] waiting for requests on 8080" << endl; //8080번 포트에서 대기할 것이라 선언
	//서버 실행
	server.listen("0.0.0.0", 8080); //8080번 포트를 열고, 요청 올 때까지 대기

	return 0;
}
