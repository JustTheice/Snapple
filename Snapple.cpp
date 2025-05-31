// 贪吃的苹果蛇😍 正版
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <easyx.h>
#include <string>
#include <vector>
#include <time.h>
#include <algorithm>
#include <functional>
#include <imm.h> // wzy饮入

//Novice引入
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib") 

//#define DEBUG //仅在开发阶段打开
using namespace std;

//------------------- data -------------------
//用户结构
typedef struct {
	int id;
	string username;
	string password;
	int current_level; //当前关卡
	int skinIdx; //当前选择皮肤编号
	int cleared_levels; //通关数，掩码
} User;

User* user = nullptr;

//页面类型枚举，用于controller控制页面跳转
enum Pages { LOGIN, SIGNUP, MENU, GAME, WIN, LOSE, SKIN, LEVEL, MUSIC, TEAM, DESCRIBE, RANK, LOADING };
enum GameElement {
	AIR, APPLE, SNAKE_BODY, BARRIER, DESTINATION, SNAKE_HEAD
}; // 游戏元素枚举，分别表示空地、苹果、蛇身、障碍物、目标点
enum Direction { UP = VK_UP, DOWN = VK_DOWN, LEFT = VK_LEFT, RIGHT = VK_RIGHT }; //蛇的方向枚举，和easyx库的msg.message相统一

constexpr int N_LEVELS = 10; //关卡总数
constexpr int MAP_WIDTH = 30, MAP_HEIGHT = 20; //地图宽高（格子数）

GameElement gameMap[MAP_HEIGHT][MAP_WIDTH]; //地图

//具体位置坐标
typedef struct {
	int x;	//当前位置的横轴坐标（格子号）
	int y;	//当前位置的纵轴坐标（格子号）
} Position;

/*
	蛇信息链表：
	[链表元素].x表示蛇身体的第i节身体的x坐标,[链表元素].y表示蛇身体的第i节身体的y坐标
	snake.size()获得当前蛇的长度
	snake.front()获得蛇头坐标,snake.back()获得蛇尾坐标
*/
list<Position> snake;
Position dest; //终点坐标
Direction headdir;	//记录蛇当前朝向,以防180°转向

int level = 1; //当前关卡，初始为第一关
bool music_loaded; //音乐是否已加载
//bool rank_list_loaded; //排行榜是否已加载
bool music_play; //音乐开关
bool sound_play; //音效开关

const string FPATH_RANK = "data/rank.txt"; //排行榜文件路径
const string FPATH_USERS = "data/users.txt"; //全部用户文件路径
const string FDIR_LEVELS = "data/levels"; //全部关卡文件夹，每个关卡在其中以单独文件形式存储，文件名为关卡号
const string FDIR_SKINS = "assets/skins"; //全部皮肤文件夹

//排行榜item结构
typedef struct {
	string username;
	int score; //分数(已通关数)
} RankItem;
vector<RankItem> rank_list; //排行榜
constexpr int RANK_CAP = 10; //排行榜容量

typedef struct {
	string head;
	string body;
} Skin; //皮肤，存蛇头和蛇身的图片路径。色块可以用纯色图片，后续可加入其它图片，拓展性强。
constexpr int N_SKINS = 6; //皮肤总数
//全部皮肤数组//全部皮肤数组
Skin skins[N_SKINS] = { {"assets/skins/1/snakehead_right1.jpg", "assets/skins/1/snakebody_h1.jpg"},
	{"assets/skins/2/snakehead_right2.jpg", "assets/skins/2/snakebody_h2.jpg"},
	{"assets/skins/3/snakehead_right3.jpg", "assets/skins/3/snakebody_h3.jpg"},
	{"assets/skins/4/snakehead_right4.jpg", "assets/skins/4/snakebody_h4.jpg"},
	{"assets/skins/1/snakehead_right1.jpg", "assets/skins/1/snakebody_h1.jpg"},
	{ "assets/skins/2/snakehead_right2.jpg", "assets/skins/2/snakebody_h2.jpg"}
};

Pages flag;	//当前所在的页面，用于controller控制页面跳转
Pages last_flag; //记录上一个页面所在的页面，需要返回上个页面时使用，同时跳转页面时需要更新(由于游戏界面和主界面都能进入选择关卡界面，以便区分)


//------------------- 自定义区 -------------------
/*
	(新增加的结构体)
	负责人：wzy
	功能：输入框
*/
// -------------------------1.0----------------------------
typedef struct _wzy_TextBox
{
	int pos_x, pos_y;          // 左上角坐标
	int size_x, size_y;        // 输入框大小
	std::string text;		   // 输入框当前内容
	size_t maxlen = 16;		   // 文本框最大内容长度
	bool isFocused = false;    // 是否被点击
	bool showCursor = true;    // 是否显示光标
	const int TIME = 500;      // 切换光标的时长
	int time = 0;              // 按钮经过的时间
	int cursor_pos = 0;        // 记录光标位置
}wzy_TextBox;
// -------------------------1.0----------------------------
// -------------------------3.0----------------------------

// -------------------------3.0----------------------------


//------------------- 自定义区 -------------------

// ------------------------Beta-----------------------------
//安静
char level_title[50] = { 0 }; // 第 i 关 字符串

//Jay
static IMAGE* headImg, * bodyImg, * tailImg_up, * tailImg_down, * tailImg_left, * tailImg_right, * headImg_down, * headImg_up, * headImg_left, * turn_leftanddown, * turn_leftandup, * turn_rightanddown, * turn_rightandup;

static IMAGE headImg0[2], bodyImg0[2], tailImg_up0[2], tailImg_down0[2], tailImg_left0[2], tailImg_right0[2], headImg_down0[2], headImg_up0[2], headImg_left0[2], turn_leftanddown0[2], turn_leftandup0[2], turn_rightanddown0[2], turn_rightandup0[2];

static IMAGE headImg1[2], bodyImg1[2], tailImg_up1[2], tailImg_down1[2], tailImg_left1[2], tailImg_right1[2], headImg_down1[2], headImg_up1[2], headImg_left1[2], turn_leftanddown1[2], turn_leftandup1[2], turn_rightanddown1[2], turn_rightandup1[2];

static IMAGE headImg2[2], bodyImg2[2], tailImg_up2[2], tailImg_down2[2], tailImg_left2[2], tailImg_right2[2], headImg_down2[2], headImg_up2[2], headImg_left2[2], turn_leftanddown2[2], turn_leftandup2[2], turn_rightanddown2[2], turn_rightandup2[2];

static IMAGE headImg3[2], bodyImg3[2], tailImg_up3[2], tailImg_down3[2], tailImg_left3[2], tailImg_right3[2], headImg_down3[2], headImg_up3[2], headImg_left3[2], turn_leftanddown3[2], turn_leftandup3[2], turn_rightanddown3[2], turn_rightandup3[2];



static IMAGE  img_replay[2], img_replay_big[2], img_back[2], img_back_big[2], img_levelselect[2], img_levelselect_big[2], img_skin[2], img_skin_big[2], img_up[2], img_up_big[2], img_down[2], img_down_big[2], img_left[2], img_left_big[2], img_right[2], img_right_big[2];
static IMAGE img_bg;
const int CELL_SIZE = 40;
static IMAGE img_apple[2], img_barrier[2], img_dest[2];
// ------------------------Beta-----------------------------

//------------------- data -------------------


//------------------- service -------------------(2-3人)
/*
	负责人: wzy
	功能: checkPassword: 检查密码是否符合要求
		可以使用之前每日练习题的代码
	参数: password 用户名
	返回值: true: 合法
			false: 不合法
*/
bool checkPassword(string password);

/*
	负责人: best进行时
	功能: login: 根据用户名和密码获取用户信息
		读取用户信息文件，若用户信息配则将当前行打包成User结构体，注意要用堆空间
	参数: username 用户名, password 密码
	返回值: 0成功，1用户名不存在，2密码错误
*/
int login(string username, string password);

/*
	负责人: 补充
	功能: updateUserFile: 更新用户信息至文件
	参数: User& user: 用户信息
	返回值: 0成功，1用户不存在，2其他原因
*/
int updateUserFile(const User* user);


/*
	负责人: wzy
	功能: signup: 用户注册
		(根据时间)生成一个用户id，并将整合好的User信息存入文件
	参数:
		string nm：用户名
		string pwd：密码
		int current_level：当前关卡
		int skinIdx：当前皮肤编号
		int cleared_levels：通关数，掩码
	返回值: 0: 注册功能
			1: 用户名已存在
			2: 其它原因导致注册失败
*/
int signup(string nm, string pwd, int current_level = 1, int skinIdx = 0, int cleared_levels = 0);


/*
	负责人: Jay
	功能: initGame: 初始化关卡
		1. 根据关卡号读取文件，获取关卡数据
		2. 将关卡数据存入gameMap
		3. 初始化全局变量snake链表和dest终点坐标
	参数: void
	返回值: void
*/
void initGame();

/*
	难点
	负责人: Jay
	功能: snakeMove: 移动蛇
		1.根据蛇头位置和方向，判断下一向的元素
		2.根据元素做出不同处理
			如果可以移动，修改snake链表，将蛇尾当作新的蛇头
		3.返回对应结果
	参数:
		Direction dir: 移动方向
	返回值:
		0: 正常移动/到达胜利点
		1: 方向相反
		2: 撞到自己/障碍物
		3: 吃到食物
*/
int snakeMove(Direction dir);

/*
	负责人: Jay
	功能: checkAndDrop: 蛇的掉落（根据snakeMove的返回值调用）
		遍历snake链表，判断是否下方全是空气
			若是，蛇的y坐标整体+1
			否则，返回
	参数：void
	返回值:
		true: 有掉落
		false: 不掉落
*/
bool checkAndDrop();

/*
	负责人: Jay
	功能: isLose: 失败判断
		蛇身达到底部边界 且 处于掉落状态时，游戏失败
	参数：
		bool drop_state: 掉落状态
	返回值:
		true: 胜利
		false: 未胜利
*/
bool isLose(bool drop_state);

/*
	负责人: Jay
	功能: isWin: 胜利判断
		看蛇头坐标snake.front()是否等于终点坐标dest
	参数：void
	返回值:
		true: 胜利
		false: 未胜利
*/
inline
bool isWin();

/*
	负责人: Noob
	功能: setMusic和setSound: 控制音效开关
		分别根据参数改变music_play和sound_play的值为true或false
	参数: bool state
	返回值: void
*/
inline
void setMusic(bool state);
inline
void setSound(bool state);

/*
	负责人: Noob
	功能: playGameMusic: 播放背景音乐
	参数: void
	返回值: true: 播放成功
			false: 播放失败
*/
bool playGameMusic();

/*
	负责人: Noob
	功能: stopGameMusic: 停止播放背景音乐
	参数: void
	返回值: true: 停止播放成功
			false: 停止播放失败
*/
bool stopGameMusic();

/*
	负责人: 补充
	功能: loadGameMusic: 加载背景音乐文件
	参数: void
	返回值: true: 加载成功
			false: 加载失败
*/
bool loadGameMusic();

/*
	负责人: 补充
	功能: loadGameMusic: 加载音效文件
	参数: void
	返回值: true: 加载成功
			false: 加载失败
*/
bool loadGameSound();

/*
	负责人: Noob
	功能: playGameSound: 播放音效
	参数: type：根据传入值播放不同音效：
			1：碰撞音效
			2：到达目标点音效
			3： 吃苹果音效
			4：正常移动音效
			5：胜利音效
			6：失败音效
	返回值: true: 播放成功
			false: 播放失败
*/
bool playGameSound(int type);


// 难点: 排行榜相关函数
/*
	负责人: 安静
	功能: loadRank: 从排行榜文件中读取信息并存入全局变量rank_list
	参数: void
	返回值: void
*/
void loadRank();

/*
	负责人: 安静
	功能: saveRank: 将全局变量rank_list所含的从排行榜信息存入文件
	参数: void
	返回值: void
*/
void saveRank();

/*
	负责人: 安静
	功能: updateRank: 用用户名和分数信息更新排行榜（修改全局变量rank_list中的元素）
		1.若分数高于最低分则允许插入；若有重名则用高分代替低分
		2.插入后，调用saveRank将rank_list存到文件中
	参数:
		string nm: 用户名
		int sc: 分数
	返回值:
		true: 更新成功
		false: 更新失败(排行榜已满)
*/
bool updateRank(string nm, int sc);


/*
	负责人:
	功能: changeSkin: 换肤
		更改当前user数据中的skinIdx属性
	参数：
		int skinIdx: 皮肤编号
	返回值:
		true: 换肤成功
		false: 皮肤未拥有(暂不考虑)
*/
inline
bool changeSkin(int skinIdx);

//------------------- Beta -------------------

/*
	负责人: 补充
	功能: getLevelMask: 获得当前关卡的掩码
	参数：
		int level: 关卡号
	返回值:
		int 对应掩码
*/
inline
int getLevelMask(int level);

/*
	负责人: 补充
	功能: get1Count: 获得数的二进制串中1的个数
	参数：
		int n: 数
	返回值:
		int i的个数
*/
inline
int get1Count(int num);

#ifdef DEBUG
void printUserInfo();
#endif // DEBUG


//------------------- Beta -------------------


//------------------- 自定义区 -------------------

//------------------- 自定义区 -------------------


//------------------- service -------------------


// -------------------------- view --------------------------(4-5人)
/*
	负责人：best进行时
	功能：loginView:用户可以选择登录和注册
		while(true){
			1.登录：
				获取用户输入账号密码
				调用getUserByName()
					登陆成功：
						更新全局user信息
						调用loadRank初始化排行榜
						跳转菜单界面last_flag=flag, flag=MENU;break;
					登陆失败：提示登录失败,continue
			2.注册：跳转到注册界面
					last_flag=flag,flag=SIGNUP;break;
		}
	参数：void
	返回值：void
*/
void loginView();

/*
	负责人: best进行时
	功能: 登录成功后，跳转至菜单页面的中间等待过程页面
	参数：void
	返回值：void
*/
void loadingView();

/*
	负责人：wzy
	功能：signupView:用户可以选择注册和返回登陆界面
		while(true){
			1.注册：
				用户输入账号密码和确认密码,调用signup()
					注册成功：提示注册成功,跳转登录界面
					注册失败：提示注册失败,continue
			2.返回：跳转到登录界面
		}
	参数：void
	返回值：void
*/
void signupView();
//end

/*
	负责人：默语
	功能：menuView:展示选项，用户可以选择开始游戏、选关、玩法说明、制作组、排行榜、音乐设置、退出游戏
		while(true){
			1.展示选项
			2.用户输入
			3.根据输入进行处理
				开始游戏：跳转游戏界面
				选关：跳转选关界面
				玩法说明：跳转玩法说明界面 0
				制作组：跳转制作组界面 0
				排行榜：跳转排行榜界面
				音乐设置：跳转音乐设置界面
				退出游戏：调用exit(0)
		}
	参数：void
	返回值：void
*/
void menuView();
//end

/*
	负责人: 麦兜
	功能: drawMap: 绘制地图
		根据gameMap数组，绘制地图
	参数: void
	返回值: void
*/
void drawMap();

/*
	负责人: 麦兜
	功能: drawSnake: 绘制蛇身
		根据snake链表和user数据中的skinIdx属性，绘制蛇身。应注意蛇头和蛇身选择不同图片，以及蛇头的方向问题（需要根据当前蛇头朝向进行图片转向）
	参数: void
	返回值: void
*/
void drawSnake();

/*
	负责人：麦兜
	功能：gameView: 根据gameMap数组和snake链表,打印游戏界面
		调用上方drawMap()和drawSnake()函数，简化操作
		由于蛇可能有不同皮肤，在绘制gameMap时先忽略绘制蛇，绘制蛇由drawSnake单独完成
	参数：void
	返回值：void
*/
void gameView_showMap();


/*
	难点
	负责人：麦兜
	功能：gameView: 游戏界面整合
		初始化当前关卡数据(调用函数initGame())
			while(true){
				1.绘制地图，调用函数gameView_showMap()
				2.接收玩家输入:
					换肤：跳转换肤界面
					选关：跳转选关界面
					重新开始：调用函数initGame(),continue;
					返回首页：跳转菜单界面
					移动：执行结果 = 调用函数move()
						根据执行结果：
							未移动：continue;(拓展：反馈动画)
							吃到食物：播放音效
							正常移动：判断胜利(调用isWin())
								胜利：跳转胜利界面
							掉落控制
			}
			额外：掉落控制部分可用渐减的帧间隔模拟重力效果，代码可参考:
				while(drop_result = checkAndDrop()){
					绘制地图，调用gameView_showMap()
					判断失败(调用isLose(drop_result))
						若失败：跳转失败界面
				}
	参数：void
	返回值：void
*/
void gameView();
//end

/*
	负责人：安静
	功能：winView: 打印胜利界面，用户可点击下一关或返回
		1.可能将当前用户存入排行榜(调用updateRank()和saveRank());
		2.获取用户输入
		3.根据输入进行处理
			下一关：level++; 跳转游戏界面
			返回：跳转菜单界面
	参数：void
	返回值：void
*/
void winView();

/*
	负责人：Jay
	功能：loseView: 打印失败界面，用户可点击重新开始或返回
		1.获取用户输入
		2.根据输入进行处理
			重新开始：跳转游戏界面
			返回：跳转菜单界面
	参数：void
	返回值：void
*/
void loseView();

/*
	负责人：wzy
	功能：skinSelect:展示皮肤选项，用户可以选择皮肤和返回
		while(true){
			1.展示选项
			2.接受用户输入
				选择皮肤: 调用函数changeSkin();continue;
				返回: 跳转到游戏页面
		}
	参数：void
	返回值：void
*/
void skinSelectView();

/*
	负责人：默语
	功能：levelSelectView:展示关卡选项，用户可以选择对应关卡，或者返回
		1.展示选项
		2.接受用户输入
			选择关卡: 设置level值,跳转到游戏界面
			返回: 跳转到上一个页面(菜单界面或者游戏界面，使用last_flag)
	参数：void
	返回值：void
*/
void levelSelectView();

/*
	负责人：Noob
	功能：musicView:展示音乐设置，用户可以设置音乐或者返回，调用setMusic()和setSound()函数
	参数：void
	返回值：void
*/
void musicView();

/*
	负责人：默语
	功能：teamView:展示团队介绍，用户可以选择返回
	参数：void
	返回值：void
*/
void teamView();

/*
	负责人：默语
	功能：describeView:展示玩法说明，用户可以选择返回
	参数：void
	返回值：void
*/
void describeView();

/*
	负责人：安静
	功能：rankView:展示排行榜(对rank_list遍历)，用户可以选择返回
	参数：void
	返回值：void
*/
void rankView();

/*
	统一封装函数
	功能：button：绘制按钮
	参数：
		ExMessage& msg: 当前页面的msg对象
		x, y, w, h: 按钮的坐标与宽高
		const char* text: 文字内容
		int ft_sz: 文字大小 传0为自动调整为按钮高度的0.85倍
		const char* ft_fm: 字体 默认微软雅黑
		COLORREF hover_color: 悬浮颜色 默认蓝色
		COLORREF ft_cl: 字体颜色 默认黑色
		COLORREF fill_color: 按钮填充色 默认白色
		int br: 圆角度 默认5
	返回值: 
		true: 按钮被点击
		false: 按钮未被点击
	使用示例:
		while(1){
			if(button(paramList...))
				执行点击操作
		}
*/
bool button(ExMessage& msg, int x, int y, int w, int h,
	const char* text);
bool button(ExMessage& msg, int x, int y, int w, int h,
	const char* text, COLORREF ft_cl, int ft_sz = 0, const char* ft_fm = "微软雅黑",
	COLORREF fill_color = WHITE, COLORREF br_color = WHITE, COLORREF hover_color = BLUE, int br = 5, 
	int bd = 3, int b_style = PS_SOLID
);

// --------------------------自定义区--------------------------
// -------------------------1.0----------------------------
#pragma comment(lib, "MSIMG32.LIB")
void wzy_inputTextBox(wzy_TextBox& textbox, const ExMessage& msg);
void wzy_updateTextBox(wzy_TextBox& textbox, int delta);
void wzy_drawTextBox(wzy_TextBox& textbox, bool is_password);
inline void wzy_putimage_alpha(int dst_x, int dst_y, IMAGE* img);
// -------------------------1.0----------------------------

// -------------------------3.0----------------------------
//Jay
void smoothMove(Direction dir, int x, int y);
void smoothDown();
// -------------------------3.0----------------------------
// --------------------------自定义区--------------------------

// -------------------------- view --------------------------

void controller() {
#ifdef DEBUG
	initgraph(1200, 800, EX_SHOWCONSOLE);
#endif // DEBUG
	initgraph(1200, 800);
	while (true) {
		switch (flag)
		{
		case LOGIN:
			loginView();
			break;
		case SIGNUP:
			signupView();
			break;
		case MENU:
			menuView();
			break;
		case GAME:
			gameView();
			break;
		case WIN:
			winView();
			break;
		case LOSE:
			loseView();
			break;
		case SKIN:
			skinSelectView();
			break;
		case LEVEL:
			levelSelectView();
			break;
		case MUSIC:
			musicView();
			break;
		case TEAM:
			teamView();
			break;
		case DESCRIBE:
			describeView();
			break;
		case RANK:
			rankView();
			break;
		case LOADING:
			loadingView();
			break;
		}

	}
}

int main() {
	flag = LOGIN;
	last_flag = MENU;
	controller();
	return 0;
}

#ifdef DEBUG
void printUserInfo() {
	cout << user->id << endl
		<< user->username << endl
		<< user->password << endl
		<< user->current_level << endl
		<< user->cleared_levels << endl;
}
#endif // DEBUG



bool checkPassword(string password)
{
	// 用于验证是否合法, 0:包含小写, 1:包含数字, 2:包含特殊符号
	int flag[3] = { 0 };

	// 开头大写判断
	if (!isupper(password[0]))
		return false;

	int size = password.size();

	// 长度判断
	for (int i = 1; i < size; i++)
	{
		//小写字母
		if (islower(password[i])) flag[0] = 1;
		//数字
		else if (isdigit(password[i])) flag[1] = 1;
		//特殊符号
		else if (password[i] == '~' ||
			password[i] == '!' ||
			password[i] == '@' ||
			password[i] == '#' ||
			password[i] == '$' ||
			password[i] == '%' ||
			password[i] == '*'
			) flag[2] = 1;
	}

	if (flag[0] && flag[1] && flag[2])
	{
		return true;
	}
	return false;
}

int login(string username, string password) {
	ifstream fin(FPATH_USERS.c_str());
	if (!fin) {
		cerr << "无法打开文件: " << FPATH_USERS << endl;
		return 1;
	}

	string line;
	while (getline(fin, line)) {
		// 跳过空行或前导空格行
		if (line.empty() || line.find_first_not_of(' ') == string::npos)
			continue;

		istringstream iss(line);
		User temp;

		// 确保整行格式正确再处理
		if (!(iss >> temp.id >> temp.username >> temp.password >> temp.current_level >> temp.skinIdx >> temp.cleared_levels)) {
			continue; // 格式错误的行跳过，避免崩溃
		}

		if (temp.username == username) {
			if (temp.password == password) {
				user = new User(temp); // 成功登录，堆空间存储
				user->password = "******"; // 屏蔽密码
				fin.close();
				return 0; // 登录成功
			}
			else {
				fin.close();
				return 2; // 密码错误
			}
		}
	}

	fin.close();
	return 1; // 用户名不存在
}

int updateUserFile(const User* user) {
	fstream fs(FPATH_USERS, ios::in | ios::out);
	if (!fs)
		return 2;

	vector<User*> users;
	int idx = -1;

	string line;
	int i = 0;
	while (getline(fs, line)) {
		istringstream sin(line);
		User *us = new User;
		sin >> us->id >> us->username >> us->password >> us->current_level >> us->skinIdx >> us->cleared_levels;
		users.push_back(us);
		if (us->id == user->id)
			idx = i;
		i++;
	}

	if (idx == -1)
		return 1;

	// 修改并重新写入文件
	string pwd = users[idx]->password;
	delete users[idx];
	users[idx] = const_cast<User*>(user);
	users[idx]->password = pwd;
	fs.clear();
	fs.seekp(0, ios::beg);
#ifdef DEBUG
	cout << "更新用户信息: " << endl;
	printUserInfo();
#endif // DEBUG

	for (int i = 0; i < users.size(); i++) {
		fs << users[i]->id << " " << users[i]->username << " " << users[i]->password << " " 
			<< users[i]->current_level << " " << users[i]->skinIdx << " " << users[i]->cleared_levels << endl;
		if(i != idx)
			delete users[i];
	}
	return 0;
}

int signup(string nm, string pwd, int current_level, int skinIdx, int cleared_levels)
{
	fstream fs(FPATH_USERS, ios::in | ios::out);
	if (!fs)
		return 2;

	// 检查用户名是否已存在
	string line;
	while (getline(fs, line)) {
		istringstream sin(line);
		int _;
		string name;
		sin >> _ >> name;
		if (name == nm)
			return 1;
	}

	// 生成用户ID
	int id = static_cast<int>(time(0));

	// 写入文件
	fs.clear();
	fs.seekp(0, ios::end);
	fs << id << " " << nm << " " << pwd << " " << current_level << " " << skinIdx << " " << cleared_levels << endl;

	fs.close();

	return 0;
}


void initGame() {
	string path = FDIR_LEVELS + '/' + to_string(level) + ".txt";
	int element = 0, x = 0, y = 0;
	ifstream fin;
	fin.open(path);
	if (fin.is_open() == true)
	{
		for (int i = 0; i < MAP_HEIGHT; i++) {
			for (int j = 0; j < MAP_WIDTH; j++) {
				fin >> element;
				gameMap[i][j] = static_cast<GameElement>(element);
				if (element == DESTINATION)
				{
					dest.x = i;
					dest.y = j;
				}
			}
		}
		fin >> element;
		headdir = static_cast<Direction>(element);
		snake.clear();//清空原链表
		while (!fin.eof()) {
			fin >> x >> y;
			snake.push_back({ x,y });
		}
		fin.close();
	}
}

int snakeMove(Direction dir) {
	if ((headdir == UP && dir == DOWN) || (headdir == DOWN && dir == UP) || (headdir == LEFT && dir == RIGHT) || (headdir == RIGHT && dir == LEFT))
		return 1;
	int x = snake.front().x;
	int y = snake.front().y;
	int tail_x = snake.back().x;
	int tail_y = snake.back().y;

	GameElement e;
	switch (dir)//获取移动方向上的下一个元素
	{
	case UP:
		x--;
		break;
	case DOWN:
		x++;
		break;
	case LEFT:
		y--;
		break;
	case RIGHT:
		y++;
		break;
	}
	e = gameMap[x][y];
	switch (e)//根据不同元素对应操作
	{
	case AIR:
	case DESTINATION:
		headdir = dir;
		gameMap[tail_x][tail_y] = AIR;
		gameMap[snake.front().x][snake.front().y] = SNAKE_BODY;
		gameMap[x][y] = SNAKE_HEAD;
		smoothMove(dir, x, y);
#ifdef DEBUG
		cout << "我蠕动完了" << endl;
#endif // DEBUG
		snake.pop_back();
		snake.push_front({ x, y });
		return 0;
	case APPLE:
		headdir = dir;
		gameMap[snake.front().x][snake.front().y] = SNAKE_BODY;
		gameMap[x][y] = SNAKE_HEAD;
		smoothMove(dir, x, y);
		snake.push_front({ x,y });
		return 3;
	case BARRIER:
	case SNAKE_BODY:
		return 2;
	}
	return 0;
}

bool checkAndDrop() {
	bool isdrop = true;
	for (auto e : snake) {
		if ((gameMap[e.x + 1][e.y] == APPLE) || (gameMap[e.x + 1][e.y] == BARRIER) || (gameMap[e.x + 1][e.y] == DESTINATION)) {
			isdrop = false;
		}
	}
	if (isdrop) {
		for (auto e : snake) {
			gameMap[e.x][e.y] = AIR;
		}
		for (auto e : snake) {
			gameMap[e.x + 1][e.y] = SNAKE_BODY;
		}
		gameMap[snake.front().x + 1][snake.front().y] = SNAKE_HEAD;
		smoothDown();
		for (auto& e : snake) {
			e.x++;
		}
	}
	return isdrop;
}

bool isLose(bool drop_state) {
	bool onborder = false, nobarrier = true;
	for (auto e : snake) {
		if (e.x == MAP_HEIGHT - 1) {
			onborder = true;
			continue;
		}
		if (gameMap[e.x + 1][e.y] == APPLE || gameMap[e.x + 1][e.y] == BARRIER)
			nobarrier = false;
	}
	return onborder && nobarrier;
}

inline
bool isWin() {
	return (snake.front().x == dest.x) && (snake.front().y == dest.y);
}

inline
void setMusic(bool state) {
	music_play = state;
};

inline
void setSound(bool state) {
	sound_play = state;
}

bool loadGameMusic() {
	int err = mciSendString("open assets/music/game_bgm.mp3 alias BGM", NULL, 0, NULL);
	if (err) {
		char err_text[80];
		mciGetErrorString(err, err_text, sizeof(err_text));
		cerr << "open music failed: " << err << endl;
		return false;
	}
	err = mciSendString("setaudio BGM volume to 25", NULL, 0, NULL);
	if (err) {
		char err_text[80];
		mciGetErrorString(err, err_text, sizeof(err_text));
		cerr << "set BGM volume failed: " << err << endl;
		return false;
	}
	return true;
}

inline
bool playGameMusic() {
	//在这里完成代码 
	mciSendString("play BGM repeat", NULL, 0, NULL);
	return true;
}

bool stopGameMusic() {
	mciSendString("stop BGM ", NULL, 0, NULL);
	return true;
}

bool loadGameSound() {
	int err1 = mciSendString("open assets/music/Collision.wav alias SOUND1", NULL, 0, NULL);
	int err2 = mciSendString("open assets/music/destination.wav alias SOUND2", NULL, 0, NULL);
	int err3 = mciSendString("open assets/music/apple.wav alias SOUND3", NULL, 0, NULL);
	int err4 = mciSendString("open assets/music/move.wav alias SOUND4", NULL, 0, NULL);
	int err5 = mciSendString("open assets/music/win.wav alias SOUND5", NULL, 0, NULL);
	int err6 = mciSendString("open assets/music/lose.wav alias SOUND6", NULL, 0, NULL);
	int err_sps = mciSendString("open assets/music/special_move.wav alias SPECIAL_MOVE", NULL, 0, NULL);

	// 部分音效加载失败，但不影响游戏运行
	if (err1 || err2 || err3 || err4 || err5 || err6 || err_sps)
		return false;

	return true;
}

bool playGameSound(int type) {
	// 在这里完成代码 SOUND_SPECIAL
	if (sound_play) {
		char cmd[80];
		if (type == 4 && user->skinIdx == 1)
			sprintf_s(cmd, "play SPECIAL_MOVE from 0");
		else
			sprintf_s(cmd, "play SOUND%d from 0", type);
		return !mciSendString(cmd, NULL, 0, NULL);
	}
	return false;
}

void loadRank() {
	// 功能: loadRank: 从排行榜文件中读取信息并存入全局变量rank_list
	ifstream fin("data/rank.txt");
	if (!fin)	
		throw runtime_error("open rank file failed!");
	RankItem item;
	string line;
	while (getline(fin, line)) {
		istringstream sin(line);
		sin >> item.username >> item.score;
		rank_list.push_back(item);
	}
}

void saveRank() {
	if (rank_list.size() == 0)
		return;

	ofstream fout("data/rank.txt");
	if (!fout)
		throw runtime_error("open rank file failed!");
	for (auto itm : rank_list)
		fout << itm.username << " " << itm.score << endl;
}

bool updateRank(string nm, int sc) {
	if (rank_list.size() == RANK_CAP && sc < rank_list.back().score)
		return false;

	//先找同名，有同名据分判断是否插入；再找应插处插入
	auto iter_dupnm = find_if(rank_list.begin(), rank_list.end(),
		[nm](const RankItem& itm) { return itm.username == nm; });

	if (iter_dupnm != rank_list.end())
		if (iter_dupnm->score > sc)	
			return false;
		else 
			rank_list.erase(iter_dupnm);
		
	//找位置插入(坑, STL下标插入不会自动扩容)
	auto iter = rank_list.begin();
	while (iter != rank_list.end() && iter->score >= sc)	iter++;
	rank_list.insert(iter, { nm, sc });
	
	if (rank_list.size() > RANK_CAP)
		rank_list.pop_back();
#ifdef DEBUG
	cout << "after updating "<<nm << " " << sc << ", size " << rank_list.size() << endl;
	for (auto itm : rank_list)
		cout << itm.username << " " << itm.score << endl;
#endif // DEBUG

	return true;
}

inline
bool changeSkin(int skinIdx)
{
	user->skinIdx = skinIdx;
	updateUserFile(user);
	return true;
}

inline
int getLevelMask(int level) {
	if (level > 32) //错误情况
		return 0;
	return 1 << (level - 1);
}

inline
int get1Count(int num) {
	int count = 0;
	while (num) {
		if (num & 1)
			count++;
		num >>= 1;
	}
	return count;
}



void loginView() {
	// 是否运行
	bool running = true;

	IMAGE title;            // 注册页面的标题
	IMAGE account_text;     // 账号输入框左边的文字
	IMAGE password_text;    // 密码输入框左边的文字
	// 加载图片资源
	loadimage(&title, _T(R"(assets\image\login_text.png)"));
	loadimage(&password_text, _T(R"(assets\image\password_text.png)"));
	loadimage(&account_text, _T(R"(assets\image\account_text.png)"));

	// 创建输入框
	wzy_TextBox password = { 300,420,680,60 };
	wzy_TextBox account = { 300,280,680,60 };

	// 用户消息
	ExMessage msg;

	// 帧率，控制再60帧
	const int FPS = 60;;

	// 双缓冲渲染
	BeginBatchDraw();
	while (running)
	{
		// 当前帧开始的时间
		DWORD frame_start_time = GetTickCount();
		// 上一帧（第一帧）的时间
		static DWORD last_tick_time = GetTickCount();
		// 当前帧的时间
		DWORD current_tick_time = GetTickCount();
		// 间隔时间
		DWORD delta_tick = current_tick_time - last_tick_time;

		// 更新输入框
		wzy_updateTextBox(password, delta_tick);
		wzy_updateTextBox(account, delta_tick);

		// 更新上一帧的时间
		last_tick_time = current_tick_time;

		// 绘制背景
		setbkcolor(0xADD8E6);
		cleardevice();

		bool loginClicked = button(msg, 280, 600, 240, 60, "登录", BLACK, 32, "微软雅黑", WHITE, RGB(0, 120, 215));
		bool signupClicked = button(msg, 680, 600, 240, 60, "注册", BLACK, 32, "微软雅黑", WHITE, RGB(0, 120, 215));

		if (loginClicked) {
			if (account.text.empty()) {
				MessageBox(GetHWnd(), _T("用户名不能为空！"), _T("提示"), MB_OK | MB_ICONWARNING);
			}
			else if (password.text.empty()) {
				MessageBox(GetHWnd(), _T("密码不能为空！"), _T("提示"), MB_OK | MB_ICONWARNING);
			}
			else {
				// 获取登录结果
				int loginResult = login(account.text, password.text);
				if (loginResult == 0) {
					// 登录成功
					flag = LOADING;
					last_flag = flag;
					running = false;
					EndBatchDraw();
					return;
				}
				else if (loginResult == 1) {
					MessageBox(GetHWnd(), _T("用户名不存在！"), _T("登录失败"), MB_OK | MB_ICONERROR);
				}
				else if (loginResult == 2) {
					MessageBox(GetHWnd(), _T("密码错误！"), _T("登录失败"), MB_OK | MB_ICONERROR);
					// 清空密码输入框
					password.text.clear();
				}
			}
		}

		if (signupClicked) {
			flag = SIGNUP;
			running = false;
			EndBatchDraw();
			return;
		}

		// 获取消息队列的信息
		while (peekmessage(&msg))
		{
			wzy_inputTextBox(account, msg);
			wzy_inputTextBox(password, msg);
		}

		// 绘制控件和图片
		wzy_putimage_alpha(450, 100, &title);
		wzy_putimage_alpha(160, 420, &password_text);
		wzy_putimage_alpha(160, 280, &account_text);
		wzy_drawTextBox(password, true);
		wzy_drawTextBox(account, false);

		FlushBatchDraw();

		// 计算当前使用了多少时间，控制帧率
		DWORD frame_end_time = GetTickCount();
		DWORD frame_delta_time = frame_end_time - frame_start_time;
		if (frame_delta_time < 1000 / FPS)
			Sleep(1000 / FPS - frame_delta_time);
	}
	cleardevice();
	EndBatchDraw();
}

void loadingView() {
	bool music_loaded = false, sound_loaded = false, data_loaded = false;

	// 加载背景
	IMAGE img_bg;
	loadimage(&img_bg, "assets/image/loading.png", 1200, 800);

	// 进度条参数
	const int progress_width = 400;
	const int progress_height = 20;
	const int progress_x = (getwidth() - progress_width) / 2;
	const int progress_y = getheight() / 2 + 100;

	// 文字参数
	const char* loading_text = "加载中...";
	const int text_x = getwidth() / 2 - textwidth(loading_text) / 2;
	const int text_y = getheight() / 2 - 150;

	// 动画控制变量
	int progress = 0;
	DWORD last_time = GetTickCount();
	DWORD frame_interval = 50;  // 帧间隔(ms)

	// 加载任务列表
	const int total_tasks = 4;
	int current_task = 0;
	const char* task_names[] = {
		"加载游戏数据...",
		"加载音效资源...",
		"加载音乐资源...",
		"初始化游戏系统...",
	};

	BeginBatchDraw();

	// 动画循环
	while (progress < 100) {
		cleardevice();
		putimage(0, 0, &img_bg);

		// 绘制文字
		settextcolor(WHITE);
		settextstyle(40, 0, "微软雅黑");
		outtextxy(text_x, text_y, loading_text);

		// 绘制当前任务文字
		settextstyle(30, 0, "微软雅黑");
		outtextxy(progress_x, progress_y - 40, task_names[current_task]);

		// 绘制进度条背景
		setfillcolor(RGB(100, 100, 100));
		fillroundrect(progress_x, progress_y,
			progress_x + progress_width, progress_y + progress_height, 10, 10);

		// 绘制进度条前景
		setfillcolor(RGB(0, 150, 255));
		fillroundrect(progress_x, progress_y,
			progress_x + (progress_width * progress / 100),
			progress_y + progress_height, 10, 10);

		// 绘制百分比文字
		char percent_text[10];
		sprintf_s(percent_text, "%d%%", progress);
		settextstyle(25, 0, "微软雅黑");
		outtextxy(progress_x + progress_width + 10, progress_y - 5, percent_text);

		FlushBatchDraw();

		// 更新进度
		DWORD current_time = GetTickCount();
		if (current_time - last_time >= frame_interval) {
			last_time = current_time;

			// 模拟进度更新
			progress += 2;  // 调慢一点让旋转更明显
			if (progress > 100) progress = 100;

			// 根据进度更新当前任务
			if (progress >= 25 && current_task < 1) {
				if (!data_loaded) {
					loadRank(); // 加载排行榜
					level = user->current_level;
					data_loaded = true;
					current_task = 1;
#ifdef DEBUG
					printUserInfo();
#endif // DEBUG
					continue;
				}

			}
			else if (progress >= 50 && current_task < 2) {
				if (!sound_loaded) {
					sound_loaded = loadGameSound();
					if (sound_loaded) {
#ifdef DEBUG
						cout << "音效加载成功" << endl;
#endif // DEBUG
						sound_play = true;
						playGameSound(3);
						current_task = 2;
						continue;
					}
				}
			}
			else if (progress >= 75 && current_task < 3) {
				if (!music_loaded) {
					music_loaded = loadGameMusic(); //加载音乐
					if (music_loaded) {
#ifdef DEBUG
						cout << "音乐加载成功" << endl;
#endif // DEBUG
						music_play = playGameMusic(); // 播放背景音乐
						current_task = 3;
						continue;
					}
				}
			}
		}

		// 处理退出消息
		ExMessage msg;
		if (peekmessage(&msg, EX_KEY)) {
			if (msg.message == WM_KEYDOWN && msg.vkcode == VK_ESCAPE) {
				break;
			}
		}
		
		Sleep(10);
	}
	EndBatchDraw();

	last_flag = LOADING;
	flag = MENU;
}

void signupView()
{
	// 是否运行
	bool running = true;

	IMAGE title;            // 注册页面的标题
	IMAGE account_text;     // 账号输入框左边的文字
	IMAGE password_text;    // 密码输入框左边的文字
	IMAGE repassword_text;    // 密码输入框左边的文字
	// 加载图片资源
	loadimage(&title, _T(R"(assets\image\signup_text.png)"));
	loadimage(&account_text, _T(R"(assets\image\account_text.png)"));
	loadimage(&password_text, _T(R"(assets\image\password_text.png)"));
	loadimage(&repassword_text, _T(R"(assets\image\repassword_text.png)"));

	// 创建输入框
	wzy_TextBox account = { 300,280,680,60 };
	wzy_TextBox password = { 300,380,680,60 };
	wzy_TextBox repassword = { 300,480,680,60 };

	// 用户消息
	ExMessage msg;

	// 帧率，控制再60帧
	const int FPS = 60;;

	// 双缓冲渲染
	BeginBatchDraw();

	while (running)
	{
		// 当前帧开始的时间
		DWORD frame_start_time = GetTickCount();

		// 上一帧（第一帧）的时间
		static DWORD last_tick_time = GetTickCount();
		// 当前帧的时间
		DWORD current_tick_time = GetTickCount();
		// 间隔时间
		DWORD delta_tick = current_tick_time - last_tick_time;

		// 更新输入框
		wzy_updateTextBox(account, delta_tick);
		wzy_updateTextBox(password, delta_tick);
		wzy_updateTextBox(repassword, delta_tick);

		// 更新上一帧的时间
		last_tick_time = current_tick_time;

		// 绘制背景
		setbkcolor(0xADD8E6);
		cleardevice();

		bool loginClicked = button(msg, 280, 600, 240, 60, "去登录", BLACK, 32, "微软雅黑", WHITE, RGB(0, 120, 215));
		bool signupClicked = button(msg, 680, 600, 240, 60, "注册", BLACK, 32, "微软雅黑", WHITE, RGB(0, 120, 215));

		if (loginClicked) {
			flag = LOGIN;
			running = false;
		}

		if (signupClicked) {
			if (account.text.empty()) {
				MessageBox(GetHWnd(), "账号不能为空", "提示", MB_OK);
			}
			else if (!checkPassword(password.text)) {
				MessageBox(GetHWnd(), "密码太简单！\n密码的首字母应该为大写字母，至少包含一个小写字母、数字和特殊符号", "提示", MB_OK);
			}
			else if (password.text != repassword.text) {
				MessageBox(GetHWnd(), "确认密码与密码不一致", "提示", MB_OK);
			}
			else {
				int code = signup(account.text, password.text);
				if (code == 0) {
					MessageBox(GetHWnd(), "注册成功！即将转到登录界面", "提示", MB_OK);
					flag = LOGIN;
					running = false;
					return;
				}
				else if (code == 1) {
					MessageBox(GetHWnd(), "用户名已存在！", "提示", MB_OK);
				}
				else if (code == 2) {
					MessageBox(GetHWnd(), "注册失败！", "提示", MB_OK);
				}
			}
		}

		// 获取消息队列的信息
		while (peekmessage(&msg))
		{
			wzy_inputTextBox(account, msg);
			wzy_inputTextBox(password, msg);
			wzy_inputTextBox(repassword, msg);
		}

		// 绘制控件和图片
		wzy_putimage_alpha(450, 100, &title);
		wzy_putimage_alpha(160, 280, &account_text);
		wzy_putimage_alpha(160, 380, &password_text);
		wzy_putimage_alpha(0, 480, &repassword_text);
		wzy_drawTextBox(account, false);
		wzy_drawTextBox(password, true);
		wzy_drawTextBox(repassword, true);

		FlushBatchDraw();

		// 计算当前使用了多少时间，控制帧率
		DWORD frame_end_time = GetTickCount();
		DWORD frame_delta_time = frame_end_time - frame_start_time;
		if (frame_delta_time < 1000 / FPS)
			Sleep(1000 / FPS - frame_delta_time);
	}

	cleardevice();
	EndBatchDraw();
}

void menuView() {
	ExMessage msg = { 0 };
	IMAGE img;
	loadimage(&img, "./assets/image/menu.png", 1200, 800);

	setbkcolor(WHITE);
	cleardevice();
	setbkmode(TRANSPARENT);
	int arri = 0;
	int arrj = 0;
	static int arrowhead[7][2] = { {385,70},{895, 242},{895, 322} ,{895, 402},{895, 482},{895, 562},{385, 722} };
	int bx = arrowhead[0][0];
	int by = arrowhead[0][1];

	constexpr int BTN_X = 870, BTN_H = 76, BTN_W = 350, BTN_H_SPACE = 80, BTN_Y_START = 202;
	constexpr int BTN_GAME_X = 350, BTN_GAME_Y = 30, BTN_GAME_W = 500;
	constexpr int BTN_EXIT_X = 350, BTN_EXIT_Y = 682, BTN_EXIT_W = 500;
	static vector<bool> state_hover(7, false); //第0个为开始按钮的状态, 最后一个为退出按钮的, 其余顺序
	static auto checkStateHover = [&](const int i)->bool {
		if (i == 0)
			return msg.x > BTN_GAME_X && msg.x<BTN_GAME_X + BTN_GAME_W && msg.y>BTN_GAME_Y && msg.y < BTN_GAME_Y + BTN_H;
		if (i == 6)
			return msg.x > BTN_EXIT_X && msg.x<BTN_EXIT_X + BTN_EXIT_W && msg.y>BTN_EXIT_Y && msg.y < BTN_EXIT_Y + BTN_H;
		return msg.x > BTN_X && msg.x < BTN_X + BTN_W && msg.y>BTN_Y_START + (i - 1) * BTN_H_SPACE && msg.y < BTN_Y_START + (i - 1) * BTN_H_SPACE + BTN_H;
	};
	int clickedIdx = -1;

	BeginBatchDraw();
	while (flag == MENU)
	{
		cleardevice();
		putimage(0, 0, &img);
		
		setlinecolor(RGB(93, 175, 251));
		button(msg, BTN_GAME_X, BTN_GAME_Y, BTN_GAME_W, BTN_H, "开始游戏！", BLACK, 0, "微软雅黑", RGB(93, 175, 251), RGB(129, 201, 247), BLUE, 5);
		button(msg, BTN_X, 202, BTN_W, BTN_H, "选择关卡！", BLACK, 0, "微软雅黑", RGB(129, 201, 247), RGB(131, 206, 248), BLUE, 5);
		button(msg, BTN_X, 282, BTN_W, BTN_H, "玩法说明！", BLACK, 0, "微软雅黑", RGB(131, 206, 248), RGB(137, 210, 245), BLUE, 5);
		button(msg, BTN_X, 362, BTN_W, BTN_H, "制作组！", BLACK, 0, "微软雅黑", RGB(137, 210, 245), RGB(140, 214, 241), BLUE, 5);
		button(msg, BTN_X, 442, BTN_W, BTN_H, "排行榜！", BLACK, 0, "微软雅黑", RGB(140, 214, 241), RGB(140, 214, 241), BLUE, 5);
		button(msg, BTN_X, 522, BTN_W, BTN_H, "音乐设置！", BLACK, 0, "微软雅黑", RGB(148, 220, 244), RGB(148, 220, 244), BLUE, 5);
		button(msg, BTN_EXIT_X, BTN_EXIT_Y, BTN_EXIT_W, BTN_H, "退出游戏！", BLACK, 0, "微软雅黑", RGB(155, 223, 250), RGB(155, 223, 250), BLUE, 5);

		setfillcolor(BLACK);
		fillcircle(arrowhead[arri][0], arrowhead[arri][1], 20);
		FlushBatchDraw();

		// 阻塞新一轮绘图
		while (true) {
			if (peekmessage(&msg, EX_MOUSE | EX_KEY | EX_WINDOW))
			{
				//窗口移动
				if (msg.message == WM_MOVE) {
					break;
				}

				// 监视键盘输入打断阻塞
				if (msg.message == WM_KEYDOWN)
				{
					switch (msg.vkcode)
					{
					case 'w':
					case 'W':
					case VK_UP:
						if (arri > 0 && arri <= 6)
						{
							bx = arrowhead[arri - 1][arrj];
							by = arrowhead[arri - 1][arrj + 1];
							arri--;
						}
						else if (arri == 0)
						{
							bx = arrowhead[6][0];
							by = arrowhead[6][1];
							arri = 6;
						}
						break;
					case 's':
					case 'S':
					case VK_DOWN:
						if (arri >= 0 && arri < 6)
						{
							bx = arrowhead[arri + 1][arrj];
							by = arrowhead[arri + 1][arrj + 1];
							arri++;
						}
						else if (arri == 6)
						{
							bx = arrowhead[0][0];
							by = arrowhead[0][1];
							arri = 0;
						}
						break;
					case VK_RETURN:
						if (arri == 0)
						{
							flag = GAME;
						}
						else if (arri == 1)
						{
							flag = LEVEL;
						}
						else if (arri == 2)
						{
							flag = DESCRIBE;
						}
						else if (arri == 3)
						{
							flag = TEAM;
						}
						else if (arri == 4)
						{
							flag = RANK;
						}
						else if (arri == 5)
						{
							flag = MUSIC;
						}
						else if (arri == 6)
						{
							exit(0);
						}
					}
					break;
				}

				// 如果鼠标进入或离开某个按钮区域，打断阻塞
				bool bk_flag = false;
				for (int i = 0; i < state_hover.size(); i++) {
					if (state_hover[i] != checkStateHover(i)) {
						state_hover[i] = !state_hover[i];
	/*					if (state_hover[i])
							arri = i;*/
						bk_flag = true;
						break;
					}
				}
				if (bk_flag)
					break;
				// 如果鼠标在某个按钮区域点击，打断阻塞
				for (int i = 0; i < state_hover.size(); i++) {
					if (state_hover[i] && msg.message == WM_LBUTTONDOWN) {
						clickedIdx = i;
						bk_flag = true;
						break;
					}
				}
				if (clickedIdx >= 0 && clickedIdx < state_hover.size()) {
					last_flag = MENU;
					switch (clickedIdx)
					{
					case 0: flag = GAME; break;
					case 1: flag = LEVEL; break;
					case 2: flag = DESCRIBE; break;
					case 3: flag = TEAM; break;
					case 4: flag = RANK; break;
					case 5: flag = MUSIC; break;
					case 6: exit(0); break;
					default:
						break;
					}
				}
				if (bk_flag)
					break;

			}
			//Sleep(5);
		}

		msg.message = 0;
	}
	EndBatchDraw();
}

void drawMap() {
	static bool isLoaded = false;
	// 提前加载所有用到的图片（只需加载一次，可放到初始化阶段）
	if (!isLoaded) {
		loadimage(img_apple + 0, _T("assets/image/apple_mask.jpg"), CELL_SIZE, CELL_SIZE);
		loadimage(img_apple + 1, _T("assets/image/apple.jpg"), CELL_SIZE, CELL_SIZE);
		loadimage(img_barrier + 0, _T("assets/image/barrier_mask.jpg"), 59, 61);
		loadimage(img_barrier + 1, _T("assets/image/barrier.jpg"), 59, 61);
		loadimage(img_dest + 0, _T("assets/image/destination_mask.jpg"), 65, 65);
		loadimage(img_dest + 1, _T("assets/image/destination.jpg"), 65, 65);

		isLoaded = true;
	}

	// 开始绘制地图
	for (int i = MAP_HEIGHT - 1; i >= 0; i--) {
		for (int j = MAP_WIDTH - 1; j >= 0; j--) {
			int x = j * CELL_SIZE;
			int y = i * CELL_SIZE;

			switch (gameMap[i][j]) {
			case AIR:
				//putimage(x, y, &img_air);
				break;
			case APPLE:
				putimage(x, y, img_apple + 0, NOTSRCERASE);
				putimage(x, y, img_apple + 1, SRCINVERT);
				break;
			case BARRIER:
				putimage(x, y, img_barrier + 0, NOTSRCERASE);
				putimage(x, y, img_barrier + 1, SRCINVERT);
				break;
			case DESTINATION:
				putimage(x, y, img_dest + 0, NOTSRCERASE);
				putimage(x, y, img_dest + 1, SRCINVERT);
				break;
			default:
				//putimage(x, y, &img_air); // 默认用AIR替代
				break;
			}
		}
	}
}

void drawSnake() {
	if (snake.empty()) return;
	// 取皮肤图片路径
	string headPath = skins[user->skinIdx].head;
	string bodyPath = skins[user->skinIdx].body;

	// 加载图片（缩放到格子大小）
	static bool isLoaded = false;
	if (!isLoaded) {
		loadimage(headImg0 + 0, "assets/skins/1/snakehead_mask_right1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(headImg0 + 1, "assets/skins/1/snakehead_right1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(headImg_down0 + 0, "assets/skins/1/snakehead_mask_down1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(headImg_down0 + 1, "assets/skins/1/snakehead_down1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(headImg_up0 + 0, "assets/skins/1/snakehead_mask_up1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(headImg_up0 + 1, "assets/skins/1/snakehead_up1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(headImg_left0 + 0, "assets/skins/1/snakehead_mask_left1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(headImg_left0 + 1, "assets/skins/1/snakehead_left1.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(turn_leftanddown0 + 0, "assets/skins/1/turn_mask_leftanddown1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftanddown0 + 1, "assets/skins/1/turn_leftanddown1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup0 + 0, "assets/skins/1/turn_mask_leftandup1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup0 + 1, "assets/skins/1/turn_leftandup1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown0 + 0, "assets/skins/1/turn_mask_rightanddown1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown0 + 1, "assets/skins/1/turn_rightanddown1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup0 + 0, "assets/skins/1/turn_mask_rightandup1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup0 + 1, "assets/skins/1/turn_rightandup1.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(tailImg_up0 + 0, "assets/skins/1/snaketail_mask_up1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_up0 + 1, "assets/skins/1/snaketail_up1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down0 + 0, "assets/skins/1/snaketail_mask_down1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down0 + 1, "assets/skins/1/snaketail_down1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left0 + 0, "assets/skins/1/snaketail_mask_left1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left0 + 1, "assets/skins/1/snaketail_left1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right0 + 0, "assets/skins/1/snaketail_mask_right1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right0 + 1, "assets/skins/1/snaketail_right1.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(bodyImg0 + 0, "assets/skins/1/snakebody_h1.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(bodyImg0 + 1, "assets/skins/1/snakebody_v1.jpg", CELL_SIZE, CELL_SIZE, true);


		//第二套皮肤
		loadimage(headImg1 + 0, "assets/skins/2/snakehead_mask_right2.jpg", 48, CELL_SIZE, true);
		loadimage(headImg1 + 1, "assets/skins/2/snakehead_right2.jpg", 48, CELL_SIZE, true);
		loadimage(headImg_down1 + 0, "assets/skins/2/snakehead_mask_down2.jpg", CELL_SIZE, 48, true);
		loadimage(headImg_down1 + 1, "assets/skins/2/snakehead_down2.jpg", CELL_SIZE, 48, true);
		loadimage(headImg_up1 + 0, "assets/skins/2/snakehead_mask_up2.jpg", CELL_SIZE, 48, true);
		loadimage(headImg_up1 + 1, "assets/skins/2/snakehead_up2.jpg", CELL_SIZE, 48, true);
		loadimage(headImg_left1 + 0, "assets/skins/2/snakehead_mask_left2.jpg", 48, CELL_SIZE, true);
		loadimage(headImg_left1 + 1, "assets/skins/2/snakehead_left2.jpg", 48, CELL_SIZE, true);

		loadimage(turn_leftanddown1 + 0, "assets/skins/2/turn_mask_leftanddown2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftanddown1 + 1, "assets/skins/2/turn_leftanddown2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup1 + 0, "assets/skins/2/turn_mask_leftandup2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup1 + 1, "assets/skins/2/turn_leftandup2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown1 + 0, "assets/skins/2/turn_mask_rightanddown2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown1 + 1, "assets/skins/2/turn_rightanddown2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup1 + 0, "assets/skins/2/turn_mask_rightandup2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup1 + 1, "assets/skins/2/turn_rightandup2.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(tailImg_up1 + 0, "assets/skins/2/snaketail_mask_up2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_up1 + 1, "assets/skins/2/snaketail_up2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down1 + 0, "assets/skins/2/snaketail_mask_down2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down1 + 1, "assets/skins/2/snaketail_down2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left1 + 0, "assets/skins/2/snaketail_mask_left2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left1 + 1, "assets/skins/2/snaketail_left2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right1 + 0, "assets/skins/2/snaketail_mask_right2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right1 + 1, "assets/skins/2/snaketail_right2.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(bodyImg1 + 0, "assets/skins/2/snakebody_h2.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(bodyImg1 + 1, "assets/skins/2/snakebody_v2.jpg", CELL_SIZE, CELL_SIZE, true);

		//第三套皮肤
		loadimage(headImg2 + 0, "assets/skins/3/snakehead_mask_right3.jpg", 43, CELL_SIZE, true);
		loadimage(headImg2 + 1, "assets/skins/3/snakehead_right3.jpg", 43, CELL_SIZE, true);
		loadimage(headImg_down2 + 0, "assets/skins/3/snakehead_mask_down3.jpg", CELL_SIZE, 43, true);
		loadimage(headImg_down2 + 1, "assets/skins/3/snakehead_down3.jpg", CELL_SIZE, 43, true);
		loadimage(headImg_up2 + 0, "assets/skins/3/snakehead_mask_up3.jpg", CELL_SIZE, 43, true);
		loadimage(headImg_up2 + 1, "assets/skins/3/snakehead_up3.jpg", CELL_SIZE, 43, true);
		loadimage(headImg_left2 + 0, "assets/skins/3/snakehead_mask_left3.jpg", 43, CELL_SIZE, true);
		loadimage(headImg_left2 + 1, "assets/skins/3/snakehead_left3.jpg", 43, CELL_SIZE, true);

		loadimage(turn_leftanddown2 + 0, "assets/skins/3/turn_mask_leftanddown3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftanddown2 + 1, "assets/skins/3/turn_leftanddown3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup2 + 0, "assets/skins/3/turn_mask_leftandup3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup2 + 1, "assets/skins/3/turn_leftandup3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown2 + 0, "assets/skins/3/turn_mask_rightanddown3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown2 + 1, "assets/skins/3/turn_rightanddown3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup2 + 0, "assets/skins/3/turn_mask_rightandup3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup2 + 1, "assets/skins/3/turn_rightandup3.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(tailImg_up2 + 0, "assets/skins/3/snaketail_mask_up3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_up2 + 1, "assets/skins/3/snaketail_up3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down2 + 0, "assets/skins/3/snaketail_mask_down3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down2 + 1, "assets/skins/3/snaketail_down3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left2 + 0, "assets/skins/3/snaketail_mask_left3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left2 + 1, "assets/skins/3/snaketail_left3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right2 + 0, "assets/skins/3/snaketail_mask_right3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right2 + 1, "assets/skins/3/snaketail_right3.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(bodyImg2 + 0, "assets/skins/3/snakebody_h3.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(bodyImg2 + 1, "assets/skins/3/snakebody_v3.jpg", CELL_SIZE, CELL_SIZE, true);

		//第四套皮肤
		loadimage(headImg3 + 0, "assets/skins/4/snakehead_mask_right4.jpg", 38, CELL_SIZE, true);
		loadimage(headImg3 + 1, "assets/skins/4/snakehead_right4.jpg", 38, CELL_SIZE, true);
		loadimage(headImg_down3 + 0, "assets/skins/4/snakehead_mask_down4.jpg", CELL_SIZE, 38, true);
		loadimage(headImg_down3 + 1, "assets/skins/4/snakehead_down4.jpg", CELL_SIZE, 38, true);
		loadimage(headImg_up3 + 0, "assets/skins/4/snakehead_mask_up4.jpg", CELL_SIZE, 38, true);
		loadimage(headImg_up3 + 1, "assets/skins/4/snakehead_up4.jpg", CELL_SIZE, 38, true);
		loadimage(headImg_left3 + 0, "assets/skins/4/snakehead_mask_left4.jpg", 38, CELL_SIZE, true);
		loadimage(headImg_left3 + 1, "assets/skins/4/snakehead_left4.jpg", 38, CELL_SIZE, true);

		loadimage(turn_leftanddown3 + 0, "assets/skins/4/turn_mask_leftanddown4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftanddown3 + 1, "assets/skins/4/turn_leftanddown4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup3 + 0, "assets/skins/4/turn_mask_leftandup4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_leftandup3 + 1, "assets/skins/4/turn_leftandup4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown3 + 0, "assets/skins/4/turn_mask_rightanddown4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightanddown3 + 1, "assets/skins/4/turn_rightanddown4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup3 + 0, "assets/skins/4/turn_mask_rightandup4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(turn_rightandup3 + 1, "assets/skins/4/turn_rightandup4.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(tailImg_up3 + 0, "assets/skins/4/snaketail_mask_up4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_up3 + 1, "assets/skins/4/snaketail_up4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down3 + 0, "assets/skins/4/snaketail_mask_down4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_down3 + 1, "assets/skins/4/snaketail_down4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left3 + 0, "assets/skins/4/snaketail_mask_left4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_left3 + 1, "assets/skins/4/snaketail_left4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right3 + 0, "assets/skins/4/snaketail_mask_right4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(tailImg_right3 + 1, "assets/skins/4/snaketail_right4.jpg", CELL_SIZE, CELL_SIZE, true);

		loadimage(bodyImg3 + 0, "assets/skins/4/snakebody_h4.jpg", CELL_SIZE, CELL_SIZE, true);
		loadimage(bodyImg3 + 1, "assets/skins/4/snakebody_v4.jpg", CELL_SIZE, CELL_SIZE, true);

		isLoaded = true;
	}

	switch (user->skinIdx)
	{
	case 1:
		headImg = headImg1;
		bodyImg = bodyImg1;
		tailImg_up = tailImg_up1;
		tailImg_down = tailImg_down1;
		tailImg_left = tailImg_left1;
		tailImg_right = tailImg_right1;
		headImg_down = headImg_down1;
		headImg_up = headImg_up1;
		headImg_left = headImg_left1;
		turn_leftanddown = turn_leftanddown1;
		turn_leftandup = turn_leftandup1;
		turn_rightanddown = turn_rightanddown1;
		turn_rightandup = turn_rightandup1;
		break;
	case 2:
		headImg = headImg2;
		bodyImg = bodyImg2;
		tailImg_up = tailImg_up2;
		tailImg_down = tailImg_down2;
		tailImg_left = tailImg_left2;
		tailImg_right = tailImg_right2;
		headImg_down = headImg_down2;
		headImg_up = headImg_up2;
		headImg_left = headImg_left2;
		turn_leftanddown = turn_leftanddown2;
		turn_leftandup = turn_leftandup2;
		turn_rightanddown = turn_rightanddown2;
		turn_rightandup = turn_rightandup2;
		break;
	case 3:
		headImg = headImg3;
		bodyImg = bodyImg3;
		tailImg_up = tailImg_up3;
		tailImg_down = tailImg_down3;
		tailImg_left = tailImg_left3;
		tailImg_right = tailImg_right3;
		headImg_down = headImg_down3;
		headImg_up = headImg_up3;
		headImg_left = headImg_left3;
		turn_leftanddown = turn_leftanddown3;
		turn_leftandup = turn_leftandup3;
		turn_rightanddown = turn_rightanddown3;
		turn_rightandup = turn_rightandup3;
		break;
	case 0:
	default:
		headImg = headImg0;
		bodyImg = bodyImg0;
		tailImg_up = tailImg_up0;
		tailImg_down = tailImg_down0;
		tailImg_left = tailImg_left0;
		tailImg_right = tailImg_right0;
		headImg_down = headImg_down0;
		headImg_up = headImg_up0;
		headImg_left = headImg_left0;
		turn_leftanddown = turn_leftanddown0;
		turn_leftandup = turn_leftandup0;
		turn_rightanddown = turn_rightanddown0;
		turn_rightandup = turn_rightandup0;
		break;
	}

	// 绘制蛇头
	Position head = snake.front();
	int headX = head.x * CELL_SIZE;
	int headY = head.y * CELL_SIZE;
	IMAGE* rotatedhead = headImg;
	// 旋转蛇头
	switch (headdir) {
	case UP:
		rotatedhead = headImg_up;
		break;
	case DOWN:
		rotatedhead = headImg_down;
		break;
	case LEFT:
		rotatedhead = headImg_left;
		break;
	case RIGHT:
	default:
		// 默认方向是右，不旋转
		break;
	}
	putimage(headY, headX, rotatedhead + 0, NOTSRCERASE);
	putimage(headY, headX, rotatedhead + 1, SRCINVERT);
	// 绘制蛇身（跳过头部）
	auto it = ++snake.begin();
	int	dx_prev, dy_prev, dx_next, dy_next, tailx, taily;
	for (; it != prev(snake.end()); ++it) {
		dx_prev = it->x - prev(it)->x; // 前一段方向 1代表由下到上 -1代表由上到下 0代表同一行
		dy_prev = it->y - prev(it)->y;//1代表由右到左 -1代表由左到右 0代表同一列
		dx_next = next(it)->x - it->x; // 后一段方向
		dy_next = next(it)->y - it->y;
		if (dx_prev == dx_next && dy_prev == dy_next) {// 直身
			if (dx_prev) {
				putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, bodyImg + 1);
			}
			else
			{
				putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, bodyImg);
			}
		}
		else {
			IMAGE* rotatedhead = headImg;
			// 转弯，根据 dx_prev/dy_prev 和 dx_next/dy_next 选择转弯图片
			if (dx_prev == 1 && dy_next == 1) {
				rotatedhead = turn_rightandup;
			}
			else if (dx_prev == 1 && dy_next == -1) {
				// 左转向下
				rotatedhead = turn_leftandup;
			}
			else if (dx_prev == -1 && dy_next == -1) {
				// 左转向下
				rotatedhead = turn_leftanddown;
			}
			else if (dx_prev == -1 && dy_next == 1) {
				// 右转向上（←↑）
				rotatedhead = turn_rightanddown;
			}
			else if (dy_prev == 1 && dx_next == 1) {
				// 向上转右
				rotatedhead = turn_leftanddown;
			}
			else if (dy_prev == 1 && dx_next == -1) {
				// 向上转左
				rotatedhead = turn_leftandup;
			}
			else if (dy_prev == -1 && dx_next == 1) {
				// 向下转右
				rotatedhead = turn_rightanddown;

			}
			else if (dy_prev == -1 && dx_next == -1) {
				// 向下转左
				rotatedhead = turn_rightandup;

			}
			putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, rotatedhead + 0, NOTSRCERASE);
			putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, rotatedhead + 1, SRCINVERT);
		}
	}
	//绘画尾部
	dx_prev = prev(it)->x;
	dy_prev = prev(it)->y;
	tailx = it->x;
	taily = it->y;
	switch (dx_prev - tailx)
	{
	case 1:
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_down + 0, NOTSRCERASE);
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_down + 1, SRCINVERT);
		break;
	case -1:
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_up + 0, NOTSRCERASE);
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_up + 1, SRCINVERT);
		break;
	default:
		if (dy_prev - taily == 1) {
			putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_right + 0, NOTSRCERASE);
			putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_right + 1, SRCINVERT);
			break;
		}
		else {
			putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_left + 0, NOTSRCERASE);
			putimage(taily * CELL_SIZE, tailx * CELL_SIZE, tailImg_left + 1, SRCINVERT);
			break;
		}
	}
}

void gameView_showMap() {
	// 清屏
	cleardevice();
	loadimage(&img_bg, "assets/image/background.png");
	putimage(0, 0, &img_bg);
	drawSnake();
	drawMap();
}

void gameView() {
	static int last_level = 0;

	if (!((last_flag == LEVEL || last_flag == SKIN) && last_level == level)) {
		initGame(); // 初始化当前关卡数据
		last_level = level;
	}
#ifdef DEBUG
	cout << "第" << level << "关" << endl;
#endif // DEBUG

	ExMessage msg;
	int move_result;
	Direction dir = RIGHT; // 初始方向可根据需要调整
	static bool isLoaded = false, onreplay = false, onback = false, onskin = false, onlevel = false, onleft = false, onright = false, onup = false, ondown = false, ispressed = false;
	if (!isLoaded) {
		loadimage(img_replay + 0, _T("assets/image/replay_mask.jpg"), 80, 80);
		loadimage(img_replay + 1, _T("assets/image/replay.jpg"), 80, 80);
		loadimage(img_replay_big + 0, _T("assets/image/replay_mask_big.jpg"), 100, 100);
		loadimage(img_replay_big + 1, _T("assets/image/replay_big.jpg"), 100, 100);

		loadimage(img_skin + 0, _T("assets/image/skin_mask.jpg"), 80, 80);
		loadimage(img_skin + 1, _T("assets/image/skin.jpg"), 80, 80);
		loadimage(img_skin_big + 0, _T("assets/image/skin_mask_big.jpg"), 100, 100);
		loadimage(img_skin_big + 1, _T("assets/image/skin_big.jpg"), 100, 100);

		loadimage(img_levelselect + 0, _T("assets/image/levelselect_mask.jpg"), 80, 80);
		loadimage(img_levelselect + 1, _T("assets/image/levelselect.jpg"), 80, 80);
		loadimage(img_levelselect_big + 0, _T("assets/image/levelselect_mask_big.jpg"), 100, 100);
		loadimage(img_levelselect_big + 1, _T("assets/image/levelselect_big.jpg"), 100, 100);

		//方向键
		loadimage(img_up + 0, _T("assets/image/up_mask.jpg"), 80, 80);
		loadimage(img_up + 1, _T("assets/image/up.jpg"), 80, 80);
		loadimage(img_up_big + 0, _T("assets/image/up_mask_big.jpg"), 100, 100);
		loadimage(img_up_big + 1, _T("assets/image/up_big.jpg"), 100, 100);

		loadimage(img_down + 0, _T("assets/image/down_mask.jpg"), 80, 80);
		loadimage(img_down + 1, _T("assets/image/down.jpg"), 80, 80);
		loadimage(img_down_big + 0, _T("assets/image/down_mask_big.jpg"), 100, 100);
		loadimage(img_down_big + 1, _T("assets/image/down_big.jpg"), 100, 100);

		loadimage(img_left + 0, _T("assets/image/left_mask.jpg"), 80, 80);
		loadimage(img_left + 1, _T("assets/image/left.jpg"), 80, 80);
		loadimage(img_left_big + 0, _T("assets/image/left_mask_big.jpg"), 100, 100);
		loadimage(img_left_big + 1, _T("assets/image/left_big.jpg"), 100, 100);

		loadimage(img_right + 0, _T("assets/image/right_mask.jpg"), 80, 80);
		loadimage(img_right + 1, _T("assets/image/right.jpg"), 80, 80);
		loadimage(img_right_big + 0, _T("assets/image/right_mask_big.jpg"), 100, 100);
		loadimage(img_right_big + 1, _T("assets/image/right_big.jpg"), 100, 100);

		loadimage(img_back + 0, _T("assets/image/back_mask.jpg"), 80, 64);
		loadimage(img_back + 1, _T("assets/image/back.jpg"), 80, 64);
		loadimage(img_back_big + 0, _T("assets/image/back_mask_big.jpg"), 100, 80);
		loadimage(img_back_big + 1, _T("assets/image/back_big.jpg"), 100, 80);
		isLoaded = true;
	}
	sprintf_s(level_title, "第 %d 关", level);
	bool need_check = false;
	BeginBatchDraw();
	while (flag == GAME) {
		// 1.绘图
		// 蛇移动
		gameView_showMap();

		if (!onreplay) {
			putimage(50, getheight() / 2, img_replay + 0, NOTSRCERASE);
			putimage(50, getheight() / 2, img_replay + 1, SRCINVERT);
		}
		else {
			putimage(50 - 10, getheight() / 2 - 10, img_replay_big + 0, NOTSRCERASE);
			putimage(50 - 10, getheight() / 2 - 10, img_replay_big + 1, SRCINVERT);
		}
		if (!onback) {
			putimage(50, 30, img_back + 0, NOTSRCERASE);
			putimage(50, 30, img_back + 1, SRCINVERT);
		}
		else {
			putimage(50 - 10, 30 - 8, img_back_big + 0, NOTSRCERASE);
			putimage(50 - 10, 30 - 8, img_back_big + 1, SRCINVERT);
		}

		if (!onskin) {
			putimage(50, 150, img_skin + 0, NOTSRCERASE);
			putimage(50, 150, img_skin + 1, SRCINVERT);
		}
		else {
			putimage(50 - 10, 150 - 10, img_skin_big + 0, NOTSRCERASE);
			putimage(50 - 10, 150 - 10, img_skin_big + 1, SRCINVERT);
		}

		if (!onlevel) {
			putimage(50, 300, img_levelselect + 0, NOTSRCERASE);
			putimage(50, 300, img_levelselect + 1, SRCINVERT);
		}
		else {
			putimage(50 - 10, 300 - 10, img_levelselect_big + 0, NOTSRCERASE);
			putimage(50 - 10, 300 - 10, img_levelselect_big + 1, SRCINVERT);
		}
		//方向键
		if (!onup) {
			putimage(950, 500, img_up + 0, NOTSRCERASE);
			putimage(950, 500, img_up + 1, SRCINVERT);
		}
		else {
			putimage(950 - 10, 500 - 10, img_up_big + 0, NOTSRCERASE);
			putimage(950 - 10, 500 - 10, img_up_big + 1, SRCINVERT);
		}

		if (!ondown) {
			putimage(950, 650, img_down + 0, NOTSRCERASE);
			putimage(950, 650, img_down + 1, SRCINVERT);
		}
		else {
			putimage(950 - 10, 650 - 10, img_down_big + 0, NOTSRCERASE);
			putimage(950 - 10, 650 - 10, img_down_big + 1, SRCINVERT);
		}

		if (!onleft) {
			putimage(875, 575, img_left + 0, NOTSRCERASE);
			putimage(875, 575, img_left + 1, SRCINVERT);
		}
		else {
			putimage(875 - 10, 575 - 10, img_left_big + 0, NOTSRCERASE);
			putimage(875 - 10, 575 - 10, img_left_big + 1, SRCINVERT);
		}

		if (!onright) {
			putimage(1025, 575, img_right + 0, NOTSRCERASE);
			putimage(1025, 575, img_right + 1, SRCINVERT);
		}
		else {
			putimage(1025 - 10, 575 - 10, img_right_big + 0, NOTSRCERASE);
			putimage(1025 - 10, 575 - 10, img_right_big + 1, SRCINVERT);
		}

		// 打印关卡标题
		settextcolor(RGB(0x63, 0x3c, 0x2d));
		settextstyle(50, 0, "微软雅黑");
		outtextxy((getwidth() - textwidth(level_title)) / 2, 100, level_title);

		FlushBatchDraw();
		// 掉落绘图
		if (need_check) {
			int drop_result;
			while ((drop_result = checkAndDrop())) {
				if (isLose(drop_result)) {
					last_flag = GAME;
					flag = LOSE;
					return;
				}
			}
			need_check = false;
		}

		// 2. 处理玩家输入
		while (true) {
			if (peekmessage(&msg, EX_KEY | EX_MOUSE | EX_WINDOW)) {
				if (msg.message == WM_MOVE)
					break;

				// 鼠标输入(移动到按钮上/点击按钮)
				if (msg.x > 50 && msg.x < 50 + 80 && msg.y>400 && msg.y < 400 + 80) {
					if (onreplay == false) {
						onreplay = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						last_flag = GAME;
						flag = GAME;
						return;
					}
				}
				else {
					if (onreplay == true) {
						onreplay = false;
						break;
					}
				}

				if (msg.x > 50 && msg.x < 50 + 80 && msg.y>30 && msg.y < 30 + 64)
				{
					if (onback == false) {
						onback = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						last_flag = GAME;
						flag = MENU;
						return;
					}
				}
				else {
					if (onback == true) {
						onback = false;
						break;
					}
				}

				if (msg.x > 50 && msg.x < 50 + 80 && msg.y>150 && msg.y < 150 + 80)
				{
					if (onskin == false) {
						onskin = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						last_flag = GAME;
						flag = SKIN;
						return;
					}
				}
				else {
					if (onskin == true) {
						onskin = false;
						break;
					}
				}

				if (msg.x > 50 && msg.x < 50 + 80 && msg.y>300 && msg.y < 300 + 80)
				{
					if (onlevel == false) {
						onlevel = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						last_flag = GAME;
						flag = LEVEL;
						return;
					}
				}
				else {
					if (onlevel == true) {
						onlevel = false;
						break;
					}
				}

				//方向键
				if (msg.x > 950 && msg.x < 950 + 80 && msg.y>500 && msg.y < 500 + 80)
				{
					if (onup == false) {
						onup = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						dir = UP;
						ispressed = true;
					}
				}
				else {
					if (onup == true) {
						onup = false;
						break;
					}
				}

				if (msg.x > 950 && msg.x < 950 + 80 && msg.y>650 && msg.y < 650 + 80)
				{
					if (ondown == false) {
						ondown = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						dir = DOWN;
						ispressed = true;
					}
				}
				else {
					if (ondown == true) {
						ondown = false;
						break;
					}
				}

				if (msg.x > 875 && msg.x < 875 + 80 && msg.y>575 && msg.y < 575 + 80)
				{
					if (onleft == false) {
						onleft = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						dir = LEFT;
						ispressed = true;
					}
				}
				else {
					if (onleft == true) {
						onleft = false;
						break;
					}
				}

				if (msg.x > 1025 && msg.x < 1025 + 80 && msg.y>575 && msg.y < 575 + 80)
				{
					if (onright == false) {
						onright = true;
						break;
					}
					if (msg.message == WM_LBUTTONDOWN) {
						dir = RIGHT;
						ispressed = true;
					}
				}
				else {
					if (onright == true) {
						onright = false;
						break;
					}
				}

				// 键盘输入
				if (ispressed || msg.message == WM_KEYDOWN) {
					if (ispressed) {
						msg.vkcode = dir;
						ispressed = false;
					}
					switch (msg.vkcode) {
					case 'Q':  // 换肤界面
						last_flag = GAME;
						flag = SKIN;
						return;
					case 'L':  // 选关界面
						last_flag = GAME;
						flag = LEVEL;
						return;
					case 'R':  // 重新开始
						last_flag = GAME;
						flag = GAME;
						return;
					case VK_ESCAPE:  // 返回首页
						flag = MENU;
						return;

					case 'W':
						dir = UP;
						break;
					case 'S':
						dir = DOWN;
						break;
					case 'A':
						dir = LEFT;
						break;
					case 'D':
						dir = RIGHT;
						break;
					case VK_UP:
					case VK_DOWN:
					case VK_LEFT:
					case VK_RIGHT:
						dir = (Direction)msg.vkcode;
						break;
					}
					move_result = snakeMove(dir);
					switch (move_result) {
					case 0: // 正常移动,到达胜利点
						if (isWin()) {
							initGame(); // 重新初始化本关状态
							// 通关处理
							playGameSound(2);
							// 根据掩码计算通关数，更新信息
							int cleared_levels_mask = user->cleared_levels | getLevelMask(level);
							int n_cleared = get1Count(cleared_levels_mask);
#ifdef DEBUG
							cout << "通过第" << level << "关，当前通关数：" << n_cleared << endl;
#endif // DEBUG							
							level = level % N_LEVELS + 1;
							user->current_level = level;
							user->cleared_levels = cleared_levels_mask;
							updateUserFile(user);

							if (updateRank(user->username, n_cleared))
								saveRank();
							flag = WIN;
							return;
						}
						playGameSound(4);
						need_check = true;
						break;
					case 1: // 方向相反，不做处理
					case 2: // 撞墙或撞自己
						// 可以添加反馈动画或提示
						playGameSound(1);
						continue;
					case 3: // 吃到食物
						playGameSound(3);
						need_check = true;
						break;
					}
					break;
				}
			}
			else Sleep(10);
		}
	}
	EndBatchDraw();
}

void winView() {
	//加载相关资源，设置相关数据常量
	ExMessage msg = { 0 }; //当前页的msg对象
	IMAGE img_bg, img_banner[2];
	loadimage(&img_bg, "assets/image/background.png", getwidth(), getheight());
	loadimage(img_banner + 0, "assets/image/win_mask.jpg", 950, 300);
	loadimage(img_banner + 1, "assets/image/win.jpg", 950, 300);
	setbkmode(TRANSPARENT);
	static const char* s = "哎哟不错哦";
	playGameSound(5);

	constexpr int BACK_X = 400, NEXT_X = 650, BTN_Y = 600;
	constexpr int BTN_W = 150, BTN_H = 50;
	// 页面交互部分状态记录
	bool on_back = false, on_next = false; // 记录鼠标是否悬浮在按钮 重新开始,下一关 上的状态
	// check_on_back和check_on_next分别根据当前msg中的鼠标坐标，判断鼠标是否悬浮在 重新开始,下一关 按钮上
	auto check_on_back = [&]() -> bool {return msg.x > BACK_X && msg.x < BACK_X + BTN_W && msg.y>BTN_Y && msg.y < BTN_Y + BTN_H; };
	auto check_on_next = [&]() -> bool {return msg.x > NEXT_X && msg.x < NEXT_X + BTN_W && msg.y>BTN_Y && msg.y < BTN_Y + BTN_H; };

	BeginBatchDraw();
	// 循环监测(鼠标悬浮)与绘制
	while (flag == WIN) {
		cleardevice();
		putimage(0, 0, &img_bg);
		putimage((getwidth() - img_banner[0].getwidth()) / 2, 100, img_banner + 0, NOTSRCERASE);
		putimage((getwidth() - img_banner[1].getwidth()) / 2, 100, img_banner + 1, SRCINVERT);
		settextcolor(RGB(0x22, 0x22, 0x22));
		settextstyle(50, 0, "微软雅黑");
		outtextxy((getwidth() - textwidth(s)) / 2, 500, s);

		// 绘制按钮及点击逻辑判断
		button(msg, BACK_X, 600, BTN_W, BTN_H, "返回");
		button(msg, NEXT_X, 600, BTN_W, BTN_H, "下一关");
		FlushBatchDraw();

		// 监控键鼠事件，阻塞绘制
		while (true) {
			msg = { 0 };
			if (peekmessage(&msg, EX_MOUSE | EX_WINDOW | EX_KEY)) {
				// 移动窗口，推出阻塞重新绘制
				if (msg.message == WM_MOVE)
					break;

				// 判断键盘事件，符合条件退出阻塞重新绘制
				if (msg.message == WM_KEYDOWN) {
					switch (msg.vkcode)
					{
					case VK_ESCAPE:
						last_flag = WIN;
						flag = MENU;
						return;
					case VK_RETURN:
						//level = level % N_LEVELS + 1; // 不再需要在这里更新关卡，在gameView中更新关卡即可
						last_flag = WIN;
						flag = GAME;
						return;
					}
				}

				// 如果鼠标进入或离开 重新开始按钮 的区域，退出阻塞重新绘制
				if (check_on_back() != on_back) {
					on_back = !on_back;
					break;
				}
				// 如果鼠标进入或离开 下一关按钮 的区域，退出阻塞重新绘制
				if (check_on_next() != on_next) {
					on_next = !on_next;
					break;
				}
				// 跳转判断
				if (on_back && msg.message == WM_LBUTTONDOWN) {
					last_flag = WIN;
					flag = MENU;
					return;
				}
				if (on_next && msg.message == WM_LBUTTONDOWN) {
					//level = level % N_LEVELS + 1; 不再需要，在gameView中更新关卡即可
					last_flag = WIN;
					flag = GAME;
					return;
				}
			}
		}
	}
	EndBatchDraw();
}

void loseView() {
	playGameSound(6);
	ExMessage msg = { 0 };
	IMAGE img_bg, img_banner[2];
	char s[] = "多吃苹果少喝奶茶！";
	loadimage(&img_bg, "assets/image/background.png", getwidth(), getheight());
	loadimage(img_banner + 0, "assets/image/lose_mask.jpg", 950, 300);
	loadimage(img_banner + 1, "assets/image/lose.jpg", 950, 300);
	setbkmode(TRANSPARENT);

	constexpr int REPLAY_X = 650, GOBACK_X = 400, BTN_Y = 600;
	constexpr int BTN_W = 150, BTN_H = 50;
	bool on_back = false, on_replay = false; // 记录鼠标是否悬浮在按钮 重新开始,下一关 上的状态
	// check_on_back和check_on_replay分别根据当前msg中的鼠标坐标，判断鼠标是否悬浮在 重新开始,下一关 按钮上
	auto check_on_back = [&]() -> bool {return msg.x > GOBACK_X && msg.x < GOBACK_X + BTN_W && msg.y>BTN_Y && msg.y < BTN_Y + BTN_H; };
	auto check_on_replay = [&]() -> bool {return msg.x > REPLAY_X && msg.x < REPLAY_X + BTN_W && msg.y>BTN_Y && msg.y < BTN_Y + BTN_H; };

	BeginBatchDraw();
	while (true) {
		cleardevice();
		putimage(0, 0, &img_bg);
		putimage((getwidth() - img_banner[0].getwidth()) / 2, 100, img_banner + 0, NOTSRCERASE);
		putimage((getwidth() - img_banner[1].getwidth()) / 2, 100, img_banner + 1, SRCINVERT);
		settextcolor(RGB(0x22, 0x22, 0x22));
		settextstyle(50, 0, "微软雅黑");
		outtextxy((getwidth() - textwidth(s)) / 2, 500, s);
		peekmessage(&msg, EX_MOUSE);

		button(msg, GOBACK_X, 600, BTN_W, BTN_H, "返回");
		button(msg, REPLAY_X, 600, BTN_W, BTN_H, "重新开始");
		FlushBatchDraw();

		// 监控键鼠事件，阻塞绘制
		while (true) {
			msg = { 0 };
			if (peekmessage(&msg, EX_MOUSE | EX_WINDOW | EX_KEY)) {
				if (msg.message == WM_MOVE)
					break;

				if (msg.message == WM_KEYDOWN) {
					switch (msg.vkcode)
					{
					case VK_ESCAPE:
						last_flag = LOSE;
						flag = MENU;
						return;
					case 'R':
					case 'r':
					case VK_RETURN:
						last_flag = LOSE;
						flag = GAME;
						return;
					}
				}
				// 如果鼠标进入或离开 重新开始按钮 的区域，退出阻塞重新绘制
				if (check_on_back() != on_back) {
					on_back = !on_back;
					break;
				}
				// 如果鼠标进入或离开 下一关按钮 的区域，退出阻塞重新绘制
				if (check_on_replay() != on_replay) {
					on_replay = !on_replay;
					break;
				}
				// 跳转判断
				if (on_back && msg.message == WM_LBUTTONDOWN) {
					last_flag = LOSE;
					flag = MENU;
					return;
				}
				if (on_replay && msg.message == WM_LBUTTONDOWN) {
					last_flag = LOSE;
					flag = GAME;
					return;
				}
			}
		}
	}
	EndBatchDraw();
}

void skinSelectView()
{
	//static string[N_SKINS] = {
	//	
	//}
	// 是否运行
	bool running = true;

	// 加载图片资源
	IMAGE title;
	loadimage(&title, _T("assets/image/skin_text.png"));

	std::vector<IMAGE> skin_images(N_SKINS);
	std::vector<IMAGE> skin_masks(N_SKINS);
	char path[80] = { 0 };
	for (int i = 0; i < N_SKINS; ++i)
	{
		sprintf_s(path, "assets/skins/%d/snakehead_right%d.jpg", i + 1, i + 1);
		loadimage(&skin_images[i], path);
		sprintf_s(path, "assets/skins/%d/snakehead_mask_right%d.jpg", i + 1, i + 1);
		loadimage(&skin_masks[i], path);
	}

	// 大小和位置信息
	const int skin_w = 180, skin_h = 180;
	const int spacing = 120;
	const int base_x = 210;
	const int base_y = 280;

	int start_index = 0;      // 页面显示的第一个皮肤的下标
	int selected_skin = 0;    // 选中的皮肤

	// 控制左移和右移按钮每次点击仅运行一次
	// 按钮上一帧的信息
	bool rightButtonClickedLastFrame = false;
	bool leftButtonClickedLastFrame = false;

	// 用户消息
	ExMessage msg;

	// 帧率，控制再60帧
	const int FPS = 60;;

	setbkmode(TRANSPARENT);
	settextcolor(BLACK);

	BeginBatchDraw();
	while (running)
	{
		// 当前帧开始的时间
		DWORD frame_start_time = GetTickCount();

		// 上一帧（第一帧）的时间
		static DWORD last_tick_time = GetTickCount();
		// 当前帧的时间
		DWORD current_tick_time = GetTickCount();
		// 间隔时间
		DWORD delta_tick = current_tick_time - last_tick_time;

		// 更新上一帧的时间
		last_tick_time = current_tick_time;

		// 绘制背景
		setbkcolor(0xADD8E6);
		cleardevice();

		if (button(msg, 650, 640, 300, 90, "确认", BLACK, 30, "微软雅黑", WHITE, RGB(0, 120, 215)))
		{
			if (changeSkin(selected_skin))
				MessageBox(GetHWnd(), "更换皮肤成功", "提示", MB_OK);
		}

		bool leftButtonClickedNow = button(msg, 100, 340, 60, 60, "<-", BLACK, 30, "微软雅黑", WHITE, RGB(0, 120, 215));

		if (leftButtonClickedNow && !leftButtonClickedLastFrame)
		{
			if (start_index > 0) start_index--;
		}
		leftButtonClickedLastFrame = leftButtonClickedNow;

		bool rightButtonClickedNow = button(msg, 1040, 340, 60, 60, "->", BLACK, 30, "微软雅黑", WHITE, RGB(0, 120, 215));

		if (rightButtonClickedNow && !rightButtonClickedLastFrame)
		{
			if (start_index + 3 < N_SKINS)
				start_index++;
		}
		rightButtonClickedLastFrame = rightButtonClickedNow;

		if (button(msg, 250, 640, 300, 90, "返回", BLACK, 30, "微软雅黑", WHITE, RGB(0, 120, 215)))
		{
			last_flag = SKIN;
			flag = GAME;
			running = false;
		}

		// 绘制控件和图片
		wzy_putimage_alpha(450, 60, &title);

		for (int i = 0; i < 3; ++i)
		{
			int skin_idx = start_index + i;
			if (skin_idx >= N_SKINS) break;
			int x = base_x + i * (skin_w + spacing);

			if (button(msg, x, base_y, skin_w, skin_h, "", BLACK, 30, "微软雅黑", WHITE, RGB(0, 120, 215)))
			{
				selected_skin = start_index + i;
			}

			// 绘制皮肤大图
			putimage(x + 54, base_y + 54, &skin_masks[skin_idx], NOTSRCERASE);
			putimage(x + 54, base_y + 54, &skin_images[skin_idx], SRCINVERT);
			//wzy_putimage_alpha(x + 54, base_y + 54, &skin_images[skin_idx]);
			//wzy_putimage_alpha(x + 54, base_y + 54, &skin_masks[skin_idx]);

			std::string indexText = "皮肤" + std::to_string(skin_idx + 1);
			if (skin_idx == 1)
				indexText = "皮肤2.5";
			int textW = textwidth(indexText.c_str());
			int textH = textheight(indexText.c_str());
			outtextxy(x + (skin_w - textW) / 2, base_y + skin_h + 8, indexText.c_str());

			if (skin_idx == selected_skin)
			{
				setlinecolor(RED);
				setlinestyle(PS_SOLID, 5);
				rectangle(x - 5, base_y - 5, x + skin_w + 5, base_y + skin_h + 5);
			}
		}

		while (peekmessage(&msg))
		{

		}

		FlushBatchDraw();

		// 计算当前使用了多少时间，控制帧率
		DWORD frame_end_time = GetTickCount();
		DWORD frame_delta_time = frame_end_time - frame_start_time;
		if (frame_delta_time < 1000 / FPS)
			Sleep(1000 / FPS - frame_delta_time);
	}

	cleardevice();
	EndBatchDraw();
}

void levelSelectView() {
	// 改了我3个小时的界面
	IMAGE img_bg;
	IMAGE img_esc;
	static IMAGE imglevel[10];
	static IMAGE img_lock[2];
	int arri = level - 1;
	int arrpoint[10][2] = { {180,368} ,{390,368} ,{598,368},{805, 368},{1012, 368},{183, 616},{389, 613},{597, 613},{805, 614},{1014, 614} };
	int bx = arrpoint[arri][0];
	int by = arrpoint[arri][1];

	ExMessage msg = { 0 };
	
	// 交互区定义和资源加载
	constexpr int BTN_W = 150, BTN_H = 178, BTN_X_SPAN = 58, BTN_Y_SPAN = 68;
	constexpr int BTN_X_START = 105, BTN_Y_START = 160;
	constexpr int BTN_W_LOCK = 180, BTN_H_LOCK = 220;
	constexpr int N_LEVELS_ONPAGE = min(10, N_LEVELS), N_BTNS = N_LEVELS_ONPAGE + 1;
	for (int i = 0; i < 10; i++)
	{
		std::string path = "assets/image/level" + to_string(i + 1) + ".png";
		loadimage(&imglevel[i], path.c_str(), 150, 178);
	}
	loadimage(&img_esc, "assets/image/ESC.png", 83, 97);
	loadimage(&img_bg, "assets/image/level.png", 1200, 800);
	loadimage(&img_lock[0], "assets/image/level_lock_mask.jpg", BTN_W_LOCK, BTN_H_LOCK);
	loadimage(&img_lock[1], "assets/image/level_lock.jpg", BTN_W_LOCK, BTN_H_LOCK);
	// 交互区检测函数
	bool states_onhover[N_BTNS] = { false };
	bool is_downing = false;
	int hoverIdx = -1;
	static auto getBtnPosXY = [&](int i)->Position {
		if (i == 0)
			return { 1108, 14 };
		int row = (i - 1) / 5, col = (i - 1) % 5;
		int x = BTN_X_START + col * (BTN_W + BTN_X_SPAN),
			y = BTN_Y_START + row * (BTN_H + BTN_Y_SPAN);
		return { x,y };	
	};
	static auto checkOnHover = [&](int i)->bool {
		if (i == 0)
			return msg.x > 1108 && msg.y > 14 && msg.x < 1191 && msg.y < 111;
		Position posI = getBtnPosXY(i);
		return msg.x > posI.x && msg.y > posI.y && msg.x < posI.x + BTN_W && msg.y < posI.y + BTN_H;
	};
	bool states_open[N_LEVELS + 1] = { false };
	// 计算锁关
	int cleared_levels_mask = user->cleared_levels, i = 1;
	bool pre_cleared = false;
	while (cleared_levels_mask) {
		bool cleared = cleared_levels_mask & 1;
		states_open[i] = pre_cleared || cleared;
		cleared_levels_mask >>= 1;
#ifdef DEBUG
		cout << "第" << i << "关: " << states_open[i] << endl;
#endif // DEBUG
		i++;
		pre_cleared = cleared;
	}
	if (pre_cleared && i <= N_LEVELS)
		states_open[i] = true;
	states_open[1] = true; //第一关总是开放的


	BeginBatchDraw();
	while (flag == LEVEL)
	{
		// 绘图部分
		cleardevice();
		putimage(0, 0, &img_bg);
		setfillcolor(BLACK);
		fillcircle(arrpoint[arri][0], arrpoint[arri][1], 20);
		// 悬浮图片
		for (int i = 0; i < N_BTNS; i++) {
			if (states_onhover[i]) {
				Position pos = getBtnPosXY(i);
				putimage(pos.x, pos.y, i ? &imglevel[i-1] : &img_esc);
			}
		}
		if (hoverIdx >= 0) {
			int i = hoverIdx;
			Position pos = getBtnPosXY(i);
			putimage(pos.x, pos.y, i ? &imglevel[i - 1] : &img_esc);
		}
		// 关卡锁
		for (int i = 1; i <= N_LEVELS_ONPAGE; i++) {
			if (!states_open[i]) {
				Position pos = getBtnPosXY(i);
				putimage(pos.x - 5, pos.y - 33, &img_lock[0], NOTSRCERASE);
				putimage(pos.x - 5, pos.y - 33, &img_lock[1], SRCINVERT);
			}
		}

		FlushBatchDraw();

		// 阻塞用户交互
		while (true) {
			if (peekmessage(&msg, EX_KEY | EX_MOUSE | EX_WINDOW))
			{
				if (msg.message == WM_MOVE)
					break;
				//键盘事件判定
				if (msg.message == WM_KEYDOWN)
				{
					if (is_downing)
						break;
					is_downing = true;
					//按键判定
					switch (msg.vkcode)
					{
					case VK_ESCAPE:
						flag = last_flag;
						last_flag = LEVEL;
						break;
						//指示图标移动
					case 'w':
					case 'W':
					case UP:
						arri = (arri + 5) % 10;
						break;
					case 's':
					case 'S':
					case DOWN:
						arri = (arri + 5) % 10;
						break;
					case 'a':
					case 'A':
					case LEFT:
						if (arri > 0 && arri <= 9)
						{
							arri--;
						}
						else if (arri == 0)
						{
							arri = 9;
						}
						break;
					case 'd':
					case 'D':
					case RIGHT:
						if (arri >= 0 && arri < 9)
						{
							arri++;
						}
						else if (arri == 9)
						{
							arri = 0;
						}
						break;
					case VK_RETURN:
						if (!states_open[arri + 1]) {
							MessageBox(GetHWnd(), "关卡已上锁，请用鼠标撬锁！", "提示", MB_OK);
							break;
						}
						level = arri + 1;
						flag = GAME;
						return;
					}
					if(hoverIdx >= 0)
						states_onhover[hoverIdx] = false;
					states_onhover[arri + 1] = true;
					break;
				}
				if (msg.message == WM_KEYUP) {
					states_onhover[arri + 1] = false;
					is_downing = false;
					break;
				}
				//鼠标事件判定
				bool need_redraw = false;
				for (int i = 0; i < N_BTNS; i++) {
					bool hover_state = checkOnHover(i);
					if (states_onhover[i] != hover_state) {
						states_onhover[i] = hover_state;
						need_redraw = true;
					}
					if (hover_state) {
						hoverIdx = i;
						if (msg.message == WM_LBUTTONDOWN) { //鼠标点击
							if (i == 0) { // 返回
								flag = last_flag;
								last_flag = LEVEL;
								return;
							}
							// 根据锁关状态，进入关卡
							if (!states_open[i]) {
								int ret = MessageBox(GetHWnd(), "是否撬锁？", "关卡未解锁", MB_YESNO | MB_ICONQUESTION);
								if (ret == IDYES) { //用户点击“是”
									//from_lock = true;
								}
								else {
									continue;

								}
							}
							level = i;
							flag = GAME;
							last_flag = LEVEL;
							return;
						}
					}
					
				}
				if (need_redraw)
					break;
			}
		}
	}
	EndBatchDraw();
}

void musicView() {
	constexpr int X_OFF = 670, X_ON = 730;
	//在此处完成代码
	IMAGE img_bg, img_back;
	loadimage(&img_bg, "assets/image/background.png");
	//loadimage(&img_back, "assets/image/back.jpg");
	ExMessage msg = { 0 };

	constexpr int BTN_BACK_X = 50, BTN_BACK_Y = 50, BTN_BACK_W = 100, BTN_BACK_H = 50;
	bool on_back = false;
	auto checkOnBack = [&]()->bool {return msg.x > BTN_BACK_X && msg.x < BTN_BACK_X + BTN_BACK_W && msg.y>BTN_BACK_Y && msg.y < BTN_BACK_Y + BTN_BACK_H; };

	BeginBatchDraw();
	while (true) {
		cleardevice();
		putimage(0, 0, &img_bg);
		//putimage(10, 10, &img_back);
		button(msg, BTN_BACK_X, BTN_BACK_Y, BTN_BACK_W, BTN_BACK_H, "返回", BLACK, 0, "微软雅黑", RGB(191, 229, 240), BLACK, BLUE, 5);
		//绘制音乐设置按钮
		setlinecolor(RED);
		setfillcolor(RED);
		roundrect(650, 320, 650 + 100, 320 + 30, 40, 20);
		roundrect(650, 450, 650 + 100, 450 + 30, 40, 20);
		//绘制文字
		settextstyle(40, 0, "微软雅黑");
		settextcolor(RED);
		setbkmode(TRANSPARENT);
		outtextxy(450, 315, "音乐");
		outtextxy(450, 445, "音效");
		fillcircle(music_play ? X_ON : X_OFF, 335, 10);
		fillcircle(sound_play ? X_ON : X_OFF, 465, 10);

		FlushBatchDraw();

		//交互判断
		while (true) {
			if (peekmessage(&msg, EX_MOUSE | EX_WINDOW | EX_KEY)) {
				if (msg.message == WM_MOVE)
					break;

				//返回 
				if (msg.message == WM_KEYDOWN)
				{
					switch (msg.vkcode)
					{
					case VK_ESCAPE:
						flag = MENU;
						return;
					}
				}

				bool bf = false;
				if (on_back != checkOnBack()) {
					on_back = !on_back;
					bf = true;
				}
				if (on_back && msg.message == WM_LBUTTONDOWN) {
					flag = MENU;
					return;
				}
				if (bf)
					break;

				//音乐
				if ((msg.x >= 650 && msg.x <= 650 + 100 && msg.y >= 320 && msg.y <= 320 + 30) && msg.message == WM_LBUTTONDOWN)
				{
					setMusic(!music_play);
					if (music_play)
						playGameMusic();
					else
						stopGameMusic();
					break;
				}
				//音效
				if ((msg.x >= 650 && msg.x <= 650 + 100 && msg.y >= 450 && msg.y <= 450 + 30) && msg.message == WM_LBUTTONDOWN) {
					setSound(!sound_play);
					if (sound_play)
						playGameSound(4);
					break;
				}

			} 
			else 
				Sleep(10);
		}
	}
	EndBatchDraw();
}

void teamView() {
	ExMessage msg = { 0 };
	IMAGE img_bk;
	loadimage(&img_bk, "./assets/image/background.png", 1200, 800);

	constexpr int BTN_BACK_X = 50, BTN_BACK_Y = 50, BTN_BACK_W = 100, BTN_BACK_H = 50;
	bool on_back = false;
	auto checkOnBack = [&]()->bool {return msg.x > BTN_BACK_X && msg.x < BTN_BACK_X + BTN_BACK_W && msg.y>BTN_BACK_Y && msg.y < BTN_BACK_Y + BTN_BACK_H; };
	
	BeginBatchDraw();
	while (flag == TEAM)
	{
		cleardevice();
		putimage(0, 0, &img_bk);
		settextcolor(BLACK);
		settextstyle(72, 0, "微软雅黑");
		outtextxy(330, 50, "制作组人员介绍");
		settextstyle(60, 0, "微软雅黑");
		outtextxy(400, 182, "组长：麦兜");
		outtextxy(400, 262, "副组长/技术官：安静");
		outtextxy(400, 342, "技术官：Jay");
		outtextxy(400, 422, "监督官：novice");
		outtextxy(400, 502, "产品经理：Best 进行时");
		outtextxy(400, 582, "信息官：wzy");
		outtextxy(400, 662, "信息官：默语");
		setlinecolor(RGB(191, 229, 240));
		button(msg, BTN_BACK_X, BTN_BACK_Y, BTN_BACK_W, BTN_BACK_H, "返回", BLACK, 0, "微软雅黑", RGB(191, 229, 240), BLACK, BLUE, 5);
		FlushBatchDraw();

		while (true) {
			if (peekmessage(&msg, EX_MOUSE | EX_KEY | EX_WINDOW)) {
				if (msg.message == WM_MOVE)
					break;
				
				if (msg.message == WM_KEYDOWN)
				{
					switch (msg.vkcode)
					{
					case VK_ESCAPE:
						flag = MENU;
						return;
					}
				}

				bool bf = false;
				if (on_back != checkOnBack()) {
					on_back = !on_back;
					bf = true;
				}
				if (on_back && msg.message == WM_LBUTTONDOWN) {
					flag = MENU;
					return;
				}
				if (bf)
					break;
			}
			else Sleep(10);
		}
		
		//Sleep(10);
	}
	EndBatchDraw();
}

void describeView() {
	ExMessage msg = { 0 };
	IMAGE img_bk;
	loadimage(&img_bk, "./assets/image/background.png", 1200, 800);

	constexpr int BTN_BACK_X = 50, BTN_BACK_Y = 50, BTN_BACK_W = 100, BTN_BACK_H = 50;
	bool on_back = false;
	auto checkOnBack = [&]()->bool {return msg.x > BTN_BACK_X && msg.x < BTN_BACK_X + BTN_BACK_W && msg.y>BTN_BACK_Y && msg.y < BTN_BACK_Y + BTN_BACK_H; };

	BeginBatchDraw();
	while (flag == DESCRIBE)
	{
		cleardevice();
		putimage(0, 0, &img_bk);
		settextcolor(BLACK);
		settextstyle(90, 0, "微软雅黑");
		outtextxy(450, 50, "玩法说明！");
		settextstyle(50, 0, "微软雅黑");
		outtextxy(100, 200, "1.按方向键/WSAD或者点击图标移动蛇体。");
		outtextxy(100, 280, "2.移动苹果蛇到达终点获得胜利。");
		outtextxy(100, 360, "3.吃到苹果后蛇长加一。");
		outtextxy(100, 440, "4.掉落地图则游戏失败。");
		outtextxy(100, 520, "5.ESC返回菜单，回车进入下一关，R键重新开始.");
		outtextxy(100, 600, "6.请注意自身蛇长和中空距离或者被障碍卡位。");
		outtextxy(100, 680, "7.通过障碍，到达终点，赢取游戏胜利。");
		button(msg, 50, 50, 100, 50, "返回", BLACK, 0, "微软雅黑", RGB(191, 229, 240), RGB(191, 229, 240), BLUE, 5);
		FlushBatchDraw();

		while (true) {
			if (peekmessage(&msg, EX_MOUSE | EX_KEY | EX_WINDOW)) {
				if (msg.message == WM_MOVE)
					break;

				if (msg.message == WM_KEYDOWN)
				{
					switch (msg.vkcode)
					{
					case VK_ESCAPE:
						flag = MENU;
						return;
					}
				}

				bool bf = false;
				if (on_back != checkOnBack()) {
					on_back = !on_back;
					bf = true;
				}
				if (on_back && msg.message == WM_LBUTTONDOWN) {
					flag = MENU;
					return;
				}
				if (bf)
					break;
			}
			else Sleep(10);
		}
	}
	EndBatchDraw();
}

void rankView() {
	// 加载并绘制背景图片
	IMAGE img_bg, img_btn;
	constexpr int BTN_X = 10, BTN_Y = 30, BTN_W = 100, BTN_H = 80;
	loadimage(&img_bg, "assets/image/background.png"); // 或者其他格式
	//loadimage(&img_btn, "assets/image/back.jpg", BTN_W, BTN_H); // 或者其他格式
	ExMessage msg = { 0 };

	constexpr int BTN_BACK_X = 50, BTN_BACK_Y = 50, BTN_BACK_W = 100, BTN_BACK_H = 50;
	constexpr int NAME_X = 350, SCORE_X = 800;
	constexpr int FONT_SIZE = 50;
	const char* s = "排行榜";
	bool on_back = false;
	auto checkOnBack = [&]()->bool {return msg.x > BTN_BACK_X && msg.x < BTN_BACK_X + BTN_BACK_W && msg.y>BTN_BACK_Y && msg.y < BTN_BACK_Y + BTN_BACK_H; };

	BeginBatchDraw();
	while (flag == RANK) {
		cleardevice();
		putimage(0, 0, &img_bg);
		//putimage(BTN_X, BTN_Y, &img_btn);
		button(msg, BTN_BACK_X, BTN_BACK_Y, BTN_BACK_W, BTN_BACK_H, "返回", BLACK, 0, "微软雅黑", RGB(191, 229, 240), BLACK, BLUE, 5);
		// 排行榜打印
		setbkmode(TRANSPARENT);
		settextcolor(YELLOW);
		settextstyle(72, 0, "微软雅黑");
		outtextxy((getwidth() - textwidth(s)) / 2, 50, s);
		settextcolor(RGB(0x22, 0x22, 0x22));
		settextstyle(FONT_SIZE, 0, "微软雅黑");
		int ty = 180;
		outtextxy(NAME_X, ty, "昵称");
		outtextxy(SCORE_X, ty, "通关数");
		ty += FONT_SIZE * 1.2;
		for (int i = 0; i < rank_list.size(); i++) {
			char name[50] = { 0 };
			sprintf_s(name, rank_list[i].username.c_str());
			outtextxy(NAME_X, ty, _T(name));
			char score[10] = { 0 };
			sprintf_s(score, "%d", rank_list[i].score);
			outtextxy(SCORE_X, ty, score);
			ty += FONT_SIZE * 1.1;
		}
		FlushBatchDraw();

		while (true) {
			if (peekmessage(&msg, EX_KEY | EX_WINDOW | EX_MOUSE)) {
				if (msg.message == WM_MOVE)
					break;
				//按ESC键返回
				if (msg.message == WM_KEYDOWN)
				{
					switch (msg.vkcode)
					{
					case VK_ESCAPE:
						flag = MENU;
						return;
					}
				}

				bool bf = false;
				if (on_back != checkOnBack()) {
					on_back = !on_back;
					bf = true;
				}
				if (on_back && msg.message == WM_LBUTTONDOWN) {
					flag = MENU;
					return;
				}
				if (bf)
					break;
			}
			else Sleep(10);
		}

	}
	EndBatchDraw();
}

bool button(ExMessage& msg, int x, int y, int w, int h,
	const char* text)
{
	return button(msg, x, y, w, h, text, BLACK, 0, "微软雅黑", WHITE, BLACK, BLUE, 5);
}
bool button(ExMessage& msg, int x, int y, int w, int h,
	const char* text, COLORREF ft_cl, int ft_sz, const char* ft_fm,
	COLORREF fill_color, COLORREF br_color, COLORREF hover_color, int br, int bd, int b_style
) {
	if (msg.x > x && msg.y > y && msg.x < x + w && msg.y < y + h)
		setfillcolor(hover_color);
	else
		setfillcolor(fill_color);
	//绘制按钮
	setlinecolor(br_color);
	setlinestyle(b_style, bd);
	fillroundrect(x, y, x + w, y + h, br, br);
	//绘制按钮文本
	setbkmode(TRANSPARENT);
	settextstyle(ft_sz ? ft_sz : h * 0.85, 0, ft_fm);
	settextcolor(ft_cl);
	int hSpace = (w - textwidth(text)) / 2;
	int vSpace = (h - textheight(text)) / 2;
	outtextxy(x + hSpace, y + vSpace, text);

	if (msg.message == WM_LBUTTONDOWN && msg.x > x && msg.y > y && msg.x < x + w && msg.y < y + h)
		return true;
	return false;
}


// --------------------------自定义区----------------------------

// -------------------------1.0----------------------------
void wzy_drawTextBox(wzy_TextBox& textbox, bool is_password)
{
	setlinecolor(LIGHTGRAY);		// 设置画线颜色
	setbkcolor(0xeeeeee);			// 设置背景颜色
	setfillcolor(0xeeeeee);			// 设置填充颜色
	// 输入框
	fillrectangle(textbox.pos_x, textbox.pos_y, textbox.pos_x + textbox.size_x, textbox.pos_y + textbox.size_y);

	// 字体大小
	int font_height = textbox.size_y - 10;
	if (font_height < 10) font_height = 10;
	settextstyle(font_height, 0, "黑体");

	// 绘制输入框当前内容
	settextcolor(BLACK);
	settextcolor(BLACK);

	// 更改位置
	std::string output_text = textbox.text;
	if (is_password)
	{
		output_text = std::string(output_text.size(), '*');
	}
	// end

	outtextxy(textbox.pos_x + 10, textbox.pos_y + 5, output_text.c_str());

	// 根据条件绘制光标
	if (textbox.isFocused && textbox.showCursor)
	{
		// 防止 cursor_pos 越界
		if (textbox.cursor_pos > textbox.text.length()) {
			textbox.cursor_pos = textbox.text.length();
		}
		int text_width = textwidth(textbox.text.substr(0, textbox.cursor_pos).c_str());

		//int text_width = textwidth(textbox.text.substr(0, textbox.cursor_pos).c_str());
		int cursor_x = textbox.pos_x + 10 + text_width;
		int cursor_y1 = textbox.pos_y + 5;
		int cursor_y2 = textbox.pos_y + textbox.size_y - 5;
		setlinecolor(BLACK);
		line(cursor_x, cursor_y1, cursor_x, cursor_y2);
	}
}

#pragma comment(lib, "imm32.lib")
void wzy_inputTextBox(wzy_TextBox& textbox, const ExMessage& msg)
{
	switch (msg.message)
	{
	case WM_LBUTTONDOWN:
		// 判断是否点击在输入框内
		if (msg.x >= textbox.pos_x && msg.x <= textbox.pos_x + textbox.size_x &&
			msg.y >= textbox.pos_y && msg.y <= textbox.pos_y + textbox.size_y)
		{
			// 更改位置
			// 禁用输入法
			HWND hwnd = GetHWnd();
			ImmAssociateContext(hwnd, NULL);
			// end

			textbox.isFocused = true;
		}
		else
		{
			textbox.isFocused = false;
		}
		break;

	case WM_KEYDOWN:
		if (!textbox.isFocused) break;

		switch (msg.vkcode)
		{
		case VK_LEFT:
			if (textbox.cursor_pos > 0)
				textbox.cursor_pos--;
			break;
		case VK_RIGHT:
			if (textbox.cursor_pos < textbox.text.length())
				textbox.cursor_pos++;
			break;
		}
		break;
		// ✅ 光标安全限制
		if (textbox.cursor_pos > textbox.text.length()) {
			textbox.cursor_pos = textbox.text.length();
		}
		break;

	case WM_CHAR:
		if (!textbox.isFocused) break;

		switch (msg.ch)
		{
		case 8: // 退格键，删去一个字符
			if (textbox.cursor_pos > 0)
			{
				textbox.text.erase(textbox.cursor_pos - 1, 1);
				textbox.cursor_pos--;
			}
			break;

		case 13: // 回车键，结束
			textbox.isFocused = false;
			break;

		default:
			if (iswprint(msg.ch) &&
				textbox.text.length() < textbox.maxlen)
			{
				textbox.text.insert(textbox.cursor_pos, 1, msg.ch);
				textbox.cursor_pos++;
			}
			break;
		}
		break;
		// ✅ 光标安全限制
		if (textbox.cursor_pos > textbox.text.length()) {
			textbox.cursor_pos = textbox.text.length();
		}
		break;
	}

}

void wzy_updateTextBox(wzy_TextBox& textbox, int delta)
{
	textbox.time += delta;
	// 当经过的时间超过设定时间，减去设定时间，并更新光标状态
	if (textbox.time >= textbox.TIME)
	{
		textbox.time -= textbox.TIME;
		textbox.showCursor = !textbox.showCursor;
	}
}

inline void wzy_putimage_alpha(int dst_x, int dst_y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), dst_x, dst_y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}
// -------------------------1.0----------------------------


// -------------------------3.0----------------------------
//Jay
void smoothMove(Direction dir, int x, int y) {
	//FPS 60f
	const clock_t FPS = 60, F_TIME = 1000 / FPS;    //1秒60帧 = 16.6 每一帧应该花费的时间
	int startTime = 0;
	int frameTime = 0;  //当前帧执行时间
	int headx = snake.front().x * CELL_SIZE;
	int heady = snake.front().y * CELL_SIZE;
	float dx = 0.0, dy = 0.0, sum = 0.0;
	int	dx_prev = 0, dy_prev = 0, dx_next = 0, dy_next = 0, tailx = 0, taily = 0;
	IMAGE* rotatedhead = headImg, * rotatedtail = headImg;
	sprintf_s(level_title, "第 %d 关", level);

	switch (dir)//蛇头旋转
	{
	case UP:
		dx = -1; dy = 0;
		rotatedhead = headImg_up;
		break;
	case DOWN:
		dx = 1; dy = 0;
		rotatedhead = headImg_down;
		break;
	case LEFT:
		dx = 0; dy = -1;
		rotatedhead = headImg_left;
		break;
	case RIGHT:
		dx = 0; dy = 1;
		break;
	}

	//蛇尾旋转
	dx_prev = prev(prev(snake.end()))->x;
	dy_prev = prev(prev(snake.end()))->y;
	tailx = prev(snake.end())->x;
	taily = prev(snake.end())->y;

	switch (dx_prev - tailx)
	{
	case 1:
		rotatedtail = tailImg_down;
		break;
	case -1:
		rotatedtail = tailImg_up;
		break;
	default:
		if (dy_prev - taily == 1) {
			rotatedtail = tailImg_right;
			break;
		}
		else {
			rotatedtail = tailImg_left;
			break;
		}
	}

	IMAGE* rotated = turn_rightandup;
	int istrun = false;
	int ddx_prev = snake.front().x - x;
	int ddy_prev = snake.front().y - y;
	int ddx_next = next(snake.begin())->x - snake.front().x;
	int ddy_next = next(snake.begin())->y - snake.front().y;

	// 计算移动步长（60帧下每帧移动1/60距离）
	dx *= 4.0f;
	dy *= 4.0f;
	//双缓冲防止闪屏
	BeginBatchDraw();
	int max_frame = FPS * 2; //最大帧保护
	while (max_frame--) {
		startTime = clock();
		cleardevice();
		putimage(0, 0, &img_bg);
		putimage(50, getheight() / 2, img_replay + 0, NOTSRCERASE);
		putimage(50, getheight() / 2, img_replay + 1, SRCINVERT);
		putimage(50, 30, img_back + 0, NOTSRCERASE);
		putimage(50, 30, img_back + 1, SRCINVERT);
		putimage(50, 150, img_skin + 0, NOTSRCERASE);
		putimage(50, 150, img_skin + 1, SRCINVERT);
		putimage(50, 300, img_levelselect + 0, NOTSRCERASE);
		putimage(50, 300, img_levelselect + 1, SRCINVERT);
		putimage(950, 500, img_up + 0, NOTSRCERASE);
		putimage(950, 500, img_up + 1, SRCINVERT);
		putimage(950, 650, img_down + 0, NOTSRCERASE);
		putimage(950, 650, img_down + 1, SRCINVERT);
		putimage(875, 575, img_left + 0, NOTSRCERASE);
		putimage(875, 575, img_left + 1, SRCINVERT);
		putimage(1025, 575, img_right + 0, NOTSRCERASE);
		putimage(1025, 575, img_right + 1, SRCINVERT);

		// 头尾节点移动逻辑

		// 更新坐标
		headx += static_cast<int>(dx);
		heady += static_cast<int>(dy);
		sum += 4.0;

		// 绘制图片
		IMAGE* rotated = turn_rightandup;
		bool isrotated = false;
		// 转弯，根据 dx_prev/dy_prev 和 dx_next/dy_next 选择转弯图片
		if (ddx_prev == 1 && ddy_next == 1) {
			rotated = turn_rightandup; isrotated = true;
		}
		else if (ddx_prev == 1 && ddy_next == -1) {
			// 左转向下
			rotated = turn_leftandup; isrotated = true;
		}
		else if (ddx_prev == -1 && ddy_next == -1) {
			// 左转向下
			rotated = turn_leftanddown; isrotated = true;
		}
		else if (ddx_prev == -1 && ddy_next == 1) {
			// 右转向上（←↑）
			rotated = turn_rightanddown; isrotated = true;
		}
		else if (ddy_prev == 1 && ddx_next == 1) {
			// 向上转右
			rotated = turn_leftanddown; isrotated = true;
		}
		else if (ddy_prev == 1 && ddx_next == -1) {
			// 向上转左
			rotated = turn_leftandup; isrotated = true;
		}
		else if (ddy_prev == -1 && ddx_next == 1) {
			// 向下转右
			rotated = turn_rightanddown; isrotated = true;
		}
		else if (ddy_prev == -1 && ddx_next == -1) {
			// 向下转左
			rotated = turn_rightandup; isrotated = true;
		}
		if (isrotated) {
			putimage(snake.front().y * CELL_SIZE, snake.front().x * CELL_SIZE, rotated + 0, NOTSRCERASE);
			putimage(snake.front().y * CELL_SIZE, snake.front().x * CELL_SIZE, rotated + 1, SRCINVERT);
		}
		else {// 直身
			if (ddx_prev) {
				putimage(snake.front().y * CELL_SIZE, snake.front().x * CELL_SIZE, bodyImg + 1);
			}
			else
			{
				putimage(snake.front().y * CELL_SIZE, snake.front().x * CELL_SIZE, bodyImg);
			}
		}
		putimage(heady, headx, rotatedhead + 0, NOTSRCERASE);
		putimage(heady, headx, rotatedhead + 1, SRCINVERT);//蛇头移动

		// 中间节点保持原位（不更新坐标）
		// 绘制蛇身（跳过头部）
		auto it = ++snake.begin();
		for (; it != prev(snake.end()); ++it) {
			dx_prev = it->x - prev(it)->x; // 前一段方向 1代表由下到上 -1代表由上到下 0代表同一行
			dy_prev = it->y - prev(it)->y;//1代表由右到左 -1代表由左到右 0代表同一列
			dx_next = next(it)->x - it->x; // 后一段方向
			dy_next = next(it)->y - it->y;
			if (dx_prev == dx_next && dy_prev == dy_next) {// 直身
				if (dx_prev) {
					putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, bodyImg + 1);
				}
				else
				{
					putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, bodyImg);
				}
			}
			else {
				IMAGE* rotatedhead = headImg;
				// 转弯，根据 dx_prev/dy_prev 和 dx_next/dy_next 选择转弯图片
				if (dx_prev == 1 && dy_next == 1) {
					rotatedhead = turn_rightandup;
				}
				else if (dx_prev == 1 && dy_next == -1) {
					// 左转向下
					rotatedhead = turn_leftandup;
				}
				else if (dx_prev == -1 && dy_next == -1) {
					// 左转向下
					rotatedhead = turn_leftanddown;
				}
				else if (dx_prev == -1 && dy_next == 1) {
					// 右转向上（←↑）
					rotatedhead = turn_rightanddown;
				}
				else if (dy_prev == 1 && dx_next == 1) {
					// 向上转右
					rotatedhead = turn_leftanddown;
				}
				else if (dy_prev == 1 && dx_next == -1) {
					// 向上转左
					rotatedhead = turn_leftandup;
				}
				else if (dy_prev == -1 && dx_next == 1) {
					// 向下转右
					rotatedhead = turn_rightanddown;
				}
				else if (dy_prev == -1 && dx_next == -1) {
					// 向下转左
					rotatedhead = turn_rightandup;
				}
				putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, rotatedhead + 0, NOTSRCERASE);
				putimage(it->y * CELL_SIZE, it->x * CELL_SIZE, rotatedhead + 1, SRCINVERT);
			}
		}
		//绘画尾部
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE, rotatedtail + 0, NOTSRCERASE);
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE, rotatedtail + 1, SRCINVERT);
		drawMap();
		// 打印关卡标题
		settextcolor(RGB(0x63, 0x3c, 0x2d));
		settextstyle(50, 0, "微软雅黑");
		outtextxy((getwidth() - textwidth(level_title)) / 2, 100, level_title);

		FlushBatchDraw();
		if (sum >= 40)return;
		frameTime = clock() - startTime;    //56-16 = 6;
		if (frameTime < F_TIME) {
			Sleep(F_TIME - frameTime);
		}
	}
	EndBatchDraw();
}
//Jay
void smoothDown() {
	// FPS 60f
	const clock_t F_TIME = 1000 / 60;    //1秒60帧 = 16.6 每一帧应该花费的时间
	int startTime = 0;
	int frameTime = 0;  //当前帧执行时间

	// 重力参数
	float gravity = 0.2f;   // 重力加速度
	static float velocity = 0.0f;  // 当前下落速度
	float maxVelocity = 10.0f; // 最大下落速度

	//绘制蛇头
	int headx = snake.front().x * CELL_SIZE;
	int heady = snake.front().y * CELL_SIZE;
	float dx = 0.0f;  // 下落距离（x方向）
	int dx_prev = 0, dy_prev = 0, dx_next = 0, dy_next = 0, tailx = 0, taily = 0;
	IMAGE* rotatedhead = headImg, * rotatedtail = headImg;

	switch (headdir) { //蛇头旋转
	case UP:
		rotatedhead = headImg_up;
		break;
	case DOWN:
		rotatedhead = headImg_down;
		break;
	case LEFT:
		rotatedhead = headImg_left;
		break;
	case RIGHT:
		break;
	}

	//蛇尾旋转
	dx_prev = prev(prev(snake.end()))->x;
	dy_prev = prev(prev(snake.end()))->y;
	tailx = snake.back().x;
	taily = snake.back().y;

	switch (dx_prev - tailx) {
	case 1:
		rotatedtail = tailImg_down;
		break;
	case -1:
		rotatedtail = tailImg_up;
		break;
	default:
		if (dy_prev - taily == 1) {
			rotatedtail = tailImg_right;
			break;
		}
		else {
			rotatedtail = tailImg_left;
			break;
		}
	}

	IMAGE* rotated = turn_rightandup;

	//双缓冲防止闪屏
	BeginBatchDraw();
	while (true) {
		startTime = clock();
		cleardevice();
		putimage(0, 0, &img_bg);
		putimage(50, getheight() / 2, img_replay + 0, NOTSRCERASE);
		putimage(50, getheight() / 2, img_replay + 1, SRCINVERT);
		putimage(50, 30, img_back + 0, NOTSRCERASE);
		putimage(50, 30, img_back + 1, SRCINVERT);
		putimage(50, 150, img_skin + 0, NOTSRCERASE);
		putimage(50, 150, img_skin + 1, SRCINVERT);
		putimage(50, 300, img_levelselect + 0, NOTSRCERASE);
		putimage(50, 300, img_levelselect + 1, SRCINVERT);
		putimage(950, 500, img_up + 0, NOTSRCERASE);
		putimage(950, 500, img_up + 1, SRCINVERT);
		putimage(950, 650, img_down + 0, NOTSRCERASE);
		putimage(950, 650, img_down + 1, SRCINVERT);
		putimage(875, 575, img_left + 0, NOTSRCERASE);
		putimage(875, 575, img_left + 1, SRCINVERT);
		putimage(1025, 575, img_right + 0, NOTSRCERASE);
		putimage(1025, 575, img_right + 1, SRCINVERT);

		// 应用重力
		velocity += gravity;
		if (velocity > maxVelocity) {
			velocity = maxVelocity;
		}
		dx += velocity;

		//绘制蛇头（注意x是垂直方向，所以第一个参数是heady，第二个是headx + dx）
		putimage(heady, headx + dx, rotatedhead + 0, NOTSRCERASE);
		putimage(heady, headx + dx, rotatedhead + 1, SRCINVERT);

		// 绘制蛇身（跳过头部）
		auto it = ++snake.begin();
		for (; it != prev(snake.end()); ++it) {
			dx_prev = it->x - prev(it)->x; // 前一段方向 1代表由下到上 -1代表由上到下 0代表同一行
			dy_prev = it->y - prev(it)->y;//1代表由右到左 -1代表由左到右 0代表同一列
			dx_next = next(it)->x - it->x; // 后一段方向
			dy_next = next(it)->y - it->y;
			if (dx_prev == dx_next && dy_prev == dy_next) {// 直身
				if (dx_prev) {
					putimage(it->y * CELL_SIZE, it->x * CELL_SIZE + dx, bodyImg + 1);
				}
				else {
					putimage(it->y * CELL_SIZE, it->x * CELL_SIZE + dx, bodyImg);
				}
			}
			else {
				IMAGE* rotatedhead = headImg;
				// 转弯，根据 dx_prev/dy_prev 和 dx_next/dy_next 选择转弯图片
				if (dx_prev == 1 && dy_next == 1) {
					rotatedhead = turn_rightandup;
				}
				else if (dx_prev == 1 && dy_next == -1) {
					// 左转向下
					rotatedhead = turn_leftandup;
				}
				else if (dx_prev == -1 && dy_next == -1) {
					// 左转向下
					rotatedhead = turn_leftanddown;
				}
				else if (dx_prev == -1 && dy_next == 1) {
					// 右转向上（←↑）
					rotatedhead = turn_rightanddown;
				}
				else if (dy_prev == 1 && dx_next == 1) {
					// 向上转右
					rotatedhead = turn_leftanddown;
				}
				else if (dy_prev == 1 && dx_next == -1) {
					// 向上转左
					rotatedhead = turn_leftandup;
				}
				else if (dy_prev == -1 && dx_next == 1) {
					// 向下转右
					rotatedhead = turn_rightanddown;
				}
				else if (dy_prev == -1 && dx_next == -1) {
					// 向下转左
					rotatedhead = turn_rightandup;
				}
				putimage(it->y * CELL_SIZE, it->x * CELL_SIZE + dx, rotatedhead + 0, NOTSRCERASE);
				putimage(it->y * CELL_SIZE, it->x * CELL_SIZE + dx, rotatedhead + 1, SRCINVERT);
			}
		}
		//绘画尾部
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE + dx, rotatedtail + 0, NOTSRCERASE);
		putimage(taily * CELL_SIZE, tailx * CELL_SIZE + dx, rotatedtail + 1, SRCINVERT);
		drawMap();
		// 打印关卡标题
		settextcolor(RGB(0x63, 0x3c, 0x2d));
		settextstyle(50, 0, "微软雅黑");
		outtextxy((getwidth() - textwidth(level_title)) / 2, 100, level_title);

		FlushBatchDraw();

		// 检查是否到达目标位置（一个单元格的距离）
		if (dx >= CELL_SIZE) {
			return;
		}

		frameTime = clock() - startTime;
		if (frameTime < F_TIME) {
			Sleep(F_TIME - frameTime);
		}
	}
	EndBatchDraw();
}
// -------------------------3.0----------------------------
// --------------------------自定义区----------------------------
