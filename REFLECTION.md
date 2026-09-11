# BÁO CÁO BÀI TẬP (REFLECTION.md)
**Môn học**: Cấu trúc dữ liệu và Giải thuật  
**Bài tập**: AuctionHub - Sorting Practice Lab  
**Sinh viên**: Lê Thanh Vinh (MSSV: 25110075)  

---

## 1. Lý do lựa chọn giải thuật cho từng module thực tế

* **Module A: Live Auction Monitor → Insertion Sort**  
  Trong luồng đấu giá trực tiếp, các lượt đặt giá (bid) mới gửi lên thường có xu hướng nhỉnh hơn giá trước đó một chút (~0.1%), nên dữ liệu đưa vào gần như đã có thứ tự sẵn (nearly-sorted). Insertion Sort là lựa chọn tốt nhất ở đây vì với dữ liệu gần sắp xếp, các phần tử mới chỉ cần so sánh và dịch chuyển rất ít bước, đạt thời gian chạy O(n) và tốn O(1) bộ nhớ, giúp hệ thống cập nhật giá mới ngay lập tức với độ trễ cực thấp.

* **Module B: Top-K Bid Finder → Selection Sort**  
  Để hiển thị widget "Top 3 Bidders", hệ thống chỉ cần lấy ra 3 lượt bid cao nhất chứ không cần tốn công sắp xếp toàn bộ danh sách. Selection Sort mỗi lượt quét sẽ tìm phần tử lớn nhất đưa về đúng vị trí đầu, do đó để lấy Top 3 ta chỉ cần chạy đúng 3 vòng lặp ngoài. Thuật toán chạy trực tiếp trên mảng hiện có (in-place) với chi phí O(K * n), hoàn toàn không cần cấp phát thêm vector phụ nào.

* **Module C: Price Anomaly Validator → Interchange Sort**  
  Công cụ kiểm tra lỗi giá chỉ chạy trên các đợt dữ liệu nhỏ (tối đa 50 bản ghi) trước khi lưu vào cơ sở dữ liệu. Với kích thước nhỏ (n <= 50), chi phí O(n^2) cũng chỉ tốn tối đa khoảng 2500 phép so sánh và chạy trong chưa đầy 1 ms. Điểm mạnh của Interchange Sort là code rất ngắn gọn, trực quan, dễ rà soát lỗi và không có nguy cơ tràn bộ nhớ như các thuật toán đệ quy phức tạp.

* **Module D: Stabilization Detector → Bubble Sort (Early Stop)**  
  Mục đích của module này là phát hiện xem luồng giá trong 10 phút đã ổn định (không còn biến động) hay chưa. Bubble Sort có cờ kiểm tra tráo đổi (swapped flag) nên nếu dữ liệu vốn đã được sắp xếp từ trước, thuật toán sẽ không thực hiện lần swap nào trong lượt duyệt đầu tiên và dừng ngay lập tức, trả về trạng thái STABLE chỉ sau đúng 1 lần quét O(n).

* **Module E: Full Historical Sorter → Quick Sort**  
  Báo cáo tổng kết cuối ngày phải xử lý hàng triệu bản ghi lịch sử đấu giá nên hiệu năng là ưu tiên hàng đầu. Quick Sort có thời gian chạy trung bình O(n log n) rất nhanh và tận dụng bộ nhớ đệm tốt. Việc kết hợp thêm kỹ thuật chọn pivot lấy trung vị 3 phần tử (median-of-three) giúp giải thuật tránh bị rơi vào trường hợp chạy chậm O(n^2) khi gặp dữ liệu đã có thứ tự từ trước.

---

## 2. Kết quả đo đạc thời gian chạy Quick Sort trên 100,000 bản ghi

* **Dữ liệu kiểm thử**: 100,000 bản ghi Bid (sinh ngẫu nhiên bằng mt19937).
* **Môi trường chạy**: Trình biên dịch g++ 15.2.0 (C++17) trên Windows, chạy qua VS Code terminal.
* **Thời gian đo được**:
  - **Chạy ở chế độ Debug (VS Code Task outDebug.exe)**: khoảng **378 ms** (kết quả hiển thị trong ảnh screenshot_runtime.png).
  - **Chạy khi biên dịch cờ tối ưu (-O2)**: chỉ mất khoảng **31 - 44 ms** (trung bình ~37 ms).
* **Kiểm tra tính đúng đắn (Verification)**: `PASSED` (toàn bộ 100,000 phần tử đều đúng thứ tự tăng dần theo giá amount, nếu trùng giá thì so tiếp timestamp tăng dần).

---

## 3. Lỗi gặp phải trong quá trình làm bài và cách khắc phục

* **Lỗi gặp phải**:  
  Lúc đầu khi chạy thử Task E với 100,000 phần tử, chương trình chạy xong ngay lập tức và in thời gian là `0 ms`, sau đó dòng kiểm tra in `Verification: PASSED` rất bất thường.
* **Nguyên nhân**:  
  Khi xem lại hàm `main()`, tôi thấy mình mới chỉ gọi lệnh `largeBids.reserve(N);`. Hàm `reserve` trong C++ chỉ cấp phát trước dung lượng bộ nhớ đệm chứ chưa hề thêm phần tử nào vào vector, làm cho `size()` của vector vẫn bằng 0. Vì vector rỗng nên Quick Sort không xử lý gì, và vòng lặp kiểm tra mảng cũng bị bỏ qua dẫn đến kết quả pass giả.
* **Cách khắc phục**:  
  Tôi đã viết vòng lặp dùng lệnh `largeBids.push_back(...)` kết hợp với `std::mt19937` để thực sự tạo và nạp đủ 100,000 bản ghi ngẫu nhiên vào vector. Sau khi sửa, chương trình đo được thời gian chạy thực tế 378 ms và kiểm tra thứ tự thành công.

---

## 4. Ba câu hỏi đã tham khảo từ AI và phân loại theo quy định

1. **Prompt 1 (Green Zone)**:  
   *"Tại sao Insertion Sort lại chạy rất nhanh với độ phức tạp O(n) khi dữ liệu đầu vào gần như đã có thứ tự (nearly-sorted)?"*  
   * **Đánh giá**: **Green Zone** (Được phép). Đây là câu hỏi lý thuyết để hiểu bản chất của thuật toán sắp xếp chèn khi số lượng phép dịch chuyển phần tử ở mức tối thiểu.

2. **Prompt 2 (Green Zone)**:  
   *"Kỹ thuật chọn pivot median-of-three trong Quick Sort hoạt động thế nào và nó giúp tránh trường hợp xấu nhất ra sao?"*  
   * **Đánh giá**: **Green Zone** (Được phép). Câu hỏi giúp tìm hiểu nguyên lý toán học và cách phân hoạch để tự tay viết code cho hàm `medianOfThree`.

3. **Prompt 3 (Yellow Zone)**:  
   *"Cho mình xin đoạn code mẫu C++ dùng std::mt19937 và uniform_real_distribution để sinh số thực ngẫu nhiên từ 1.0 đến 10000.0."*  
   * **Đánh giá**: **Yellow Zone** (Được phép ở mức độ công cụ phụ trợ). Đây chỉ là câu hỏi về cú pháp thư viện sinh dữ liệu test ngẫu nhiên, không hỏi code giải thuật chính của bài tập.

---

## 5. Phân tích trường hợp xấu nhất của Quick Sort khi dùng Median-of-Three

* **Độ phức tạp worst-case**: Vẫn là **O(n^2)**.
* **Giải thích nguyên nhân**:
  1. **Hạn chế của việc lấy mẫu 3 điểm**: Median-of-three chỉ chọn trung vị của đúng 3 phần tử (đầu, giữa, cuối). Dù cách này giải quyết được trường hợp mảng đã tăng dần hoặc giảm dần sẵn, nhưng nếu mảng có thứ tự đặc biệt sao cho trung vị của 3 điểm này vẫn là phần tử nhỏ nhất hoặc lớn nhất của đoạn đang xét, phân hoạch vẫn bị lệch hoàn toàn (một bên 0 phần tử, một bên n - 1 phần tử), khiến độ sâu đệ quy đạt O(n) và tổng thời gian là O(n^2).
  2. **Trường hợp các phần tử trùng giá trị (Duplicate Keys)**: Nếu mảng chứa nhiều phần tử có giá trị bằng nhau, cách phân hoạch Lomuto vẫn sẽ dồn các phần tử về một phía, dẫn đến việc chia mảng không đều và hiệu năng bị suy giảm về O(n^2).
* **Kết luận**: Kỹ thuật median-of-three giúp Quick Sort chạy rất nhanh và ổn định trong hầu hết các bài toán thực tế, nhưng về mặt lý thuyết toán học thì nó vẫn chưa thể đảm bảo 100% không bị rơi vào O(n^2).
