/*
  游戏开发日志
 1.导入素材
    放进源文件文件夹里
 2.从用户界面入手 需要什么写什么

   选择图形库或者其他引擎
   基于easyx图形库
   1）创建游戏窗口
   2）设计游戏背景
     a.?重背景不同的速度同时移动
     b.循环滚动背景的实现
   3）实现游戏背景
     a.加载背景资源
     b.渲染(背景知识：坐标)
 4.实现球的滚动
   ?球的滚动出现问题，需要进一步修复
 5.实现球的跳跃
 6.优化帧等待
 7.实现小乌龟的随机出现
 8.使用结构体优化
 9.实现障碍物初始化
 10.显示多个障碍物
 11.实现球的蹲
 12.实现柱子障碍物
 13.


 出现的问题：
 1.球在一次跳跃后会出现停止转动的情况，这就导致下蹲后球的位置发生变化，一动不动
 2.柱子上方图片拉丝的问题以及柱子抠图问题以及柱子高度问题 高度在17几行
 3.前面几次按下下蹲技能后会出现画面紊乱现象但是重复几次跳跃又会恢复正常  第一次按下蹲不太对劲
 4.考虑是否更换背景音乐 182
 5.结束和初始画面
 6.赢了之后的画面 win.png
*/

#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include<vector>
#include "tools.h"
#include<Windows.h>

using namespace std;//声明命名空间

#undef UNICODE
#undef _UNICODE

#define WIN_SCORE 3

#define WIN_WIDTH 650
#define WIN_HEIGHT 600
#define OBSTACLE_COUNT 10

//背景图片
IMAGE imgBgs[3];//3指的是游戏背景的图片数，还有init里的for循环的3
int bgX[3]; //背景图片的x坐标
int bgSpeed[3] = { 1,2,4 };

IMAGE imgball[15];
int ballX;//球的x坐标
int ballY;//球的y坐标
int ballIndex;//球滚动时的序号

bool balljump;//表示小球正在跳跃


int jumpheightmax;
int balljumpoff;
int update;//表示是否需要马上刷新画面

/*IMAGE imgTortoise;//小乌龟
int TortoiseX;//小乌龟的水平坐标
int TortoiseY;//小乌龟的竖直坐标
bool TortoiseExist;//当前窗口是否有小乌龟*/

int ballblood;
int score;//分数

typedef enum 
{
    TORTOISE,//乌龟 0
    LION,//狮子 1
    HOOK1,
    HOOK2,
    HOOK3,
    HOOK4,
    OBSTACLE_TYPE_COUNT//6
}obstacle_type;

vector<vector<IMAGE>>obstacleImgs;//存放障碍图片
//IMAGE obstacleImgs[3][12];//c99

typedef struct obstacle 
{
    int type;//障碍物类型 
    int imgIndex;//当前显示的序号
    int x, y;//障碍物x，y坐标
    int speed;
    int power;//杀伤力
    bool exist;
    bool hited;//表示是否已经发生碰撞
    bool passed;
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];

int lastObsIndex;//last obstacle index

IMAGE imgballdown[6];
bool balldown;//表示球是否处于扁的状态

IMAGE imgSZ[10];

//游戏的初始化
void init() 
{
    //创建游戏窗口
    initgraph(WIN_WIDTH, WIN_HEIGHT, EW_SHOWCONSOLE);

    //加载游戏背景资源
    TCHAR name[64];
    for (int i = 0; i < 3; i++) 
    {
        //“picture/bg001.png”"picture/bg002.png"
        sprintf_s(name, "picture/bg%03d.png", i + 1);
        loadimage(&imgBgs[i], name);
        bgX[i] = 0;
    }

    //加载球滚动的图片帧素材
    for (int i = 0; i < 15; i++) 
    {
        //"picture/ball.jpg"
        sprintf(name, "picture/ball%d.png", i + 1);
        loadimage(&imgball[i], name);

    }
    //设置球的初始位置
    ballX = WIN_WIDTH * 0.5 - imgball[0].getwidth() * 0.5;
    ballY = 600 - imgball[0].getheight() * 2;
    ballIndex = 0;

    balljump = false;
    jumpheightmax = 600 - imgball[0].getheight() * 2 - 120;//可以通过更改120来更改跳跃的高度
    balljumpoff = -4;

    update = true;

    //加载小乌龟
   //loadimage(&imgTortoise, "picture/t1.png");
   //TortoiseExist = false;
   //TortoiseY = 600 - imgTortoise.getheight()*1.8;//此处可微调小乌龟y坐标
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

    //初始化障碍物
    for (int i = 0; i < OBSTACLE_COUNT; i++)
    {
        obstacles[i].exist = false;
    }

    //加载下蹲素材
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
        loadimage(&imgH, name, 100, 529, true);//柱子的大小这里改哦
        imgHookArray.push_back(imgH);
        obstacleImgs.push_back(imgHookArray);
    }

    ballblood = 100;

    //预加载音效
    preLoadSound("picture/hit.mp3");

    mciSendString("play picture/bg.mp3 repeat", 0, 0, 0);//背景音乐

    lastObsIndex = -1;
    score = 0;

    //加载数字图片
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
        obstacles[i].power = 5;//伤害值可调节
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
            int off = 15;//偏移量根据实际情况调整
            if (!balldown) 
            {
                //非下蹲（奔跑、跳跃）
                a1x = ballX + off;
                a2x = ballX + imgball[ballIndex].getwidth() - off;
                a1y = ballY + off;
                a2y = ballY + imgball[ballIndex].getheight();
            }
            else
            {
                //处于下蹲状态
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
                printf("血量剩余 %d\n", ballblood);
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

    //实现跳跃
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
    {//不跳跃
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


    //创建障碍物
    static int frameCount = 0;
    static int enemyFre = 100;
    frameCount++;
    if (frameCount > enemyFre) 
    {
        frameCount = 0;
        /*if (!TortoiseExist) {
            TortoiseExist = true;
            TortoiseX = WIN_WIDTH;*/
        enemyFre = 100 + rand() % 50;//100到150出现一次：1.5秒到3秒 
        creatObstacle();

    }
    /* if (TortoiseExist) {
         TortoiseX -= bgSpeed[2];
         if (TortoiseX < -imgTortoise.getwidth()) {
             TortoiseExist = false;
         }

     }*/
     //更新所有障碍物的坐标
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

    //玩家和障碍物的“碰撞检测”处理
    checkHit();
}

void updateBg()
{
    putimage(bgX[0], 10, &imgBgs[0]);
    putimage(bgX[1], 80, &imgBgs[1]);
    putimage(bgX[2], 550, &imgBgs[2]);


}//渲染游戏背景

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

//处理用户按键的输入
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

}//空格键实现跳跃
void updateEnemy() 
{
    //渲染小乌龟等
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
    drawBloodBar(10, 50, 200, 15, 2, BLUE, DARKGRAY, RED, ballblood / 100.0);//血条的绘制（距左边距离，距上边距离，长度，宽度，）
}

void checkOver() 
{
    if (ballblood <= 0) 
    {
        loadimage(0, "picture/人物.jpg");
        FlushBatchDraw();
        mciSendString("stop picture/bg.mp3", 0, 0, 0);
        system("pause");
        //暂停之后，咋样咋样复活或直接开始下一局，
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

    int x = 20;//分数的位置
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
    init();//显示初始画面
    loadimage(0, "picture/人物.jpg");
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



        //Sleep(30);//休眠
    }
    updateBg();

    system("pause");
    return 0;
}
