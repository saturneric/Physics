//
//  main.c
//  physics
//
//  Created by Eric on 16/5/6.
//  Copyright © 2016年 Bakantu Eric. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <GLUT/GLUT.h>
#define E_G -9.8
#define A_G 6.67259e-11
#define o_MAX 16
#define f_MAX 32
#define X_WIN 400
#define Y_WIN 400
#define w_MAX 32
#define Pi 3.1415926536
#define CLEAN system("clear")
#define GL_SILENCE_DEPRECATION

typedef struct{
    double x;
    double y;
    double mass;
    int gravity;
    int e_gravity;
    double vx;
    double vy;
    double a_x;
    double a_y;
    int elasticity;
} obj;

typedef struct {
    double size;
    double angle;
    double time;
    obj *p_obj;
} force;

typedef struct {
    double x0;
    double y0;
    double x1;
    double y1;
    int on;
} wall;

int opengl_init(int argc, const char * argv[]);
int add_obj(obj *objs[o_MAX]);
int add_force(obj *objs[o_MAX], force *forces[f_MAX]);
int physics_engine(obj *objs[o_MAX],force *forces[f_MAX],wall *walls[w_MAX]);
int print(obj *objs[o_MAX], force *forces[f_MAX]);
int move_obj(obj *objs[o_MAX]);
int delete_obj(obj *objs[o_MAX]);
void o_display(void);
void key_down(unsigned char key, int x, int y);
void mouse_click(int button, int state, int x, int y);
void keep_physics_engine(void);
int set_wall(void);
void w_display(void);
int set_size(void);

obj *i_objs[o_MAX];
force *i_forces[f_MAX];
wall *i_walls[w_MAX];
int wall_point = 0;
int p_wall = 0;
int engine_on = 1;
int settings = 0;
double x_MAX = 1000, y_MAX = 1000;
double ENGINE_TIME = 0.01;


int main(int argc, const char * argv[]) {
    for (int i = 0; i < o_MAX; i++) i_objs[i] = NULL;
    for (int i = 0; i < f_MAX; i++) i_forces[i] = NULL;
    for (int i = 0; i < f_MAX; i++) i_walls[i] = NULL;

    CLEAN;
    opengl_init(argc, argv);
    //physics_engine(i_objs, i_forces);
    for (int i = 0; i < o_MAX; i++) {
        if(i_objs[i] != NULL) free(i_objs[i]);
        if(i_forces[i] != NULL) free(i_forces[i]);
        if(i_walls[i] != NULL) free(i_walls[i]);
    }
    return 0;
}


int add_obj(obj *objs[o_MAX]){
    CLEAN;
    int p_c = 0;
    while (objs[p_c] != NULL) p_c++;
    objs[p_c] = (obj *)malloc(sizeof(obj));
    printf("X: ");
    scanf("%lf",&objs[p_c]->x);
    printf("Y: ");
    scanf("%lf",&objs[p_c]->y);
    printf("MASS: ");
    scanf("%lf",&objs[p_c]->mass);
    printf("IF EARTH_GRAVITY: ");
    scanf("%d",&objs[p_c]->e_gravity);
    printf("IF GRAVITY: ");
    scanf("%d",&objs[p_c]->gravity);
    printf("VX: ");
    scanf("%lf",&objs[p_c]->vx);
    printf("VY: ");
    scanf("%lf",&objs[p_c]->vy);
    //printf("IF ELASTICITY: ");
    //scanf("%d",&objs[p_c]->elasticity);
    fflush(stdin);
    return 0;
}

int add_force(obj *objs[o_MAX], force *forces[f_MAX]){
    CLEAN;
    int p_c = 0, i_obj;
    while (forces[p_c] != NULL) p_c++;
    forces[p_c] = (force *)malloc(sizeof(force));
    printf("SIZE: ");
    scanf("%lf",&forces[p_c]->size);
    printf("ANGLE: ");
    scanf("%lf",&forces[p_c]->angle);
    printf("TIME: ");
    scanf("%lf",&forces[p_c]->time);
    printf("OBJ: ");
    scanf("%d",&i_obj);
    if(objs[i_obj] != NULL) forces[p_c]->p_obj = objs[i_obj];
    else forces[p_c]->p_obj = NULL;
    fflush(stdin);
    return 0;
}


int physics_engine(obj *objs[o_MAX],force *forces[f_MAX],wall *walls[w_MAX]){
    for(int i = 0; i < o_MAX; i++){
        double a_x = 0.0, a_y = 0.0;
        if (objs[i] == NULL) continue;
        if(fabs(objs[i]->x) > x_MAX || fabs(objs[i]->y) > y_MAX){
            free(objs[i]);
            objs[i] = NULL;
            continue;
        }
        else{
            if(objs[i]->e_gravity){
                a_y += E_G * ENGINE_TIME;
            }
            if(objs[i]->gravity){
                for (int k = 0; k < o_MAX; k ++){
                    if ((objs[k] == NULL)||(k == i)) continue;
                    else{
                        double d_x = objs[k]->x - objs[i]->x;
                        double d_y = objs[k]->y - objs[i]->y;
                        double d_angle = atan(d_y / d_x);
                        if (d_y < 0 && d_x < 0) {
                            d_angle = -d_angle;
                            d_angle -= Pi/2;
                        }
                        else  if (d_y > 0 && d_x < 0) {
                            d_angle = -d_angle;
                            if (d_x < 0) d_angle += Pi/2;
                        }
                        double dis = sqrt((d_x * d_x) + (d_y * d_y));
                        double g_force =(A_G * objs[i]->mass * objs[k]->mass)/(dis * dis);

                        a_x += (g_force * cos(d_angle)) / objs[i]->mass;
                        a_y += (g_force * sin(d_angle)) / objs[i]->mass;
                    }
                }
            }
        }
        for (int k = 0; k < f_MAX; k++){
            if (forces[k] == NULL)continue;
            else if (forces[k]->p_obj == objs[i]){
                a_x += (forces[k]->size * cos(forces[k]->angle)) / objs[i]->mass *ENGINE_TIME;
                a_y += (forces[k]->size * sin(forces[k]->angle)) / objs[i]->mass *ENGINE_TIME;
            }
            if(forces[k]->time > 0) forces[k]->time -= ENGINE_TIME;
            if(forces[k]->time <= 0) {
                free(forces[k]);
                forces[k] = NULL;
            }
        }
        objs[i]->a_x = a_x;
        objs[i]->a_y = a_y;
    }
    usleep(1000000*ENGINE_TIME);
    move_obj(objs);
    return 0;
}

int print(obj *objs[o_MAX], force *forces[f_MAX]){
    CLEAN;
    printf("#OBJ:\n");
    for (int i = 0; i < o_MAX; i++){
        if (objs[i] != NULL){
            printf("OBJ[%d](%p): (%lf, %lf) \nMASS: %lf\n\n",i,objs[i],objs[i]->x,objs[i]->y,objs[i]->mass);
            glVertex2f(objs[i]->x/x_MAX, objs[i]->y/y_MAX);
        }
    }
    printf("\n#FORCE:\n");
    for (int i = 0; i < o_MAX; i++){
        if(forces[i] != NULL){
            printf("FORCE[%d]: SIZE:%.5lf ANGLE: %.5lf P_OBJ: %p\n, TIME: %.2lf",\
                   i,forces[i]->size,forces[i]->angle,forces[i]->p_obj,forces[i]->time);
        }
    }
    printf("\n");
    return 0;
}

int move_obj(obj *objs[o_MAX]){
    for (int i = 0; i < o_MAX; i++){
        if (objs[i] == NULL) continue;
        if (objs[i]->a_x != 0){
            double a_x = objs[i]->a_x;
            objs[i]->x += objs[i]->vx + 0.5 * a_x * ENGINE_TIME * ENGINE_TIME;
            objs[i]->vx += a_x;
        }
        if (objs[i]->a_y != 0){
            double a_y = objs[i]->a_y;
            objs[i]->y += objs[i]->vy + 0.5 * a_y * ENGINE_TIME * ENGINE_TIME;
            objs[i]->vy += a_y;
        }

    }
    return  0;
}
int delete_obj(obj *objs[o_MAX]){
    CLEAN;
    int d_obj;
    printf("DELETE OBJ: ");
    scanf("%d",&d_obj);
    if (objs[d_obj] != NULL){
        free(objs[d_obj]);
        objs[d_obj] = NULL;
    }
    fflush(stdin);
    return 0;
    
}

int opengl_init(int argc, const char * argv[]){
    glutInit(&argc, (char **)argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowPosition(X_WIN/2, Y_WIN/2);
    glutInitWindowSize(X_WIN, Y_WIN);
    glutCreateWindow("Physics");
    glutDisplayFunc(&o_display);
    glutIdleFunc(&keep_physics_engine);
    glutKeyboardFunc(&key_down);
    glutMouseFunc(&mouse_click);
    glutMainLoop();
    return 0;
}

void o_display(void){
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(4.0f);
    
    glBegin(GL_POINTS);
    glColor3f(0,255,0);
    for (int i = 0; i < o_MAX; i++){
        if (i_objs[i] != NULL){
            glVertex2f(i_objs[i]->x/x_MAX, i_objs[i]->y/y_MAX);
        }
    }
    glEnd();
    
    glLineWidth(2);
    glDisable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    for (int i = 0; i < w_MAX; i++){
        if (i_walls[i] != NULL){
            if (i_walls[i]->on == 1){
                glVertex2f(i_walls[i]->x0/X_WIN*2,i_walls[i]->y0/Y_WIN*2);
                glVertex2f(i_walls[i]->x1/X_WIN*2,i_walls[i]->y1/Y_WIN*2);
            }
        }
    }
    glEnd();
    glFlush();
}

void key_down(unsigned char key, int x, int y){
    if(key == 's'){
        CLEAN;
        int ifnot = 1;
        char input = '\0';
        while (ifnot){
            printf("DEALWHITH:\n");
            printf("1.ADD OBJ\n");
            printf("2.ADD FORCE\n");
            printf("3.EXIT PROGRAM\n");
            printf("4.DELETE OBJ\n");
            printf("5.SET WALL\n");
            printf("6.SET SIZE\n");
            printf("q.START ENGINE\n");
            fflush(stdin);
            input = getchar();
            fflush(stdin);
            switch (input) {
                case '1':
                    add_obj(i_objs);
                    break;
                case '2':
                    add_force(i_objs, i_forces);
                    break;
                case '3':
                    exit(0);
                    break;
                case '4':
                    delete_obj(i_objs);
                    break;
                case '5':
                    engine_on = 0;
                    settings = 1;
                    wall_point = 0;
                    p_wall = 0;
                    while (i_walls[p_wall] != NULL) p_wall++;
                    i_walls[p_wall] = (wall *)malloc(sizeof(wall));
                    i_walls[p_wall]->on = 0;
                    ifnot = 0;
                    break;
                case '6':
                    set_size();
                    break;
                case 'q':
                    ifnot = 0;
                    break;
                default:
                    break;
            }
            CLEAN;
        }
    }
}

void keep_physics_engine(void){
    if (engine_on) {
        physics_engine(i_objs, i_forces,i_walls);
        print(i_objs,i_forces);
    }
    else{
        set_wall();
    }
    o_display();
}
int set_wall(void){
    if (wall_point == 2){
        engine_on = 1;
        settings = 0;
        wall_point = 0;
        i_walls[p_wall]->on = 1;
    }
    return 0;
}

void mouse_click(int button, int state, int x, int y){
    if (state == GLUT_DOWN){
        if (!engine_on && settings == 1 && button == GLUT_RIGHT_BUTTON){
            printf("%d,%d\n",x-X_WIN/2,-(y-Y_WIN/2));
            printf("x = %d y = %d\n",x,y);
            if(wall_point < 2){
                if(wall_point == 0){
                    i_walls[p_wall]->x0 = (x-X_WIN/2);
                    i_walls[p_wall]->y0 = -(y-Y_WIN/2);
                }
                else if(wall_point == 1){
                    i_walls[p_wall]->x1 = (x-X_WIN/2);
                    i_walls[p_wall]->y1 = -(y-Y_WIN/2);
                }
                wall_point++;
            }
        }
    }
}

void w_display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < o_MAX; i++){
        if (i_objs[i] != NULL){
            glVertex2f(i_objs[i]->x/x_MAX, i_objs[i]->y/y_MAX);
        }
    }
    glEnd();
    glFlush();
}

int set_size(void){
    CLEAN;
    printf("XMAX: ");
    scanf("%lf",&x_MAX);
    printf("YMAX: ");
    scanf("%lf",&y_MAX);
    printf("ENGINE_TIME: ");
    scanf("%lf",&ENGINE_TIME);
    return 0;
}
