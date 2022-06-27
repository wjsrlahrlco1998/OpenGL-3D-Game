#include <GL/glut.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>
#include "model.hpp"

using namespace cv;

#define _WINDOW_WIDTH 1000
#define _WINDOW_HEIGHT 1000

#define PI 3.14159265

int isHit(struct zombie* zombie);

GLfloat camx = -13, camy = 13, camz = 0;
GLfloat cam2x = 0, cam2y = 0, cam2z = 0;
GLfloat cam_upx = 0, cam_upy = 1, cam_upz = 0;

struct zombie {
	CModel zModel;
	GLdouble zombie_Larm_angle, zombie_Rarm_angle, zombie_Lleg_angle, zombie_Rleg_angle;
	GLdouble zombie_x = 3, zombie_y = 0, zombie_z = 0;
	GLdouble zombie_angle = 0;
	int zombie_Rarm_angle_dir = 1, zombie_Larm_angle_dir = -1, zombie_Rleg_angle_dir = -1, zombie_Lleg_angle_dir = 1;
	int zombie_hp = 100;
	int stop_count = 0;
};

struct zombie zombie_0;
struct zombie zombie_1;
struct zombie zombie_2;

struct zombie zombie_boss;
int zombie_boss_skill_count = 0;

struct box {
	CModel mBox;
	GLdouble box_x = 3, box_y = -1.5, box_z = -2;
	int ability = 0;
	int used = 1; // 사용여부
};

struct box box_0;
struct box box_1;
struct box box_0_open;
struct box box_1_open;

CModel hero;
GLdouble hero_Larm_angle, hero_Rarm_angle, hero_Lleg_angle, hero_Rleg_angle;
GLdouble hero_x = 0, hero_y = 0, hero_z = 0;
GLdouble hero_angle = 0;
int hero_Rarm_angle_dir = 1, hero_Larm_angle_dir = -1, hero_Rleg_angle_dir = -1, hero_Lleg_angle_dir = 1;
int bAttack = 0, bWalk = 0, bFastWalk = 0, bOpen = 0;
int bAttack_count = 0, bWalk_count = 0, bFastWalk_count = 0;
int bWalk_right = 0, bWalk_left = 0, bWalk_up = 0, bWalk_down = 0;
int bFastWalk_right = 0, bFastWalk_left = 0, bFastWalk_up = 0, bFastWalk_down = 0;
int hero_hp = 100;
int attack_power = 40;
int speed_up = 1;

CModel maps;
GLdouble stage_z1 = -5, stage_z2 = 5;
GLdouble stage_1_x1 = -5, stage_1_x2 = 5, stage_1_y = 0;
GLdouble stage_2_x1 = 11, stage_2_x2 = 21, stage_2_y = -3;
GLdouble stage_3_x1 = 27, stage_3_x2 = 37, stage_3_y = -5;
GLdouble stair_z1 = -1, stair_z2 = 1;
GLdouble stair_1_1_x1 = 5, stair_1_1_x2 = 7, stair_1_1_y = -1;
GLdouble stair_1_2_x1 = 7, stair_1_2_x2 = 9, stair_1_2_y = -2;
GLdouble stair_1_3_x1 = 9, stair_1_3_x2 = 11, stair_1_3_y = -3;
GLdouble stair_2_1_x1 = 21, stair_2_1_x2 = 23, stair_2_1_y = -3;
GLdouble stair_2_2_x1 = 23, stair_2_2_x2 = 25, stair_2_2_y = -4;
GLdouble stair_2_3_x1 = 25, stair_2_3_x2 = 27, stair_2_3_y = -5;

CModel start, defeat, victory;
int start_count = 0;
int isDefeat = 0;
int isVictory = 0;
int victory_y = -10;

Mat image[6];
GLuint tex_ids[6] = { 1, 2, 3, 4, 5};
int nTex = 5; //텍스쳐 개수(그래픽 개수에 따라서 다름, 보통 6개)

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = (float)width / height;
	gluPerspective(40, ratio, 0.1, 1000);
}

void InitVisibility() {
	glEnable(GL_CULL_FACE); //후면제거가능 설정
	glPolygonMode(GL_FRONT, GL_FILL); //GL_FILL = 면을 채움, GL_LINE = 면을 선으로 표시
	glPolygonMode(GL_BACK, GL_LINE);

	glFrontFace(GL_CCW); //전면지정 (ccw = 반시계방향이 앞면, cw = 시계방향으로 앞면)
	glCullFace(GL_BACK); //후면제거
	glEnable(GL_DEPTH_TEST); //가시성제어 설정
}

void init_light() {
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void init_texture() {
	glGenTextures(nTex, tex_ids); //1->2 텍스쳐 개수
	glEnable(GL_TEXTURE_2D);
}

void set_light() {
	//빛의 색상 & 위치 정의
	GLfloat light_global_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_0_pos[] = { camx, camy, camz, 1.0 };
	GLfloat light_0_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_0_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	//빛 활성화, 빛 색상 & 위치 적용
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_global_ambient);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_0_pos);
}

void set_material_color(double r, double g, double b) {
	//물체의 색상 지정( 빛의 색상이 우선순위를 가진다 )
	glDisable(GL_COLOR_MATERIAL);
	GLfloat matrial_0_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat matrial_0_diffuse[] = { r, g, b, 1.0 };
	GLfloat matrial_0_specular[] = { r, g, b, 1.0 };
	GLfloat matrial_0_shininess[] = { 25.0 }; //0~128 반짝임 정도
	glMaterialfv(GL_FRONT, GL_AMBIENT, matrial_0_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matrial_0_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matrial_0_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matrial_0_shininess);
}

void init_zombie_boss(struct zombie* zombie) {
	zombie->zombie_hp = 200;
	zombie->zombie_x = 35;
	zombie->zombie_y = -5;
	zombie->zombie_z = 0;
}

void zombie_walk(struct zombie *zombie) {
	zombie->zombie_Rarm_angle += zombie->zombie_Rarm_angle_dir * 1;
	zombie->zombie_Larm_angle += zombie->zombie_Larm_angle_dir * 1;
	zombie->zombie_Rleg_angle += zombie->zombie_Rleg_angle_dir * 1;
	zombie->zombie_Lleg_angle += zombie->zombie_Lleg_angle_dir * 1;

	if (zombie->zombie_Rarm_angle > 30)
		zombie->zombie_Rarm_angle_dir = -1;
	else if (zombie->zombie_Rarm_angle < -30)
		zombie->zombie_Rarm_angle_dir = 1;

	if (zombie->zombie_Larm_angle > 30)
		zombie->zombie_Larm_angle_dir = -1;
	else if (zombie->zombie_Larm_angle < -30)
		zombie->zombie_Larm_angle_dir = 1;

	if (zombie->zombie_Rleg_angle > 30)
		zombie->zombie_Rleg_angle_dir = -1;
	else if (zombie->zombie_Rleg_angle < -30)
		zombie->zombie_Rleg_angle_dir = 1;

	if (zombie->zombie_Lleg_angle > 30)
		zombie->zombie_Lleg_angle_dir = -1;
	else if (zombie->zombie_Lleg_angle < -30)
		zombie->zombie_Lleg_angle_dir = 1;

}

void zombie_follow(struct zombie *zombie) {
	if (hero_x == zombie->zombie_x && hero_z > zombie->zombie_z) {
		zombie->zombie_angle = 270;
	}
	else if (hero_x == zombie->zombie_x && hero_z < zombie->zombie_z) {
		zombie->zombie_angle = 90;
	}
	else if (hero_z == zombie->zombie_z && hero_x > zombie->zombie_x) {
		zombie->zombie_angle = 0;
	}
	else if (hero_z == zombie->zombie_z && hero_x < zombie->zombie_x) {
		zombie->zombie_angle = 180;
	}
	else if (hero_x < zombie->zombie_x && hero_z > zombie->zombie_z) {
		zombie->zombie_angle = 270 - atan2(fabs(hero_x - zombie->zombie_x), fabs(hero_z - zombie->zombie_z)) * 180 / PI;
	}
	else if (hero_x < zombie->zombie_x && hero_z < zombie->zombie_z) {
		zombie->zombie_angle = 90 + atan2(fabs(hero_x - zombie->zombie_x), fabs(hero_z - zombie->zombie_z)) * 180 / PI;
	}
	else if (hero_x > zombie->zombie_x && hero_z > zombie->zombie_z) {
		zombie->zombie_angle = 360 - atan2(fabs(hero_z - zombie->zombie_z), fabs(hero_x - zombie->zombie_x)) * 180 / PI;
	}
	else if (hero_x > zombie->zombie_x && hero_z < zombie->zombie_z) {
		zombie->zombie_angle = atan2(fabs(hero_z - zombie->zombie_z), fabs(hero_x - zombie->zombie_x)) * 180 / PI;
	}

	if (zombie_boss_skill_count < 20) {
		if ((zombie->zombie_x < hero_x) && (zombie->zombie_x != hero_x))
			zombie->zombie_x += 0.02;
		else if ((zombie->zombie_x > hero_x) && (zombie->zombie_x != hero_x))
			zombie->zombie_x -= 0.02;

		if ((zombie->zombie_z < hero_z) && (zombie->zombie_z != hero_z))
			zombie->zombie_z += 0.02;
		else if ((zombie->zombie_z > hero_z) && (zombie->zombie_z != hero_z))
			zombie->zombie_z -= 0.02;
	}
	else {
		if ((zombie->zombie_x < hero_x) && (zombie->zombie_x != hero_x))
			zombie->zombie_x += 0.08;
		else if ((zombie->zombie_x > hero_x) && (zombie->zombie_x != hero_x))
			zombie->zombie_x -= 0.08;

		if ((zombie->zombie_z < hero_z) && (zombie->zombie_z != hero_z))
			zombie->zombie_z += 0.08;
		else if ((zombie->zombie_z > hero_z) && (zombie->zombie_z != hero_z))
			zombie->zombie_z -= 0.08;
	}
}

void zombie_dash(struct zombie *zombie) {
	zombie->zombie_Rleg_angle += zombie->zombie_Rleg_angle_dir * 5;
	zombie->zombie_Lleg_angle += zombie->zombie_Lleg_angle_dir * 5;

	if (zombie->zombie_Rleg_angle > 45)
		zombie->zombie_Rleg_angle_dir = -1;
	else if (zombie->zombie_Rleg_angle < -45)
		zombie->zombie_Rleg_angle_dir = 1;

	if (zombie->zombie_Lleg_angle > 45)
		zombie->zombie_Lleg_angle_dir = -1;
	else if (zombie->zombie_Lleg_angle < -45)
		zombie->zombie_Lleg_angle_dir = 1;
}

void zombie_damaged(struct zombie *zombie) {
	zombie->stop_count = 6;
}

void zombie_hit(struct zombie* zombie) {
	if (bAttack == 1 && isHit(zombie) && bAttack_count == 5) {
		bAttack = 0;
		zombie_damaged(zombie);
		zombie->zombie_hp -= attack_power;
	}
}

void zombie_jump(struct zombie* zombie) {
	if (zombie->stop_count > 3)
		zombie->zombie_y += 0.04;
	else 
		zombie->zombie_y -= 0.04;
}

int isdetect(struct zombie *zombie) {
	if (sqrt((hero_z - zombie->zombie_z) * (hero_z - zombie->zombie_z) + (hero_x - zombie->zombie_x) * (hero_x - zombie->zombie_x)) < 5) { // 점과 점 사이의 거리 이용
		return 1;
	}
	else {
		return 0;
	}
}

void hero_damaged(struct zombie* zombie) {
	double dir = sqrt((hero_z - zombie->zombie_z) * (hero_z - zombie->zombie_z) + (hero_x - zombie->zombie_x) * (hero_x - zombie->zombie_x));
	if (dir < 0.8)
		hero_hp -= 1;
}

void hero_walk() {
	hero_Rarm_angle += hero_Rarm_angle_dir * 2;
	hero_Larm_angle += hero_Larm_angle_dir * 2;
	hero_Rleg_angle += hero_Rleg_angle_dir * 2;
	hero_Lleg_angle += hero_Lleg_angle_dir * 2;

	if (hero_Rarm_angle > 30)
		hero_Rarm_angle_dir = -1;
	else if (hero_Rarm_angle < -30)
		hero_Rarm_angle_dir = 1;

	if (hero_Larm_angle > 30)
		hero_Larm_angle_dir = -1;
	else if (hero_Larm_angle < -30)
		hero_Larm_angle_dir = 1;

	if (hero_Rleg_angle > 30)
		hero_Rleg_angle_dir = -1;
	else if (hero_Rleg_angle < -30)
		hero_Rleg_angle_dir = 1;

	if (hero_Lleg_angle > 30)
		hero_Lleg_angle_dir = -1;
	else if (hero_Lleg_angle < -30)
		hero_Lleg_angle_dir = 1;
}

void hero_attack() {
	hero_Rarm_angle += hero_Rarm_angle_dir * 5;

	if (hero_Rarm_angle > 20)
		hero_Rarm_angle_dir = -1;
	else if (hero_Rarm_angle < -20)
		hero_Rarm_angle_dir = 1;
}

void hero_fast_walk() {
	hero_Rleg_angle += hero_Rleg_angle_dir * 5;
	hero_Lleg_angle += hero_Lleg_angle_dir * 5;

	if (hero_Rleg_angle > 30)
		hero_Rleg_angle_dir = -1;
	else if (hero_Rleg_angle < -30)
		hero_Rleg_angle_dir = 1;

	if (hero_Lleg_angle > 30)
		hero_Lleg_angle_dir = -1;
	else if (hero_Lleg_angle < -30)
		hero_Lleg_angle_dir = 1;
}

int isHit(struct zombie *zombie) {
	double dir = sqrt((hero_z - zombie->zombie_z) * (hero_z - zombie->zombie_z) + (hero_x - zombie->zombie_x) * (hero_x - zombie->zombie_x));
	double angle_up = atan2(fabs(hero_z - zombie->zombie_z), fabs(hero_x - zombie->zombie_x)) * 180 / PI;
	double angle_left = atan2(fabs(hero_x - zombie->zombie_x), fabs(hero_z - zombie->zombie_z)) * 180 / PI;
	double angle_down = atan2(fabs(hero_z - zombie->zombie_z), fabs(hero_x - zombie->zombie_x)) * 180 / PI;
	double angle_right = atan2(fabs(hero_x - zombie->zombie_x), fabs(hero_z - zombie->zombie_z)) * 180 / PI;
		if (dir < 3) {
			if (hero_angle == 0 && angle_up <= 15) {
				return 1;
			}
			else if (hero_angle == 90 && angle_left <= 15) {
				return 1;
			}
			else if (hero_angle == 180 && angle_down <= 15) {
				return 1;
			}
			else if (hero_angle == -90 && angle_right <= 15) {
				return 1;
			}
			else {
				return 0;
			}
		}
		else {
			return 0;
		}
}

int isGoUp() {
	
		if (hero_x >= stage_1_x1 && hero_x <= stage_1_x2 && hero_z >= stage_z1 && hero_z <= stage_z2) { // Stage 1에 존재
			return 1;
		}
		else if (hero_x >= stage_2_x1 && hero_x <= stage_2_x2 && hero_z >= stage_z1 && hero_z <= stage_z2) { // Stage 2에 존재
			return 1;
		}
		else if (hero_x >= stage_3_x1 && hero_x <= stage_3_x2 && hero_z >= stage_z1 && hero_z <= stage_z2) { // Stage 3에 존재
			return 1;
		}
		else if (hero_x >= stair_1_1_x1 && hero_x <= stair_1_1_x2 && hero_z >= stair_z1 && hero_z <= stair_z2) { // 계단 1-1에 존재
			return 2;
		}
		else if (hero_x >= stair_1_2_x1 && hero_x <= stair_1_2_x2 && hero_z >= stair_z1 && hero_z <= stair_z2) { // 계단 1-2에 존재
			return 3;
		}
		else if (hero_x >= stair_1_3_x1 && hero_x <= stair_1_3_x2 && hero_z >= stair_z1 && hero_z <= stair_z2) { // 계단 1-3에 존재
			return 4;
		}
		else if (hero_x >= stair_2_1_x1 && hero_x <= stair_2_1_x2 && hero_z >= stair_z1 && hero_z <= stair_z2) { // 계단 2-1에 존재
			return 5;
		}
		else if (hero_x >= stair_2_2_x1 && hero_x <= stair_2_2_x2 && hero_z >= stair_z1 && hero_z <= stair_z2) { // 계단 2-2에 존재
			return 6;
		}
		else if (hero_x >= stair_2_3_x1 && hero_x <= stair_2_3_x2 && hero_z >= stair_z1 && hero_z <= stair_z2) { // 계단 2-3에 존재
			return 7;
		}
		else {
			return 0;
		}
}

int openAble(struct box *b) {
	if (sqrt((hero_z - b->box_z) * (hero_z - b->box_z) + (hero_x - b->box_x) * (hero_x - b->box_x)) < 1.5) {
		return 1;
	}
	else {
		return 0;
	}
}

void display_model_zombie(struct zombie *zombie) {
	GLfloat x, y, z, nx, ny, nz;
	GLfloat tx, ty;
	int v_id, vt_id, vn_id;
	int idx;

	//불러온 오브젝트 그리기
	for (int o = 0; o < zombie->zModel.objs.size(); o++) {
		
		glBindTexture(GL_TEXTURE_2D, tex_ids[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1].cols, image[1].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[1].data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 안티에일리어싱
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 안티에일리어싱
		

		// 이동하고 싶은 오브젝트 번호(o == ?)를 if문에 넣고 조정가능
		// 0 : 오른팔, 1 : 왼팔, 2 : 오른다리, 3 : 왼다리
		glPushMatrix();
		if (o == 0) {
			glRotated(zombie->zombie_Rarm_angle, 0, 0, 1);
		}
		else if (o == 1) {
			glRotated(zombie->zombie_Larm_angle, 0, 0, 1);
		}
		else if (o == 2) {
			glRotated(zombie->zombie_Rleg_angle, 0, 0, 1);
		}
		else if (o == 3) {
			glRotated(zombie->zombie_Lleg_angle, 0, 0, 1);
		}

		int nFaces = zombie->zModel.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = zombie->zModel.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = zombie->zModel.objs[o].f[k].v_pairs[i].d[0];
				vt_id = zombie->zModel.objs[o].f[k].v_pairs[i].d[1];
				vn_id = zombie->zModel.objs[o].f[k].v_pairs[i].d[2];
				x = zombie->zModel.objs[o].v[v_id - 1].d[0];
				y = zombie->zModel.objs[o].v[v_id - 1].d[1];
				z = zombie->zModel.objs[o].v[v_id - 1].d[2];

				nx = zombie->zModel.objs[o].vn[vn_id - 1].d[0];
				ny = zombie->zModel.objs[o].vn[vn_id - 1].d[1];
				nz = zombie->zModel.objs[o].vn[vn_id - 1].d[2];

				tx = zombie->zModel.objs[o].vt[vt_id - 1].d[0];
				ty = zombie->zModel.objs[o].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void display_model_zombieBoss(struct zombie* zombie) {
	GLfloat x, y, z, nx, ny, nz;
	GLfloat tx, ty;
	int v_id, vt_id, vn_id;
	int idx;

	//불러온 오브젝트 그리기
	for (int o = 0; o < zombie->zModel.objs.size(); o++) {

		glBindTexture(GL_TEXTURE_2D, tex_ids[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2].cols, image[2].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[2].data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 안티에일리어싱
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 안티에일리어싱


		// 이동하고 싶은 오브젝트 번호(o == ?)를 if문에 넣고 조정가능
		// 0 : 오른팔, 1 : 왼팔, 2 : 오른다리, 3 : 왼다리
		glPushMatrix();
		if (o == 0) {
			glRotated(zombie->zombie_Rarm_angle, 0, 0, 1);
		}
		else if (o == 1) {
			glRotated(zombie->zombie_Larm_angle, 0, 0, 1);
		}
		else if (o == 2) {
			glRotated(zombie->zombie_Rleg_angle, 0, 0, 1);
		}
		else if (o == 3) {
			glRotated(zombie->zombie_Lleg_angle, 0, 0, 1);
		}

		int nFaces = zombie->zModel.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = zombie->zModel.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = zombie->zModel.objs[o].f[k].v_pairs[i].d[0];
				vt_id = zombie->zModel.objs[o].f[k].v_pairs[i].d[1];
				vn_id = zombie->zModel.objs[o].f[k].v_pairs[i].d[2];
				x = zombie->zModel.objs[o].v[v_id - 1].d[0];
				y = zombie->zModel.objs[o].v[v_id - 1].d[1];
				z = zombie->zModel.objs[o].v[v_id - 1].d[2];

				nx = zombie->zModel.objs[o].vn[vn_id - 1].d[0];
				ny = zombie->zModel.objs[o].vn[vn_id - 1].d[1];
				nz = zombie->zModel.objs[o].vn[vn_id - 1].d[2];

				tx = zombie->zModel.objs[o].vt[vt_id - 1].d[0];
				ty = zombie->zModel.objs[o].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void display_model_hero(CModel m) {
	GLfloat x, y, z, nx, ny, nz;
	GLfloat tx, ty;
	int v_id, vt_id, vn_id;
	int idx;

	//불러온 오브젝트 그리기

	for (int o = 0; o < m.objs.size(); o++) {
		idx = 4;
		
		glBindTexture(GL_TEXTURE_2D, tex_ids[idx]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[idx].cols, image[idx].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[idx].data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 안티에일리어싱
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 안티에일리어싱
		

		// 이동하고 싶은 오브젝트 번호(o == ?)를 if문에 넣고 조정가능
		// 0 : 몸, 1 : 오른팔, 2 : 왼팔, 3 : 오른다리, 4 : 왼다리
		


		glPushMatrix();

		if (o == 0) {
			glRotated(hero_Rarm_angle, 0, 0, 1);
		}
		else if (o == 1) {
			glRotated(hero_Larm_angle, 0, 0, 1);
		}
		else if (o == 2) {
			glRotated(hero_Rleg_angle, 0, 0, 1);
		}
		else if (o == 3) {
			glRotated(hero_Lleg_angle, 0, 0, 1);
		}

		int nFaces = m.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = m.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = m.objs[o].f[k].v_pairs[i].d[0];
				vt_id = m.objs[o].f[k].v_pairs[i].d[1];
				vn_id = m.objs[o].f[k].v_pairs[i].d[2];
				x = m.objs[o].v[v_id - 1].d[0];
				y = m.objs[o].v[v_id - 1].d[1];
				z = m.objs[o].v[v_id - 1].d[2];

				nx = m.objs[o].vn[vn_id - 1].d[0];
				ny = m.objs[o].vn[vn_id - 1].d[1];
				nz = m.objs[o].vn[vn_id - 1].d[2];

				tx = m.objs[o].vt[vt_id - 1].d[0];
				ty = m.objs[o].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void display_model_box(struct box *B) {
	GLfloat x, y, z, nx, ny, nz;
	GLfloat tx, ty;
	int v_id, vt_id, vn_id;
	int idx;

	//불러온 오브젝트 그리기

	for (int o = 0; o < B->mBox.objs.size(); o++) {
		
		glBindTexture(GL_TEXTURE_2D, tex_ids[3]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3].cols, image[3].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[3].data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 안티에일리어싱
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 안티에일리어싱

		glPushMatrix();

		int nFaces = B->mBox.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = B->mBox.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = B->mBox.objs[o].f[k].v_pairs[i].d[0];
				vt_id = B->mBox.objs[o].f[k].v_pairs[i].d[1];
				vn_id = B->mBox.objs[o].f[k].v_pairs[i].d[2];
				x = B->mBox.objs[o].v[v_id - 1].d[0];
				y = B->mBox.objs[o].v[v_id - 1].d[1];
				z = B->mBox.objs[o].v[v_id - 1].d[2];

				nx = B->mBox.objs[o].vn[vn_id - 1].d[0];
				ny = B->mBox.objs[o].vn[vn_id - 1].d[1];
				nz = B->mBox.objs[o].vn[vn_id - 1].d[2];

				tx = B->mBox.objs[o].vt[vt_id - 1].d[0];
				ty = B->mBox.objs[o].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void display_model_map(CModel m) {
	GLfloat x, y, z, nx, ny, nz;
	GLfloat tx, ty;
	int v_id, vt_id, vn_id;
	int idx;

	//불러온 오브젝트 그리기

	for (int o = 0; o < m.objs.size(); o++) {
		glBindTexture(GL_TEXTURE_2D, tex_ids[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0].cols, image[0].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[0].data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 안티에일리어싱
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 안티에일리어싱
		

		glPushMatrix();

		int nFaces = m.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = m.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = m.objs[o].f[k].v_pairs[i].d[0];
				vt_id = m.objs[o].f[k].v_pairs[i].d[1];
				vn_id = m.objs[o].f[k].v_pairs[i].d[2];
				x = m.objs[o].v[v_id - 1].d[0];
				y = m.objs[o].v[v_id - 1].d[1];
				z = m.objs[o].v[v_id - 1].d[2];

				nx = m.objs[o].vn[vn_id - 1].d[0];
				ny = m.objs[o].vn[vn_id - 1].d[1];
				nz = m.objs[o].vn[vn_id - 1].d[2];

				tx = m.objs[o].vt[vt_id - 1].d[0];
				ty = m.objs[o].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void display_model_text(CModel m) {
	GLfloat x, y, z, nx, ny, nz;
	GLfloat tx, ty;
	int v_id, vt_id, vn_id;
	int idx;

	//불러온 오브젝트 그리기

	for (int o = 0; o < m.objs.size(); o++) {

		glPushMatrix();

		int nFaces = m.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = m.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = m.objs[o].f[k].v_pairs[i].d[0];
				vt_id = m.objs[o].f[k].v_pairs[i].d[1];
				vn_id = m.objs[o].f[k].v_pairs[i].d[2];
				x = m.objs[o].v[v_id - 1].d[0];
				y = m.objs[o].v[v_id - 1].d[1];
				z = m.objs[o].v[v_id - 1].d[2];

				nx = m.objs[o].vn[vn_id - 1].d[0];
				ny = m.objs[o].vn[vn_id - 1].d[1];
				nz = m.objs[o].vn[vn_id - 1].d[2];

				tx = m.objs[o].vt[vt_id - 1].d[0];
				ty = m.objs[o].vt[vt_id - 1].d[1];

				glNormal3f(nx, ny, nz);
				glTexCoord2f(tx, ty);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
		glPopMatrix();
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(camx, camy, camz, cam2x, cam2y, cam2z, cam_upx, cam_upy, cam_upz);


	set_light();
	//Victory text
	if (isVictory == 1) {
		glPushMatrix();
		set_material_color(0.7, 0.8, 0);
		glTranslated(32, victory_y, 0);
		glRotated(180, 0, 1, 0);
		display_model_text(victory);
		glPopMatrix();
	}

	//Defeat text
	if (isDefeat == 1) {
		glPushMatrix();
		set_material_color(0.7, 0, 0);
		glTranslated(hero_x, hero_y + 3, hero_z);
		glRotated(180, 0, 1, 0);
		display_model_text(defeat);
		glPopMatrix();
	}

	//Start text
	if (start_count <= 10) {
		glPushMatrix();
			set_material_color(0, 0, 0.8);
			glTranslated(0, 3, 0);
			glRotated(180, 0, 1, 0);
			display_model_text(start);
		glPopMatrix();
	}

	// 좀비_보스 배치
	if (zombie_boss.zombie_hp > 0) {
		glPushMatrix();
		if(isHit(&zombie_boss))
			set_material_color(1, 0, 0);
		else 
			set_material_color(1, 1, 1);
		glTranslated(zombie_boss.zombie_x, zombie_boss.zombie_y, zombie_boss.zombie_z);
		glRotated(zombie_boss.zombie_angle, 0, 1, 0);
		glScaled(0.7, 0.7, 0.7);
		display_model_zombieBoss(&zombie_boss);
		glPopMatrix();
	}
	else {
		zombie_boss.zombie_x = -20;
		isVictory = 1;
	}

	// 좀비_2 배치
	if (zombie_2.zombie_hp > 0) {
		glPushMatrix();
		if (isHit(&zombie_2))
			set_material_color(1, 0, 0);
		else
			set_material_color(1, 1, 1);
		glTranslated(zombie_2.zombie_x, zombie_2.zombie_y, zombie_2.zombie_z);
		glRotated(zombie_2.zombie_angle, 0, 1, 0);
		glScaled(0.5, 0.5, 0.5);
		display_model_zombie(&zombie_2);
		glPopMatrix();
	}
	else {
		zombie_2.zombie_x = -20;
	}

	// 좀비_1 배치
	if (zombie_1.zombie_hp > 0) {
		glPushMatrix();
		if (isHit(&zombie_1))
			set_material_color(1, 0, 0);
		else
			set_material_color(1, 1, 1);
		glTranslated(zombie_1.zombie_x, zombie_1.zombie_y, zombie_1.zombie_z);
		glRotated(zombie_1.zombie_angle, 0, 1, 0);
		glScaled(0.5, 0.5, 0.5);
		display_model_zombie(&zombie_1);
		glPopMatrix();
	}
	else {
		zombie_1.zombie_x = -20;
	}

	// 좀비_0 배치
	if (zombie_0.zombie_hp > 0) {
		glPushMatrix();
		if (isHit(&zombie_0))
			set_material_color(1, 0, 0);
		else
			set_material_color(1, 1, 1);
			glTranslated(zombie_0.zombie_x, zombie_0.zombie_y, zombie_0.zombie_z);
			glRotated(zombie_0.zombie_angle, 0, 1, 0);
			glScaled(0.5, 0.5, 0.5);
			display_model_zombie(&zombie_0);
		glPopMatrix();
	}
	else {
		zombie_0.zombie_x = -20;
	}
	
	// 주인공 배치
	if (hero_y >= -10 && hero_hp > 0) {
		glPushMatrix();
			set_material_color(1, 1, 1);
			glTranslated(hero_x, hero_y, hero_z);
			glRotated(hero_angle, 0, 1, 0);
			glScaled(0.5, 0.5, 0.5);
			display_model_hero(hero);
		glPopMatrix();
	}
	else {
		bWalk = 0;
		bFastWalk = 0;
		bAttack = 0;
		glPushMatrix();
			set_material_color(0.5, 0, 0);
			glTranslated(hero_x, hero_y-2.0, hero_z);
			glRotated(90, 1, 0, 0);
			glScaled(0.5, 0.5, 0.5);
			display_model_hero(hero);
			glPopMatrix();
		isDefeat = 1;
	}

	// 상자_1 배치
	glPushMatrix();
	if (openAble(&box_1)) {
		set_material_color(1, 1, 0);
	}
	else {
		set_material_color(1, 1, 1);
	}
	glTranslated(box_1.box_x, box_1.box_y, box_1.box_z);
	glRotated(-90, 0, 1, 0);
	glScaled(0.5, 0.5, 0.5);
	if (box_1.used == 1) {
		display_model_box(&box_1);
	}
	else {
		display_model_box(&box_1_open);
		box_1_open.used = 0;
	}
	glPopMatrix();
	
	// 상자_0 배치
	glPushMatrix();
		if (openAble(&box_0)) {
			set_material_color(1, 1, 0);
		}
		else {
			set_material_color(1, 1, 1);
		}
		glTranslated(box_0.box_x, box_0.box_y, box_0.box_z);
		glRotated(-90, 0, 1, 0);
		glScaled(0.5, 0.5, 0.5);
		if (box_0.used == 1) {
			display_model_box(&box_0);
		}
		else {
			display_model_box(&box_0_open);
			box_0_open.used = 0;
		}
	glPopMatrix();
	

	glPushMatrix();
		set_material_color(1, 1, 1);
		glTranslated(0, -2.4, 0);
		glRotated(180, 0, 1, 0);
		display_model_map(maps);
	glPopMatrix();
	
	glFlush();
}

void MySpecialKeyboard(int key, int x, int y) {
	int modifier = glutGetModifiers(); // shift + 방향키 : 더 빠른 걸음
	switch (key) {
	case GLUT_KEY_LEFT:
		if (modifier == GLUT_ACTIVE_SHIFT) {
			bWalk = 0;
			bFastWalk = 1;
			bFastWalk_right = 0;
			bFastWalk_left = 1;
			bFastWalk_up = 0;
			bFastWalk_down = 0;
			hero_angle = 90;
		}
		else {
			bFastWalk = 0;
			bWalk = 1;
			bWalk_right = 0;
			bWalk_left = 1;
			bWalk_up = 0;
			bWalk_down = 0;
			hero_angle = 90;
		}
		break;
	case GLUT_KEY_RIGHT:
		if (modifier == GLUT_ACTIVE_SHIFT) {
			bWalk = 0;
			bFastWalk = 1;
			bFastWalk_right = 1;
			bFastWalk_left = 0;
			bFastWalk_up = 0;
			bFastWalk_down = 0;
			hero_angle = -90;
		}
		else {
			bFastWalk = 0;
			bWalk = 1;
			bWalk_right = 1;
			bWalk_left = 0;
			bWalk_up = 0;
			bWalk_down = 0;
			hero_angle = -90;
		}
		break;
	case GLUT_KEY_UP:
		if (modifier == GLUT_ACTIVE_SHIFT) {
			bWalk = 0;
			bFastWalk = 1;
			bFastWalk_right = 0;
			bFastWalk_left = 0;
			bFastWalk_up = 1;
			bFastWalk_down = 0;
			hero_angle = 0;
		}
		else {
			bFastWalk = 0;
			bWalk = 1;
			bWalk_right = 0;
			bWalk_left = 0;
			bWalk_up = 1;
			bWalk_down = 0;
			hero_angle = 0;
		}
		break;
	case GLUT_KEY_DOWN:
		if (modifier == GLUT_ACTIVE_SHIFT) {
			bWalk = 0;
			bFastWalk = 1;
			bFastWalk_right = 0;
			bFastWalk_left = 0;
			bFastWalk_up = 0;
			bFastWalk_down = 1;
			hero_angle = 180;
		}
		else {
			bFastWalk = 0;
			bWalk = 1;
			bWalk_right = 0;
			bWalk_left = 0;
			bWalk_up = 0;
			bWalk_down = 1;
			hero_angle = 180;
		}
		break;
	}
	glutPostRedisplay();
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
	switch (KeyPressed) {
	case 'a':
		bAttack = 1;
		break;
	case 'e' :
		bOpen = 1;
		break;
	}
	glutPostRedisplay();
}

void myTimer(int value) {
	glutTimerFunc(20, myTimer, 1);
	if(start_count <= 10)
		start_count++;
	if (isVictory == 1 && victory_y < -3)
		victory_y += 0.8;

	/* Start 주인공 움직임 */
	if (bOpen == 1 && openAble(&box_0) && box_0.used == 1) {
		if (box_0.ability == 0) {
			attack_power += 10;
			box_0.used = 0;
		}
		bOpen = 0;
	}

	if (bOpen == 1 && openAble(&box_1) && box_1.used == 1) {
		if (box_1.ability == 0) {
			speed_up = 2;
			box_1.used = 0;
		}
		bOpen = 0;
	}
	int i = isGoUp();
	if (isGoUp() != 0) {
		switch (i) {
		case 2 :
			if (hero_y >= stair_1_1_y) {
				hero_y -= 0.25;
				camy -= 0.25;
				cam2y -= 0.25;
			}
			break;
		case 3 :
			if (hero_y >= stair_1_2_y) {
				hero_y -= 0.25;
				camy -= 0.25;
				cam2y -= 0.25;
			}
			break;
		case 4 :
			if (hero_y >= stair_1_3_y) {
				hero_y -= 0.25;
				camy -= 0.25;
				cam2y -= 0.25;
			}
			break;
		case 5 :
			if (hero_y >= stair_2_1_y) {
				hero_y -= 0.25;
				camy -= 0.25;
				cam2y -= 0.25;
			}
			break;
		case 6 :
			if (hero_y >= stair_2_2_y) {
				hero_y -= 0.25;
				camy -= 0.25;
				cam2y -= 0.25;
			}
			break;
		case 7 :
			if (hero_y >= stair_2_3_y) {
				hero_y -= 0.25;
				camy -= 0.25;
				cam2y -= 0.25;
			}
			break;
		}
	}
	else {
		hero_y -= 0.1;
		camy -= 0.1;
		cam2y -= 0.1;
	}

	if (bAttack == 1 && bAttack_count < 10) {
		hero_attack();
		bAttack_count++;
	}
	else if (bAttack_count >= 10) {
		bAttack = 0;
		bAttack_count = 0;
	}

	zombie_hit(&zombie_0);
	zombie_hit(&zombie_1);
	zombie_hit(&zombie_2);
	zombie_hit(&zombie_boss);
	

	if (bWalk == 1 && bWalk_count < 14) {
		hero_walk();
		bWalk_count++;
		if (bWalk_right == 1 && hero_z <= stage_z2 - 1) {
			hero_z += 0.05 * speed_up;
			camz += 0.05 * speed_up;
			cam2z += 0.05 * speed_up;
		}
		else if (bWalk_left == 1 && hero_z >= stage_z1 + 1) {
			hero_z -= 0.05 * speed_up;
			camz -= 0.05 * speed_up;
			cam2z -= 0.05 * speed_up;
		}
		else if (bWalk_up == 1) {
			if (hero_x != 5 && hero_x != 11 && hero_x != 21 && hero_x != 27 && hero_x != 37) {
				hero_x += 0.05 * speed_up;
				camx += 0.05 * speed_up;
				cam2x += 0.05 * speed_up;
			}
			else if (hero_z >= stair_z1 && hero_z <= stair_z2) {
				hero_x += 0.05 * speed_up;
				camx += 0.05 * speed_up;
				cam2x += 0.05 * speed_up;
			}

		}
		else if (bWalk_down == 1) {
			if (hero_x != -5 && hero_x != 5 && hero_x != 11 && hero_x != 21 && hero_x != 27 && hero_x != 37) {
				hero_x -= 0.05 * speed_up;
				camx -= 0.05 * speed_up;
				cam2x -= 0.05 * speed_up;
			}
			else if (hero_z >= stair_z1 && hero_z <= stair_z2) {
				hero_x -= 0.05 * speed_up;
				camx -= 0.05 * speed_up;
				cam2x -= 0.05 * speed_up;
			}
		}
	}
	else if (bWalk_count >= 14) {
		bWalk = 0;
		bWalk_right = 0;
		bWalk_left = 0;
		bWalk_up = 0;
		bWalk_down = 0;
		bWalk_count = 0;
	}

	if (bFastWalk == 1 && bFastWalk_count < 24) {
		hero_fast_walk();
		bFastWalk_count++;
		if (bFastWalk_right == 1 && hero_z <= stage_z2 - 1) {
			hero_z += 0.1 * speed_up;
			camz += 0.1 * speed_up;
			cam2z += 0.1 * speed_up;
		}
		else if (bFastWalk_left == 1 && hero_z >= stage_z1 + 1) {
			hero_z -= 0.1 * speed_up;
			camz -= 0.1 * speed_up;
			cam2z -= 0.1 * speed_up;
		}
		else if (bFastWalk_up == 1) {
			if (hero_x != 5 && hero_x != 11 && hero_x != 21 && hero_x != 27 && hero_x != 37) {
				hero_x += 0.1 * speed_up;
				camx += 0.1 * speed_up;
				cam2x += 0.1 * speed_up;
			}
			else if (hero_z >= stair_z1 && hero_z <= stair_z2) {
				hero_x += 0.1 * speed_up;
				camx += 0.1 * speed_up;
				cam2x += 0.1 * speed_up;
			}
		}
		else if (bFastWalk_down == 1) {
			if (hero_x != -5 && hero_x != 5 && hero_x != 11 && hero_x != 21 && hero_x != 27 && hero_x != 37) {
				hero_x -= 0.1 * speed_up;
				camx -= 0.1 * speed_up;
				cam2x -= 0.1 * speed_up;
			}
			else if (hero_z >= stair_z1 && hero_z <= stair_z2) {
				hero_x -= 0.1 * speed_up;
				camx -= 0.1 * speed_up;
				cam2x -= 0.1 * speed_up;
			}
		}
	}
	else if (bFastWalk_count >= 24) {
		bFastWalk = 0;
		bFastWalk_right = 0;
		bFastWalk_left = 0;
		bFastWalk_up = 0;
		bFastWalk_down = 0;
		bFastWalk_count = 0;
	}

	hero_damaged(&zombie_0);
	hero_damaged(&zombie_1);
	hero_damaged(&zombie_2);
	hero_damaged(&zombie_boss);

	/* End 주인공 움직임 */
	
	/* Start 좀비 움직임 */
	// 좀비_0
	if (isdetect(&zombie_0)) {
		if (zombie_0.stop_count == 0) {
			zombie_walk(&zombie_0);
			zombie_follow(&zombie_0);
		}
		else {
			zombie_jump(&zombie_0);
			zombie_0.stop_count--;
		}
	}

	// 좀비_1
	if (isdetect(&zombie_1)) {
		if (zombie_1.stop_count == 0) {
			zombie_walk(&zombie_1);
			zombie_follow(&zombie_1);
		}
		else {
			zombie_jump(&zombie_1);
			zombie_1.stop_count--;
		}
	}

	// 좀비_2
	if (isdetect(&zombie_2)) {
		if (zombie_2.stop_count == 0) {
			zombie_walk(&zombie_2);
			zombie_follow(&zombie_2);
		}
		else {
			zombie_jump(&zombie_2);
			zombie_2.stop_count--;
		}
	}

	if (isdetect(&zombie_boss)) {
		if (zombie_boss.stop_count == 0) {
			zombie_follow(&zombie_boss);
			if (zombie_boss_skill_count >= 20 && zombie_boss_skill_count < 30) {
				zombie_dash(&zombie_boss);
				if (zombie_boss_skill_count == 30)
					zombie_boss_skill_count = 0;
			}
			else {
				zombie_walk(&zombie_boss);
			}
			zombie_boss_skill_count++;
		}
		else {
			zombie_boss.stop_count--;
		}
	}
	/* End 좀비 움직임*/

	glutPostRedisplay();
}

int main(int argc, char** argv) {
	//텍스쳐 이미지 읽어오기
	image[0] = imread("D:/image/rock.jpg", IMREAD_COLOR);
	cvtColor(image[0], image[0], COLOR_BGR2RGB); // BGR -> RGB로 바꿈
	image[1] = imread("D:/image/zombie3.jpg", IMREAD_COLOR);
	cvtColor(image[1], image[1], COLOR_BGR2RGB); // BGR -> RGB로 바꿈
	image[2] = imread("D:/image/zombie_boss.jpg", IMREAD_COLOR);
	cvtColor(image[2], image[2], COLOR_BGR2RGB); // BGR -> RGB로 바꿈
	image[3] = imread("D:/image/diamond.jpg", IMREAD_COLOR);
	cvtColor(image[3], image[3], COLOR_BGR2RGB); // BGR -> RGB로 바꿈
	image[4] = imread("D:/image/hero3.jpg", IMREAD_COLOR);
	cvtColor(image[4], image[4], COLOR_BGR2RGB); // BGR -> RGB로 바꿈
	

	//모델 읽어오기
	string filepath = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/hero1.obj";
	string filepath2 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/zombie.obj";
	string filepath3 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/zombie_boss.obj";
	string filepath4 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/box.obj";
	string filepath5 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/box_att.obj";
	string filepath6 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/box_spd.obj";
	string filepath7 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/maps.obj";
	string filepath8 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/Start.obj";
	string filepath9 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/Defeat.obj";
	string filepath10 = "D:/3학년2학기/컴퓨터그래픽스/오브젝트/Victory.obj";

	ifstream fin(filepath);
	ifstream fin2(filepath2);
	ifstream fin2_1(filepath2);
	ifstream fin2_2(filepath2);
	ifstream fin3(filepath3);
	ifstream fin4(filepath4);
	ifstream fin4_1(filepath4);
	ifstream fin5(filepath5);
	ifstream fin6(filepath6);
	ifstream fin7(filepath7);
	ifstream fin8(filepath8);
	ifstream fin9(filepath9);
	ifstream fin10(filepath10);

	victory.loadObj(fin10);
	defeat.loadObj(fin9);
	start.loadObj(fin8);
	zombie_2.zModel.loadObj(fin2_2);
	zombie_1.zModel.loadObj(fin2_1);
	maps.loadObj(fin7);
	box_1_open.mBox.loadObj(fin6);
	box_0_open.mBox.loadObj(fin5);
	box_0.mBox.loadObj(fin4);
	box_1.mBox.loadObj(fin4_1);
	zombie_boss.zModel.loadObj(fin3);
	zombie_0.zModel.loadObj(fin2);
	hero.loadObj(fin);
	fin.close();
	fin2.close();
	fin3.close();
	fin4.close();
	fin4_1.close();
	fin5.close();
	fin6.close();
	fin7.close();
	fin8.close();
	fin9.close();
	fin10.close();

	init_zombie_boss(&zombie_boss);
	
	box_1.box_x = 19;
	box_1.box_y = -4.5;
	box_1.box_z = 3.5;

	zombie_1.zombie_x = 19;
	zombie_1.zombie_y = -3;
	zombie_1.zombie_z = -1.5;

	zombie_2.zombie_x = 19;
	zombie_2.zombie_y = -3;
	zombie_2.zombie_z = 1.5;

	//gl 관련
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE); //RGBA 모드가 alpha값을 쓰는 모드
	glutInitWindowSize(_WINDOW_WIDTH, _WINDOW_HEIGHT);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.0/*alpha*/); //alpha 값은 투명도

	glutCreateWindow("3D_Game");
	init_light(); // 윈도우 생성후!!
	init_texture();

	InitVisibility();
	glutDisplayFunc(display);
	glutKeyboardFunc(MyKeyboard);
	glutSpecialFunc(MySpecialKeyboard);
	glutTimerFunc(20, myTimer, 1);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}