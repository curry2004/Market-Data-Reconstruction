#include <bits/stdc++.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <format>  
#include <iomanip>
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")

using namespace std;
typedef long long ll;
typedef long double ld;
struct MBO {
    string ts_recv,ts_event,rtype,publisher_id,instrument_id,action,side,price,size,channel_id,order_id,flags,ts_in_delta,sequence,symbol;
};
struct MBP10{
    string ts_recv,ts_event,rtype,publisher_id,instrument_id,action,side;
    int depth;
    string price,size,flags,ts_in_delta,sequence,bid_px_00,bid_sz_00,bid_ct_00,ask_px_00,ask_sz_00,ask_ct_00,bid_px_01,bid_sz_01,bid_ct_01,ask_px_01,ask_sz_01,ask_ct_01,bid_px_02,bid_sz_02,bid_ct_02,ask_px_02,ask_sz_02,ask_ct_02,bid_px_03,bid_sz_03,bid_ct_03,ask_px_03,ask_sz_03,ask_ct_03,bid_px_04,bid_sz_04,bid_ct_04,ask_px_04,ask_sz_04,ask_ct_04,bid_px_05,bid_sz_05,bid_ct_05,ask_px_05,ask_sz_05,ask_ct_05,bid_px_06,bid_sz_06,bid_ct_06,ask_px_06,ask_sz_06,ask_ct_06,bid_px_07,bid_sz_07,bid_ct_07,ask_px_07,ask_sz_07,ask_ct_07,bid_px_08,bid_sz_08,bid_ct_08,ask_px_08,ask_sz_08,ask_ct_08,bid_px_09,bid_sz_09,bid_ct_09,ask_px_09,ask_sz_09,ask_ct_09,symbol;
};

map<ld,int> bids;
map<ld,int> asks;
map<ld,int> bid_orders,ask_orders;
set<ld,greater<ld>> bid_prices;
set<ld> ask_prices;
MBO InputMBO(stringstream &ss) {
    string ts_recv,ts_event,rtype,publisher_id,instrument_id,action,side,price,size,channel_id,order_id,flags,ts_in_delta,sequence,symbol;
    getline(ss, ts_recv, ',');
    getline(ss, ts_event, ',');
    getline(ss, rtype, ',');
    getline(ss, publisher_id, ',');
    getline(ss, instrument_id, ',');
    getline(ss, action, ',');
    getline(ss, side, ',');
    getline(ss, price, ',');
    getline(ss, size, ',');
    getline(ss, channel_id, ',');
    getline(ss, order_id, ',');
    getline(ss, flags, ',');
    getline(ss, ts_in_delta, ',');
    getline(ss, sequence,',');
    getline(ss, symbol);    
    return {ts_recv, ts_event, rtype, publisher_id,instrument_id, action, side,price,size,channel_id,order_id,flags,ts_in_delta,sequence,symbol};

}
void update_book(MBO &mbo,MBP10 &mbp) {
    string action=mbo.action,side=mbo.side;
    if(action=="A"){
        if(side=="B"){
            ld price=stold(mbo.price);
            int size=stoi(mbo.size);
            bids[price]+=size;
            bid_prices.insert(price);
            int depth = distance(bid_prices.begin(), find_if(bid_prices.begin(), bid_prices.end(), [&](ld p) { return p <= price; }));
            mbp.depth = depth;
            bid_orders[price]+=1;
        }else{
            ld price=stold(mbo.price);
            int size=stoi(mbo.size);
            asks[price]+=size;
            ask_prices.insert(price);
            int depth = distance(ask_prices.begin(), find_if(ask_prices.begin(), ask_prices.end(), [&](ld p) { return p >= price; }));
            mbp.depth = depth;
            ask_orders[price]+=1;
        }
    }
    else if(action=="C"){
        if(side=="B"){
            ld price=stold(mbo.price);
            int size=stoi(mbo.size);
            bids[price]-=size;
            int depth = distance(bid_prices.begin(), find_if(bid_prices.begin(), bid_prices.end(), [&](ld p) { return p <= price; }));
            mbp.depth = depth;
            if(bids[price]==0)bid_prices.erase(price);
            bid_orders[price]-=1;
        }else{
            ld price=stold(mbo.price);
            int size=stoi(mbo.size);
            asks[price]-=size;
            int depth = distance(ask_prices.begin(), find_if(ask_prices.begin(), ask_prices.end(), [&](ld p) { return p >= price; }));
            mbp.depth = depth;
            if(asks[price]==0)ask_prices.erase(price);
            ask_orders[price]-=1;
        }
    }
    else if(action=="R"){
        mbp.depth=0;
    }
    else if(action=="T" && side!="N"){
        if(side=="A"){
            ld price=stold(mbo.price);
            int size=stoi(mbo.size);
            bids[price]-=size;
            int depth = distance(bid_prices.begin(), find_if(bid_prices.begin(), bid_prices.end(), [&](ld p) { return p <= price; }));
            mbp.depth = depth;
            if(bids[price]==0)bid_prices.erase(price);
            bid_orders[price]-=1;
        }
        else if(side=="B"){
            ld price=stold(mbo.price);
            int size=stoi(mbo.size);
            asks[price]-=size;
            int depth = distance(ask_prices.begin(), find_if(ask_prices.begin(), ask_prices.end(), [&](ld p) { return p >= price; }));
            mbp.depth = depth;
            if(asks[price]==0)ask_prices.erase(price);
            ask_orders[price]-=1;
        }
    }
}
void top10_bids_and_asks(ofstream &fout) {
    auto it_bid = bid_prices.begin();
    auto it_ask = ask_prices.begin();
    int n1= bid_prices.size();
    int n2= ask_prices.size();
    int i=0;
    while(i<10){
        ld price_bid=*it_bid;
        ld price_ask=*it_ask;              
        if(i<n1 && price_bid>0){
            fout << "," << fixed << setprecision(2) << price_bid << "," << bids[price_bid] << "," << bid_orders[price_bid];
            it_bid++;
            
        }
        else fout<<",,0,0";
        if(i<n2 && price_ask>0){
            fout << "," << fixed << setprecision(2) << price_ask << "," << asks[price_ask] << "," << ask_orders[price_ask];
            it_ask++;
        }
        else fout<<",,0,0";
        i++;
    }

}
void MBP10_reconstruct(MBO &mbo,MBP10 &mbp,ofstream &fout,int &cnt) {
    if(mbp.depth<=11){
        fout<<cnt-1<<","<<mbo.ts_event<<","<<mbo.ts_event<<","<<10<<","<<mbo.publisher_id<<","<<mbo.instrument_id<<","<<mbo.action<<","<<mbo.side<<","<<mbp.depth<<","<<mbo.price<<","<<mbo.size<<","<<mbo.flags<<","<<mbo.ts_in_delta<<","<<mbo.sequence;
        top10_bids_and_asks(fout);
        fout<<","<<mbo.symbol<<","<<mbo.order_id;
        fout << "\n";
    }
    else cnt--;
}
void TFC_compression(MBO &mbo,MBP10 &mbp,ofstream &fout,int &cnt) {
    if(mbp.depth<=11){if(mbo.side=="A"){
        fout<<cnt-1<<","<<mbo.ts_event<<","<<mbo.ts_event<<","<<10<<","<<mbo.publisher_id<<","<<mbo.instrument_id<<","<<mbo.action<<","<<"B"<<","<<mbp.depth<<","<<mbo.price<<","<<mbo.size<<","<<mbo.flags<<","<<mbo.ts_in_delta<<","<<mbo.sequence;
        top10_bids_and_asks(fout);
        fout<<","<<mbo.symbol<<","<<mbo.order_id;
        fout << "\n";
    }
    else if(mbo.side=="B"){
        fout<<cnt-1<<","<<mbo.ts_event<<","<<mbo.ts_event<<","<<10<<","<<mbo.publisher_id<<","<<mbo.instrument_id<<","<<mbo.action<<","<<"A"<<","<<mbp.depth<<","<<mbo.price<<","<<mbo.size<<","<<mbo.flags<<","<<mbo.ts_in_delta<<","<<mbo.sequence;
        top10_bids_and_asks(fout);
        fout<<","<<mbo.symbol<<","<<mbo.order_id;
        fout << "\n";
    }}
    else cnt--;
}
void print_memory_usage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        SIZE_T rss = pmc.WorkingSetSize;
        SIZE_T peak_rss = pmc.PeakWorkingSetSize;
        std::cerr << "Memory Usage (RSS): " << rss / 1024 << " KB\n";
        std::cerr << "Peak Memory Usage (HWM): " << peak_rss / 1024 << " KB\n";
    } else {
        std::cerr << "Failed to get memory usage.\n";
    }
}

int32_t main(int argc, char* argv[]){
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    auto start = chrono::high_resolution_clock::now();
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    string filename = argv[1]; 
    
    // string filename="mbo.csv";
    ifstream file(filename);
    ofstream fout("mbp_reconstructed.csv");
    fout << ",ts_recv,ts_event,rtype,publisher_id,instrument_id,action,side,depth,price,size,flags,ts_in_delta,sequence";
    for (int i = 0; i < 10; ++i)
        fout << ",bid_px_" << setw(2) << setfill('0') << i << ",bid_sz_" << setw(2) << i << ",bid_ct_" << setw(2) << i<< ",ask_px_" << setw(2) << setfill('0') << i << ",ask_sz_" << setw(2) << i << ",ask_ct_" << setw(2) << i;
    fout << ",symbol,order_id";
    fout << "\n";
    
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }
    string line;
    int flag_T=0;
    int cnt=-1;
    while (getline(file, line)) {
        stringstream ss(line);
        MBO mbo = InputMBO(ss); 
        MBP10 mbp;
        cnt++;
        if(mbo.action=="T" && mbo.side!="N"){
            update_book(mbo,mbp);
            TFC_compression(mbo,mbp,fout,cnt);
            for (int i = 0; i < 2; ++i) {
                if (!getline(file, line)) break; // Ensure we don't go out of bounds
            }
            continue;
        }
        if(!cnt)continue;
        update_book(mbo,mbp);
        MBP10_reconstruct(mbo, mbp, fout, cnt);
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end - start;
    std::cerr << "Time taken: " << diff.count() << " s\n";
    print_memory_usage();
}



