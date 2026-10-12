/*************************************************************
 @Author: 韩世杰
 @Date:2019-07-20
 @Description:例题3.2.3-1 平面三次B样条曲线
 根据题目所给四个控制顶点绘制对应的B样条曲线
 方法一：递归实现了de Boor Cox公式，并利用其计算出了最后p(t)的值
 方法二：手工计算出了最后的表达式（见同一目录下的递归实现Word）
 使用<glut.h>绘制出两种方法对应的图像，完全重合，验证了递归算法的正确性
 **************************************************************/

#define GL_SILENCE_DEPRECATION        //预定义防止报错

 // MAC Xcode环境
// #include <GLUT/GLUT.h>

// Windows10  VC2017环境
#include "pch.h"
#include <glut.h>

// 通用声明
#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;
#define MAX 0x3f3f3f3f
#define RGB 255
#define PI 3.1415926535897

//定义题目给出的四个控制顶点
struct point { float x, y; }control[4];

// 定义存储数据的类
class B_line
{
private:
	float t_[11];                        //控制节点t[0]到t[10]
	int size = 3;                        //控制顶点数为4
public:
	void control_point_input();
	friend void draw();
	float N_cal(int k, int i, float t);    //计算B样条基函数
}problem;
void draw();

int main(int argc, char *argv[]) {
	problem.control_point_input();
	glutInit(&argc, argv);                           
	glutInitDisplayMode(GLUT_RGBA);      
	glutInitWindowPosition(150, 100);            
	glutInitWindowSize(600, 600);               
	glutCreateWindow("例题3.2.3-1 结果");    
	glutDisplayFunc(&draw);                          
	glutMainLoop();									
	return 0;
}

// de_Boor-Cox算法
float B_line::N_cal(int k, int i, float t)
{
	if (k == 0) return (t <t_[i + 1] && t >= t_[i]) ? 1 : 0;
	return (t - t_[i])*N_cal(k - 1, i, t) / (t_[i + k] - t_[i]) + (t_[i + k + 1] - t)*N_cal(k - 1, i + 1, t) / (t_[i + k + 1] - t_[i + 1]);
}

//绘图函数
void draw()
{
	// 准备工作
	glClearColor(255 / RGB, 255 / RGB, 255 / RGB, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	gluOrtho2D(-4, 4, -4, 4);
	glBegin(GL_LINES);
	glColor3f(0 / RGB, 0 / RGB, 0 / RGB);
	glVertex2f(-100.0f, 0.0f);
	glVertex2f(100.0f, 0.0f);
	glVertex2f(0.0f, -100.0f);
	glVertex2f(0.0f, 100.0f);
	glEnd();

	// 将控制顶点依次连线
	glBegin(GL_LINES);
	glVertex2f(control[0].x, control[0].y);
	glVertex2f(control[1].x, control[1].y);
	glVertex2f(control[1].x, control[1].y);
	glVertex2f(control[2].x, control[2].y);
	glVertex2f(control[2].x, control[2].y);
	glVertex2f(control[3].x, control[3].y);
	glEnd();

	// 描出四个控制顶点
	int n = 1000;
	glBegin(GL_POLYGON);
	for (int m = 0; m < n; m++) { glVertex2f(0.03*cos(2 * PI*m / n) + control[0].x, 0.03*sin(2 * PI*m / n) + control[0].y); }
	glEnd();
	glBegin(GL_POLYGON);
	for (int m = 0; m < n; m++) { glVertex2f(0.03*cos(2 * PI*m / n) + control[1].x, 0.03*sin(2 * PI*m / n) + control[1].y); }
	glEnd();
	glBegin(GL_POLYGON);
	for (int m = 0; m < n; m++) { glVertex2f(0.03*cos(2 * PI*m / n) + control[2].x, 0.03*sin(2 * PI*m / n) + control[2].y); }
	glEnd();
	glBegin(GL_POLYGON);
	for (int m = 0; m < n; m++) { glVertex2f(0.03*cos(2 * PI*m / n) + control[3].x, 0.03*sin(2 * PI*m / n) + control[3].y); }
	glEnd();

	//作出曲线
	point temp;
	glBegin(GL_POINTS);
	for (float t = 3; t <= 4; t += 0.0001) {
		temp.x = 0; temp.y = 0;
		//递归计算表达式并绘制图像
		for (int ii = 0; ii <= 3; ii++)
		{
			temp.x += (problem.N_cal(3, ii, t)* control[ii].x);
			temp.y += (problem.N_cal(3, ii, t)* control[ii].y);
		}
		glVertex2d(temp.x, temp.y);

	}
	glEnd();

	//手工计算表达式并绘制图像
	glBegin(GL_POINTS);
	for (float t = 0; t <= 1; t += 0.0001)
	{
		temp.x = ((-pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*control[0].x + (3 * pow(t, 3) - 6 * pow(t, 2) + 4)*control[1].x + (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*control[2].x + t *t*t* control[3].x) / 6;
		temp.y = ((-pow(t, 3) + 3 * pow(t, 2) - 3 * t + 1)*control[0].y + (3 * pow(t, 3) - 6 * pow(t, 2) + 4)*control[1].y + (-3 * pow(t, 3) + 3 * pow(t, 2) + 3 * t + 1)*control[2].y + t * t*t* control[3].y) / 6;
		glVertex2d(temp.x, temp.y);
	}
	glEnd();

	//    以下为验证部分，作出了点0和2连线中点和点1的连线，
	//    曲线的性质决定其必定会穿过这两条线的三分之一点
	//    对点1和点3中点和点2点连线，同理
	    //glBegin(GL_LINES);
	    //float midx=(control[0].x+control[2].x)/2;
	    //float midy=(control[0].y+control[2].y)/2;
	    //glVertex2f(midx,midy);
	    //glVertex2f(control[1].x,control[1].y);
	    //glEnd();
	    //glBegin(GL_LINES);
	    //float midx0=(control[1].x+control[3].x)/2;
	    //float midy0=(control[1].y+control[3].y)/2;
	    //glVertex2f(midx0,midy0);
	    //glVertex2f(control[2].x,control[2].y);
	    //glEnd();
	glFlush();
}

// 控制顶点数据输入
void B_line::control_point_input()
{
	control[0].x = -3; control[0].y = -1;
	control[1].x = -2.4; control[1].y = 0.6;
	control[2].x = 1; control[2].y = 2;
	control[3].x = 3; control[3].y = 0.4;
	t_[0] = 0; t_[1] = 1; t_[2] = 2; t_[3] = 3;
	t_[4] = 4; t_[5] = 5; t_[6] = 6; t_[7] = 7;
	t_[8] = 8; t_[9] = 9; t_[10] = 10;
}
