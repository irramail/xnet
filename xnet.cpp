//
// xnet.cpp
//
#include <iostream>
#include <fstream>
#include <vector>
#include "xlib++/display.hpp"
#include "xlib++/window.hpp"
#include "xlib++/graphics_context.hpp"
#include "xlib++/command_button.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

using namespace xlib;
class main_window;
static unsigned int level_Y = 0;
static std::string eth, ip, mask, gw, dns, mac, proxy, port, login, passwd, modem, usb, prov;

const std::string FNDMESG = "/tmp/eth.rm";
const std::string FNINTERFACES = "/etc/network/interfaces";
const std::string FNRESOLV = "/etc/resolv.conf";
const std::string FNCRONTAB = "/etc/crontab";
const std::string FNCHAP = "/etc/ppp/chap-secrets";
const std::string FNPAP = "/etc/ppp/pap-secrets";
const std::string FNTMPXSESSIONRC = "/tmp/.xsessionrc";
const std::string FNRESXSESSIONRC = "/home/uid0001/.xsessionrc";

void saveConfig() {
  std::vector<std::string> ixsessionrc;
  if(proxy.length()) {
    std::string proxy_str, tmp;
    proxy_str = "http_proxy=http://";
    if (login.length())
      proxy_str += login;
    if (passwd.length())
      proxy_str += ":" + passwd;
    if (login.length())
      proxy_str.push_back('@');
    proxy_str += proxy + ":" + port + "/ ";

    std::ifstream ifile;
    ifile.open(".xsessionrc");
    while(ifile.good()) {
      std::getline(ifile, tmp);
      if(tmp.find("DISPLAY=:0.0 ~/main &") != std::string::npos)
	tmp =  proxy_str + "DISPLAY=:0.0 ~/main &";
      if(tmp.find("DISPLAY=:0.0 ~/mp/test &") != std::string::npos)
	tmp =  proxy_str + "DISPLAY=:0.0 ~/mp/test &";
      if(tmp.find("DISPLAY=:0.0 ~/updated &") != std::string::npos)
	tmp =  proxy_str + "DISPLAY=:0.0 ~/updated &";

      if(tmp.length())
	ixsessionrc.push_back(tmp);
    }
    ifile.close();
  }
  else {
    std::string tmp;
    std::ifstream ifile;
    ifile.open(".xsessionrc");
    while(ifile.good()) {
      std::getline(ifile, tmp);
      if(tmp.find("DISPLAY=:0.0 ~/main &") != std::string::npos)
	tmp = "DISPLAY=:0.0 ~/main &";
      if(tmp.find("DISPLAY=:0.0 ~/mp/test &") != std::string::npos)
	tmp = "DISPLAY=:0.0 ~/mp/test &";
      if(tmp.find("DISPLAY=:0.0 ~/updated &") != std::string::npos)
	tmp = "DISPLAY=:0.0 ~/updated &";

      if(tmp.length())
	ixsessionrc.push_back(tmp);
    }
    ifile.close();
  }

  std::ofstream ofilexs;
  ofilexs.open(".xsessionrc");
  for(std::vector<std::string>::iterator it = ixsessionrc.begin(); it != ixsessionrc.end(); it++)
    ofilexs << *it <<std::endl;
  ofilexs.close();

  if ( eth.length() && ip.length() && mask.length() && gw.length() && dns.length() && !modem.length() && !usb.length() & !prov.length()) {
    //    std::cout << eth << " " << ip << " " << mask << " " << gw << " " << dns << std::endl;

    std::ofstream of_networks;

    of_networks.open("/tmp/interfaces");
    
    of_networks << "auto lo" << std::endl;
    of_networks << "iface lo inet loopback" << std::endl;
    of_networks << std::endl;
    
    of_networks << "allow-hotplug eth" <<  eth << std::endl;
    of_networks << "iface eth" << eth << " inet static" << std::endl;
    if (mac.size() > 7 )
      of_networks << "\thwaddress ether " << mac << std::endl;
    if (ip.size() > 0 )
      of_networks << "\taddress " << ip << std::endl;
    if (mask.size() > 0 )
      of_networks << "\tnetmask " << mask << std::endl;
    if (gw.size() > 0 )
    of_networks << "\tgateway " << gw << std::endl;
    if (dns.size() > 0 )
      of_networks << "\tdns-nameservers " << dns << std::endl;
    
    of_networks.close();
    
    of_networks.open("/tmp/resolv.conf");
    of_networks << "nameserver " << dns << std::endl;
    of_networks.close();

    std::ofstream ofile;
    ofile.open("/tmp/netinter.sh");
    ofile << "#!/bin/sh" << std::endl;
    ofile << "xterm -e sudo cp -f /tmp/interfaces " << FNINTERFACES << std::endl;
    ofile << "xterm -e sudo cp -f /tmp/resolv.conf " << FNRESOLV << std::endl;
    ofile.close();
    
    pid_t child;
    
    if (!(child = fork())) {
      execlp("/bin/sh", "/bin/sh", "/tmp/netinter.sh", NULL);
      exit(0);
    }
    wait(0);
  }

  if ( eth.length() && ip.empty() && mask.empty() && gw.empty() && dns.empty() && modem.empty() && usb.empty() & prov.empty()) {
    //    std::cout << eth << std::endl;
    
    std::ofstream of_networks;
    
    of_networks.open("/tmp/interfaces");
    
    of_networks << "auto lo" << std::endl;
    of_networks << "iface lo inet loopback" << std::endl;
    of_networks << std::endl;
    
    of_networks << "allow-hotplug eth" <<  eth << std::endl;
    of_networks << "iface eth" << eth << " inet dhcp" << std::endl;
    if (mac.size() > 7 )
      of_networks << "\thwaddress ether " << mac << std::endl;
    
    of_networks.close();

    std::ofstream ofile;
    ofile.open("/tmp/netinter.sh");
    ofile << "#!/bin/sh" << std::endl;
    ofile << "xterm -e sudo cp -f /tmp/interfaces " << FNINTERFACES  << std::endl;
    ofile.close();
    
    pid_t child;
    
    if (!(child = fork())) {
      execlp("/bin/sh", "/bin/sh", "/tmp/netinter.sh", NULL);
      exit(0);
    }
    wait(0);
  }
  
  if ( modem.length() && usb.length() && prov.length() && !eth.length() && !ip.length() && !mask.length() && !gw.length() && !dns.length()) {
    //    std::cout << modem << " " << usb << " " << prov << std::endl;

    std::string fnwvdial, wvdial_str;
    fnwvdial = "/tmp/netwvdial.conf";

    std::ofstream owdial;
    std::ofstream of_networks;

    if (modem == "1") {
      owdial.open(fnwvdial.c_str());

      owdial << "[Dialer Defaults]" << std::endl;
      owdial << "Init2 = ATQ0 V1 E1 S0=0 &C1 &D2 +FCLASS=0" << std::endl;
      owdial << "Modem Type = Analog Modem" << std::endl;
      owdial << "Phone = #777" << std::endl;
      owdial << "ISDN = 0" << std::endl;
      owdial << "Username = cdma" << std::endl;
      owdial << "Init1 = ATZ" << std::endl;
      owdial << "Password = cdma" << std::endl;
      owdial << "Modem = /dev/ttyUSB" << usb << std::endl;
      owdial << "Baud = 460800" << std::endl;

      owdial.close();

    }

    if (modem == "2") {
      owdial.open(fnwvdial.c_str());

      owdial << "[Dialer Defaults]" << std::endl;
      owdial << "Init2 = ATQ0 V1 E1 S0=0 &C1 &D2 +FCLASS=0" << std::endl;
      owdial << "Modem Type = Analog Modem" << std::endl;
      owdial << "Phone = #777" << std::endl;
      owdial << "ISDN = 0" << std::endl;
      owdial << "Username = cdma" << std::endl;
      owdial << "Init1 = ATZ" << std::endl;
      owdial << "Password = cdma" << std::endl;
      owdial << "Modem = /dev/ttyACM" << usb << std::endl;
      owdial << "Baud = 460800" << std::endl;

      owdial.close();
    }

    if (modem == "3") {
      owdial.open(fnwvdial.c_str());
      owdial << "[Dialer Defaults]" << std::endl;
      owdial << "Modem = /dev/ttyUSB" << usb << std::endl;
      owdial << "Baud = 460800" << std::endl;
      owdial << "Init1 = ATZ" << std::endl;
      owdial << "Init2 = ATQ0 V1 E1 S0=0 &C1 &D2 +FCLASS=0" << std::endl;
      owdial << "Init3 = AT+CGDCONT=1,\"IP\",\"internet.mts.ru\"" << std::endl;
      owdial << "Modem Type = USB Modem" << std::endl;
      owdial << "Dial Command = ATDT" << std::endl;
      owdial << "ISDN = no" << std::endl;
      owdial << "Timeout = 120" << std::endl;
      owdial << "Idle Seconds = 500" << std::endl;
      owdial << "Stupid Mode = yes" << std::endl;
      owdial << "Phone = *99#" << std::endl;
      owdial << "Username = mts" << std::endl;
      owdial << "Password = mts" << std::endl;
      owdial << "New PPPD = yes" << std::endl;

      owdial.close();
    }

    std::ofstream ofile;
    ofile.open("/tmp/netwvdial.sh");
    ofile << "#!/bin/sh" << std::endl;
    ofile << "xterm -e sudo cp -f " << fnwvdial  << " /etc/wvdial.conf" << std::endl;
    ofile << "xterm -e sudo sh -c \"echo '* * * * *          root    /usr/bin/wvdial' >> " << FNCRONTAB << "\"" << std::endl;
    ofile << "xterm -e sudo sh -c \"echo 'internet        *        mobile' >> " << FNCHAP << "\"" << std::endl;
    ofile << "xterm -e sudo sh -c \"echo 'cdma        *        cdma' >> " << FNCHAP << "\"" << std::endl;
    ofile << "xterm -e sudo sh -c \"echo 'internet        *        mobile' >> " << FNPAP << "\"" << std::endl;
    ofile << "xterm -e sudo sh -c \"echo 'cdma        *        cdma' >> " << FNPAP << "\"" << std::endl;
    ofile << "xterm -e sudo sh -c \"echo 'auto lo' > " << FNINTERFACES << "\"" << std::endl;
    ofile << "xterm -e sudo sh -c \"echo 'iface lo inet loopback' >> " << FNINTERFACES << "\"" << std::endl;
    ofile.close();
    
    pid_t child;
    
    if (!(child = fork())) {
      execlp("/bin/sh", "/bin/sh", "/tmp/netwvdial.sh", NULL);
      exit(0);
    }
    wait(0);
    /*
    of_networks.open(FNCRONTAB.c_str(), std::ios::out | std::ios::app);
    of_networks << "* * * * * \t root\t /usr/bin/wvdial" << std::endl;
    of_networks.close();

    of_networks.open(FNCHAP.c_str(), std::ios::out | std::ios::app);
    of_networks << "internet \t* \tmobile" << std::endl;
    of_networks << "cdma \t* \tcdma" << std::endl;
    of_networks.close();

    of_networks.open(FNPAP.c_str(), std::ios::out | std::ios::app);
    of_networks << "internet \t* \tmobile" << std::endl;
    of_networks << "cdma \t* \tcdma" << std::endl;
    of_networks.close();

    of_networks.open(FNINTERFACES.c_str());
    of_networks << "auto lo" << std::endl;
    of_networks << "iface lo inet loopback" << std::endl;
    of_networks << std::endl;
    of_networks.close();
    */
  }
}

class hello_button : public command_button
{
public:
  hello_button ( main_window& w );
  ~hello_button(){}

  void on_click();
  void on_key_press(character c) {

    if (c.is_backspace_key())
      {
        if (get_name().length() > 1)
          set_name(get_name().substr(0,get_name().length()-2) + "|");
          std::string result;
          result = get_name().substr(0, get_name().length() - 1);

          if ( get_id() == 0 ) eth = result;
          if ( get_id() == 1 ) ip = result;
          if ( get_id() == 2 ) mask = result;
          if ( get_id() == 3 ) gw = result;
          if ( get_id() == 4 ) mac = result;
          if ( get_id() == 5 ) dns = result;
          if ( get_id() == 6 ) proxy = result;
          if ( get_id() == 7 ) port = result;
          if ( get_id() == 8 ) login = result;
          if ( get_id() == 9 ) passwd = result;
          if ( get_id() == 10 ) modem = result;
          if ( get_id() == 11 ) usb = result;
          if ( get_id() == 12 ) prov = result;
      }

    if (c.is_printable())
      {
	bool change = true;
	//          ethN             modemN           usbN               provN
	if (get_id() == 0 || get_id() == 10 || get_id() == 11 || get_id() == 12)
          if ((get_name().length() >= 3) || (! isdigit(c.get_text()[0])))
	    change = false;

	//       proxy port
        if (get_id() == 7)
          if ((get_name().length() >= 6) || (! isdigit(c.get_text()[0])))
            change = false;

	//        login            passwoed
        if (get_id() == 8 || get_id() == 9)
          if (get_name().length() >= 17)
            change = false;

	//           ip              mask              gw              dns              proxy
        if (get_id() == 1 || get_id() == 2 || get_id() == 3 || get_id() == 5 || get_id() == 6)
          if ((get_name().length() >= 16) || ((! isdigit(c.get_text()[0])) && c.get_text() != "."))
            change = false;

	//          mac
        if (get_id() == 4)
          if ((get_name().length() >= 18) || ((! isdigit(c.get_text()[0])) && c.get_text() != ":" && c.get_text() != "a" && c.get_text() != "b" && c.get_text() != "c" && c.get_text() != "d" && c.get_text() != "f"))
            change = false;

	if(!(eth.empty() && ip.empty() && mask.empty() && gw.empty() && mac.empty() && dns.empty() && proxy.empty() && port.empty() && login.empty() && passwd.empty()) && (get_id() == 10 || get_id() == 11 || get_id() == 12))
	  change = false;

	if(eth.empty() && (get_id() == 1 || get_id() == 4))
	  change = false;

	if(ip.empty() && (get_id() == 2))
	  change = false;

	if(mask.empty() && (get_id() == 3))
	  change = false;

	if(proxy.empty() && (get_id() == 7))
	  change = false;

	if((port.empty()) && (get_id() == 8 || get_id() == 9))
	  change = false;

	if(login.empty() && (get_id() == 9))
	  change = false;

	if(!(modem.empty() && usb.empty() && prov.empty()) && (get_id() < 10))
	  change = false;

	if (get_name()=="0|" && change == true && get_id() != 4)
	  set_name("|");
        if (change) {
	  add2name(c.get_text());
          std::string result;
          result = get_name().substr(0, get_name().length() - 1);

          if ( get_id() == 0 ) eth = result;
          if ( get_id() == 1 ) ip = result;
          if ( get_id() == 2 ) mask = result;
          if ( get_id() == 3 ) gw = result;
          if ( get_id() == 4 ) mac = result;
          if ( get_id() == 5 ) dns = result;
          if ( get_id() == 6 ) proxy = result;
          if ( get_id() == 7 ) port = result;
          if ( get_id() == 8 ) login = result;
          if ( get_id() == 9 ) passwd = result;
          if ( get_id() == 10 ) modem = result;
          if ( get_id() == 11 ) usb = result;
          if ( get_id() == 12 ) prov = result;
	}
      }
  }

private:
  main_window& m_parent;
};

class hello_rd_button : public command_button
{
public:
  hello_rd_button ( main_window& w );
  ~hello_rd_button(){}

  void on_click();

private:
  main_window& m_parent;
};

class main_window : public window
{
 public:
  main_window ( event_dispatcher& e )
    : window ( e )
  {
    m_ETH = new hello_button ( *this );
    m_rd_ETH = new hello_rd_button ( *this );
    m_IP = new hello_button ( *this );
    m_rd_IP = new hello_rd_button ( *this );
    m_MASK = new hello_button ( *this );
    m_rd_MASK = new hello_rd_button ( *this );
    m_GW = new hello_button ( *this );
    m_rd_GW = new hello_rd_button ( *this );
    m_MAC = new hello_button ( *this );
    m_rd_MAC = new hello_rd_button ( *this );
    m_DNS = new hello_button ( *this );
    m_rd_DNS = new hello_rd_button ( *this );
    m_PROXY = new hello_button ( *this );
    m_rd_PROXY = new hello_rd_button ( *this );
    m_PORT = new hello_button ( *this );
    m_rd_PORT = new hello_rd_button ( *this );
    m_LOGIN = new hello_button ( *this );
    m_rd_LOGIN = new hello_rd_button ( *this );
    m_PASSWD = new hello_button ( *this );
    m_rd_PASSWD = new hello_rd_button ( *this );
    m_MODEM = new hello_button ( *this );
    m_rd_MODEM = new hello_rd_button ( *this );
    m_USB = new hello_button ( *this );
    m_rd_USB = new hello_rd_button ( *this );
    m_PROV = new hello_button ( *this );
    m_rd_PROV = new hello_rd_button ( *this );
    m_SAVE = new hello_button ( *this );
    m_rd_SAVE = new hello_rd_button ( *this );
    m_EXIT = new hello_button ( *this );
    m_rd_EXIT = new hello_rd_button ( *this );

    m_rd_ETH->set_name("ETH[0,1,...,99]");
    m_rd_IP->set_name("IP[192.168.0.9]");
    m_rd_MASK->set_name("MASK[255.255.255.0]");
    m_rd_GW->set_name("GW[192.168.0.1]");
    m_rd_MAC->set_name("MAC[00:_:_:_:_:_]");
    m_rd_DNS->set_name("DNS[8.8.8.8]");
    m_rd_PROXY->set_name("Proxy[192.168.0.1]");
    m_rd_PORT->set_name("Port[3128, 8080]");
    m_rd_LOGIN->set_name("Login[user]");
    m_rd_PASSWD->set_name("Pasword[passwd]");
    m_rd_MODEM->set_name("AD/AP/MTS+Mn[1/2/3]");
    m_rd_USB->set_name("USB0,U1,U2[0/1/2]");
    m_rd_PROV->set_name("PROV[0,1,2,3]");
    m_rd_SAVE->set_name("Save");
    m_rd_EXIT->set_name("Exit");
    m_SAVE->hide();
    m_EXIT->hide();
  }
  ~main_window(){ delete m_ETH, m_IP, m_MASK, m_GW, m_DNS, m_MAC, m_PROXY, m_PORT, m_LOGIN, m_PASSWD, m_MODEM, m_USB, m_PROV, m_SAVE, m_EXIT, m_rd_ETH, m_rd_IP, m_rd_MASK, m_rd_GW, m_rd_DNS, m_rd_MAC, m_rd_PROXY, m_rd_PORT, m_rd_LOGIN, m_rd_PASSWD, m_rd_MODEM, m_rd_USB, m_rd_PROV, m_rd_SAVE, m_rd_EXIT; }

  void on_hello_click() { }
  void on_key_press(character c) { }
  void set_name(std::string name) { }

private:

  hello_button *m_ETH, *m_IP, *m_MASK, *m_GW, *m_DNS, *m_MAC, *m_PROXY, *m_PORT, *m_LOGIN, *m_PASSWD, *m_MODEM, *m_USB, *m_PROV, *m_SAVE, *m_EXIT;
  hello_rd_button *m_rd_ETH, *m_rd_IP, *m_rd_MASK, *m_rd_GW, *m_rd_DNS, *m_rd_MAC, *m_rd_PROXY, *m_rd_PORT, *m_rd_LOGIN, *m_rd_PASSWD, *m_rd_MODEM, *m_rd_USB, *m_rd_PROV, *m_rd_SAVE, *m_rd_EXIT;
};


//
// Hello button
//
hello_button::hello_button ( main_window& w )
  : command_button ( w, rectangle(point(150, level_Y*35 + 20),125,30 ), "|" ),
    m_parent ( w )
{ set_id(level_Y); }
void hello_button::on_click() { m_parent.on_hello_click(); }

hello_rd_button::hello_rd_button ( main_window& w )
  : command_button ( w, rectangle(point(20, level_Y*35 + 20),125,30 ), "" ),
    m_parent ( w )
{ set_id(level_Y++); }
void hello_rd_button::on_click() { if (get_id() == 14) exit(0); if (get_id() == 13) saveConfig(); }

main()
{
      display d("");

      event_dispatcher events ( d );
      main_window w ( events ); // top-level
      events.run();
}

