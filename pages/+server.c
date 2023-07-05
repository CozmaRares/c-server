#include "../src/page_logic/page_includes.h"
#include "../src/utils/utils.h"

int main() {
    int pipefd = open_pipe();
    send_entry(pipefd, "hello", "Hello from server");
    close_pipe(pipefd);
}
