#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
using namespace boost::asio;
using boost::system::error_code;
io_service service;

void print(char* m, int n) {
    std::string s = m;
    std::cout << s.substr(0, n);
}

ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
void sync_echo() {
    ip::tcp::socket sock(service);
    sock.connect(ep);

    char aunt[4];
    char bufreg[100];
    int b = sock.receive(boost::asio::buffer(bufreg,sizeof(bufreg)));
    print(bufreg, b);
    std::cin >> aunt;
    Sleep(1000);
    sock.send(boost::asio::buffer(aunt,sizeof(aunt)));

    bool RESULT;
    b = sock.receive(boost::asio::buffer(&RESULT,sizeof(RESULT)));
    if (!RESULT) {
        printf("Uncorrect password\n");
        exit(0);
    }
    printf("Correct password\n");
    Sleep(1000);

    double X, Y, Z;
    printf("Enter X: ");
    scanf_s("%lf", &X);
    sock.send(boost::asio::buffer(&X, sizeof(X)));
    printf("Enter Y: ");
    scanf_s("%lf", &Y);
    sock.send(boost::asio::buffer(&Y, sizeof(Y)));
    printf("Enter Z: ");
    scanf_s("%lf", &Z);
    sock.send(boost::asio::buffer(&Z, sizeof(Z)));

    int z;
    sock.receive(boost::asio::buffer(&z, sizeof(z)));
    printf("Time\tPoint ID\tTemperature\tDisplacement X\n");
    for (int i = 0; i < z; i++) {
        double time, dx, temp;
        sock.receive(boost::asio::buffer(&time, sizeof(time)));
        sock.receive(boost::asio::buffer(&dx, sizeof(dx)));
        sock.receive(boost::asio::buffer(&temp, sizeof(temp)));
        printf("%i\t%3lf\t%2le\t%2lf\n", time, dx, temp);
    }
    sock.close();
}

int main(int argc, char* argv[]) {
    sync_echo();
}