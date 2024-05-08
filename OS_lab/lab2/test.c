#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define M 5
#define N 3

int available[N] = {3, 3, 2};
int max[M][N] = {{7, 5, 3}, {3, 2, 2}, {9, 0, 2}, {2, 2, 2}, {4, 3, 3}};
int allocation[M][N] = {{0, 1, 0}, {2, 0, 0}, {3, 0, 2}, {2, 1, 1}, {0, 0, 2}};
int need[M][N], p[M];

void init() {
  int i, j;
  for (i = 0; i < M; i++) {
    for (j = 0; j < N; j++) {
      need[i][j] = max[i][j] - allocation[i][j];
    }
  }
}
void output() {
  int i, j;
  printf("\nThe table of location is:");
  for (i = 0; i < M; i++) {
    printf("\nP[%d]: ", i);
    for (j = 0; j < N; j++) {
      printf(" %d ", allocation[i][j]);
    }
  }
}
int compare(int need[], int work[]) {
  int j;
  for (j = 0; j < N; j++) {
    if (need[j] > work[j]) {
      return FALSE;
    }
  }
  return TRUE;
}
int isSecurity(int available[], int need[][N], int allocation[][N]) {
  int i, j, k = 0, flag, finish[M], work[N];
  for (i = 0; i < M; i++) {
    finish[i] = FALSE;
  }
  for (j = 0; j < N; j++) {
    work[j] = available[j];
  }
  while (TRUE) {
    flag = FALSE;
    for (i = 0; i < M; i++) {
      if (finish[i] == FALSE && compare(need[i], work) == TRUE) {
        for (j = 0; j < N; j++) {
          work[j] += allocation[i][j];
        }
        finish[i] = TRUE;
        p[k++] = i;
        flag = TRUE;
        break;
      }
    }
    if (flag == FALSE) {
      for (i = 0; i < M; i++) {
        if (finish[i] == FALSE) return FALSE;
      }
      return TRUE;
    }
  }
}
void operate() {
  int i, j, flag, f1, request[N];
  printf("****************************************************************\n");
  printf("when you input a value which is samller than zero,it will stop!\n");
  while (TRUE) {
    f1 = FALSE;
    printf("Input which process send a request\n");
    scanf("%d", &i);
    if (i >= M) {
      printf("This process does not exist!\n");
      continue;
    }
    getchar();
    if (i < 0) break;
    printf("Input a request\n");
    for (j = 0; j < N; j++) {
      scanf("%d", &request[j]);
    }
    for (j = 0; j < N; j++) {
      if (request[j] > need[i][j]) {
        printf("The demand is out of need!\n");
        output();
        printf(
            "\n*************************************************************"
            "*\n");
        f1 = TRUE;
        break;
      }
    }
    for (j = 0; j < N && f1 == FALSE; j++) {
      if (request[j] > available[j]) {
        printf("The request is out of available!\n");
        output();
        printf(
            "\n*************************************************************"
            "*\n");
        f1 = TRUE;
        break;
      }
    }
    if (f1 == TRUE) continue;
    for (j = 0; j < N; j++) {
      available[j] -= request[j];
      allocation[i][j] += request[j];
      need[i][j] -= request[j];
    }
    flag = isSecurity(available, need, allocation);
    if (flag == TRUE) {
      printf("The request is security!\n");
      printf("The security sort is:\n");
      for (i = 0; i < M; i++) {
        i == 4 ? printf("P[%d]", i) : printf("P[%d]-->", p[i]);
      }
      output();
      printf(
          "\n**************************************************************\n");
    } else {
      printf("The request is insecurity!\n");
      printf("this request should wait!\n");
      for (j = 0; j < N; j++) {
        available[j] -= request[j];
        allocation[i][j] += request[j];
        need[i][j] -= request[j];
      }
      output();
      printf(
          "\n*************************************************************\n");
    }
  }
}
int main() {
  //   clrscr();
  init();
  operate();
  return 0;
}