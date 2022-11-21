#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <fstream>
#include <vector>
using namespace boost::asio;
using namespace boost::posix_time;
using boost::system::error_code;

io_service service;

struct info {
    int id;
    double x;
    double y;
    double z;
    double time;
    double temp;
    double dx;
};

void print(char* m, int n){
    std::string s = m;
    std::cout << s.substr(0, n);
}

void handle_connections() {
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
    while (true) {
        ip::tcp::socket sock(service);
        acceptor.accept(sock);
        std::cout << "Client connection, authentication confirmation..." << std::endl;

        char aunt[] = "Enter password (3 syms)\n";
        char bufpass[4];
        char pass[] = "onf";
        sock.send(boost::asio::buffer(aunt, sizeof(aunt)));
        int b = sock.receive(boost::asio::buffer(bufpass, sizeof(bufpass)));
        bool AUNT = true;
        if (bufpass[0] == pass[0] && bufpass[1] == pass[1] && bufpass[2] == pass[2]) {
            printf("AUNT OK\n");
            sock.send(boost::asio::buffer(&AUNT,sizeof(AUNT)));
        }
        else {
            printf("AUNT DONT OK\n");
            AUNT = false;
            sock.send(boost::asio::buffer(&AUNT, sizeof(AUNT)));
            continue;
        }
        double X, Y, Z;
        sock.receive(boost::asio::buffer(&X, sizeof(X)));
        printf("x : %lf\n", X);
        sock.receive(boost::asio::buffer(&Y, sizeof(Y)));
        printf("y : %lf\n", Y);
        sock.receive(boost::asio::buffer(&Z, sizeof(Z)));
        printf("z : %lf\n", Z);

        std::vector<info> infos;
        std::vector<info> allinfo;
        std::string line;

        printf("Start reading files\n");
        std::ifstream BD_Coords;
        BD_Coords.open("BD_Coords.txt");
        if (!(BD_Coords.is_open())) {
            printf("Error open BD_Coords\n");
            exit(0);
        }
        char _id1[100], _id2[100], _x[100], _y[100], _z[100], _time[100], _temp[100], _dx1[100], _dx2[100];
        BD_Coords >> _id1 >> _id2 >> _x >> _y >> _z;
        std::cout << _id1 << " " <<  _id2 << " " << _x << " " << _y << " " << _z << std::endl;
        while (!BD_Coords.eof()) {
            info tmp;
            BD_Coords >> tmp.id >> tmp.x >> tmp.y >> tmp.z;
            infos.push_back(tmp);
        }
        printf("BD_Coords comlete\n");
        BD_Coords.close();

        std::ifstream BD;
        BD.open("BD.txt");
        if (!(BD.is_open())) {
            printf("Error open BD\n");
            exit(0);
        }
        BD >> _time >> _id1 >> _id2 >> _temp >> _dx1 >> _dx2;
        std::cout << _id1 << " " << _id2 << " " << _temp << " " << _dx1 << " " << _dx2 << std::endl;
        while (!BD.eof()) {
            info tmp2;
            BD >> tmp2.time >> tmp2.id >> tmp2.temp >> tmp2.dx;
            std::cout << tmp2.time << " " << tmp2.id << " " << tmp2.temp << " " << tmp2.dx << std::endl;
            for (int i = 0; i < infos.size(); i++) {
                if (infos[i].id == tmp2.id) {
                    tmp2.x = infos[i].x;
                    tmp2.y = infos[i].y;
                    tmp2.z = infos[i].z;
                    allinfo.push_back(tmp2);
                    printf("ID: %i\n", tmp2.id);
                    break;
                }
            }
        }
        printf("BD comlete\n");
        BD.close();

        printf("End reading files\n");

        printf("%s %s\t%s\t%s %s\t%s\n", _id1, _id2, _time, _dx1, _dx2, _temp);

        int z = 0;
        for (int i = 0; i < allinfo.size(); i++) {
            if (allinfo[i].x == X && allinfo[i].z == Z && allinfo[i].y == Y) {
                z++;
                printf("%i\t%3lf\t%2le\t%2lf\n", allinfo[i].id, allinfo[i].time, allinfo[i].dx, allinfo[i].temp);
            }
        }
        sock.send(boost::asio::buffer(&z, sizeof(z)));
        for (int i = 0; i < allinfo.size(); i++) {
            if (allinfo[i].x == X && allinfo[i].z == Z && allinfo[i].y == Y) {
                sock.send(boost::asio::buffer(&allinfo[i].time, sizeof(allinfo[i].time)));
                sock.send(boost::asio::buffer(&allinfo[i].dx, sizeof(allinfo[i].dx)));
                sock.send(boost::asio::buffer(&allinfo[i].temp, sizeof(allinfo[i].temp)));
            }
        }

        sock.close();
    }
}

int main(int argc, char* argv[]) {
    handle_connections();
}