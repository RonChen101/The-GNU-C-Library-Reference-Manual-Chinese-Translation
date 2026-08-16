#include<stdio.h>
#include<string.h>
typedef struct student {
  char name[20];
  int age;
  char gender[10];
}student;
int main() {
  student stu_arr[5];
  int len = 5;
  void init_stu(student * stu_arr, int len);
  init_stu(stu_arr, len);
  void stat_stu(student * stu_arr, int len);
  stat_stu(stu_arr, len);
  return 0;
}
void init_stu(student * stu_arr, int len){
  for(int i = 0; i < len; i++) {
    printf("请输入姓名，年龄，性别：");
    scanf("%s %d %s", stu_arr[i].name, &stu_arr[i].age, stu_arr[i].gender);
  }
}
void stat_stu(student * stu_arr, int len) {
  int male = 0, female = 0;
  for(int i = 0; i < len; i++) {
    if(strcmp(stu_arr[i].gender, "男") == 0) male++; else female++;
  }
  printf("统计结果：男->%d 女->%d\n", male, female);
}