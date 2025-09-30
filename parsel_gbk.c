#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <ctype.h>  
#include <windows.h>

double note_time = 60000.0/38;


// 定义链表节点结构  
typedef struct Node {  
    char **letters; // 字母组合数组  
    int count;      // 组合数量  
    struct Node *next;  
} Node;  
  
// 创建新节�?  
Node* create_node() {  
    Node* newNode = (Node*)malloc(sizeof(Node));  
    newNode->letters = NULL;  
    newNode->count = 0;  
    newNode->next = NULL;  
    return newNode;  
}  
  
// 释放节点内存  
void free_node(Node* node) {  
    if (node) {  
        if (node->letters) {  
            for (int i = 0; i < node->count; i++) {  
                free(node->letters[i]);  
            }  
            free(node->letters);  
        }  
        free(node);  
    }  
}  
  
// 解析单个字母组合  
char* parse_letter_group(const char* input, int* start_pos) {  
    int len = 0;  
    // 跳过开头的非字母字�?  
    while (input[*start_pos] && !isalpha(input[*start_pos])) {  
        (*start_pos)++;  
    }  
      
    // 计算字母组合长度  
    while (input[*start_pos + len] && isalpha(input[*start_pos + len])) {  
        len++;  
    }  
      
    if (len == 0) return NULL;  
      
    char* result = (char*)malloc(len + 1);  
    strncpy(result, input + *start_pos, len);  
    result[len] = '\0';  
    *start_pos += len;  
    return result;  
}  
  
// 解析单个部分  
Node* parse_section(const char* section) {  
    Node* node = create_node();  
    int pos = 0;  
    int section_len = strlen(section);  
      
    while (pos < section_len) {  
        // 跳过非字母字�? 
        while (pos < section_len && !isalpha(section[pos])) {  
            pos++;  
        }  
          
        if (pos >= section_len) break;  
          
        // 解析字母组合  
        char* group = parse_letter_group(section, &pos);  
        if (group) {  
            node->count++;  
            node->letters = realloc(node->letters, node->count * sizeof(char*));  
            node->letters[node->count - 1] = group;  
        }  
    }  
      
    return node;  
}  
  
// 解析文件内容  
Node* parse_file(const char* filename) {  
    FILE* file = fopen(filename, "r");  
    if (!file) {  
        perror("无法打开文件");  
        return NULL;  
    }  
      
    // 读取整个文件内容  
    fseek(file, 0, SEEK_END);  
    long file_size = ftell(file);  
    fseek(file, 0, SEEK_SET);  
      
    char* content = (char*)malloc(file_size + 1);  
    if (!content) {  
        fclose(file);  
        perror("内存分配失败");  
        return NULL;  
    }  
      
    fread(content, 1, file_size, file);  
    fclose(file);  
    content[file_size] = '\0';  
      
    Node* head = NULL;  
    Node* current = NULL;  
      
    // 分割内容到各个部�? 
    char* start = content;  
    char* end = content;  
    while (*end) {  
        // 寻找斜杠分隔�? 
        if (*end == '/') {  
            // 跳过连续斜杠  
            while (end[1] == '/') end++;  
              
            // 提取部分内容  
            *end = '\0';  
            Node* section_node = parse_section(start);  
              
            if (!head) {  
                head = section_node;  
                current = head;  
            } else {  
                current->next = section_node;  
                current = current->next;  
            }  
              
            start = end + 1;  
        }  
        end++;  
    }  
      
    // 处理最后一部分  
    if (start < end) {  
        Node* section_node = parse_section(start);  
        if (current) {  
            current->next = section_node;  
        } else {  
            head = section_node;  
        }  
    }  
      
    free(content);  
    return head;  
}  


// 发送数�?
int SendKeyboard(char vk){

    UINT scan_code = MapVirtualKey((UINT) vk, MAPVK_VK_TO_VSC);

    INPUT ip[2];
    ip[0].type = INPUT_KEYBOARD;
    ip[0].ki.wScan = scan_code;
    ip[0].ki.dwFlags = KEYEVENTF_SCANCODE; 
    
    
    // 初始化第二个INPUT结构体，模拟按键释放事件
    ip[1].type = INPUT_KEYBOARD;
    ip[1].ki.wScan = scan_code;
    ip[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP 表示释放事件
    
    
    // 发送输入事�?
    UINT sent = SendInput(2, ip, sizeof(INPUT));
    if (sent != 2) {
        printf("发送失败，错误码：%lu\n", GetLastError());
        return 1;
    }
    return 0;
}

int SendString_ByChar(char* vks){
    for(int i=0; vks[i]!= '\0'; i++){
        SendKeyboard(vks[i]);
    }
    //printf("successfully send");
    SendKeyboard(' ');
}
// 打印链表内容  
void print_list(Node* head) {  
    Node* current = head;  
    int index = 0;
    MSG msg = {0};  
    while (current) { 
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_HOTKEY) {
                printf("Ctrl+P detected, stop\n");
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        printf("note %d: include %d combines\n", index++, current->count);
        int note_num = current->count;
        int interval =(int)(note_time / note_num);
        for (int i = 0; i < note_num; i++) {  
            printf("index %d: %s\n", i + 1, current->letters[i]);
            SendString_ByChar(current->letters[i]);
            Sleep(interval);  
        }  
        //SendKeyboard();
        
        current = current->next;  
    }  
}  
  
int main(int argc, char* argv[]) {  
    
    /*if (argc != 2) {  
        printf("用法: %s <文件�??>\n", argv[0]);  
        return 1;  
    }  */
    
    // 注册全局热键（Ctrl+P，id=1�?
    if (!RegisterHotKey(NULL, 1, MOD_CONTROL, 'P')) {
        printf("热键注册失败！错误代码：%d\n", GetLastError());
        return 1;
    }

    char* dir="./kanon.txt";

    Node* head = parse_file(dir);  
    if (!head) {  
        printf("解析文件失败\n");  
        return 1;  
    }  
    HWND hwnd= (HWND) FindWindow("UnityWndClass", "ԭ��");
    if (hwnd == NULL) {
        printf("未找到窗口\n");
        return 1;
    }
	else {
        // 激活目标窗口（需处理Windows前台限制�??
        if (!SetForegroundWindow(hwnd)) {
            printf("激活窗口失败，尝试使用最小化/恢复法\n");
            ShowWindow(hwnd, SW_MINIMIZE);
            ShowWindow(hwnd, SW_RESTORE);
        }
        else {  
        Sleep(1000);
        print_list(head);  
        }
    }

    // 释放链表内存  
    Node* current = head;  
    while (current) {  
        Node* temp = current;  
        current = current->next;  
        free_node(temp);  
    }  
    //注销热键
    UnregisterHotKey(NULL, 1);
    return 0;  
}

//终端采用gbk编码解码方式 用chcp 65001改为utf-8编码格式
