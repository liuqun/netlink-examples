#include <cstdio>
#include <QtNetwork>
#include <QHostAddress>
#include <QDebug>
#include <QString>
#include <QObject>

static
void PrintAllInterfaces(void)
{
    //获取所有网络接口(Interface)的列表
    for (auto netInterface: QNetworkInterface::allInterfaces())
    {
        //设备名
        using std::printf;
        printf("Device: %s\n", netInterface.name().toStdString().c_str());

        //MAC地址
        qDebug() << "\tMAC(Hardware address):" << netInterface.hardwareAddress();

        //遍历每一个IP地址(每个包含一个IP地址，一个子网掩码和一个广播地址)
        for(auto entry: netInterface.addressEntries())
        {
            //IP地址(同时支持IPv4和IPv6地址)
            qDebug() << "\tIP(v4/v6) Address:" << entry.ip().toString();

            //子网掩码
            qDebug() << "\tNetmask:" << entry.netmask().toString();

            //广播地址
            qDebug() << "\tBroadcast:" << entry.broadcast().toString();
        }
    }
}

int main()
{
    PrintAllInterfaces();
    return 0;
}
// vi: set ts=4 sw=4 expandtab :
