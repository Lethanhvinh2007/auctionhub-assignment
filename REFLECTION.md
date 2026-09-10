# BÁO CÁO ĐÁNH GIÁ VÀ TỔNG KẾT (REFLECTION.md)
**Môn học**: Data Structures and Algorithms  
**Dự án**: AuctionHub Sorting Practice Lab  
**Học viên**: Lê Thanh Vinh (25110075)  

---

## 1. Biện minh thực tế cho từng giải thuật (Real-World Justification)

* **Module A: Live Auction Monitor → Insertion Sort**  
  Trong luồng đấu giá trực tiếp, các lượt bid đến liên tục và mỗi bid mới thường chỉ nhỉnh hơn giá trước đó một khoảng nhỏ (~0.1%), tạo ra dữ liệu gần như đã có thứ tự sẵn (nearly-sorted). Insertion Sort là lựa chọn hoàn hảo vì trên dữ liệu gần sắp xếp, nó chỉ tốn rất ít phép so sánh và dịch chuyển, đạt độ phức tạp thời gian $O(n)$ cùng $O(1)$ bộ nhớ phụ, giúp hệ thống cập nhật thứ tự tức thời với độ trễ cực thấp.

* **Module B: Top-K Bid Finder → Selection Sort**  
  Đối với widget hiển thị "Top 3 Bidders", hệ thống chỉ cần trích xuất $K = 3$ giá trị lớn nhất mà không nhất thiết phải tốn chi phí sắp xếp toàn diện toàn bộ danh sách. Selection Sort tìm giá trị cực trị và đặt trực tiếp vào vị trí đích qua từng lượt, do đó khi tìm Top-$K$, giải thuật chỉ cần chạy $K$ vòng lặp ngoài với chi phí $O(K \cdot n)$ hoàn toàn tại chỗ (in-place), không phát sinh thêm bất kỳ vector hay vùng nhớ phụ nào.

* **Module C: Price Anomaly Validator → Interchange Sort**  
  Module kiểm thử QA chỉ làm việc với các lô dữ liệu kích thước rất nhỏ ($\le 50$ bản ghi) trước khi lưu vào database. Với $n \le 50$, tổng số phép duyệt và hoán vị tối đa chỉ khoảng 2,500 phép toán (thực thi trong tích tắc dưới 1 ms), do đó độ phức tạp lý thuyết $O(n^2)$ không hề là điểm nghẽn, trong khi Interchange Sort có cấu trúc mã nguồn ngắn gọn, trực quan, dễ kiểm toán mã (code audit) và không có nguy cơ tràn ngăn xếp như giải thuật đệ quy.

* **Module D: Stabilization Detector → Bubble Sort (Early Stop)**  
  Mục tiêu của module là phát hiện xem cửa sổ giá 10 phút đã ổn định (không còn biến động giá nào) hay chưa. Bubble Sort có cờ dừng sớm (`swapped`) kiểm tra ngay lượt duyệt đầu tiên: nếu không có bất kỳ cặp phần tử nghịch thế nào cần đổi chỗ trên toàn bộ mảng, hàm lập tức kết luận mảng đã đạt trạng thái ổn định (`STABLE`) chỉ với đúng 1 lượt duyệt $O(n)$.

* **Module E: Full Historical Sorter → Quick Sort**  
  Báo cáo thống kê cuối ngày cần tổng hợp và sắp xếp hàng triệu bản ghi đấu giá lịch sử, đòi hỏi giải thuật phải tối ưu hóa triệt để tốc độ và tận dụng bộ nhớ đệm (cache locality). Quick Sort có chi phí trung bình $O(n \log n)$ với hệ số hằng số ẩn (constant factor) rất nhỏ, kết hợp chiến lược chọn pivot `median-of-three` giúp triệt tiêu nguy cơ rơi vào phân hoạch suy biến trên dữ liệu thực tế.

---

## 2. Kết quả đo đạc thời gian chạy Quick Sort trên 100,000 bản ghi (Measured Runtime)

* **Số lượng bản ghi**: $100,000$ đối tượng `Bid` (sinh ngẫu nhiên bằng `std::mt19937` và nạp qua `loadBids`).
* **Môi trường đo**: g++ 15.2.0 (C++17) trên hệ điều hành Windows qua VS Code terminal.
* **Thời gian thực thi giải thuật (`quickSortWrapper`)**:
  - **Chế độ Debug** (không cờ tối ưu, có debug symbols): **378 ms** (ghi nhận thực tế trên terminal chụp tại `screenshot_runtime.png`).
  - **Chế độ Release** (biên dịch cờ tối ưu `-O2`): **~31 – 44 ms** (trung bình ~37 ms).
* **Xác minh tính đúng đắn (Verification)**: `PASSED` (Toàn bộ 100,000 phần tử đều thỏa mãn thứ tự tăng dần theo `amount`, hòa so sánh theo `timestamp`).

---

## 3. Phân tích lỗi gặp phải và cách chẩn đoán (Bug Encountered & Diagnosis)

* **Hiện tượng Bug**:  
  Khi lần đầu chạy thử nghiệm Task E với $N = 100,000$, chương trình in ra thời gian chạy là `0 ms` và dòng kiểm tra in ra `Verification: PASSED` một cách bất thường.
* **Nguyên nhân**:  
  Khi rà soát lại hàm `main()`, tôi nhận thấy mình chỉ khai báo `largeBids.reserve(N);`. Hàm `reserve()` trong C++ STL chỉ cấp phát trước dung lượng vùng nhớ đệm (capacity) chứ không làm thay đổi kích thước thực tế của vector (`size()` vẫn bằng 0). Do vector rỗng, hàm `quickSortWrapper` không chạy phân hoạch nào, và vòng lặp kiểm tra `for (size_t i = 1; i < largeBids.size(); ++i)` bị bỏ qua, dẫn đến thông báo pass ảo.
* **Cách khắc phục**:  
  Bổ sung vòng lặp sinh dữ liệu thực tế với `largeBids.push_back(...)` kết hợp các phân phối ngẫu nhiên `std::uniform_real_distribution` và `std::uniform_int_distribution`, đảm bảo vector chứa đủ 100,000 phần tử trước khi gọi Quick Sort.

---

## 4. Ba câu hỏi đã tham khảo từ LLM và phân loại (Prompt Discipline)

1. **Prompt 1 (Green Zone)**:  
   *"Giải thích tại sao Insertion Sort lại đạt độ phức tạp O(n) khi dữ liệu đầu vào gần như đã có thứ tự (nearly-sorted)?"*  
   * **Đánh giá**: **Green Zone** (Hoàn toàn hợp lệ). Câu hỏi thuần túy về mặt lý thuyết giải thuật, giúp hiểu rõ cơ chế số phép so sánh và dịch chuyển phần tử tối thiểu trong vòng lặp `while`.

2. **Prompt 2 (Green Zone)**:  
   *"Kỹ thuật chọn pivot median-of-three trong Quick Sort hoạt động như thế nào và tại sao nó loại bỏ được trường hợp suy biến trên mảng đã sắp xếp?"*  
   * **Đánh giá**: **Green Zone** (Hoàn toàn hợp lệ). Câu hỏi hỗ trợ hiểu bản chất phân hoạch và toán học của thuật toán, hỗ trợ tự viết logic cho hàm `medianOfThree`.

3. **Prompt 3 (Yellow Zone)**:  
   *"Cho tôi ví dụ snippet code C++ sử dụng std::mt19937 và uniform_real_distribution để sinh số thực ngẫu nhiên trong khoảng từ 1.0 đến 10000.0."*  
   * **Đánh giá**: **Yellow Zone** (Thận trọng - Được phép theo quy định đề bài). Đây là câu hỏi cú pháp thư viện tiện ích (utility) để chuẩn bị dữ liệu kiểm thử, không can thiệp vào mã logic của 5 thuật toán chính.

---

## 5. Phân tích độ phức tạp trường hợp xấu nhất của Quick Sort với Median-of-Three

* **Độ phức tạp worst-case**: Vẫn là **$O(n^2)$**.
* **Giải thích nguyên nhân**:
  1. **Bản chất phép lấy mẫu cục bộ**: Chiến lược `median-of-three` chỉ lấy trung vị của đúng 3 phần tử (đầu mảng `lo`, giữa mảng `mid`, và cuối mảng `hi`). Đối với tập dữ liệu lớn, giá trị trung vị của 3 phần tử này hoàn toàn có thể không phản ánh đúng trung vị thực sự của toàn bộ mảng con.
  2. **Tập dữ liệu đối kháng (Adversarial Input)**: Một đối thủ có thể chủ động tạo ra một chuỗi dữ liệu đặc thù (được gọi là *median-of-three killer sequence*, được nhà khoa học máy tính David Musser công bố), trong đó tại mỗi tầng phân hoạch đệ quy, trung vị của 3 điểm được chọn luôn rơi vào phần tử cực tiểu hoặc cực đại của mảng con. Khi đó, mỗi bước phân hoạch chỉ loại bỏ được 1 hoặc 2 phần tử, khiến độ sâu đệ quy đạt $O(n)$ và tổng thời gian là $O(n^2)$.
  3. **Trường hợp mảng chứa nhiều khóa trùng nhau (Duplicate Keys)**: Khi toàn bộ dữ liệu có cùng một giá trị, kỹ thuật phân hoạch Lomuto (so sánh `<= pivot`) sẽ dồn tất cả phần tử về một phía, gây mất cân bằng tối đa và suy biến về $O(n^2)$.
* **Kết luận**: `median-of-three` giải quyết triệt để trường hợp xấu nhất phổ biến trong thực tế (mảng đã sắp xếp sẵn hoặc đảo ngược hoàn toàn), nhưng về mặt toán học lý thuyết, nó **không thể** đảm bảo chặn trên $O(n \log n)$ trong mọi trường hợp (muốn đảm bảo tuyệt đối cần dùng giải thuật hybrid như IntroSort để chuyển sang HeapSort khi đệ quy quá sâu).
