#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>


using namespace std;
using namespace std::chrono;


struct Bid {
    string bidderId;
    double amount;       // bid price
    long long timestamp; // Unix ms
    int auctionId;
};




#pragma region isLess
bool isLess(const Bid&  a, const Bid& b){
    //So sánh theo giá tiền (amount) trước
    if( a.amount != b.amount){
        return a.amount < b.amount;
    }
    //nếu giá bằng nhau thì ai bid trước (timestamp nhỏ hơn) xếp trước
    return a.timestamp<b.timestamp;
}

#pragma endregion

void printBids(const vector<Bid>& v, size_t limit = 10) {
    //in ra danh sách tối đa limit phần tử
    for (size_t i = 0; i < min(v.size(), limit); ++i)
        cout << "[" << v[i].bidderId << "] $" << v[i].amount
             << " @ " << v[i].timestamp << "\n";
    if (v.size() > limit) cout << "... (" << v.size() << " total)\n";
}   

bool loadBids(const string& path, vector<Bid>& out) {
    //Mở file và nạp danh sách bids
    ifstream f(path);
    if (!f) return false;
    Bid b;
    //đọc từng dòng gồm bidderId, amount, timestamp, auctionId
    while (f >> b.bidderId >> b.amount >> b.timestamp >> b.auctionId) {
        out.push_back(b);
    }
    return true;
}

// ---------- TODO: Implement these ----------

// Task A - Insertion Sort (Live Auction Monitor)
void insertionSort(vector<Bid>& v){
    int n =static_cast<int>(v.size()); // int n = (int)v.size();
    //Lấy từng phần tử từ vị trí 1 để tìm chỗ chèn thích hợp
    for(int i=1;i<n;i++){
        Bid key = v[i];
        int j =i-1;
        while(j>=0 && isLess(key,v[j])){
            v[j+1] = v[j];  //Dịch các phần tử lớn hơn key sang bên phải
            --j;
        }
        v[j+1]= key; //chèn key vào đúng vị trí tìm được
    }
}          

// Task B - Selection Sort (Top-K Bid Finder)
void selectionSort(vector<Bid>& v){
    int n = static_cast<int>(v.size()); 
    for(int i=0;i<n-1;i++){
        int idxMax=i;
        //sắp xếp giảm dần descending theo amount
        for(int j=i+1;j<n;j++){
            if(v[j].amount>v[idxMax].amount){
                idxMax = j; //cập nhật vị trí lớn nhất
            }

        }
        if(idxMax!=i){
            swap(v[idxMax],v[i]); //đổi chỗ phần tử lớn nhất về vị trí i
        }

    }

    //In ra top 3 người trả giá cao nhất sau khi đã xếp giảm dần
    cout<<"Top 3 Bidders ( highest amount):\n";
    for(size_t i = 0;i< min(v.size(),(size_t)3);i++){
        cout<< "  " << (i + 1) << ". [" << v[i].bidderId<< "] $"<< v[i].amount<< " @ "<< v[i].timestamp << "\n";
    }

}

// Task C - Interchange Sort (Price Anomaly Validator)
void interchangeSort(vector<Bid>& v){
    int n = static_cast<int>(v.size());
    long long solanswap = 0;
    
    //So sánh từng cặp, sai thứ tự thì đổi chỗ liền
    for(int i=0;i<n-1;i++){
        for(int j=i+1;j<n;j++){
            if(v[j].amount<v[i].amount){
                swap(v[j],v[i]);
                solanswap++; //tăng số lần swap
            }
        }
    }
    cout<<"Interchange sort total swaps : "<<solanswap<<endl;
}    

// Task D - Bubble Sort with Early Stop (Stabilization Detector)
bool bubbleSortEarlyStop(vector<Bid>& v){
    int n = static_cast<int>(v.size());
    bool stable = true; //giả sử ban đầu ổn định

    for(int i=0;i<n-1;i++){
        bool ISswap = false; //giả sử chưa swap
        for(int j = 0;j<n-i-1;j++){
            if(v[j].amount>v[j+1].amount){
                swap(v[j],v[j+1]); //Nếu sai thứ tự thì đổi chỗ
                ISswap=true; //đánh dấu là có swap xảy ra
            }
        }
        //nếu lượt đầu đã có swap thì chưa ổn định nên stable = false;
        if(ISswap&&i==0){
            stable = false;
        }
        if(!ISswap) break; //mảng đã hoàn tất thứ tự thì thoát sớm
    }
    if(stable){
        cout << "STABLE\n";
    }
    else{
        cout << "UNSTABLE\n";
    }
    return stable;

}

// Task E - Quick Sort (Full Historical Sorter)
void medianOfThree(vector<Bid>& v, int lo, int hi){
    //Tìm trung vị trong 3 vị trí đầu, giữa, cuối
    int mid = lo + (hi-lo)/2; //tính vị trí giữa (tránh tràn số)
    if(isLess(v[mid],v[lo])){
        swap(v[lo],v[mid]);
    }
    if(isLess(v[hi],v[lo])){
        swap(v[hi], v[lo]);
    }
    if(isLess(v[hi],v[mid])){
        swap(v[hi],v[mid]);
    }
    // Đưa phần tử trung vị về vị trí hi để phân hoạch Lomuto
    swap(v[mid],v[hi]);
}

int partition(vector<Bid>& v, int lo, int hi){
    //Kiểm tra nếu đoạn có từ 3 phần tử trở lên thì dùng medianOfThree
    if( hi - lo >=2 ){
        medianOfThree(v,lo,hi); 
    }
    //bắt đầu phân loại các phần tử (loại nhỏ và loại lớn so với trung vị)
    Bid pivot =v[hi]; //chọn phần tử ở cuối làm pivot
    int i = lo -1; //chỉ số của phần tử nhỏ hơn pivot

    for(int j=lo;j<hi;j++){
        //pivot >= v[j], gom các phần tử nhỏ hơn hoặc bằng pivot sang bên trái
        if(!isLess(pivot,v[j])){
            i++;
            swap(v[i],v[j]);

        }
    }
    swap(v[i+1],v[hi]); //đưa pivot về đúng giữa hai vùng
    return i+1;
    
}

void quickSort(vector<Bid>& v, int lo, int hi){
    if(lo<hi){
        int p = partition(v,lo,hi); //phân hoạch mảng và lấy vị trí pivot
        quickSort(v,lo,p-1); //đệ quy bên trái (phần nhỏ)
        quickSort(v,p+1,hi); // đệ quy bên phải (phần lớn)
    }
}

void quickSortWrapper(vector<Bid>& v){
    //Hàm bọc gọi nhanh quickSort cho cả mảng
    if(!v.empty()){
        quickSort(v,0,static_cast<int>(v.size())-1);
    }
}



int main(int argc, char* argv[]) {
    // Dữ liệu kiểm thử mẫu từ test_data/small.txt (hoặc khởi tạo mặc định nếu chưa có file)
    vector<Bid> smallBids;
    if(!loadBids("test_data/small.txt", smallBids) || smallBids.empty()){
        smallBids = {
            {"alice",100.0,1000,1},
            {"bob",102.5,1005,1},
            {"carol",101.0, 1003, 1},
            {"dave", 103.0, 1010,1}
        };
    }

    cout << "TASK A: INSERTION SORT \n";
    vector<Bid> vA = smallBids;
    insertionSort(vA);
    cout << "Sorted ascending by amount (tie-break timestamp):\n";
    printBids(vA);

    cout << "\nTASK B: SELECTION SORT\n";
    vector<Bid> vB = smallBids;
    selectionSort(vB);

    cout << "\nTASK C: INTERCHANGE SORT\n";
    vector<Bid> vC = smallBids;
    interchangeSort(vC);
    printBids(vC);

    cout << "\nTASK D: BUBBLE SORT EARLY STOP\n";
    vector<Bid> vD_stable = vA; // vA đã có thứ tự tăng dần hoàn chỉnh
    cout << "Test 1 (Already-sorted Input): ";
    bubbleSortEarlyStop(vD_stable);

    vector<Bid> vD_reversed = vA;
    reverse(vD_reversed.begin(), vD_reversed.end()); // mảng đảo ngược
    cout << "Test 2 (Reversed Input):       ";
    bubbleSortEarlyStop(vD_reversed);

    cout << "\nTASK E: QUICKSORT (100,000 BIDS)\n";
    const int N = 100000;
    vector<Bid> largeBids;
    largeBids.reserve(N);

    // Nạp từ test_data/large.txt hoặc sinh ngẫu nhiên bằng mt19937
    if (!loadBids("test_data/large.txt", largeBids) || (int)largeBids.size() < N) {
        largeBids.clear();
        largeBids.reserve(N);
        mt19937 rng(1337);
        uniform_real_distribution<double> distAmount(1.0, 10000.0);
        uniform_int_distribution<long long> distTime(1600000000000LL, 1700000000000LL);

        for (int i = 0; i < N; ++i) {
            largeBids.push_back({"bidder_" + to_string(i), distAmount(rng), distTime(rng), 1});
        }
    }

    // Đo thời gian chạy bằng std::chrono
    auto startTime = high_resolution_clock::now();
    quickSortWrapper(largeBids);
    auto endTime = high_resolution_clock::now();

    auto durationMs = duration_cast<milliseconds>(endTime - startTime).count();
    cout << "Sorted " << N << " records using QuickSort in: " << durationMs << " ms\n";

    // Kiểm tra tính đúng đắn của thứ tự sau sắp xếp
    bool isSorted = true;
    for (size_t i = 1; i < largeBids.size(); ++i) {
        if (isLess(largeBids[i], largeBids[i - 1])) {
            isSorted = false;
            break;
        }
    }
    cout << "Verification: " << (isSorted ? "PASSED (Array is correctly sorted)" : "FAILED") << "\n";

    return 0;
}