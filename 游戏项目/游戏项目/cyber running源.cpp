/*
  ��Ϸ������־
 1.�����ز�
    �Ž�Դ�ļ��ļ�����
 2.���û��������� ��Ҫʲôдʲô

   ѡ��ͼ�ο������������
   ����easyxͼ�ο�
   1��������Ϸ����
   2�������Ϸ����
     a.?�ر�����ͬ���ٶ�ͬʱ�ƶ�
     b.ѭ������������ʵ��
   3��ʵ����Ϸ����
     a.���ر�����Դ
     b.��Ⱦ(����֪ʶ������)
 4.ʵ����Ĺ���
   ?��Ĺ����������⣬��Ҫ��һ���޸�
 5.ʵ�������Ծ
 6.�Ż�֡�ȴ�
 7.ʵ��С�ڹ���������
 8.ʹ�ýṹ���Ż�
 9.ʵ���ϰ����ʼ��
 10.��ʾ����ϰ���
 11.ʵ����Ķ�
 12.ʵ�������ϰ���
 13.


 ���ֵ����⣺
 1.����һ����Ծ������ֹͣת�����������͵����¶׺����λ�÷����仯��һ������
 2.�����Ϸ�ͼƬ��˿�������Լ����ӿ�ͼ�����Լ����Ӹ߶����� �߶���17����
 3.ǰ�漸�ΰ����¶׼��ܺ����ֻ��������������ظ�������Ծ�ֻ�ָ�����  ��һ�ΰ��¶ײ�̫�Ծ�
 4.�����Ƿ������������ 182
 5.�����ͳ�ʼ����
 6.Ӯ��֮��Ļ��� win.png
*/

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include<vector>
#include "tools.h"
#include<Windows.h>

using namespace std;//���������ռ�

#undef UNICODE
#undef _UNICODE

#define WIN_SCORE 3

#define WIN_WIDTH 650
#define WIN_HEIGHT 600
#define OBSTACLE_COUNT 10

//����ͼƬ
IMAGE imgBgs[3];//3ָ������Ϸ������ͼƬ��������init���forѭ����3
int bgX[3]; //����ͼƬ��x����
int bgSpeed[3] = { 1,2,4 };

IMAGE imgball[15];
int ballX;//���x����
int ballY;//���y����
int ballIndex;//�����ʱ�����

bool balljump;//��ʾС��������Ծ


int jumpheightmax;
int balljumpoff;
int update;//��ʾ�Ƿ���Ҫ����ˢ�»���

/*IMAGE imgTortoise;//С�ڹ�
int TortoiseX;//С�ڹ��ˮƽ����
int TortoiseY;//С�ڹ����ֱ����
bool TortoiseExist;//��ǰ�����Ƿ���С�ڹ�*/

int ballblood;
int score;//����

typedef enum 
{
    TORTOISE,//�ڹ� 0
    LION,//ʨ�� 1
    HOOK1,
    HOOK2,
    HOOK3,
    HOOK4,
    OBSTACLE_TYPE_COUNT//6
}obstacle_type;

vector<vector<IMAGE>>obstacleImgs;//����ϰ�ͼƬ
//IMAGE obstacleImgs[3][12];//c99

typedef struct obstacle 
{
    int type;//�ϰ������� 
    int imgIndex;//��ǰ��ʾ�����
    int x, y;//�ϰ���x��y����
    int speed;
    int power;//ɱ����
    bool exist;
    bool hited;//��ʾ�Ƿ��Ѿ�������ײ
    bool passed;
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int lastObsIndex;//last obstacle index

IMAGE imgballdown[6];
bool balldown;//��ʾ���Ƿ��ڱ��״̬

IMAGE imgSZ[10];

//��Ϸ�ĳ�ʼ��
void init() 
{
    //������Ϸ����
    initgraph(WIN_WIDTH, WIN_HEIGHT, EW_SHOWCONSOLE);

    //������Ϸ������Դ
    TCHAR name[64];
    for (int i = 0; i < 3; i++) 
    {
        //��picture/bg001.png��"picture/bg002.png"
        sprintf_s(name, "picture/bg%03d.png", i + 1);
        loadimage(&imgBgs[i], name);
        bgX[i] = 0;
    }

    //�����������ͼƬ֡�ز�
    for (int i = 0; i < 15; i++) 
    {
        //"picture/ball.jpg"
        sprintf(name, "picture/ball%d.png", i + 1);
        loadimage(&imgball[i], name);

    }
    //������ĳ�ʼλ��
    ballX = WIN_WIDTH * 0.5 - imgball[0].getwidth() * 0.5;
    ballY = 600 - imgball[0].getheight() * 2;
    ballIndex = 0;

    balljump = false;
    jumpheightmax = 600 - imgball[0].getheight() * 2 - 120;//����ͨ������120��������Ծ�ĸ߶�
    balljumpoff = -4;

    update = true;

    //����С�ڹ�
   //loadimage(&imgTortoise, "picture/t1.png");
   //TortoiseExist = false;
   //TortoiseY = 600 - imgTortoise.getheight()*1.8;//�˴���΢��С�ڹ�y����
    IMAGE imgTort;
    loadimage(&imgTort, "picture/t1.png");
    vector<IMAGE>imgTortArray;
    imgTortArray.push_back(imgTort);
    obstacleImgs.push_back(imgTortArray);

    IMAGE imgLion;
    vector<IMAGE>imgLionArray;
    for (int i = 0; i < 6; i++) 
    {
        sprintf(name, "picture/p%d.png", i + 1);
        loadimage(&imgLion, name);
        imgLionArray.push_back(imgLion);
    }
    obstacleImgs.push_back(imgLionArray);

    //��ʼ���ϰ���
    for (int i = 0; i < OBSTACLE_COUNT; i++)
    {
        obstacles[i].exist = false;
    }

    //�����¶��ز�
    for (int i = 0; i < 6; i++) 
    {
        sprintf(name, "picture/bian%d.png", i + 1);
        loadimage(&imgballdown[i], name);
        balldown = false;
    }
    IMAGE imgH;
    for (int i = 0; i < 4; i++)
    {
        vector<IMAGE>imgHookArray;
        sprintf(name, "picture/h%d.png", i + 1);
        loadimage(&imgH, name, 100, 529, true);//���ӵĴ�С�����Ŷ
        imgHookArray.push_back(imgH);
        obstacleImgs.push_back(imgHookArray);
    }

    ballblood = 100;

    //Ԥ������Ч
    preLoadSound("picture/hit.mp3");

    mciSendString("play picture/bg.mp3 repeat", 0, 0, 0);//��������

    lastObsIndex = -1;
    score = 0;

    //��������ͼƬ
    for (int i = 0; i < 10; i++) 
    {
        sprintf(name, "picture/sz/%d.png", i);
        loadimage(&imgSZ[i], name);
    }
}
void creatObstacle() 
{
    int i;
    for (i = 0; i < OBSTACLE_COUNT; i++) 
    {
        if (obstacles[i].exist == false) 
        {
            break;
        }
    }
    if (i >= OBSTACLE_COUNT) 
    {

        return;
    }
    obstacles[i].exist = true;
    obstacles[i].hited = false;
    obstacles[i].imgIndex = 0;
    obstacles[i].type = (obstacle_type)(rand() % 3);

    if (lastObsIndex >= 0 &&
        obstacles[lastObsIndex].type >= HOOK1 &&
        obstacles[lastObsIndex].type <= HOOK4 &&
        obstacles[i].type == LION &&
        obstacles[lastObsIndex].x > (WIN_WIDTH - 500)) 
    {
        obstacles[i].type = TORTOISE;
    }
    lastObsIndex = i;

    if (obstacles[i].type == HOOK1) 
    {
        obstacles[i].type += rand() % 4;

    }
    obstacles[i].x = WIN_WIDTH;
    obstacles[i].y = 600 - obstacleImgs[obstacles[i].type][0].getwidth() * 1.5;
    if (obstacles[i].type == TORTOISE) 
    {
        obstacles[i].speed = 0;
        obstacles[i].power = 5;//�˺�ֵ�ɵ���
    }
    else if (obstacles[i].type == LION)
    {
        obstacles[i].speed = 2;
        obstacles[i].power = 10;
    }
    else if (obstacles[i].type >= HOOK1 && obstacles[i].type <= HOOK4) 
    {
        obstacles[i].speed = 0;
        obstacles[i].power = 20;
        obstacles[i].y = 0;
    }

    obstacles[i].passed = false;
}

void checkHit() 
{
    for (int i = 0; i < OBSTACLE_COUNT; i++)
    {
        if (obstacles[i].exist && obstacles[i].hited == false) 
        {
            int a1x, a1y, a2x, a2y;
            int off = 15;//ƫ��������ʵ���������
            if (!balldown) 
            {
                //���¶ף����ܡ���Ծ��
                a1x = ballX + off;
                a2x = ballX + imgball[ballIndex].getwidth() - off;
                a1y = ballY + off;
                a2y = ballY + imgball[ballIndex].getheight();
            }
            else
            {
                //�����¶�״̬
                a1x = ballX + off;
                a1y = 600 - imgballdown[ballIndex].getheight();
                a2x = ballX + imgballdown[ballIndex].getwidth() - off;
                a2y = 600;
            }
            IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];
            int b1x = obstacles[i].x + off;
            int b1y = obstacles[i].y + off;
            int b2x = obstacles[i].x + img.getwidth() - off;
            int b2y = obstacles[i].y + img.getheight() - 10;

            if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y)) 
            {
                ballblood -= obstacles[i].power;
                printf("Ѫ��ʣ�� %d\n", ballblood);
                playSound("picture/hit.mp3");
                obstacles[i].hited = true;
            }

        }
    }
}
void fly()
{
    for (int i = 0; i < 3; i++)
    {
        bgX[i] -= bgSpeed[i];
        if (bgX[i] < -400) 
        {
            bgX[i] = 0;
        }
    }

    //ʵ����Ծ
    if (balljump) 
    {
        if (ballY < jumpheightmax) 
        {
            balljumpoff = 4;
        }

        ballY += balljumpoff;
    }
    if (ballY > 600 - imgball[0].getheight() * 2) 
    {
        balljump = false;
        balljumpoff = -4;
    }


    else 
    {//����Ծ
        ballIndex = (ballIndex + 1) % 15;

    }
    if (balldown) 
    {
        static int count = 0;
        count++;
        if (count >= 6) 
        {
            count = 0;
            ballIndex++;
            if (ballIndex >= 6)
            {
                ballIndex = 0;
                balldown = false;
            }
        }

    }
    else {
        ballIndex = (ballIndex + 1) % 15;

    }


    //�����ϰ���
    static int frameCount = 0;
    static int enemyFre = 100;
    frameCount++;
    if (frameCount > enemyFre) 
    {
        frameCount = 0;
        /*if (!TortoiseExist) {
            TortoiseExist = true;
            TortoiseX = WIN_WIDTH;*/
        enemyFre = 100 + rand() % 50;//100��150����һ�Σ�1.5�뵽3�� 
        creatObstacle();

    }
    /* if (TortoiseExist) {
         TortoiseX -= bgSpeed[2];
         if (TortoiseX < -imgTortoise.getwidth()) {
             TortoiseExist = false;
         }

     }*/
     //���������ϰ��������
    for (int i = 0; i < OBSTACLE_COUNT; i++) 
    {
        if (obstacles[i].exist) 
        {
            obstacles[i].x -= obstacles[i].speed + bgSpeed[2];
            if (obstacles[i].x < obstacleImgs[obstacles[i].type][0].getwidth() * (-2)) 
            {
                obstacles[i].exist = false;

            }
            int len = obstacleImgs[obstacles[i].type].size();
            obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;

        }
    }

    //��Һ��ϰ���ġ���ײ��⡱����
    checkHit();
}

void updateBg()
{
    putimage(bgX[0], 10, &imgBgs[0]);
    putimage(bgX[1], 80, &imgBgs[1]);
    putimage(bgX[2], 550, &imgBgs[2]);


}//��Ⱦ��Ϸ����

void jump() 
{
    balljump = true;
    update = true;
}

void down() 
{
    update = true;
    balldown = true;
    ballIndex = 0;
}

//�����û�����������
void keyEvent() 
{
    char ch;
    if (_kbhit()) 
    {
        ch = _getch();
        if (ch == ' ') 
        {
            jump();
        }
        else if (ch == 'a') 
        {
            down();
        }

    }

}//�ո��ʵ����Ծ
void updateEnemy() 
{
    //��ȾС�ڹ��
    //if (TortoiseExist) {
      //  putimagePNG2(TortoiseX,TortoiseY,WIN_WIDTH,&imgTortoise);
    //}

    for (int i = 0; i < OBSTACLE_COUNT; i++) 
    {
        if (obstacles[i].exist) 
        {
            putimagePNG2(obstacles[i].x, obstacles[i].y, WIN_WIDTH,
                &obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
        }
    }
}

void updateball() 
{
    if (balldown == false) 
    {
        putimagePNG2(ballX, ballY, &imgball[ballIndex]);
    }
    else 
    {
        int y = 560 - imgballdown[ballIndex].getheight();
        putimagePNG2(ballX, y, &imgballdown[ballIndex]);
    }
}

void updateBloodBar() 
{
    drawBloodBar(10, 50, 200, 15, 2, BLUE, DARKGRAY, RED, ballblood / 100.0);//Ѫ���Ļ��ƣ�����߾��룬���ϱ߾��룬���ȣ���ȣ���
}

void checkOver() 
{
    if (ballblood <= 0) 
    {
        loadimage(0, "picture/����.jpg");
        FlushBatchDraw();
        mciSendString("stop picture/bg.mp3", 0, 0, 0);
        system("pause");
        //��֮ͣ��զ��զ�������ֱ�ӿ�ʼ��һ�֣�
        ballblood = 100;
        score = 0;
        mciSendString("play picture/bg.mp3 repeat", 0, 0, 0);
    }
}

void checkScore() 
{
    for (int i = 0; i < OBSTACLE_COUNT; i++) 
    {
        if (obstacles[i].exist &&
            obstacles[i].passed == false &&
            obstacles[i].hited == false &&
            obstacles[i].x + obstacleImgs[obstacles[i].type][0].getwidth() < ballX) 
        {
            score++;
            obstacles[i].passed = true;
            printf("score: %d\n", score);
        }
    }
}

void updateScore() 
{
    char str[8];
    sprintf(str, "%d", score);

    int x = 20;//������λ��
    int y = 75;

    for (int i = 0; str[i]; i++) 
    {
        int sz = str[i] - '0';
        putimagePNG(x, y, &imgSZ[sz]);
        x += imgSZ[sz].getwidth() + 5;
    }
}

void checkWin() 
{
    if (score >= WIN_SCORE) 
    {
        FlushBatchDraw();
        mciSendString("play picture/win.mp3", 0, 0, 0);
        Sleep(1000);
        loadimage(0, "picture/win.png");
        FlushBatchDraw();
        mciSendString("stop picture/bg.mp3", 0, 0, 0);
        system("pause");

        ballblood = 100;
        score = 0;
        mciSendString("play picture/bg.mp3 repeat", 0, 0, 0);

    }
}

int main(void) 
{
    init();//��ʾ��ʼ����
    loadimage(0, "picture/����.jpg");
    system("pause");
    int timer = 0;
    while (1) 
    {
        keyEvent();
        timer += getDelay();
        if (timer > 30)
        {
            timer = 0;
            update = true;
        }
        if (update) 
        {
            update = false;
            BeginBatchDraw();
            updateBg();
            //putimagePNG2(ballX, ballY, &imgball[ballIndex]);
            updateball();
            updateEnemy();
            updateBloodBar();
            updateScore();
            checkWin();
            EndBatchDraw();

            checkOver();
            checkScore();

            fly();
        }



        //Sleep(30);//����
    }
    updateBg();

    system("pause");
    return 0;
}
