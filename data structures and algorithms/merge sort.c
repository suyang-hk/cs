#include <stdio.h>
#include <stdlib.h>

// 定义链表节点
typedef struct ListNode {
    int val;
    struct ListNode* next;
} ListNode;

// **1. 获取链表中点，并拆分链表**
ListNode* getMiddle(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* slow = head;
    ListNode* fast = head->next; // 让 fast 先走一步，确保 slow 停在前半部分

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    ListNode* mid = slow->next;
    slow->next = NULL;  // 断开链表
    return mid;
}

// **2. 合并两个有序链表**
ListNode* merge(ListNode* left, ListNode* right) {
    if (!left) return right;
    if (!right) return left;

    ListNode dummy;  // 哨兵节点，简化操作
    ListNode* tail = &dummy;
    
    while (left && right) {
        if (left->val <= right->val) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    tail->next = left ? left : right;  // 连接剩余部分
    return dummy.next;
}

// **3. 归并排序（递归）**
ListNode* mergeSort(ListNode* head) {
    if (!head || !head->next) return head;  // 递归终止条件

    ListNode* mid = getMiddle(head);  // 获取中点
    ListNode* left = mergeSort(head);  // 递归排序左半部分
    ListNode* right = mergeSort(mid);  // 递归排序右半部分

    return merge(left, right);  // 合并排序后的链表
}

// **辅助函数：创建节点**
ListNode* createNode(int val) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    newNode->val = val;
    newNode->next = NULL;
    return newNode;
}

// **辅助函数：打印链表**
void printList(ListNode* head) {
    while (head) {
        printf("%d -> ", head->val);
        head = head->next;
    }
    printf("NULL\n");
}

// **辅助函数：释放链表**
void freeList(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        free(temp);
    }
}

// **测试代码**
int main() {
    ListNode* head = createNode(4);
    head->next = createNode(3);
    head->next->next = createNode(1);
    head->next->next->next = createNode(2);
    head->next->next->next->next = createNode(5);

    printf("原链表: ");
    printList(head);

    head = mergeSort(head);

    printf("排序后: ");
    printList(head);

    freeList(head);
    return 0;
}
