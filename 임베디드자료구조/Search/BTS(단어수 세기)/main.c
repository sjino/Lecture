#include <string.h>
#include <stdio.h>
#include "BTS_library.h"
#pragma warning(disable:4996)
typedef struct _wcount
{
    int count;
    char word[30];
}wcount;
node* head;
const char delim[] =" \t\r\n,.-";
int intcmp(const void* a, const void* b) //정수형 비교 함수
{
    return (*(int*)a -*(int*)b);
}
int floatcmp(const void* a, const void* b) //실수형 비교 함수 a>b면 1, a<b면 -1 return
{
    if ((*(float*)a -*(float*)b) ==0)
    {
        return 0;
    }
    else
    {
        return ((*(float*)a -*(float*)b) >0) ? 1 : -1;
    }
}
int charcmp(const void* a, const void* b) //문자형 비교 함수 a>b면 1, a<b면 -1 return
{
    if ((*(char*)a -*(char*)b) ==0)
    {
        return 0;
    }
    else
    {
        return ((*(char*)a -*(char*)b) >0) ? 1 : -1;
    }
}
int string_cmp(const void* a, const void* b)
{
    return strcmp((char*)a, (char*)b);
}
int wcount_cmp(const void* a, const void* b)
{
    return strcmp(((wcount*)a)->word, ((wcount*)b)->word);
}
void print_wcount(void* a)
{
    printf("%s : %d\n", ((wcount*)a)->word, ((wcount*)a)->count);
}
void make_str(void* a, char* s)
{
    strcpy(s, ((wcount*)a)->word);
}
int main()
{
    int i, num =0, cnt =0;;
    char* tok;
    char* context;
    char str[256];
    wcount wc, * wcp, *key;
    init_tree(&head);
    FILE* fp;
    fp = fopen("test.txt", "rt");
    fgets(str, 256, fp); // read one line
    do
    {
        tok = strtok(str, delim); //단어 자르기
        while (tok)
        {
            strcpy(wc.word, tok);
            wcp = (wcount*)btv_search(&wc, head, &num, sizeof(wcount), wcount_cmp); //잘린 단어 검색
            if (wcp ==NULL) //처음 들어온 단어 일때
            {
                wc.count =1; // count frequency
                btv_insert(&wc, head, &num, sizeof(wcount), wcount_cmp);//이진트리에 추가
            }
            else //이미 단어가 있을 때
            {
                (*wcp).count++; //단어 수 UP
            }
            tok = strtok(NULL, delim);
        }
        fgets(str, 256, fp);
    } while (!feof(fp));
    printf("\n");
    btv_sort_list(head->left, print_wcount);
    //btv_draw(head->left, make_str, LINK | NODE | TAIL); while (getch() != '\r'); //하나씩 확인 // 엔터키눌러서 진행
    printf("\n-------------------------------------------------\n");
    btv_balance(head, &num, sizeof(wcount));
    btv_sort_list(head->left, print_wcount);
    printf("\n-------------------------------------------------\n");
    strcpy(wc.word, "has");
    key = (wcount*)btv_search(&wc, head, &num, sizeof(wcount), wcount_cmp);
    printf("\nFind: %s : %d\n\n", key->word, key->count);
    btv_delete1(key, head, &num, sizeof(wcount), wcount_cmp);
    btv_sort_list(head->left, print_wcount);
    btv_deleteall(head, &num);
   // btv_draw(head->left, make_str, LINK | NODE | TAIL);    while (getch() != '\r');// 엔터키눌러서 진행
}
