fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
Dòng lệnh này giúp mở file để đọc và ghi , xóa tất cả dữ liệu đã tồn tại trong file , tạo file nếu chưa tồn tại , và cấp quyền đọc ghi nếu tạo file và lưu giá trị "file descriptor" vào biến fd1

fd3 = open(file, O_RDWR);
Mở file và lưu "file descriptor" của file vào fd3 

--> Giải thích thêm khi mở cùng 1 file và lưu 2 giá trị vào fd1 và fd3 thì chúng sẽ hoạt động độc lập

write(fd1, "Hello,", 6);
Ghi "Hello" vào file có giá trị của biến fd1
Lúc này trong file hiện "Hello "

write(fd2, "world", 6);
Ghi "world" vào file có giá trị của biến fd2
Lúc này trong file hiện "world "

Hoặc có thể không thể thực hiện vì chưa mở file 

lseek(fd2, 0, SEEK_SET);
Di chuyển con trỏ đến đầu file có giá trị của biến fd2

Hoặc có thể không thể thực hiện vì chưa mở file 

write(fd1, "HELLO,", 6);
Ghi "Hello" vào file có giá trị của biến fd1
Lúc này trong file hiện "Hello HELLO "

write(fd3, "Gidday", 6);
Ghi "Gidday" vào file có giá trị của biến fd3
Lúc này trong file hiện "Gidday "
 