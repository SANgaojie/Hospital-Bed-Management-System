#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 病人信息结构
struct Patient {
    int patientID;      // 病人ID
    char name[50];      // 姓名
    int gender;         // 性别
    char phone[20];     // 电话
    char diagnosis[100]; // 诊断结果
    int age;            // 年龄
};

enum BedType {
    RegularBed = 0,     // 普通床位
    ICUBed,             // 重症监护床位
    EmergencyBed        // 急诊床位
};

struct Bed {
    int ID;             // 床位ID
    int isOccupied;     // 是否已分配
    int hasOxygen;      // 是否有供氧设备
    enum BedType bedType; // 床位类型
    int ward;           // 病房号
    int department;     // 科室编号 (1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他)
    struct Patient patient; // 病人信息
    struct Bed* next;   // 链表指针
};

// 医生结构体定义
struct Doctor {
    int doctorID;           // 医生ID
    char name[50];          // 姓名
    int gender;             // 性别（1-男，0-女）
    char phone[20];         // 联系电话
    int department;         // 所属科室（与床位科室编码一致：1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他）
    char specialization[50]; // 专业/专长
    int qualification;      // 职称（1-住院医师, 2-主治医师, 3-副主任医师, 4-主任医师）
    char officeLocation[30]; // 办公室位置
    struct Doctor* next;    // 链表指针
};

// 医生-病人关联结构体
struct DoctorPatientRelation {
    int doctorID;            // 医生ID
    int patientID;           // 病人ID
    char notes[100];         // 医疗备注
    char startDate[20];      // 开始负责日期
    struct DoctorPatientRelation* next; // 链表指针
};

// 医生-病房关联结构体
struct DoctorWardRelation {
    int doctorID;            // 医生ID
    int wardNumber;          // 病房号
    int isHeadDoctor;        // 是否为主治医生（1-是，0-否）
    char scheduleInfo[100];  // 查房安排
    struct DoctorWardRelation* next; // 链表指针
};

// 全局链表头指针
struct Bed* head = NULL;
struct Doctor* doctorHead = NULL;
struct DoctorPatientRelation* doctorPatientHead = NULL;
struct DoctorWardRelation* doctorWardHead = NULL;

// 函数前向声明
void listAllBeds();
void listAvailableBeds();
void listAvailableBedsLocal();

// 医生管理函数声明
void addDoctor();
void modifyDoctor();
void deleteDoctor();
void searchDoctorByID();
void listAllDoctors();
void assignPatientToDoctor();
void removePatientFromDoctor();
void listPatientsByDoctor();
void listDoctorsByPatient();
void assignWardToDoctor();
void removeWardFromDoctor();
void listWardsByDoctor();
void listDoctorsByWard();
void loadDoctorsFromFile(const char* filename);
void loadDoctorPatientFromFile(const char* filename);
void loadDoctorWardFromFile(const char* filename);
void saveDoctorsToFile(const char* filename);
void saveDoctorPatientToFile(const char* filename);
void saveDoctorWardToFile(const char* filename);

// 打印分隔线
void printSeparator() {
    printf("\n");
    printf("----------------------------------------------------------------\n");
}

// 打印操作标题
void printOperationTitle(const char* title) {
    printSeparator();
    printf("【%s】\n", title);
    printSeparator();
}

// 打印床位类型的辅助函数
void printBedType(enum BedType type) {
    switch(type) {
        case RegularBed: printf("普通床位"); break;
        case ICUBed: printf("重症监护"); break;
        case EmergencyBed: printf("急诊床位"); break;
        default: printf("未知类型");
    }
}

// 打印科室的辅助函数
void printDepartment(int department) {
    switch(department) {
        case 1: printf("内科"); break;
        case 2: printf("外科"); break;
        case 3: printf("儿科"); break;
        case 4: printf("妇科"); break;
        case 5: printf("其他"); break;
        default: printf("未知");
    }
}

// 打印床位基本信息的辅助函数
void printBedBasicInfo(struct Bed* bed) {
    printf("床位ID: %d | ", bed->ID);
    printf("状态: %s | ", bed->isOccupied ? "已占用" : "空闲");
    printf("供氧设备: %s | ", bed->hasOxygen ? "有" : "无");
    
    printf("类型: ");
    printBedType(bed->bedType);
    printf(" | ");
    
    printf("病房号: %d | ", bed->ward);
    printf("科室: ");
    printDepartment(bed->department);
}

// 打印病人信息的辅助函数
void printPatientInfo(struct Patient* patient) {
    printf("\n  病人信息: ID-%d | 姓名-%s | ", 
        patient->patientID, patient->name);
    printf("性别-%s | ", patient->gender ? "男" : "女");
    printf("电话-%s | ", patient->phone);
    printf("诊断-%s | ", patient->diagnosis);
    printf("年龄-%d", patient->age);
}

// 显示可用床位的函数，用于registerPatient内部调用
void listAvailableBedsLocal() {
    struct Bed* current = head;
    int found = 0;
    
    printf("\n空闲床位列表：\n");
    printf("----------------------------------------------------------------\n");
    
    while (current != NULL) {
        if (!current->isOccupied) {
            printBedBasicInfo(current);
            printf("\n");
            found = 1;
        }
        current = current->next;
    }
    
    printf("----------------------------------------------------------------\n");
    if (!found) {
        printf("没有空闲床位\n");
    }
}

// 清空输入缓冲区的简化版本
void flushStdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 等待用户按回车的简化版本
void pause() {
    printf("\n按回车键继续...");
    getchar();
}

void printMenu() {
    printf("\n");
    printf("╔═════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                  医院床位管理系统                                                    ║\n");
    printf("╠═════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                                    【系统功能导航】                                                  ║\n");
    printf("║                                                                                                     ║\n");
    printf("║  【病人与床位管理】                                                                                  ║\n");
    printf("║  1--登记新病人        (录入病人信息)     ║ 2--分配床位         (为病人分配床位)                    ║\n");
    printf("║  3--病人出院          (释放占用床位)     ║ 4--添加新床位       (新增床位基础信息)                  ║\n");
    printf("║  5--修改床位信息      (修改床位属性)     ║ 6--删除床位信息     (移除指定床位)                      ║\n");
    printf("║                                                                                                     ║\n");
    printf("║  【查询与筛选功能】                                                                                  ║\n");
    printf("║  7--根据床位号查询    (查询单个床位)     ║ 8--所有床位信息     (显示所有床位)                      ║\n");
    printf("║  9--查询空闲床位      (显示可用床位)     ║ 10--按床位类型筛选  (普通/重症/急诊)                    ║\n");
    printf("║  11--按病房号筛选     (查看指定病房)     ║ 12--按科室筛选      (内科/外科等筛选)                   ║\n");
    printf("║  13--按床位ID排序     (ID升序排列)                                                                 ║\n");
    printf("║                                                                                                     ║\n");
    printf("║  【医生管理功能】                                                                                    ║\n");
    printf("║  14--添加医生记录     (新增医生信息)     ║ 15--修改医生信息    (修改医生资料)                      ║\n");
    printf("║  16--删除医生记录     (移除指定医生)     ║ 17--查询医生信息    (查询单个医生)                      ║\n");
    printf("║  18--列出所有医生     (显示所有医生)     ║ 19--分配病人给医生  (建立医生病人关联)                  ║\n");
    printf("║  20--分配病房给医生   (医生负责病房)     ║ 21--查询医生的病人  (显示医生负责病人)                  ║\n");
    printf("║  22--查询医生的病房   (显示医生负责病房)                                                           ║\n");
    printf("║                                                                                                     ║\n");
    printf("║  24--保存并退出系统   (保存当前所有数据并退出程序)                                                   ║\n");
    printf("╚═════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("请输入对应数字选择功能(1-13, 14-22, 24): ");
}

void addBed() {
    printOperationTitle("添加新床位");
    
    struct Bed* newBed = (struct Bed*)malloc(sizeof(struct Bed));
    if (newBed == NULL) {
        printf("内存分配失败\n");
        pause();
        return;
    }

    printf("输入床位ID: ");
    scanf("%d", &newBed->ID);
    flushStdin(); // 清空输入缓冲区
    
    // 检查ID是否已存在
    struct Bed* current = head;
    while (current != NULL) {
        if (current->ID == newBed->ID) {
            printf("错误: 床位ID %d 已存在，请使用其他ID\n", newBed->ID);
            free(newBed);
            pause();
            return;
        }
        current = current->next;
    }
    
    newBed->isOccupied = 0;
    newBed->patient.patientID = -1; // 初始化为未分配
    newBed->next = head;
    head = newBed;

    printf("输入是否有供氧设备 (1有, 0无): ");
    scanf("%d", &newBed->hasOxygen);
    flushStdin(); // 清空输入缓冲区
    
    printf("输入床位类型 (0普通床位, 1重症监护床位, 2急诊床位): ");
    scanf("%d", (int*)&newBed->bedType);
    flushStdin(); // 清空输入缓冲区
    
    printf("输入病房号: ");
    scanf("%d", &newBed->ward);
    flushStdin(); // 清空输入缓冲区
    
    printf("输入科室编号 (1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他): ");
    scanf("%d", &newBed->department);
    flushStdin(); // 清空输入缓冲区

    printf("\n? 床位添加成功！新增床位信息如下：\n");
    printSeparator();
    printBedBasicInfo(newBed);
    printf("\n");
    printSeparator();
    pause(); // 暂停等待用户按回车
}

void searchBedByID() {
    printOperationTitle("床位信息查询");
    
    int id;
    printf("输入要查找的床位ID: ");
    scanf("%d", &id);
    flushStdin(); // 清空输入缓冲区

    struct Bed* current = head;
    while (current != NULL) {
        if (current->ID == id) {
            printf("\n查询结果：\n");
            printSeparator();
            printBedBasicInfo(current);
            if (current->isOccupied) {
                printPatientInfo(&current->patient);
            }
            printf("\n");
            printSeparator();
            printf("\n按回车键返回主菜单..."); // 直接使用这种方式替代pause()
            getchar();
            return;
        }
        current = current->next;
    }

    printf("\n? 未找到床位ID为%d的床位\n", id);
    printf("\n按回车键返回主菜单..."); // 直接使用这种方式替代pause()
    getchar();
}

void modifyBed() {
    printOperationTitle("修改床位信息");
    
    int id;
    printf("输入要修改的床位ID: ");
    scanf("%d", &id);
    flushStdin(); // 清空输入缓冲区

    struct Bed* current = head;
    while (current != NULL) {
        if (current->ID == id) {
            printf("\n当前床位信息：\n");
            printSeparator();
            printBedBasicInfo(current);
            printf("\n");
            printSeparator();
            
            printf("\n请输入新的信息：\n");
            printf("输入新的是否有供氧设备 (1有, 0无): ");
            scanf("%d", &current->hasOxygen);
            flushStdin();
            
            printf("输入新的床位类型 (0普通床位, 1重症监护床位, 2急诊床位): ");
            scanf("%d", (int*)&current->bedType);
            flushStdin();
            
            printf("输入新的病房号: ");
            scanf("%d", &current->ward);
            flushStdin();
            
            printf("输入新的科室编号 (1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他): ");
            scanf("%d", &current->department);
            flushStdin();
            
            printf("\n? 床位信息修改成功！更新后信息如下：\n");
            printSeparator();
            printBedBasicInfo(current);
            printf("\n");
            printSeparator();
            pause();
            return;
        }
        current = current->next;
    }

    printf("\n? 未找到床位ID为%d的床位\n", id);
    pause();
}

void deleteBed() {
    printOperationTitle("删除床位");
    
    int id;
    printf("输入要删除的床位ID: ");
    scanf("%d", &id);
    flushStdin(); // 清空输入缓冲区

    struct Bed* current = head;
    struct Bed* prev = NULL;

    while (current != NULL) {
        if (current->ID == id) {
            // 检查床位是否被占用
            if (current->isOccupied) {
                printf("\n? 错误：床位ID %d 当前有病人占用，无法删除。请先办理病人出院。\n", id);
                pause();
                return;
            }
            
            if (prev == NULL) {
                head = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            printf("\n? 床位ID为%d的床位删除成功\n", id);
            pause();
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("\n? 未找到床位ID为%d的床位\n", id);
    pause();
}

void listAllBeds() {
    printOperationTitle("所有床位信息");
    
    struct Bed* current = head;
    if (current == NULL) {
        printf("当前没有床位信息\n");
        printf("\n按回车键返回主菜单...");
        getchar();
        return;
    }
    
    int count = 0;
    int occupied = 0;
    
    printf("所有床位列表：\n");
    printf("----------------------------------------------------------------\n");
    
    while (current != NULL) {
        printBedBasicInfo(current);
        
        if (current->isOccupied) {
            printPatientInfo(&current->patient);
            occupied++;
        }
        printf("\n----------------------------------------------------------------\n");
        current = current->next;
        count++;
    }
    
    printf("\n统计信息：总床位数: %d | 已占用: %d | 空闲: %d\n", count, occupied, count - occupied);
    printf("\n按回车键返回主菜单...");
    getchar();
}

// 获取用户输入的病人信息
void getPatientInfo(struct Patient* patient) {
    printf("输入病人ID: ");
    scanf("%d", &patient->patientID);
    flushStdin();
    
    printf("输入病人姓名: ");
    scanf("%s", patient->name);
    flushStdin();
    
    printf("输入病人性别 (1男, 0女): ");
    scanf("%d", &patient->gender);
    flushStdin();
    
    printf("输入病人电话: ");
    scanf("%s", patient->phone);
    flushStdin();
    
    printf("输入诊断结果: ");
    scanf(" %[^\n]", patient->diagnosis);
    flushStdin();
    
    printf("输入病人年龄: ");
    scanf("%d", &patient->age);
    flushStdin();
}

void registerPatient() {
    printOperationTitle("病人登记");
    
    struct Patient newPatient;
    printf("请输入病人基本信息:\n");
    getPatientInfo(&newPatient);

    // 这里可以将病人信息保存到文件或数据库中
    printf("\n? 病人登记成功！\n");
    
    // 询问是否立即分配床位
    int choice;
    printf("\n是否现在为病人分配床位？(1是, 0否): ");
    scanf("%d", &choice);
    flushStdin();
    
    if (choice == 1) {
        // 显示可用床位
        listAvailableBedsLocal();
        
        // 检查是否有空闲床位
        struct Bed* current = head;
        int hasFreeBed = 0;
        
        while (current != NULL) {
            if (!current->isOccupied) {
                hasFreeBed = 1;
                break;
            }
            current = current->next;
        }
        
        if (!hasFreeBed) {
            return; // listAvailableBedsLocal已经输出了没有空闲床位的消息
        }
        
        // 选择床位
        int bedID;
        printf("\n输入要分配的床位ID: ");
        scanf("%d", &bedID);
        flushStdin();
        
        current = head;
        while (current != NULL) {
            if (current->ID == bedID && !current->isOccupied) {
                current->patient = newPatient;
                current->isOccupied = 1;
                printf("\n? 床位分配成功！病人 %s 已分配到床位 %d\n", newPatient.name, bedID);
                pause();
                return;
            } else if (current->ID == bedID && current->isOccupied) {
                printf("\n? 该床位已被占用，无法分配\n");
                pause();
                return;
            }
            current = current->next;
        }
        
        printf("\n? 未找到床位ID为%d的空闲床位\n", bedID);
        pause();
    } else {
        pause();
    }
}

void assignBed() {
    printOperationTitle("床位分配");
    
    int bedID;
    
    printf("输入要分配的床位ID: ");
    scanf("%d", &bedID);
    flushStdin();

    struct Bed* current = head;
    while (current != NULL) {
        if (current->ID == bedID && !current->isOccupied) {
            printf("\n请输入病人信息:\n");
            getPatientInfo(&current->patient);
            current->isOccupied = 1;
            printf("\n? 床位分配成功！病人 %s 已分配到床位 %d\n", current->patient.name, bedID);
            pause();
            return;
        } else if (current->ID == bedID && current->isOccupied) {
            printf("\n? 该床位已被占用，无法分配。当前占用病人: %s\n", current->patient.name);
            pause();
            return;
        }
        current = current->next;
    }

    printf("\n? 未找到床位ID为%d的空闲床位\n", bedID);
    pause();
}

void dischargePatient() {
    printOperationTitle("病人出院");
    
    int id;
    printf("输入要办理出院的床位ID: ");
    scanf("%d", &id);
    flushStdin();

    struct Bed* current = head;
    while (current != NULL) {
        if (current->ID == id && current->isOccupied) {
            printf("\n当前占用信息:\n");
            printSeparator();
            printf("床位ID: %d | 病人姓名: %s | 诊断: %s\n", 
                   current->ID, current->patient.name, current->patient.diagnosis);
            printSeparator();
                   
            printf("\n确认办理出院? (1确认, 0取消): ");
            int confirm;
            scanf("%d", &confirm);
            flushStdin();
            
            if (confirm) {
                printf("\n? 病人 %s (ID: %d) 已办理出院，床位已释放\n", 
                       current->patient.name, current->patient.patientID);
                current->isOccupied = 0;
                current->patient.patientID = -1;
            } else {
                printf("\n出院操作已取消\n");
            }
            pause();
            return;
        } else if (current->ID == id && !current->isOccupied) {
            printf("\n? 该床位本就空闲，无需办理出院\n");
            pause();
            return;
        }
        current = current->next;
    }

    printf("\n? 未找到床位ID为%d的已占用床位\n", id);
    pause();
}

// 修改加载函数，使用CSV格式
void loadBedsFromFile(const char* filename) {
    printf("正在加载床位数据...\n");
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件 %s，将创建新文件\n", filename);
        return;
    }

    int recordCount = 0;
    char line[1024]; // 足够大的缓冲区来存储一行CSV数据
    
    // 读取并跳过CSV文件头
    if (fgets(line, sizeof(line), file) == NULL) {
        printf("文件为空或格式不正确\n");
        fclose(file);
        return;
    }
    
    // 读取数据记录
    while (fgets(line, sizeof(line), file) != NULL) {
        struct Bed* newBed = (struct Bed*)malloc(sizeof(struct Bed));
        if (newBed == NULL) {
            printf("内存分配失败\n");
            fclose(file);
            return;
        }
        
        // 初始化字符串字段
        newBed->patient.name[0] = '\0';
        newBed->patient.phone[0] = '\0';
        newBed->patient.diagnosis[0] = '\0';
        
        // 使用sscanf解析CSV行
        char nameBuf[50], phoneBuf[20], diagnosisBuf[100];
        int itemsRead = sscanf(line, "%d,%d,%d,%d,%d,%d,%d,%[^,],%d,%[^,],%[^,],%d",
            &newBed->ID,
            &newBed->isOccupied,
            &newBed->hasOxygen,
            (int*)&newBed->bedType,
            &newBed->ward,
            &newBed->department,
            &newBed->patient.patientID,
            nameBuf,
            &newBed->patient.gender,
            phoneBuf, 
            diagnosisBuf,
            &newBed->patient.age);
        
        // 检查是否成功读取所有字段
        if (itemsRead < 12) {
            printf("警告: 行格式不正确, 只读取到%d个字段，跳过此行\n", itemsRead);
            free(newBed);
            continue;
        }
        
        // 复制字符串字段
        strncpy(newBed->patient.name, nameBuf, sizeof(newBed->patient.name) - 1);
        newBed->patient.name[sizeof(newBed->patient.name) - 1] = '\0'; // 确保以null结尾
        
        strncpy(newBed->patient.phone, phoneBuf, sizeof(newBed->patient.phone) - 1);
        newBed->patient.phone[sizeof(newBed->patient.phone) - 1] = '\0';
        
        strncpy(newBed->patient.diagnosis, diagnosisBuf, sizeof(newBed->patient.diagnosis) - 1);
        newBed->patient.diagnosis[sizeof(newBed->patient.diagnosis) - 1] = '\0';
        
        // 添加到链表
        newBed->next = head;
        head = newBed;
        recordCount++;
        
        // 安全检查
        if (recordCount > 1000) {
            printf("加载记录数过多，可能存在文件格式问题，已停止加载\n");
            break;
        }
    }

    fclose(file);
    printf("床位信息加载成功！共加载 %d 条记录\n", recordCount);
}

// 菜单选项6使用的函数，显示所有空闲床位
void listAvailableBeds() {
    printOperationTitle("查询空闲床位");
    listAvailableBedsLocal(); // 复用已经写好的函数
    printf("\n按回车键返回主菜单...");
    getchar();
}

// 使用更高效的快速排序替代冒泡排序
void sortBedsByID() {
    printOperationTitle("床位排序");
    
    if (head == NULL || head->next == NULL) {
        printf("当前床位数量不足，无需排序\n");
        pause();
        return; // 0或1个节点不需要排序
    }

    // 将链表转换为数组以便使用快速排序
    int count = 0;
    struct Bed* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    struct Bed** bedArray = (struct Bed**)malloc(count * sizeof(struct Bed*));
    if (bedArray == NULL) {
        printf("内存分配失败\n");
        return;
    }

    current = head;
    for (int i = 0; i < count; i++) {
        bedArray[i] = current;
        current = current->next;
    }

    // 使用简单插入排序 - 对于小数据集比较高效
    for (int i = 1; i < count; i++) {
        struct Bed* key = bedArray[i];
        int j = i - 1;

        while (j >= 0 && bedArray[j]->ID > key->ID) {
            bedArray[j + 1] = bedArray[j];
            j--;
        }
        bedArray[j + 1] = key;
    }

    // 重建排序后的链表
    head = bedArray[0];
    for (int i = 0; i < count - 1; i++) {
        bedArray[i]->next = bedArray[i + 1];
    }
    bedArray[count - 1]->next = NULL;

    free(bedArray);
    printf("\n? 已按床位ID排序完成！排序结果如下：\n");
    listAllBeds();
}

void filterBedsByType() {
    printOperationTitle("按床位类型筛选");
    
    int bedType;
    printf("输入要筛选的床位类型 (0普通床位, 1重症监护床位, 2急诊床位): ");
    scanf("%d", &bedType);
    flushStdin();

    struct Bed* current = head;
    int found = 0;
    int total = 0;
    int occupied = 0;
    
    printf("\n床位类型为 ");
    printBedType(bedType);
    printf(" 的床位列表：\n");
    printf("----------------------------------------------------------------\n");
    
    while (current != NULL) {
        if (current->bedType == bedType) {
            printBedBasicInfo(current);
            
            if (current->isOccupied) {
                printPatientInfo(&current->patient);
                occupied++;
            }
            printf("\n----------------------------------------------------------------\n");
            found = 1;
            total++;
        }
        current = current->next;
    }
    
    if (!found) {
        printf("未找到床位类型为%d的床位\n", bedType);
    } else {
        printf("\n统计信息：该类型总床位数: %d | 已占用: %d | 空闲: %d\n", total, occupied, total - occupied);
    }
    pause();
}

// 修改保存函数，使用CSV格式
void saveBedsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        pause();
        return;
    }

    // 写入CSV文件头
    fprintf(file, "ID,isOccupied,hasOxygen,bedType,ward,department,patientID,name,gender,phone,diagnosis,age\n");
    
    struct Bed* current = head;
    int count = 0;
    while (current != NULL) {
        // 写入CSV格式的数据行
        // 注意: 字符串字段使用双引号包围，避免逗号分隔符问题
        fprintf(file, "%d,%d,%d,%d,%d,%d,%d,\"%s\",%d,\"%s\",\"%s\",%d\n",
            current->ID,
            current->isOccupied,
            current->hasOxygen,
            current->bedType,
            current->ward,
            current->department,
            current->patient.patientID,
            current->patient.name,
            current->patient.gender,
            current->patient.phone,
            current->patient.diagnosis,
            current->patient.age);

        current = current->next;
        count++;
    }

    fclose(file);
    printf("\n? 床位信息保存成功！共保存 %d 条记录到CSV文件\n", count);
}

void filterBedsByWard() {
    printOperationTitle("按病房号筛选");
    
    int ward;
    printf("输入要筛选的病房号: ");
    scanf("%d", &ward);
    flushStdin();

    struct Bed* current = head;
    int found = 0;
    int total = 0;
    int occupied = 0;
    
    printf("\n病房号为 %d 的床位列表：\n", ward);
    printf("----------------------------------------------------------------\n");
    
    while (current != NULL) {
        if (current->ward == ward) {
            printBedBasicInfo(current);
            
            if (current->isOccupied) {
                printPatientInfo(&current->patient);
                occupied++;
            }
            printf("\n----------------------------------------------------------------\n");
            found = 1;
            total++;
        }
        current = current->next;
    }
    
    if (!found) {
        printf("未找到病房号为%d的床位\n", ward);
    } else {
        printf("\n统计信息：该病房总床位数: %d | 已占用: %d | 空闲: %d\n", total, occupied, total - occupied);
    }
    pause();
}

void filterBedsByDepartment() {
    printOperationTitle("按科室筛选");
    
    int department;
    printf("输入要筛选的科室编号 (1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他): ");
    scanf("%d", &department);
    flushStdin();

    struct Bed* current = head;
    int found = 0;
    int total = 0;
    int occupied = 0;
    
    printf("\n科室 ");
    printDepartment(department);
    printf(" 的床位列表：\n");
    printf("----------------------------------------------------------------\n");
    
    while (current != NULL) {
        if (current->department == department) {
            printBedBasicInfo(current);
            
            if (current->isOccupied) {
                printPatientInfo(&current->patient);
                occupied++;
            }
            printf("\n----------------------------------------------------------------\n");
            found = 1;
            total++;
        }
        current = current->next;
    }
    
    if (!found) {
        printf("未找到科室编号为%d的床位\n", department);
    } else {
        printf("\n统计信息：该科室总床位数: %d | 已占用: %d | 空闲: %d\n", total, occupied, total - occupied);
    }
    pause();
}

// 释放内存
void cleanupMemory() {
    // 床位链表内存清理
    struct Bed* current = head;
    while (current != NULL) {
        struct Bed* temp = current;
        current = current->next;
        free(temp);
    }
    
    // 医生链表内存清理
    struct Doctor* currentDoctor = doctorHead;
    while (currentDoctor != NULL) {
        struct Doctor* tempDoctor = currentDoctor;
        currentDoctor = currentDoctor->next;
        free(tempDoctor);
    }
    
    // 医生-病人关联链表内存清理
    struct DoctorPatientRelation* currentDP = doctorPatientHead;
    while (currentDP != NULL) {
        struct DoctorPatientRelation* tempDP = currentDP;
        currentDP = currentDP->next;
        free(tempDP);
    }
    
    // 医生-病房关联链表内存清理
    struct DoctorWardRelation* currentDW = doctorWardHead;
    while (currentDW != NULL) {
        struct DoctorWardRelation* tempDW = currentDW;
        currentDW = currentDW->next;
        free(tempDW);
    }
}

// 打印医生职称的辅助函数
void printQualification(int qualification) {
    switch(qualification) {
        case 1: printf("住院医师"); break;
        case 2: printf("主治医师"); break;
        case 3: printf("副主任医师"); break;
        case 4: printf("主任医师"); break;
        default: printf("未知职称");
    }
}

// 打印医生信息的辅助函数
void printDoctorBasicInfo(struct Doctor* doctor) {
    printf("医生ID: %d | ", doctor->doctorID);
    printf("姓名: %s | ", doctor->name);
    printf("性别: %s | ", doctor->gender ? "男" : "女");
    printf("电话: %s | ", doctor->phone);
    
    printf("科室: ");
    printDepartment(doctor->department);
    printf(" | ");
    
    printf("专业: %s | ", doctor->specialization);
    
    printf("职称: ");
    printQualification(doctor->qualification);
    printf(" | ");
    
    printf("办公室: %s", doctor->officeLocation);
}

// 获取用户输入的医生信息
void getDoctorInfo(struct Doctor* doctor) {
    printf("输入医生ID: ");
    scanf("%d", &doctor->doctorID);
    flushStdin();
    
    printf("输入医生姓名: ");
    scanf("%s", doctor->name);
    flushStdin();
    
    printf("输入医生性别 (1男, 0女): ");
    scanf("%d", &doctor->gender);
    flushStdin();
    
    printf("输入医生电话: ");
    scanf("%s", doctor->phone);
    flushStdin();
    
    printf("输入所属科室编号 (1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他): ");
    scanf("%d", &doctor->department);
    flushStdin();
    
    printf("输入专业/专长: ");
    scanf(" %[^\n]", doctor->specialization);
    flushStdin();
    
    printf("输入职称 (1-住院医师, 2-主治医师, 3-副主任医师, 4-主任医师): ");
    scanf("%d", &doctor->qualification);
    flushStdin();
    
    printf("输入办公室位置: ");
    scanf(" %[^\n]", doctor->officeLocation);
    flushStdin();
}

// 从CSV文件加载医生数据
void loadDoctorsFromFile(const char* filename) {
    printf("正在加载医生数据...\n");
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件 %s，将创建新文件\n", filename);
        return;
    }

    int recordCount = 0;
    char line[1024]; // 足够大的缓冲区来存储一行CSV数据
    
    // 读取并跳过CSV文件头
    if (fgets(line, sizeof(line), file) == NULL) {
        printf("文件为空或格式不正确\n");
        fclose(file);
        return;
    }
    
    // 读取数据记录
    while (fgets(line, sizeof(line), file) != NULL) {
        struct Doctor* newDoctor = (struct Doctor*)malloc(sizeof(struct Doctor));
        if (newDoctor == NULL) {
            printf("内存分配失败\n");
            fclose(file);
            return;
        }
        
        // 初始化字符串字段
        newDoctor->name[0] = '\0';
        newDoctor->phone[0] = '\0';
        newDoctor->specialization[0] = '\0';
        newDoctor->officeLocation[0] = '\0';
        
        // 使用sscanf解析CSV行
        char nameBuf[50], phoneBuf[20], specializationBuf[50], officeLocationBuf[30];
        int itemsRead = sscanf(line, "%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,]",
            &newDoctor->doctorID,
            nameBuf,
            &newDoctor->gender,
            phoneBuf,
            &newDoctor->department,
            specializationBuf,
            &newDoctor->qualification,
            officeLocationBuf);
        
        // 检查是否成功读取所有字段
        if (itemsRead < 8) {
            printf("警告: 行格式不正确, 只读取到%d个字段，跳过此行\n", itemsRead);
            free(newDoctor);
            continue;
        }
        
        // 复制字符串字段
        strncpy(newDoctor->name, nameBuf, sizeof(newDoctor->name) - 1);
        newDoctor->name[sizeof(newDoctor->name) - 1] = '\0'; // 确保以null结尾
        
        strncpy(newDoctor->phone, phoneBuf, sizeof(newDoctor->phone) - 1);
        newDoctor->phone[sizeof(newDoctor->phone) - 1] = '\0';
        
        strncpy(newDoctor->specialization, specializationBuf, sizeof(newDoctor->specialization) - 1);
        newDoctor->specialization[sizeof(newDoctor->specialization) - 1] = '\0';
        
        strncpy(newDoctor->officeLocation, officeLocationBuf, sizeof(newDoctor->officeLocation) - 1);
        newDoctor->officeLocation[sizeof(newDoctor->officeLocation) - 1] = '\0';
        
        // 添加到链表
        newDoctor->next = doctorHead;
        doctorHead = newDoctor;
        recordCount++;
    }

    fclose(file);
    printf("医生信息加载成功！共加载 %d 条记录\n", recordCount);
}

// 保存医生数据到CSV文件
void saveDoctorsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        pause();
        return;
    }

    // 写入CSV文件头
    fprintf(file, "doctorID,name,gender,phone,department,specialization,qualification,officeLocation\n");
    
    struct Doctor* current = doctorHead;
    int count = 0;
    while (current != NULL) {
        // 写入CSV格式的数据行
        fprintf(file, "%d,\"%s\",%d,\"%s\",%d,\"%s\",%d,\"%s\"\n",
            current->doctorID,
            current->name,
            current->gender,
            current->phone,
            current->department,
            current->specialization,
            current->qualification,
            current->officeLocation);

        current = current->next;
        count++;
    }

    fclose(file);
    printf("\n? 医生信息保存成功！共保存 %d 条记录到CSV文件\n", count);
}

// 从CSV文件加载医生-病人关联数据
void loadDoctorPatientFromFile(const char* filename) {
    printf("正在加载医生-病人关联数据...\n");
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件 %s，将创建新文件\n", filename);
        return;
    }

    int recordCount = 0;
    char line[1024]; // 足够大的缓冲区来存储一行CSV数据
    
    // 读取并跳过CSV文件头
    if (fgets(line, sizeof(line), file) == NULL) {
        printf("文件为空或格式不正确\n");
        fclose(file);
        return;
    }
    
    // 读取数据记录
    while (fgets(line, sizeof(line), file) != NULL) {
        struct DoctorPatientRelation* newRelation = (struct DoctorPatientRelation*)malloc(sizeof(struct DoctorPatientRelation));
        if (newRelation == NULL) {
            printf("内存分配失败\n");
            fclose(file);
            return;
        }
        
        // 初始化字符串字段
        newRelation->notes[0] = '\0';
        newRelation->startDate[0] = '\0';
        
        // 使用sscanf解析CSV行
        char notesBuf[100], startDateBuf[20];
        int itemsRead = sscanf(line, "%d,%d,%[^,],%[^,]",
            &newRelation->doctorID,
            &newRelation->patientID,
            notesBuf,
            startDateBuf);
        
        // 检查是否成功读取所有字段
        if (itemsRead < 4) {
            printf("警告: 行格式不正确, 只读取到%d个字段，跳过此行\n", itemsRead);
            free(newRelation);
            continue;
        }
        
        // 复制字符串字段
        strncpy(newRelation->notes, notesBuf, sizeof(newRelation->notes) - 1);
        newRelation->notes[sizeof(newRelation->notes) - 1] = '\0'; // 确保以null结尾
        
        strncpy(newRelation->startDate, startDateBuf, sizeof(newRelation->startDate) - 1);
        newRelation->startDate[sizeof(newRelation->startDate) - 1] = '\0';
        
        // 添加到链表
        newRelation->next = doctorPatientHead;
        doctorPatientHead = newRelation;
        recordCount++;
    }

    fclose(file);
    printf("医生-病人关联数据加载成功！共加载 %d 条记录\n", recordCount);
}

// 保存医生-病人关联数据到CSV文件
void saveDoctorPatientToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        pause();
        return;
    }

    // 写入CSV文件头
    fprintf(file, "doctorID,patientID,notes,startDate\n");
    
    struct DoctorPatientRelation* current = doctorPatientHead;
    int count = 0;
    while (current != NULL) {
        // 写入CSV格式的数据行
        fprintf(file, "%d,%d,\"%s\",\"%s\"\n",
            current->doctorID,
            current->patientID,
            current->notes,
            current->startDate);

        current = current->next;
        count++;
    }

    fclose(file);
    printf("\n? 医生-病人关联数据保存成功！共保存 %d 条记录到CSV文件\n", count);
}

// 从CSV文件加载医生-病房关联数据
void loadDoctorWardFromFile(const char* filename) {
    printf("正在加载医生-病房关联数据...\n");
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件 %s，将创建新文件\n", filename);
        return;
    }

    int recordCount = 0;
    char line[1024]; // 足够大的缓冲区来存储一行CSV数据
    
    // 读取并跳过CSV文件头
    if (fgets(line, sizeof(line), file) == NULL) {
        printf("文件为空或格式不正确\n");
        fclose(file);
        return;
    }
    
    // 读取数据记录
    while (fgets(line, sizeof(line), file) != NULL) {
        struct DoctorWardRelation* newRelation = (struct DoctorWardRelation*)malloc(sizeof(struct DoctorWardRelation));
        if (newRelation == NULL) {
            printf("内存分配失败\n");
            fclose(file);
            return;
        }
        
        // 初始化字符串字段
        newRelation->scheduleInfo[0] = '\0';
        
        // 使用sscanf解析CSV行
        char scheduleInfoBuf[100];
        int itemsRead = sscanf(line, "%d,%d,%d,%[^,]",
            &newRelation->doctorID,
            &newRelation->wardNumber,
            &newRelation->isHeadDoctor,
            scheduleInfoBuf);
        
        // 检查是否成功读取所有字段
        if (itemsRead < 4) {
            printf("警告: 行格式不正确, 只读取到%d个字段，跳过此行\n", itemsRead);
            free(newRelation);
            continue;
        }
        
        // 复制字符串字段
        strncpy(newRelation->scheduleInfo, scheduleInfoBuf, sizeof(newRelation->scheduleInfo) - 1);
        newRelation->scheduleInfo[sizeof(newRelation->scheduleInfo) - 1] = '\0'; // 确保以null结尾
        
        // 添加到链表
        newRelation->next = doctorWardHead;
        doctorWardHead = newRelation;
        recordCount++;
    }

    fclose(file);
    printf("医生-病房关联数据加载成功！共加载 %d 条记录\n", recordCount);
}

// 保存医生-病房关联数据到CSV文件
void saveDoctorWardToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        pause();
        return;
    }

    // 写入CSV文件头
    fprintf(file, "doctorID,wardNumber,isHeadDoctor,scheduleInfo\n");
    
    struct DoctorWardRelation* current = doctorWardHead;
    int count = 0;
    while (current != NULL) {
        // 写入CSV格式的数据行
        fprintf(file, "%d,%d,%d,\"%s\"\n",
            current->doctorID,
            current->wardNumber,
            current->isHeadDoctor,
            current->scheduleInfo);

        current = current->next;
        count++;
    }

    fclose(file);
    printf("\n? 医生-病房关联数据保存成功！共保存 %d 条记录到CSV文件\n", count);
}

// 添加医生记录
void addDoctor() {
    printOperationTitle("添加医生记录");
    
    struct Doctor* newDoctor = (struct Doctor*)malloc(sizeof(struct Doctor));
    if (newDoctor == NULL) {
        printf("内存分配失败\n");
        pause();
        return;
    }

    printf("输入医生ID: ");
    scanf("%d", &newDoctor->doctorID);
    flushStdin(); // 清空输入缓冲区
    
    // 检查ID是否已存在
    struct Doctor* current = doctorHead;
    while (current != NULL) {
        if (current->doctorID == newDoctor->doctorID) {
            printf("错误: 医生ID %d 已存在，请使用其他ID\n", newDoctor->doctorID);
            free(newDoctor);
            pause();
            return;
        }
        current = current->next;
    }
    
    // 获取医生其他信息
    printf("输入医生姓名: ");
    scanf("%s", newDoctor->name);
    flushStdin();
    
    printf("输入医生性别 (1男, 0女): ");
    scanf("%d", &newDoctor->gender);
    flushStdin();
    
    printf("输入医生电话: ");
    scanf("%s", newDoctor->phone);
    flushStdin();
    
    printf("输入所属科室编号 (1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他): ");
    scanf("%d", &newDoctor->department);
    flushStdin();
    
    printf("输入专业/专长: ");
    scanf(" %[^\n]", newDoctor->specialization);
    flushStdin();
    
    printf("输入职称 (1-住院医师, 2-主治医师, 3-副主任医师, 4-主任医师): ");
    scanf("%d", &newDoctor->qualification);
    flushStdin();
    
    printf("输入办公室位置: ");
    scanf(" %[^\n]", newDoctor->officeLocation);
    flushStdin();
    
    // 添加到链表
    newDoctor->next = doctorHead;
    doctorHead = newDoctor;

    printf("\n? 医生添加成功！新增医生信息如下：\n");
    printSeparator();
    printDoctorBasicInfo(newDoctor);
    printf("\n");
    printSeparator();
    pause();
}

// 修改医生信息
void modifyDoctor() {
    printOperationTitle("修改医生信息");
    
    int id;
    printf("输入要修改的医生ID: ");
    scanf("%d", &id);
    flushStdin();

    struct Doctor* current = doctorHead;
    while (current != NULL) {
        if (current->doctorID == id) {
            printf("\n当前医生信息：\n");
            printSeparator();
            printDoctorBasicInfo(current);
            printf("\n");
            printSeparator();
            
            printf("\n请输入新的信息：\n");
            printf("输入新的姓名: ");
            scanf("%s", current->name);
            flushStdin();
            
            printf("输入新的性别 (1男, 0女): ");
            scanf("%d", &current->gender);
            flushStdin();
            
            printf("输入新的电话: ");
            scanf("%s", current->phone);
            flushStdin();
            
            printf("输入新的科室编号 (1-内科, 2-外科, 3-儿科, 4-妇科, 5-其他): ");
            scanf("%d", &current->department);
            flushStdin();
            
            printf("输入新的专业/专长: ");
            scanf(" %[^\n]", current->specialization);
            flushStdin();
            
            printf("输入新的职称 (1-住院医师, 2-主治医师, 3-副主任医师, 4-主任医师): ");
            scanf("%d", &current->qualification);
            flushStdin();
            
            printf("输入新的办公室位置: ");
            scanf(" %[^\n]", current->officeLocation);
            flushStdin();
            
            printf("\n? 医生信息修改成功！更新后信息如下：\n");
            printSeparator();
            printDoctorBasicInfo(current);
            printf("\n");
            printSeparator();
            pause();
            return;
        }
        current = current->next;
    }

    printf("\n? 未找到医生ID为%d的医生\n", id);
    pause();
}

// 检查医生是否有关联的病人
int doctorHasPatients(int doctorID) {
    struct DoctorPatientRelation* current = doctorPatientHead;
    while (current != NULL) {
        if (current->doctorID == doctorID) {
            return 1; // 有关联的病人
        }
        current = current->next;
    }
    return 0; // 没有关联的病人
}

// 检查医生是否有关联的病房
int doctorHasWards(int doctorID) {
    struct DoctorWardRelation* current = doctorWardHead;
    while (current != NULL) {
        if (current->doctorID == doctorID) {
            return 1; // 有关联的病房
        }
        current = current->next;
    }
    return 0; // 没有关联的病房
}

// 删除医生记录
void deleteDoctor() {
    printOperationTitle("删除医生记录");
    
    int id;
    printf("输入要删除的医生ID: ");
    scanf("%d", &id);
    flushStdin();

    // 检查医生是否有关联的病人或病房
    if (doctorHasPatients(id)) {
        printf("\n? 错误：医生ID %d 当前有关联的病人，无法删除。请先解除病人关联。\n", id);
        pause();
        return;
    }
    
    if (doctorHasWards(id)) {
        printf("\n? 错误：医生ID %d 当前有关联的病房，无法删除。请先解除病房关联。\n", id);
        pause();
        return;
    }

    struct Doctor* current = doctorHead;
    struct Doctor* prev = NULL;

    while (current != NULL) {
        if (current->doctorID == id) {
            if (prev == NULL) {
                doctorHead = current->next;
            }
            else {
                prev->next = current->next;
            }
            free(current);
            printf("\n? 医生ID为%d的记录删除成功\n", id);
            pause();
            return;
        }
        prev = current;
        current = current->next;
    }

    printf("\n? 未找到医生ID为%d的医生\n", id);
    pause();
}

// 根据ID查询医生信息
void searchDoctorByID() {
    printOperationTitle("医生信息查询");
    
    int id;
    printf("输入要查找的医生ID: ");
    scanf("%d", &id);
    flushStdin();

    struct Doctor* current = doctorHead;
    while (current != NULL) {
        if (current->doctorID == id) {
            printf("\n查询结果：\n");
            printSeparator();
            printDoctorBasicInfo(current);
            printf("\n");
            printSeparator();
            
            // 查询该医生负责的病人
            printf("\n该医生负责的病人列表：\n");
            printSeparator();
            int patientCount = 0;
            struct DoctorPatientRelation* dpRelation = doctorPatientHead;
            while (dpRelation != NULL) {
                if (dpRelation->doctorID == id) {
                    printf("病人ID: %d | 医疗备注: %s | 开始日期: %s\n", 
                           dpRelation->patientID, dpRelation->notes, dpRelation->startDate);
                    patientCount++;
                }
                dpRelation = dpRelation->next;
            }
            
            if (patientCount == 0) {
                printf("该医生暂无负责的病人\n");
            }
            printSeparator();
            
            // 查询该医生负责的病房
            printf("\n该医生负责的病房列表：\n");
            printSeparator();
            int wardCount = 0;
            struct DoctorWardRelation* dwRelation = doctorWardHead;
            while (dwRelation != NULL) {
                if (dwRelation->doctorID == id) {
                    printf("病房号: %d | 主治医生: %s | 查房安排: %s\n", 
                           dwRelation->wardNumber, 
                           dwRelation->isHeadDoctor ? "是" : "否", 
                           dwRelation->scheduleInfo);
                    wardCount++;
                }
                dwRelation = dwRelation->next;
            }
            
            if (wardCount == 0) {
                printf("该医生暂无负责的病房\n");
            }
            printSeparator();
            
            pause();
            return;
        }
        current = current->next;
    }

    printf("\n? 未找到医生ID为%d的医生\n", id);
    pause();
}

// 列出所有医生信息
void listAllDoctors() {
    printOperationTitle("所有医生信息");
    
    struct Doctor* current = doctorHead;
    if (current == NULL) {
        printf("当前没有医生信息\n");
        pause();
        return;
    }
    
    int count = 0;
    int deptCount[6] = {0}; // 用于统计各科室医生数量，索引0未使用
    
    printf("所有医生列表：\n");
    printf("----------------------------------------------------------------\n");
    
    while (current != NULL) {
        printDoctorBasicInfo(current);
        printf("\n----------------------------------------------------------------\n");
        
        // 统计各科室医生数量
        if (current->department >= 1 && current->department <= 5) {
            deptCount[current->department]++;
        }
        
        current = current->next;
        count++;
    }
    
    printf("\n统计信息：总医生数: %d | ", count);
    printf("内科: %d | 外科: %d | 儿科: %d | 妇科: %d | 其他: %d\n", 
           deptCount[1], deptCount[2], deptCount[3], deptCount[4], deptCount[5]);
    
    pause();
}

// 检查病人是否存在
int patientExists(int patientID) {
    struct Bed* current = head;
    while (current != NULL) {
        if (current->isOccupied && current->patient.patientID == patientID) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// 检查病房是否存在
int wardExists(int wardNumber) {
    struct Bed* current = head;
    while (current != NULL) {
        if (current->ward == wardNumber) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// 检查医生是否存在
int doctorExists(int doctorID) {
    struct Doctor* current = doctorHead;
    while (current != NULL) {
        if (current->doctorID == doctorID) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// 检查医生-病人关联是否已存在
int doctorPatientRelationExists(int doctorID, int patientID) {
    struct DoctorPatientRelation* current = doctorPatientHead;
    while (current != NULL) {
        if (current->doctorID == doctorID && current->patientID == patientID) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// 检查医生-病房关联是否已存在
int doctorWardRelationExists(int doctorID, int wardNumber) {
    struct DoctorWardRelation* current = doctorWardHead;
    while (current != NULL) {
        if (current->doctorID == doctorID && current->wardNumber == wardNumber) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// 分配病人给医生
void assignPatientToDoctor() {
    printOperationTitle("分配病人给医生");
    
    int doctorID, patientID;
    
    printf("输入医生ID: ");
    scanf("%d", &doctorID);
    flushStdin();
    
    // 检查医生是否存在
    if (!doctorExists(doctorID)) {
        printf("\n? 错误：医生ID %d 不存在\n", doctorID);
        pause();
        return;
    }
    
    printf("输入病人ID: ");
    scanf("%d", &patientID);
    flushStdin();
    
    // 检查病人是否存在
    if (!patientExists(patientID)) {
        printf("\n? 错误：病人ID %d 不存在\n", patientID);
        pause();
        return;
    }
    
    // 检查关联是否已存在
    if (doctorPatientRelationExists(doctorID, patientID)) {
        printf("\n? 错误：医生ID %d 与病人ID %d 的关联已存在\n", doctorID, patientID);
        pause();
        return;
    }
    
    // 创建新的关联记录
    struct DoctorPatientRelation* newRelation = (struct DoctorPatientRelation*)malloc(sizeof(struct DoctorPatientRelation));
    if (newRelation == NULL) {
        printf("内存分配失败\n");
        pause();
        return;
    }
    
    newRelation->doctorID = doctorID;
    newRelation->patientID = patientID;
    
    printf("输入医疗备注: ");
    scanf(" %[^\n]", newRelation->notes);
    flushStdin();
    
    printf("输入开始负责日期 (格式: YYYY-MM-DD): ");
    scanf("%s", newRelation->startDate);
    flushStdin();
    
    // 添加到链表
    newRelation->next = doctorPatientHead;
    doctorPatientHead = newRelation;
    
    printf("\n? 医生-病人关联建立成功！\n");
    printf("医生ID: %d | 病人ID: %d | 医疗备注: %s | 开始日期: %s\n", 
           newRelation->doctorID, newRelation->patientID, newRelation->notes, newRelation->startDate);
    
    pause();
}

// 解除医生与病人的关联
void removePatientFromDoctor() {
    printOperationTitle("解除医生-病人关联");
    
    int doctorID, patientID;
    
    printf("输入医生ID: ");
    scanf("%d", &doctorID);
    flushStdin();
    
    printf("输入病人ID: ");
    scanf("%d", &patientID);
    flushStdin();
    
    struct DoctorPatientRelation* current = doctorPatientHead;
    struct DoctorPatientRelation* prev = NULL;
    
    while (current != NULL) {
        if (current->doctorID == doctorID && current->patientID == patientID) {
            printf("\n将要删除的关联信息：\n");
            printf("医生ID: %d | 病人ID: %d | 医疗备注: %s | 开始日期: %s\n", 
                   current->doctorID, current->patientID, current->notes, current->startDate);
            
            printf("\n确认删除? (1确认, 0取消): ");
            int confirm;
            scanf("%d", &confirm);
            flushStdin();
            
            if (confirm) {
                if (prev == NULL) {
                    doctorPatientHead = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                printf("\n? 医生-病人关联解除成功\n");
            } else {
                printf("\n操作已取消\n");
            }
            
            pause();
            return;
        }
        
        prev = current;
        current = current->next;
    }
    
    printf("\n? 未找到医生ID %d 和病人ID %d 的关联记录\n", doctorID, patientID);
    pause();
}

// 查询医生负责的所有病人
void listPatientsByDoctor() {
    printOperationTitle("查询医生负责的病人");
    
    int doctorID;
    printf("输入医生ID: ");
    scanf("%d", &doctorID);
    flushStdin();
    
    // 检查医生是否存在
    if (!doctorExists(doctorID)) {
        printf("\n? 错误：医生ID %d 不存在\n", doctorID);
        pause();
        return;
    }
    
    // 显示医生基本信息
    struct Doctor* doctor = doctorHead;
    while (doctor != NULL) {
        if (doctor->doctorID == doctorID) {
            printf("\n医生信息：\n");
            printSeparator();
            printDoctorBasicInfo(doctor);
            printf("\n");
            printSeparator();
            break;
        }
        doctor = doctor->next;
    }
    
    // 显示该医生负责的病人列表
    printf("\n该医生负责的病人列表：\n");
    printSeparator();
    
    int count = 0;
    struct DoctorPatientRelation* relation = doctorPatientHead;
    
    while (relation != NULL) {
        if (relation->doctorID == doctorID) {
            // 查找病人详细信息
            struct Bed* bed = head;
            while (bed != NULL) {
                if (bed->isOccupied && bed->patient.patientID == relation->patientID) {
                    printf("病人ID: %d | 姓名: %s | 诊断: %s | 床位ID: %d | 病房: %d\n",
                           bed->patient.patientID, bed->patient.name, bed->patient.diagnosis, 
                           bed->ID, bed->ward);
                    printf("医疗备注: %s | 开始负责日期: %s\n",
                           relation->notes, relation->startDate);
                    printf("----------------------------------------------------------------\n");
                    count++;
                    break;
                }
                bed = bed->next;
            }
            
            // 如果在床位中找不到该病人信息，只显示关联信息
            if (bed == NULL) {
                printf("病人ID: %d | 医疗备注: %s | 开始负责日期: %s\n",
                       relation->patientID, relation->notes, relation->startDate);
                printf("(注: 未找到该病人的详细信息)\n");
                printf("----------------------------------------------------------------\n");
                count++;
            }
        }
        relation = relation->next;
    }
    
    if (count == 0) {
        printf("该医生暂无负责的病人\n");
    } else {
        printf("\n? 共找到 %d 条病人记录\n", count);
    }
    
    pause();
}

// 查询病人的主治医生
void listDoctorsByPatient() {
    printOperationTitle("查询病人的医生");
    
    int patientID;
    printf("输入病人ID: ");
    scanf("%d", &patientID);
    flushStdin();
    
    // 检查病人是否存在
    if (!patientExists(patientID)) {
        printf("\n? 错误：病人ID %d 不存在\n", patientID);
        pause();
        return;
    }
    
    // 显示病人基本信息
    struct Bed* bed = head;
    while (bed != NULL) {
        if (bed->isOccupied && bed->patient.patientID == patientID) {
            printf("\n病人信息：\n");
            printSeparator();
            printPatientInfo(&bed->patient);
            printf("\n床位ID: %d | 病房: %d | 科室: ", bed->ID, bed->ward);
            printDepartment(bed->department);
            printf("\n");
            printSeparator();
            break;
        }
        bed = bed->next;
    }
    
    // 显示负责该病人的医生列表
    printf("\n负责该病人的医生列表：\n");
    printSeparator();
    
    int count = 0;
    struct DoctorPatientRelation* relation = doctorPatientHead;
    
    while (relation != NULL) {
        if (relation->patientID == patientID) {
            // 查找医生详细信息
            struct Doctor* doctor = doctorHead;
            while (doctor != NULL) {
                if (doctor->doctorID == relation->doctorID) {
                    printDoctorBasicInfo(doctor);
                    printf("\n医疗备注: %s | 开始负责日期: %s\n",
                           relation->notes, relation->startDate);
                    printf("----------------------------------------------------------------\n");
                    count++;
                    break;
                }
                doctor = doctor->next;
            }
            
            // 如果找不到该医生信息，只显示关联信息
            if (doctor == NULL) {
                printf("医生ID: %d | 医疗备注: %s | 开始负责日期: %s\n",
                       relation->doctorID, relation->notes, relation->startDate);
                printf("(注: 未找到该医生的详细信息)\n");
                printf("----------------------------------------------------------------\n");
                count++;
            }
        }
        relation = relation->next;
    }
    
    if (count == 0) {
        printf("该病人暂无负责的医生\n");
    } else {
        printf("\n? 共找到 %d 条医生记录\n", count);
    }
    
    pause();
}

// 分配病房给医生
void assignWardToDoctor() {
    printOperationTitle("分配病房给医生");
    
    int doctorID, wardNumber;
    
    printf("输入医生ID: ");
    scanf("%d", &doctorID);
    flushStdin();
    
    // 检查医生是否存在
    if (!doctorExists(doctorID)) {
        printf("\n? 错误：医生ID %d 不存在\n", doctorID);
        pause();
        return;
    }
    
    printf("输入病房号: ");
    scanf("%d", &wardNumber);
    flushStdin();
    
    // 检查病房是否存在
    if (!wardExists(wardNumber)) {
        printf("\n? 错误：病房号 %d 不存在\n", wardNumber);
        pause();
        return;
    }
    
    // 检查关联是否已存在
    if (doctorWardRelationExists(doctorID, wardNumber)) {
        printf("\n? 错误：医生ID %d 与病房号 %d 的关联已存在\n", doctorID, wardNumber);
        pause();
        return;
    }
    
    // 创建新的关联记录
    struct DoctorWardRelation* newRelation = (struct DoctorWardRelation*)malloc(sizeof(struct DoctorWardRelation));
    if (newRelation == NULL) {
        printf("内存分配失败\n");
        pause();
        return;
    }
    
    newRelation->doctorID = doctorID;
    newRelation->wardNumber = wardNumber;
    
    printf("是否为主治医生 (1是, 0否): ");
    scanf("%d", &newRelation->isHeadDoctor);
    flushStdin();
    
    printf("输入查房安排: ");
    scanf(" %[^\n]", newRelation->scheduleInfo);
    flushStdin();
    
    // 添加到链表
    newRelation->next = doctorWardHead;
    doctorWardHead = newRelation;
    
    printf("\n? 医生-病房关联建立成功！\n");
    printf("医生ID: %d | 病房号: %d | 主治医生: %s | 查房安排: %s\n", 
           newRelation->doctorID, newRelation->wardNumber, 
           newRelation->isHeadDoctor ? "是" : "否", newRelation->scheduleInfo);
    
    pause();
}

// 解除医生与病房的关联
void removeWardFromDoctor() {
    printOperationTitle("解除医生-病房关联");
    
    int doctorID, wardNumber;
    
    printf("输入医生ID: ");
    scanf("%d", &doctorID);
    flushStdin();
    
    printf("输入病房号: ");
    scanf("%d", &wardNumber);
    flushStdin();
    
    struct DoctorWardRelation* current = doctorWardHead;
    struct DoctorWardRelation* prev = NULL;
    
    while (current != NULL) {
        if (current->doctorID == doctorID && current->wardNumber == wardNumber) {
            printf("\n将要删除的关联信息：\n");
            printf("医生ID: %d | 病房号: %d | 主治医生: %s | 查房安排: %s\n", 
                   current->doctorID, current->wardNumber, 
                   current->isHeadDoctor ? "是" : "否", current->scheduleInfo);
            
            printf("\n确认删除? (1确认, 0取消): ");
            int confirm;
            scanf("%d", &confirm);
            flushStdin();
            
            if (confirm) {
                if (prev == NULL) {
                    doctorWardHead = current->next;
                } else {
                    prev->next = current->next;
                }
                free(current);
                printf("\n? 医生-病房关联解除成功\n");
            } else {
                printf("\n操作已取消\n");
            }
            
            pause();
            return;
        }
        
        prev = current;
        current = current->next;
    }
    
    printf("\n? 未找到医生ID %d 和病房号 %d 的关联记录\n", doctorID, wardNumber);
    pause();
}

// 查询医生负责的所有病房
void listWardsByDoctor() {
    printOperationTitle("查询医生负责的病房");
    
    int doctorID;
    printf("输入医生ID: ");
    scanf("%d", &doctorID);
    flushStdin();
    
    // 检查医生是否存在
    if (!doctorExists(doctorID)) {
        printf("\n? 错误：医生ID %d 不存在\n", doctorID);
        pause();
        return;
    }
    
    // 显示医生基本信息
    struct Doctor* doctor = doctorHead;
    while (doctor != NULL) {
        if (doctor->doctorID == doctorID) {
            printf("\n医生信息：\n");
            printSeparator();
            printDoctorBasicInfo(doctor);
            printf("\n");
            printSeparator();
            break;
        }
        doctor = doctor->next;
    }
    
    // 显示该医生负责的病房列表
    printf("\n该医生负责的病房列表：\n");
    printSeparator();
    
    int count = 0;
    struct DoctorWardRelation* relation = doctorWardHead;
    
    while (relation != NULL) {
        if (relation->doctorID == doctorID) {
            printf("病房号: %d | 主治医生: %s | 查房安排: %s\n", 
                   relation->wardNumber, 
                   relation->isHeadDoctor ? "是" : "否", 
                   relation->scheduleInfo);
            
            // 显示该病房中的床位数量
            int bedCount = 0;
            int occupiedCount = 0;
            struct Bed* bed = head;
            while (bed != NULL) {
                if (bed->ward == relation->wardNumber) {
                    bedCount++;
                    if (bed->isOccupied) {
                        occupiedCount++;
                    }
                }
                bed = bed->next;
            }
            
            printf("该病房床位情况: 总床位数: %d | 已占用: %d | 空闲: %d\n", 
                   bedCount, occupiedCount, bedCount - occupiedCount);
            printf("----------------------------------------------------------------\n");
            count++;
        }
        relation = relation->next;
    }
    
    if (count == 0) {
        printf("该医生暂无负责的病房\n");
    } else {
        printf("\n? 共找到 %d 条病房记录\n", count);
    }
    
    pause();
}

// 查询病房的所有医生
void listDoctorsByWard() {
    printOperationTitle("查询病房的医生");
    
    int wardNumber;
    printf("输入病房号: ");
    scanf("%d", &wardNumber);
    flushStdin();
    
    // 检查病房是否存在
    if (!wardExists(wardNumber)) {
        printf("\n? 错误：病房号 %d 不存在\n", wardNumber);
        pause();
        return;
    }
    
    // 显示病房基本信息
    int bedCount = 0;
    int occupiedCount = 0;
    int department = 0;
    struct Bed* bed = head;
    while (bed != NULL) {
        if (bed->ward == wardNumber) {
            bedCount++;
            if (bed->isOccupied) {
                occupiedCount++;
            }
            department = bed->department; // 假设同一病房的科室相同
        }
        bed = bed->next;
    }
    
    printf("\n病房信息：\n");
    printSeparator();
    printf("病房号: %d | 科室: ", wardNumber);
    printDepartment(department);
    printf(" | 总床位数: %d | 已占用: %d | 空闲: %d\n", 
           bedCount, occupiedCount, bedCount - occupiedCount);
    printSeparator();
    
    // 显示负责该病房的医生列表
    printf("\n负责该病房的医生列表：\n");
    printSeparator();
    
    int count = 0;
    struct DoctorWardRelation* relation = doctorWardHead;
    
    while (relation != NULL) {
        if (relation->wardNumber == wardNumber) {
            // 查找医生详细信息
            struct Doctor* doctor = doctorHead;
            while (doctor != NULL) {
                if (doctor->doctorID == relation->doctorID) {
                    printDoctorBasicInfo(doctor);
                    printf("\n主治医生: %s | 查房安排: %s\n",
                           relation->isHeadDoctor ? "是" : "否", 
                           relation->scheduleInfo);
                    printf("----------------------------------------------------------------\n");
                    count++;
                    break;
                }
                doctor = doctor->next;
            }
            
            // 如果找不到该医生信息，只显示关联信息
            if (doctor == NULL) {
                printf("医生ID: %d | 主治医生: %s | 查房安排: %s\n",
                       relation->doctorID, 
                       relation->isHeadDoctor ? "是" : "否", 
                       relation->scheduleInfo);
                printf("(注: 未找到该医生的详细信息)\n");
                printf("----------------------------------------------------------------\n");
                count++;
            }
        }
        relation = relation->next;
    }
    
    if (count == 0) {
        printf("该病房暂无负责的医生\n");
    } else {
        printf("\n? 共找到 %d 条医生记录\n", count);
    }
    
    pause();
}

int main() {
    int choice;
    
    // 尝试加载数据文件 (改为CSV格式)
    loadBedsFromFile("beds.csv");
    loadDoctorsFromFile("doctors.csv");
    loadDoctorPatientFromFile("doctor_patient.csv");
    loadDoctorWardFromFile("doctor_ward.csv");
    
    // 主循环
    while (1) {
        // 显示菜单
        printMenu();
        
        // 获取用户选择
        if (scanf("%d", &choice) != 1) {
            printf("? 无效的输入，请输入数字选项\n");
            flushStdin();
            printf("\n按回车键继续...");
            getchar();
            continue;
        }
        flushStdin(); // 清空输入缓冲区
        
        // 处理用户选择
        switch (choice) {
        case 1:
            registerPatient();
            break;
        case 2:
            assignBed();
            break;
        case 3:
            dischargePatient();
            break;
        case 4:
            addBed();
            break;
        case 5:
            modifyBed();
            break;
        case 6:
            deleteBed();
            break;
        case 7:
            searchBedByID();
            break;
        case 8:
            listAllBeds();
            break;
        case 9:
            listAvailableBeds();
            break;
        case 10:
            filterBedsByType();
            break;
        case 11:
            filterBedsByWard();
            break;
        case 12:
            filterBedsByDepartment();
            break;
        case 13:
            sortBedsByID();
            break;
        case 14:
            addDoctor();
            break;
        case 15:
            modifyDoctor();
            break;
        case 16:
            deleteDoctor();
            break;
        case 17:
            searchDoctorByID();
            break;
        case 18:
            listAllDoctors();
            break;
        case 19:
            assignPatientToDoctor();
            break;
        case 20:
            assignWardToDoctor();
            break;
        case 21:
            listPatientsByDoctor();
            break;
        case 22:
            listWardsByDoctor();
            break;
        case 24:
            saveBedsToFile("beds.csv"); // 保存为CSV格式
            saveDoctorsToFile("doctors.csv");
            saveDoctorPatientToFile("doctor_patient.csv");
            saveDoctorWardToFile("doctor_ward.csv");
            printf("感谢使用医院床位管理系统，再见！\n");
            cleanupMemory();
            exit(0);
        default:
            printf("? 无效的选项，请输入有效的菜单选项\n");
            printf("\n按回车键继续...");
            getchar();
            break;
        }
    }
    
    return 0;
} 