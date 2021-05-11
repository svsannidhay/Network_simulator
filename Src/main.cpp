///////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <queue>
#include <map>
#include <math.h>
#include <climits>
#include <set>
#include <cstring>
#include <unordered_map>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iomanip>
#include <cmath>
#include <bitset>
#include <stdlib.h>
#include <chrono>


///////////////////////////////////////////////////////////////////////////////////////////////////
#define fio  ios_base::sync_with_stdio(false);cin.tie(NULL);
#define ll  long long int
#define ull unsigned long long int
#define cinll(x) ll x;cin >> x;
#define cini(x) int x;cin >> x;
#define cins(x) string x;cin >> x;
#define vect(x) vector<ll> x
#define vect1(x) vector<ll> x;x.push_back(0);
#define pb(x) push_back(x)
#define mp(x, y) make_pair(x, y)
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX 1e17
#define MIN -9223372036854775800
#define MOD 1000000007
#define f first
#define s second
///////////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;
using u64 = uint64_t;
//Safe_hashing for minimising collisions 
//https://codeforces.com/blog/entry/62393
struct custom_hash {
    static uint64_t splitmix64(uint64_t x) {
        // http://xorshift.di.unimi.it/splitmix64.c
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }
    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED_RANDOM = chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
};

///////////////////////////////////////// DEVICES SUPPORTED ////////////////////////////////////////
class Device{
  public:
    ll global_index;
    string mac_address;
};

class Hub{
  public:
    ll global_index;
    string mac_address;
};

class Switch{
  public:
    ll global_index;
    string mac_address;
    // Hashed Switch table for O(1) look up time 
    // Mapping mac address to the port of the switch
    unordered_map<string,ll> switch_table;
};

class Bridge{
  public:
    ll global_index;
    string mac_address;
    // Hashed Switch table for O(1) look up time
    // Mapping mac address to the port of the switch
    unordered_map<string, ll> bridge_table;
};
//////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////MAC ADDRESS GENERATION //////////////////////////////////////////
// currently supporting 65,565 different addresses 
vector< string > mac_address_list;
ll mac_index;

string to_hex(ll x) {
    string hex;
    if(x <= 9) {
        hex = to_string(x);
    }
    if(x == 10) hex = "A";
    if(x == 11) hex = "B";
    if(x == 12) hex = "C";
    if(x == 13) hex = "D";
    if(x == 14) hex = "E";
    if(x == 15) hex = "F";
    return hex;
}

string create_add(string add,ll i,ll j,ll k,ll l) {
    add += to_hex(i);
    add += to_hex(j);
    add.pb(':');
    add += to_hex(k);
    add += to_hex(l);
    return add;
}

void generate_mac_address() {
    ll count = 0;
    string add = "00:AA:BB:";
    for(ll i = 0;i < 16; i++) {
        for(ll j = 0;j < 16; j++) {
            for(ll k = 0; k < 16; k++) {
                for(ll l = 0; l < 16; l++) {
                    string full_add = create_add(add,i,j,k,l);
                    mac_address_list.pb(full_add);
                }
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////Physical Topology//////////////////////////////////////////////

// Network 
//Saving the whole topology as a graph (adjacency list)
vector< ll > connections[1001];

// Adding connections 
// duplex connection 
void addEdge(ll u,ll v) {
    connections[u].pb(v);
    connections[v].pb(u);
}

// we will map each device to its type
unordered_map< ll , pair<string,ll> > device_type;


// Caching the data of each device 
vector<Device> device_list;
ll device_no;
vector<Hub> hub_list;
ll hub_no;
vector<Bridge> bridge_list;
ll bridge_no;
vector<Switch> switch_list;
ll switch_no;


//////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////DEBUGGER///////////////////////////////////////////////

void dfs(ll current_device, vector<bool> & visited) {
    if(!visited[current_device]) {
        visited[current_device] = true;

        string type = device_type[current_device].f;
        if( type == "device") {
            Device d = device_list[device_type[current_device].s];
            cout<<"global index : "<<d.global_index<<"\n";
            cout<<"mac address : "<<d.mac_address<<"\n";
            cout<<"device type : "<<type<<"\n\n";
        } else if(type == "hub") {
            Hub h = hub_list[device_type[current_device].s];
            cout<<"global index : "<<h.global_index<<"\n";
            cout<<"mac address : "<<h.mac_address<<"\n";
            cout<<"device type : "<<type<<"\n\n";
        } else if(type == "switch") {
            Switch s = switch_list[device_type[current_device].s];
            cout<<"global index : "<<s.global_index<<"\n";
            cout<<"mac address : "<<s.mac_address<<"\n";
            cout<<"device type : "<<type<<"\n\n";
        } else if(type == "bridge") {
            Bridge b = bridge_list[device_type[current_device].s];
            cout<<"global index : "<<b.global_index<<"\n";
            cout<<"mac address : "<<b.mac_address<<"\n";
            cout<<"device type : "<<type<<"\n\n";
        }

        for(ll i = 0;i < connections[current_device].size(); i++) {
            if(!visited[connections[current_device][i]]) {
                dfs(connections[current_device][i],visited);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////SEND PACKETS (BOTH DATA AND ACK) /////////////////////////////////


void send_packet(ll current_device, vector<bool> & visited, ll ind_prev , string senders_mac,string destination_mac,bool isAck) {
    if(!visited[current_device]) {
        visited[current_device] = true;
        
        string type = device_type[current_device].f;
        // If the device is switch or bridge we will cache port and source in the caching table first
        if(type == "switch" || type == "bridge") {
            // First lets indetify the port on which our source is 
            ll port = -1;
            for(ll i = 0;i < connections[current_device].size(); i++) {
                if (connections[current_device][i] == ind_prev) {
                    port = i+1;
                    break;
                }
            }

            if(type == "switch") {
                Switch &s = switch_list[device_type[current_device].s];
                if(port != -1) { 
                    s.switch_table[senders_mac] = port;
                    // cout<<s.global_index<<"\n";
                    // cout<<s.mac_address<<"\n";
                    // cout<<"SWITCHING TABLE \n";
                    // for(auto it = s.switch_table.begin();it!= s.switch_table.end();it++) {
                    //     cout<<it->first<<" "<<it->second<<"\n"; 
                    // }
                }
                ll destPort = 0;
                destPort = s.switch_table[destination_mac];

                if(destPort != 0) {

                    cout<<"Device Type : "<< type<<"\n";
                    cout<<"Global Index : "<<s.global_index<<"\n";
                    cout<<"Mac address : "<<s.mac_address<<"\n";
                    if(isAck) cout<<"isACK \n";
                    cout<<"Sending to : "<<connections[current_device][destPort-1]<<"\n\n";

                    send_packet(connections[current_device][destPort-1],visited,current_device,senders_mac,destination_mac,isAck);
                } else {
                    //Broadcast
                    cout<<"SWITCH IS BROADCASTING : \n\n";
                    for(ll i = 0;i < connections[current_device].size(); i++) {
                        if(!visited[connections[current_device][i]]) {
                            cout<<"Device Type : "<< type<<"\n";
                            cout<<"Global Index : "<<s.global_index<<"\n";
                            cout<<"Mac address : "<<s.mac_address<<"\n";
                            if(isAck) cout<<"isACK \n";
                            cout<<"Sending to : "<<connections[current_device][i]<<"\n\n";
                            send_packet(connections[current_device][i],visited,current_device,senders_mac,destination_mac,isAck);
                        }
                    }
                }

            }

            if(type == "bridge") {
                Bridge &b = bridge_list[device_type[current_device].s];
                if(port != -1) b.bridge_table[senders_mac] = port;
                ll destPort = 0;
                destPort = b.bridge_table[destination_mac];
                if(destPort != 0) {
                    cout<<"Device Type : "<< type<<"\n";
                    cout<<"Global Index : "<<b.global_index<<"\n";
                    cout<<"Mac address : "<<b.mac_address<<"\n";
                    if(isAck) cout<<"isACK \n";
                    cout<<"Sending to : "<<connections[current_device][destPort-1]<<"\n\n";
                    send_packet(connections[current_device][destPort-1],visited,current_device,senders_mac,destination_mac,isAck);
                } else {
                    //Broadcast
                    for(ll i = 0;i < connections[current_device].size(); i++) {
                        if(!visited[connections[current_device][i]]) {
                            cout<<"Device Type : "<< type<<"\n";
                            cout<<"Global Index : "<<b.global_index<<"\n";
                            cout<<"Mac address : "<<b.mac_address<<"\n";
                            if(isAck) cout<<"isACK \n";
                            cout<<"Sending to : "<<connections[current_device][i]<<"\n\n";
                            send_packet(connections[current_device][i],visited,current_device,senders_mac,destination_mac,isAck);
                        }
                    }
                }
            }

            return;

        }      

        // if the device is Hub or a dedicated connection directly send
        Hub h;
        Device d;
        // CHECK IF WE ARE DESTINATION OR NOT
        d = device_list[device_type[current_device].s];
        if(type == "device" && d.mac_address == destination_mac && !isAck) {
            cout<<"Data packet recieved sucessfully sending back ACK";
            vector<bool> visited(10001,false);
            cout<<"\nSENDING ACK FROM "<< destination_mac<<"  to  "<<senders_mac<<"\n\n";
            send_packet(current_device,visited,-1,destination_mac,senders_mac,true);
            return;
        } else if(type == "device" && d.mac_address == destination_mac) {
            cout<<"ACK recieved sucessfully \n\n";
        }
        for(ll i = 0;i < connections[current_device].size(); i++) {
            if(!visited[connections[current_device][i]]) {
                if(type == "hub") {
                    h = hub_list[device_type[current_device].s];
                    cout<<"Device Type : "<< type<<"\n";
                    cout<<"Global Index : "<<h.global_index<<"\n";
                    cout<<"Mac address : "<<h.mac_address<<"\n";
                    if(isAck) cout<<"isACK \n";
                    cout<<"Sending to : "<<connections[current_device][i]<<"\n\n";
                } 
                if(type == "device") {
                    d = device_list[device_type[current_device].s];
                    cout<<"Device Type : "<< type<<"\n";
                    cout<<"Global Index : "<<d.global_index<<"\n";
                    cout<<"Mac address : "<<d.mac_address<<"\n";
                    if(isAck) cout<<"isACK \n";
                    cout<<"Sending to : "<<connections[current_device][i]<<"\n\n";

                    if(d.mac_address == destination_mac && !isAck) {
                        cout<<"Data packet recieved sucessfully sending back ACK";
                        vector<bool> visited(10001,false);
                        cout<<"\nSENDING ACK FROM "<< destination_mac<<"  to  "<<senders_mac<<"\n\n";
                        send_packet(current_device,visited,-1,destination_mac,senders_mac,true);
                        return;
                    } else if(d.mac_address == destination_mac) {
                        cout<<"ACK recieved sucessfully";
                    }
                }
                send_packet(connections[current_device][i],visited,current_device,senders_mac,destination_mac,isAck);
            }
        }
    }
}

    
//////////////////////////////////////////////////////////////////////////////////////////////////

// Network booter 

void boot() {
    cinll(n);
    cinll(m);
    for(ll i = 1;i <= n; i++) {
        cinll(ind);
        cins(type);
        if(type == "device" ) {
            Device d;
            d.global_index = ind;
            d.mac_address = mac_address_list[mac_index];
            device_list.pb(d);
            device_type[ind] = mp(type,device_no);
            device_no++;
            mac_index++;
            cout<< " mac assigned to device "<<ind<<" is : "<<d.mac_address<<"\n";
        } else if(type == "hub") {
            Hub h;
            h.global_index = ind;
            h.mac_address = mac_address_list[mac_index];
            hub_list.pb(h);
            device_type[ind] = mp(type,hub_no);
            hub_no++;
            mac_index++;
            cout<< " mac assigned to device "<<ind<<" is : "<<h.mac_address<<"\n";
        } else if(type == "bridge") {
            Bridge b;
            b.global_index = ind;
            b.mac_address = mac_address_list[mac_index];
            bridge_list.pb(b);
            device_type[ind] = mp(type,bridge_no);
            bridge_no++;
            mac_index++;
            cout<< " mac assigned to device "<<ind<<" is : "<<b.mac_address<<"\n";
        } else if(type == "switch") {
            Switch s;
            s.global_index = ind;
            s.mac_address = mac_address_list[mac_index];
            switch_list.pb(s);
            device_type[ind] = mp(type,switch_no);
            switch_no++;
            mac_index++;
            cout<< " mac assigned to device "<<ind<<" is : "<<s.mac_address<<"\n";
        }
    }

    cout<<"\n";
    // cout<<"Devices : ";
    // for(ll i=0;i<device_list.size();i++) {
    //     cout<<device_list[i].global_index<<" ";
    // }
    // cout<<"\n";
    // cout<<"Hubs : ";
    // for(ll i=0;i<hub_list.size();i++) {
    //     cout<<hub_list[i].global_index<<" ";
    // }
    // cout<<"\n";
    // cout<<"Bridges : ";
    // for(ll i=0;i<bridge_list.size();i++) {
    //     cout<<bridge_list[i].global_index<<" ";
    // }
    // cout<<"\n";
    // cout<<"Switches  : ";
    // for(ll i=0;i<switch_list.size();i++) {
    //     cout<<switch_list[i].global_index<<" ";
    // }
    // cout<<"\n";
    for(ll i = 0;i < m; i++) {
        cinll(u);cinll(v);
        addEdge(u,v);
    }
    vector<bool> visited(n+1,false);
    dfs(1,visited);

}

void run_network() {
    // Resolving Queries
    // With no access control protocol  
    // cinll(q);
    // for(ll i = 0;i < q; i++) {
    //     // a -> sender 
    //     // b -> receiver 
    //     // n -> no of packets
    //     cinll(a);cinll(b);cinll(n);
    //     // a and b will always be devices

    //     Device ad = device_list[device_type[a].s];
    //     Device bd = device_list[device_type[b].s]; 
        
    //     // sending packets one by one

    //     for(ll i = 0;i < n; i++) {
            
    //         vector<bool> visited(10001,false);
    //         cout<<"\nSENDING PACKET FROM "<< ad.mac_address<<"  to  "<<bd.mac_address<<"\n\n";
    //         send_packet(a,visited,-1,ad.mac_address,bd.mac_address,false);  

    //         cout<<"\n";

            
    //         for(ll i=0;i<switch_list.size();i++) {
    //             cout<<"SWITCHING TABLE \n";
    //             Switch s = switch_list[i];
    //             cout<<"Switch's global address : "<<s.global_index<<"\n";
    //             cout<<"Switch's mac address : "<<s.mac_address<<"\n";
                
    //             for(auto it = s.switch_table.begin();it!= s.switch_table.end();it++) {
    //                 cout<<it->first<<" "<<it->second<<"\n"; 
    //             }
    //             cout<<"\n";
    //         } 

    //         for(ll i=0;i < bridge_list.size();i++) {
    //             cout<<"Bridge TABLE \n";
    //             Bridge b = bridge_list[i];
    //             cout<<"Switch's global address : "<<b.global_index<<"\n";
    //             cout<<"Switch's mac address : "<<b.mac_address<<"\n";
                
    //             for(auto it = b.bridge_table.begin();it!= b.bridge_table.end();it++) {
    //                 cout<<it->first<<" "<<it->second<<"\n"; 
    //             }

    //             cout<<"\n";
    //         }

    //     }
    // }

    // with reservation (controlled access protocol) 

    // what well will do is first lets create all the reservation frames 
    // for that we will input all the queries first then run them offline 

    // We are storing it like set of queue's each queue corresponds to a device and in front of 
    //each queue contains the pair of destination and no of packets to send 
    // if no of packets became zero we pop that entry 
    queue< pair<string,ll> > reservation_frame[device_list.size() + 1];

    cinll(q);
    while(q--) {
        cinll(a);cinll(b);cinll(n);

        ll a_device_index = device_type[a].s;
        ll b_device_index = device_type[b].s;

        Device ad = device_list[a_device_index];
        Device bd = device_list[b_device_index]; 

        reservation_frame[a_device_index].push(mp(bd.mac_address,n));
    }

    // Sending packets according to reservation frames 
    bool remaining = true;
    while(remaining) {
        ll sent = false;
        for(ll i = 0;i < device_list.size(); i++) {
            if(reservation_frame[i].size() > 0) {

                Device sender = device_list[i];

                vector<bool> visited(10001,false);
                cout<<"\nSENDING PACKET FROM "<< sender.mac_address<<"  to  "<<reservation_frame[i].front().f<<"\n\n";
                send_packet(sender.global_index,visited,-1,sender.mac_address,reservation_frame[i].front().f,false);  
                sent = true;
                reservation_frame[i].front().s--;
                if(reservation_frame[i].front().s == 0) {
                    reservation_frame[i].pop();
                }

                cout<<"\n";

                
                for(ll i=0;i<switch_list.size();i++) {
                    cout<<"SWITCHING TABLE \n";
                    Switch s = switch_list[i];
                    cout<<"Switch's global address : "<<s.global_index<<"\n";
                    cout<<"Switch's mac address : "<<s.mac_address<<"\n";
                    
                    for(auto it = s.switch_table.begin();it!= s.switch_table.end();it++) {
                        cout<<it->first<<" "<<it->second<<"\n"; 
                    }
                    cout<<"\n";
                } 

                for(ll i=0;i < bridge_list.size();i++) {
                    cout<<"Bridge TABLE \n";
                    Bridge b = bridge_list[i];
                    cout<<"Switch's global address : "<<b.global_index<<"\n";
                    cout<<"Switch's mac address : "<<b.mac_address<<"\n";
                    
                    for(auto it = b.bridge_table.begin();it!= b.bridge_table.end();it++) {
                        cout<<it->first<<" "<<it->second<<"\n"; 
                    }

                    cout<<"\n";
                }

            }
        }
        remaining = sent;
    }

}

void solve() {
    generate_mac_address();
    boot();
    run_network();
    return;
}    

int main(){
    // fio;
    // cinll(t);
///////////////////////////////////////////    
    #ifndef ONLINE_JUDGE
    freopen("input.txt" , "r", stdin);
    freopen("output.txt", "w" , stdout);
    #endif
///////////////////////////////////////////
    // cinll(t);
    // for(ll i=1;i<=t;i++) {
        solve();
    // }
  return 0;
}