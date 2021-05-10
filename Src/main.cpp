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
    unordered_map<string,ll> swicth_table;
};

class Bridge{
  public:
    ll global_index;
    string mac_address;
    // Hashed Switch table for O(1) look up time
    // Mapping mac address to the port of the switch
    unordered_map<string, ll> swicth_table;
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


void solve() {
    generate_mac_address();
    boot();
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