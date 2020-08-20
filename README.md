# Administrative Note

As of upstream version 5.6.1, I'm moving away from individual repositories for each upstream version in favor of a single repository with version-based branches.  Hopefully, this will help with clutter and URL consistency moving forward.  The archived repositories are available here:
* [rtl88x2BU_WiFi_linux_v5.3.1_27678.20180430_COEX20180427-5959](https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.3.1_27678.20180430_COEX20180427-5959)
* [rtl88x2BU_WiFi_linux_v5.2.4.4_26334.20180126_COEX20171012-5044](https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.2.4.4_26334.20180126_COEX20171012-5044)
* [rtl88x2BU_WiFi_linux_v5.2.4.4_25643.20171212_COEX20171012-5044](https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.2.4.4_25643.20171212_COEX20171012-5044)
* [rtl88x2BU_WiFi_linux_v5.2.4.1_22719_COEX20170518-4444.20170613](https://github.com/cilynx/rtl88x2BU_WiFi_linux_v5.2.4.1_22719_COEX20170518-4444.20170613)

# Driver for rtl88x2bu wifi adaptors

Updated driver for rtl88x2bu wifi adaptors based on Realtek's source distributed with myriad adapters.

Realtek's 5.6.1 source was found bundled with the [Cudy WU1200 AC1200 High Gain USB Wi-Fi Adapter](https://amzn.to/351ADVq) and can be downloaded from [Cudy's website](http://www.cudytech.com/productinfo/517558.html).

Build confirmed on:

```
Linux 5.6.0-gentoo #1 SMP Tue Mar 31 09:56:02 JST 2020 GenuineIntel GNU/Linux gcc (Gentoo 9.3.0 p1) 9.3.0
```
```
Linux DELL_XPS_UBUNTU_20.04 5.4.0-42-generic #46-Ubuntu SMP Fri Jul 10 00:24:02 UTC 2020 x86_64 x86_64 x86_64 GNU/Linux
```
```
Linux ThinkPad-P52 5.4.0-9-generic #12-Ubuntu SMP Mon Dec 16 22:34:19 UTC 2019 x86_64 x86_64 x86_64 GNU/Linux
```
```
Linux version 5.3.0-050300-generic (kernel@sita) (gcc version 9.2.1 20190909 (Ubuntu 9.2.1-8ubuntu1)) #201909152230 SMP Sun Sep 15 22:32:54 UTC 2019
```
```
Linux version 5.2.0-2-amd64 (debian-kernel@lists.debian.org) (gcc version 8.3.0 (Debian 8.3.0-21)) #1 SMP Debian 5.2.9-2 (2019-08-21)
```
## DKMS installation

```bash
cd rtl88x2bu
VER=$(sed -n 's/\PACKAGE_VERSION="\(.*\)"/\1/p' dkms.conf)
sudo rsync -rvhP ./ /usr/src/rtl88x2bu-${VER}
sudo dkms add -m rtl88x2bu -v ${VER}
sudo dkms build -m rtl88x2bu -v ${VER}
sudo dkms install -m rtl88x2bu -v ${VER}
sudo modprobe 88x2bu
```

## Raspberry Pi Access Point

```bash
# Update all packages per normal
sudo apt update
sudo apt upgrade

# Install prereqs
sudo apt install git dnsmasq hostapd bc build-essential dkms raspberrypi-kernel-headers

# Reboot just in case there were any kernel updates
sudo reboot

# Pull down the driver source
git clone https://github.com/cilynx/rtl88x2bu
cd rtl88x2bu/

# Configure for RasPi
sed -i 's/I386_PC = y/I386_PC = n/' Makefile
sed -i 's/ARM_RPI = n/ARM_RPI = y/' Makefile

# DKMS as above
VER=$(sed -n 's/\PACKAGE_VERSION="\(.*\)"/\1/p' dkms.conf)
sudo rsync -rvhP ./ /usr/src/rtl88x2bu-${VER}
sudo dkms add -m rtl88x2bu -v ${VER}
sudo dkms build -m rtl88x2bu -v ${VER} # Takes ~3-minutes on a 3B+
sudo dkms install -m rtl88x2bu -v ${VER}

# Plug in your adapter then confirm your new interface name
ip addr

# Set a static IP for the new interface (adjust if you have a different interface name or preferred IP)
sudo tee -a /etc/dhcpcd.conf <<EOF
interface wlan1
    static ip_address=192.168.4.1/24
    nohook wpa_supplicant
EOF

# Clobber the default dnsmasq config
sudo tee /etc/dnsmasq.conf <<EOF
interface=wlan1
  dhcp-range=192.168.4.100,192.168.4.199,255.255.255.0,24h
EOF

# Configure hostapd
sudo tee /etc/hostapd/hostapd.conf <<EOF
interface=wlan1
driver=nl80211
ssid=pinet
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=CorrectHorseBatteryStaple
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
EOF

sudo sed -i 's|#DAEMON_CONF=""|DAEMON_CONF="/etc/hostapd/hostapd.conf"|' /etc/default/hostapd

# Enable hostapd
sudo systemctl unmask hostapd
sudo systemctl enable hostapd

# Reboot to pick up the config changes
sudo reboot
```

If you want 802.11an speeds 144Mbps you could use this config below:
```
# Configure hostapd
sudo tee /etc/hostapd/hostapd.conf <<EOF
interface=wlx74ee2ae24062
driver=nl80211
ssid=borg

macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=toe54321
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP

hw_mode=a
channel=36
wmm_enabled=1

country_code=US

require_ht=1
ieee80211ac=1
require_vht=1

#This below is supposed to get us 867Mbps and works on rtl8814au doesn't work on this driver yet
#vht_oper_chwidth=1
#vht_oper_centr_freq_seg0_idx=157

ieee80211n=1
ieee80211ac=1
EOF

$ iwconfig 
wlx74ee2ae24062  IEEE 802.11an  ESSID:"borg"  Nickname:"<WIFI@REALTEK>"
          Mode:Master  Frequency:5.18 GHz  Access Point: 74:EE:2A:E2:40:62   
          Bit Rate:144.4 Mb/s   Sensitivity:0/0  
          Retry:off   RTS thr:off   Fragment thr:off
          Power Management:off
          Link Quality=0/100  Signal level=-100 dBm  Noise level=0 dBm
          Rx invalid nwid:0  Rx invalid crypt:0  Rx invalid frag:0
          Tx excessive retries:0  Invalid misc:0   Missed beacon:0

```
If you want to setup masquerading or bridging, check out [the official Raspberry Pi docs](https://www.raspberrypi.org/documentation/configuration/wireless/access-point.md).
