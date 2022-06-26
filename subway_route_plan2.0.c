#define _CRT_SECURE_NO_DEPRECATE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <Windows.h>
#define INT_MAX 999999
#define MaxNum 344
/*
地铁站点信息
the structure of subway station
*/
typedef struct {
	int num;
	char name[16];    //车站名称 
	int IsTransferStation;   //是否是换乘车站
	int SubwayNum[3];   //车站所属线路号
}station;


typedef struct {
	char name[16];
	int num;
}name_search_stationnum;
/*
全局变量 
*/
station s[MaxNum];   //存放车站信息
char subway_name[31][16] = { "首都机场线","1号线","2号线" ,"3号线" ,"4号线" ,"5号线" ,"6号线" ,"7号线" ,"8号线北段","9号线","10号线","11号线","12号线",
							"13号线","14号线东段","15号线","16号线","17号线","18号线","19号线","西郊线","昌平线","八通线","亦庄线","大兴线","大兴机场线",
	                        "燕房线","房山线","S1线","8号线南段","14号线西段" };
name_search_stationnum r[MaxNum];
int AdjMatrix[MaxNum][MaxNum];   //邻接矩阵 
int D[MaxNum][MaxNum];    //临时矩阵 
int pre[MaxNum][MaxNum];    

/*
利用文件初始化地铁站点信息
Initialize metro station information with file
*/
void InitStation(FILE* p) {
	int num=0, way1 = -1, way2 = -1, way3 = -1;   //-1表示无
	char name[16]="";
	while (!feof(p)) {
		fscanf(p, "%d %s %d,%d,%d", &num, name, &way1, &way2, &way3);
	    //printf( "%d %s %d,%d,%d\n", num, name, way1, way2, way3);   //测试录入结果

		s[num - 1].num = num;
		strcpy(s[num - 1].name, name);
		s[num - 1].SubwayNum[0] = way1;
		s[num - 1].SubwayNum[1] = way2;
		s[num - 1].SubwayNum[2] = way3;
		s[num - 1].IsTransferStation = way2 > 0 ? 1 : 0;   //是否为换乘车站
		way1 = way2 = way3 = -1;   //重置
		strcpy(r[num].name, name);	//0号单元作“哨兵” 
		r[num].num = num;
	}
}    //使用文件初始化车站信息
/*
利用文件构造邻接矩阵，用于存储地图信息 
Using files to construct adjacency matrix for storing map information
*/
void MakeMatrix(FILE* p) {
	int i, j, value;
	for (i = 0;i < MaxNum;i++)
		for (j = 0;j < MaxNum;j++) {
			if (i == j) {
				AdjMatrix[i][j] = 0;
				D[i][j] = 0;
				pre[i][j] = j;
			}
			else {
				AdjMatrix[i][j] = INT_MAX;
				D[i][j] = INT_MAX;
				pre[i][j] = j;
			}
		}           //初始化邻接矩阵和D矩阵,辅助矩阵pre

	while (!feof(p)) {
		fscanf(p, "%d %d %d", &i, &value, &j);
		AdjMatrix[i - 1][j - 1] = value;
		AdjMatrix[j - 1][i - 1] = value;
		D[i - 1][j - 1] = value;
		D[j - 1][i - 1] = value;
	}
	//通过文件建立邻接矩阵和初始化D矩阵
}

/*
计算最短路径 
compute the shortest path
*/
void ShortestPath() {
	int i, j, k;
	for (k = 0;k < MaxNum;k++)
		for (i = 0;i < MaxNum;i++)
			for (j = 0;j < MaxNum;j++) {
				if (D[i][j] > D[i][k] + D[k][j]) {
					D[i][j] = D[i][k] + D[k][j];
					pre[i][j] = pre[i][k];
				}    //插入k点后i到j的距离变小，故更新最短距离，并记录起点的下一个点
			}
}

int Partition(int low, int high) {
	r[0] = r[low];
	char pivotloc[16];
	strcpy(pivotloc, r[low].name);
	while (low < high) {
		while (low < high && strcmp(r[high].name, pivotloc) >= 0) --high;
		r[low] = r[high];
		while (low < high && strcmp(r[low].name, pivotloc) <= 0) ++low;
		r[high] = r[low];
	}
	r[low] = r[0];
	return low;
}

void QSort(int low, int high) {
	int t;
	if (low < high) {
		t = Partition(low, high);
		QSort(low, t - 1);
		QSort(t + 1, high);
	}
}
/*
判断上一个站点与当前站点是否是一条路线 
Determine whether the previous site and the current site are the same route
*/
int sameway(int pre_stationnum, int now_stationnum, station pre, station now) {
	int i, j;

	for (i = 0;i < 3;i++)
		for (j = 0;j < 3;j++)
			if (pre.SubwayNum[i] >= 0 && now.SubwayNum[j] >= 0 && pre.SubwayNum[i] == now.SubwayNum[j])
				return pre.SubwayNum[i];
	return -1;

}

void QuickSort() {
	QSort(1, MaxNum);
}

int Search(char key[]) {
	int low = 1;
	int high = MaxNum;
	int mid;
	while (low <= high) {
		mid = (low + high) / 2;
		if (strcmp(key, r[mid].name) == 0)	return r[mid].num;
		else if (strcmp(key, r[mid].name) <= 0)	high = mid - 1;
		else low = mid + 1;
	}
	return 0;
}
int StationCounter(int start,int end) {
	int count = 1;
	int i = start-1;
	while(1){
		i = pre[i][end-1];
		if (i == end-1) break;
		count++;
	}
	return count;
}   //计算起点和终点之间的站点数目
/*修改pre_way初始化 修改pre_stationnum初始化3 修改终点站线路输出*/
/*
输出推荐路线信息、换乘次数、所需票价 
Output recommended route information, transfer times and required fares
*/
void Print(char startname[], char endname[]) {
	int startnum = Search(startname);
	int endnum = Search(endname);

	printf("\n         ");

	if (startnum == 0) {
		printf("输入的起点站名有误\n");
		return;
	}
	if (endnum == 0) {
		printf("输入的终点站名有误\n");
		return;
	}
	station start = s[startnum - 1];
	station end = s[endnum - 1];


	printf("较短路线 ：%s", start.name);							//输出起点

	int pre_way = start.SubwayNum[0];
	int pre_stationnum = start.num;
	int now_stationnum = pre[start.num - 1][end.num - 1] + 1;
	int way_start = start.num;
	station pre_station, now_station;
    int Tcount=0;
	while (1) {
		pre_station = s[pre_stationnum - 1];
		now_station = s[now_stationnum - 1];
		if (pre_station.IsTransferStation == 1) {
			int way = sameway(pre_stationnum, now_stationnum, pre_station, now_station);//相同路线 
			if (way != pre_way && way >= 0) {
				if (pre_stationnum != startnum) {
					printf("  --%s(%d站)--->  %s", subway_name[pre_way], StationCounter(pre_stationnum, way_start), pre_station.name);
					way_start = pre_stationnum;
					Tcount++;
				}	
				pre_way = way;
			}
		}

		if (now_stationnum == end.num)	break;

		pre_stationnum = now_stationnum;
		now_stationnum = pre[pre_stationnum - 1][end.num - 1] + 1;


	}

	printf("  --%s(%d站)--->  %s\n", subway_name[pre_way],StationCounter(end.num, way_start), end.name);

	printf("         共需要 %d 元         共换乘%d次\n", price(startnum - 1, endnum - 1),Tcount);
}
void resetP(station* platform) {
	platform->IsTransferStation = 0;
	platform->num = 0;
	platform->SubwayNum[0] = -1;
	platform->SubwayNum[1] = -1;
	platform->SubwayNum[2] = -1;
}
/*
输出换乘次数最少的路线 
Output the route with the least transfer times
*/ 
void PrintLeastTransfer(char startname[], char endname[]) {

	station PathPoint[20];
	int PointLocation = 0;
	for (int i = 0;i < 10;i++) {
		PathPoint[i].IsTransferStation = 0;
		PathPoint[i].num = 0;
		PathPoint[i].SubwayNum[0] = -1;
		PathPoint[i].SubwayNum[1] = -1;
		PathPoint[i].SubwayNum[2] = -1;
	}    //对路径记录初始化 
	int startnum = Search(startname);
	int endnum = Search(endname);

	printf("\n         ");

	if (startnum == 0) {
		printf("输入的起点站名有误\n");
		return;
	}
	if (endnum == 0) {
		printf("输入的终点站名有误\n");
		return;
	}

	station start = s[startnum - 1];
	station end = s[endnum - 1];
	PathPoint[PointLocation] = start;
	PointLocation++;
	int pre_way = start.SubwayNum[0];
	int pre_stationnum = start.num;
	int now_stationnum = pre[start.num - 1][end.num - 1] + 1;
    int Tcount=-1;

	station pre_station, now_station;

	while (1) {
		pre_station = s[pre_stationnum - 1];
		now_station = s[now_stationnum - 1];

		if (pre_station.IsTransferStation == 1) {
			int way = sameway(pre_stationnum, now_stationnum, pre_station, now_station);//相同路线 
			if (way != pre_way && way >= 0) {
				if (pre_stationnum != startnum)
				{
					PathPoint[PointLocation] = pre_station;
					PointLocation++;
				}
				pre_way = way;
			}
		}

		if (now_stationnum == end.num)	break;

		pre_stationnum = now_stationnum;
		now_stationnum = pre[pre_stationnum - 1][end.num - 1] + 1;

	}
	PathPoint[PointLocation] = end;  //将起点到终点的之间的所有换乘站加入记录中 
	//printf("--%d号线--->%s",pre_way,end.name);
	for (int i = 0;i < PointLocation;i++) {
		for (int j = PointLocation;j >= i + 2;j--) {
			if (sameway(0, 0, PathPoint[i], PathPoint[j]) != -1) {
				for (int n = i + 1;n < j;n++) {
					if (n == 0 || n == PointLocation) continue; //无法去掉起点和终点 
					resetP(&PathPoint[n]);
				}   //置零去掉多余的换乘站,
				i = j;
				break;
			}
		}
	}    //根据情况2去掉多余的换乘站

	for (int i = 0;i < PointLocation - 1;i++) {
		if (!PathPoint[i].num) continue;
		for (int j = PointLocation;j > i + 1;j--) {
			if (sameway(0, 0, PathPoint[i], PathPoint[j]) != -1) continue;  //两个点在同一个线上则不需要添加点 
			for (int n = 0;n < MaxNum;n++) {
				if (s[n].IsTransferStation) {
					if (sameway(0, 0, s[n], PathPoint[i]) != -1 && sameway(0, 0, s[n], PathPoint[j]) != -1) {
						if (D[PathPoint[i].num - 1][s[n].num - 1] + D[PathPoint[j].num - 1][s[n].num - 1] <= D[PathPoint[i].num - 1][PathPoint[j].num - 1] + (3000) * (j - i - 2)) {
							PathPoint[i + 1] = s[n];
							for (int s = i + 2;s < j;s++) {
								resetP(&PathPoint[s]);
							}
						}
					}
				}
			}
		}
		//在车站顺序表中寻找替换换乘车站 
	}   //根据情况1 用一个换乘站换掉三个多余换乘站 
	station pre, now;
	pre = start;
	pre_way = -1;
	printf("\n         较少换乘：");
	for (int i = 1;i <= PointLocation;i++) {
		if (PathPoint[i].num) {
			now = PathPoint[i];
			if (pre_way == sameway(0, 0, pre, now)) continue;
			printf("%s  ---%s--->  ", pre.name, subway_name[sameway(0, 0, pre, now)], StationCounter(pre.num, now.num));
			pre_way = sameway(0, 0, pre, now);
			Tcount++;
			pre = now;
		}
		else continue;
	}
	printf("%s\n", end.name);   //输出改进线路 
	printf("         共需要 %d 元         共换乘%d次\n", price(startnum - 1, endnum - 1),Tcount);
}  //打印最少换乘的路线 
/*
计算票价 
compute ticket price 
*/ 
int price(int start, int end) {
	int d = D[start][end];
	//printf("%d\n",d);
	if (d <= 6000)	return 3;
	else if (d <= 12000)	return 4;
	else if (d <= 22000)	return 5;
	else if (d <= 32000)	return 6;
	else	return (7+ (d - 32000) / 20000);
}
/*
系统界面 
the system interface 
*/ 
void InitInterface() {
	//system("cls"); 
	printf("        ------------------------------------------\n");
	printf("\n");
	printf("                 欢迎使用北京地铁查询系统         \n");
	printf("\n");
	printf("        ------------------------------------------\n");
	printf("         系统功能 ：\n");
	printf("\n");
	printf("          1、查看北京地铁\n");
	printf("          2、查询地铁路线\n");
	printf("\n");
	printf("          0、退出服务系统\n");
	printf("\n");
	printf("        ------------------------------------------\n");
	printf("\n");
}

void ShowMap(){
	ShellExecute(NULL,"open","https://map.bjsubway.com/",NULL,NULL,SW_HIDE);  //打开北京地铁的官方网址 
}

void start() {

	while (1) {

		InitInterface();
		int select;
		printf("         请输入相应功能数字： ");
		scanf("%d",&select);

		switch (select) {
			case 1: {
				//输出地铁路线 
				ShowMap();
				printf("\n");
				printf("         地图已在页面中打开 ~ \n");
				printf("\n\n         ------------------------------------------");
				printf("\n         ");
				fflush(stdin);  //清除键盘缓存区的内容 
				system("pause");
				system("cls");
				break;
				;
			}
	
			case 2: {
				printf("\n");
				char s1[16], s2[16];
				printf("         请输入起点站和终点站：");
				scanf("%s %s", s1, s2);
				Print(s1,s2);
				PrintLeastTransfer(s1, s2);
				printf("\n\n         ------------------------------------------");
				printf("\n         ");
				fflush(stdin);
				system("pause");
				system("cls");
				break;
				;
			}
	
			case 0: {
				printf("\n\n         ");
				printf("              欢迎下次使用  ");
				printf("\n\n         ------------------------------------------");
				printf("\n\n\n         ");
				system("pause");
				printf("\n\n\n");
				return;
				break;
			}
	
			default: {
				printf("\n\n         ");
				printf("输入有误，请重新输入 ");
				printf("\n\n         ------------------------------------------");
				printf("\n\n         ");
				fflush(stdin);
				system("pause");
				system("cls");
				break;
			}
		}
	}


	return;
}


int main() {
	FILE* p = fopen("station.txt", "r");
	FILE* g = fopen("distance.txt", "r");
	InitStation(p);
	fclose(p);
	MakeMatrix(g);
	fclose(g);
	ShortestPath();
	QuickSort();
	start();
	return 0;
}
