#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Chương trình kiểm thử phần tăng tốc getpid: so sánh getpid() (system call
// thật) với ugetpid() (đọc từ trang chia sẻ USYSCALL).

int main(int argc, char *argv[]) {
    int a = getpid();      // đi qua kernel bằng ecall
    int b = ugetpid();     // đọc trực tiếp từ trang chia sẻ, không vào kernel

    printf("parent: getpid=%d ugetpid=%d %s\n", a, b, a == b ? "OK" : "MISMATCH");
    if (a != b)
        exit(1);

    int pid = fork();
    if (pid < 0) {
        fprintf(2, "getpidtest: fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Tiến trình con có trang usyscall riêng, phải chứa pid của chính nó
        int c = getpid();
        int d = ugetpid();
        printf("child:  getpid=%d ugetpid=%d %s\n", c, d, c == d ? "OK" : "MISMATCH");
        if (c == a)        // pid của con không được trùng pid của cha
            printf("child: pid trung voi cha, SAI\n");
        exit(c == d && c != a ? 0 : 1);
    }

    int status;
    wait(&status);
    // Sau khi con chạy xong, giá trị của cha không được thay đổi
    printf("parent sau fork: ugetpid=%d %s\n", ugetpid(), ugetpid() == a ? "OK" : "MISMATCH");

    if (status != 0 || ugetpid() != a)
        exit(1);
    printf("getpidtest: OK\n");
    exit(0);
}
