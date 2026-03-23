#include "raylib.h"
#include <vector>
#include <cmath>
#include <fstream>
using namespace std;

enum GameScreen {MENIU, JOC, GAME_OVER, OPTIUNI, RESOLUTIONS, PAUSE};
int screenWidth = 1920;
int screenHeight = 1080;

struct Glont
{
	Vector2 pozitie;
	Vector2 viteza;
	bool activ;
};

struct Enemy
{
	Vector2 pozitie;
	Vector2 viteza;
	float timpSchimbareViteza;
	bool activ;
};


int incarcaHighScore()
{
	int scorSalvat = 0;
	ifstream fin("highscore.txt");
	if (fin.is_open())
	{
		fin >> scorSalvat;
		fin.close();
	}
	return scorSalvat;
}


void salveazaHighScore(int scor)
{
	ofstream fout("highscore.txt");
	if (fout.is_open())
	{
		fout << scor;
		fout.close();
	}
}




Enemy generate_random_enemy(Vector2 pozitie)
{
	Enemy inamic;
	bool valid = false;

	while (valid == false)
	{
		float x = GetRandomValue(0, screenWidth);
		float y = GetRandomValue(0, screenHeight);
		
		float dx = pozitie.x - x;
		float dy = pozitie.y - y;
		float d = (dx * dx) + (dy * dy);
		if (d > 30000)
		{
			inamic.pozitie.x = x;
			inamic.pozitie.y = y;
			inamic.activ = true;
			valid = true;
		}
	}
	inamic.viteza.x = (float)GetRandomValue(-3, 3);
	inamic.viteza.y = (float)GetRandomValue(-3, 3);
	if (inamic.viteza.x == 0 && inamic.viteza.y == 0)
		inamic.viteza.x = 2;
	return inamic;
}

int score = 0;

bool check_collision_enemy(Vector2 pozitie, const vector<Enemy> &inamici)
{
	float s_radius = 61.0f;
	s_radius *= s_radius;
	for (int i = 0; i < inamici.size(); i++)
	{
		if (inamici[i].activ == true)
		{
			float dx = pozitie.x - inamici[i].pozitie.x;
			float dy = pozitie.y - inamici[i].pozitie.y;

			float d = (dx * dx) + (dy * dy);
			if (d <= s_radius)
				return true;
		}
	}
	return false;
}

void check_collision_glont(vector<Glont> &gloante, vector<Enemy> &inamici)
{
	float s_radius = 35.0f;
	s_radius *= s_radius;
	for (int i = 0; i < inamici.size(); i++)
	{
		if (inamici[i].activ == true)
		{
			for (int j = 0; j < gloante.size(); j++)
			{
				if (gloante[j].activ == true)
				{
					float dx = gloante[j].pozitie.x - inamici[i].pozitie.x;
					float dy = gloante[j].pozitie.y - inamici[i].pozitie.y;

					float d = (dx * dx) + (dy * dy);

					if (d <= s_radius)
					{
						gloante[j].activ = false;
						inamici[i].activ = false;
						gloante.erase(gloante.begin() + j);
						inamici.erase(inamici.begin() + i);
						score++;
						i--;
						break;
					}
				}
			}
		}
	}
}


void draw_text_centered(const char text[], Rectangle button, int font_size, Color color)
{
	int text_width = MeasureText(text, font_size);
	int x = button.x + (button.width / 2) - (text_width / 2);
	int y = button.y + (button.height / 2) - (font_size / 2);
	DrawText(text, x, y, font_size, color);
}


int main()
{

	InitWindow(screenWidth, screenHeight, "Shooter");
	ToggleFullscreen();
	SetTargetFPS(60);

	GameScreen ecran_curent = MENIU;

	Vector2 pozitie = { screenWidth / 2, screenHeight / 2 };
	float speed = 5.0f;
	int lung_pusca = 40;
	float aimX = 0;
	float aimY = 0;
	
	float radius_player = 36.0f;
	float radius_enemy = 25.0f;
	float radius_glont = 10.0f;

	vector<Glont> gloante;
	vector<Enemy> inamici;

	float enemyX = 0;
	float enemyY = 0;
	float time_next_enemy = 1.f;

	Texture2D meniu = LoadTexture("meniu.jpeg");
	Texture2D optiuni = LoadTexture("options.jpeg");


	Rectangle buton_start = { screenWidth * 2 / 3.0, screenHeight / 2.0, screenWidth / 3.84, screenHeight / 10.8 };
	Rectangle buton_optiuni = { screenWidth * 2 / 3.0, screenHeight * 2 / 3.0, screenWidth / 3.84, screenHeight / 10.8 };
	Rectangle buton_exit = { screenWidth * 2 / 3.0, screenHeight * 5 / 6.0, screenWidth / 3.84, screenHeight / 10.8 };

	//options
	Rectangle buton_resolution = { screenWidth / 3.0, screenHeight / 3.0, screenWidth / 3.0, screenHeight / 12.0};
	Rectangle buton_back = { screenWidth / 4.0 + 10, screenHeight / 4.0 + 10, screenWidth / 20., screenHeight / 30. };
	Rectangle buton_1080 = { screenWidth / 3.0, screenHeight / 3.0, screenWidth / 3.0, screenHeight / 12.0 };
	Rectangle buton_720 = { screenWidth / 3.0, screenHeight / 2.0, screenWidth / 3.0, screenHeight / 12.0 };

	Rectangle buton_backtomenu = {(float)(screenWidth / 3.), (float)(screenHeight * 3 / 4.), (float)(screenWidth / 3.), (float)(screenHeight / 8.)};
	Rectangle buton_restart = { (float)(screenWidth / 3.), (float)(screenHeight * 4.5 / 8.), (float)(screenWidth / 3.), (float)(screenHeight / 8.)};


	int high_score = incarcaHighScore();

	bool running = true;

	while (!WindowShouldClose() && running)
	{
		if (IsKeyPressed(KEY_F))
			ToggleFullscreen();
		Vector2 mouse_pos = GetMousePosition();
		switch (ecran_curent)
		{
			case JOC:
			{
				if (IsKeyDown(KEY_W) && pozitie.y > 0)
					pozitie.y -= speed;
				if (IsKeyDown(KEY_S) && pozitie.y < screenHeight)
					pozitie.y += speed;
				if (IsKeyDown(KEY_A) && pozitie.x > 0)
					pozitie.x -= speed;
				if (IsKeyDown(KEY_D) && pozitie.x < screenWidth)
					pozitie.x += speed;

				float dx = mouse_pos.x - pozitie.x;
				float dy = mouse_pos.y - pozitie.y;
				float unghi = atan2f(dy, dx);

				aimX = pozitie.x + cosf(unghi) * lung_pusca;
				aimY = pozitie.y + sinf(unghi) * lung_pusca;
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				{
					float viteza_glont = 10.0f;
					Glont g;

					g.pozitie = pozitie;
					g.viteza.x = cosf(unghi) * viteza_glont;
					g.viteza.y = sinf(unghi) * viteza_glont;
					g.activ = true;						
					gloante.push_back(g);
				}
				for (int i = 0; i < gloante.size(); i++)
				{
					if (gloante[i].activ)
					{
						gloante[i].pozitie.x += gloante[i].viteza.x;
						gloante[i].pozitie.y += gloante[i].viteza.y;
						if (gloante[i].pozitie.x < 0 || gloante[i].pozitie.x > screenWidth || gloante[i].pozitie.y < 0 || gloante[i].pozitie.y > screenHeight)
						{
							gloante.erase(gloante.begin() + i);
							i--;
							//gloante[i].activ = false;
						}
					}
				}
				time_next_enemy -= GetFrameTime();
				if (time_next_enemy <= 0)
				{
					if (inamici.size() < 100)
					{
						Enemy inamic = generate_random_enemy(pozitie);
						inamici.push_back(inamic);
					}
					time_next_enemy = 1.f;
				}
				for (int i = 0; i < inamici.size(); i++)
				{
					if (inamici[i].activ)
					{
						inamici[i].pozitie.x += inamici[i].viteza.x;
						inamici[i].pozitie.y += inamici[i].viteza.y;
						if (inamici[i].pozitie.x <= 0 || inamici[i].pozitie.x >= screenWidth)
							inamici[i].viteza.x *= (-1);
						if (inamici[i].pozitie.y <= 0 || inamici[i].pozitie.y >= screenHeight)
							inamici[i].viteza.y *= (-1);
					}
				}
				if (check_collision_enemy(pozitie, inamici) == true)
				{
					ecran_curent = GAME_OVER;
				}
				check_collision_glont(gloante, inamici);

				if (IsKeyPressed(KEY_P))
				{
					ecran_curent = PAUSE;
				}
			break;
			}

			case MENIU:
			{

				if (CheckCollisionPointRec(mouse_pos, buton_start))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						inamici.clear();
						gloante.clear();
						pozitie = { (float)(screenWidth / 2.), (float)(screenHeight / 2.) };
						ecran_curent = JOC;
					}
				}
				if (CheckCollisionPointRec(mouse_pos, buton_optiuni))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						ecran_curent = OPTIUNI;
				}
				if (CheckCollisionPointRec(mouse_pos, buton_exit))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						running = false;
				}
				break;
			}
			case OPTIUNI:
			{
				if (CheckCollisionPointRec(mouse_pos, buton_back))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						ecran_curent = MENIU;
				}
				if (CheckCollisionPointRec(mouse_pos, buton_resolution))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						ecran_curent = RESOLUTIONS;
				}
				break;
			}
			case RESOLUTIONS:
			{
				if (CheckCollisionPointRec(mouse_pos, buton_back))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						ecran_curent = OPTIUNI;
				}

				if (CheckCollisionPointRec(mouse_pos, buton_1080))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						SetWindowSize(1920, 1080);
						screenWidth = 1920;
						screenHeight = 1080;

						//menu
						buton_start = { (float)(screenWidth * 2 / 3.0), (float)(screenHeight / 2.0), (float)(screenWidth / 3.84), (float)(screenHeight / 10.8) };
						buton_optiuni = { (float)(screenWidth * 2 / 3.0), (float)(screenHeight * 2 / 3.0), (float)(screenWidth / 3.84), (float)(screenHeight / 10.8) };
						buton_exit = { (float)(screenWidth * 2 / 3.0), (float)(screenHeight * 5 / 6.0), (float)(screenWidth / 3.84), (float)(screenHeight / 10.8) };

						//options
						buton_resolution = { (float)(screenWidth / 3.0), (float)(screenHeight / 3.0), (float)(screenWidth / 3.0), (float)(screenHeight / 12.0) };
						buton_back = { (float)(screenWidth / 4.0 + 10), (float)(screenHeight / 4.0 + 10), (float)(screenWidth / 20.), (float)(screenHeight / 30.) };
						buton_1080 = { (float)(screenWidth / 3.0), (float)(screenHeight / 3.0), (float)(screenWidth / 3.0), (float)(screenHeight / 12.0) };
						buton_720 = { (float)(screenWidth / 3.0), (float)(screenHeight / 2.0), (float)(screenWidth / 3.0), (float)(screenHeight / 12.0) };

						buton_backtomenu = { (float)(screenWidth / 3.), (float)(screenHeight * 3 / 4.), (float)(screenWidth / 3.), (float)(screenHeight / 8.) };

						buton_backtomenu = { (float)(screenWidth / 3.), (float)(screenHeight * 3 / 4.), (float)(screenWidth / 3.), (float)(screenHeight / 8.) };
						buton_restart = { (float)(screenWidth / 3.), (float)(screenHeight * 4.5 / 8.), (float)(screenWidth / 3.), (float)(screenHeight / 8.) };

						ecran_curent = MENIU;
					}
				}
				if (CheckCollisionPointRec(mouse_pos, buton_720))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						SetWindowSize(1280, 720);
						screenWidth = 1280;
						screenHeight = 720;

						//menu
						buton_start = {(float)(screenWidth * 2 / 3.0), (float)(screenHeight / 2.0), (float)(screenWidth / 3.84), (float)(screenHeight / 10.8) };
						buton_optiuni = { (float)(screenWidth * 2 / 3.0), (float)(screenHeight * 2 / 3.0), (float)(screenWidth / 3.84), (float)(screenHeight / 10.8) };
						buton_exit = { (float)(screenWidth * 2 / 3.0), (float)(screenHeight * 5 / 6.0), (float)(screenWidth / 3.84), (float)(screenHeight / 10.8) };

						//options
						buton_resolution = { (float)(screenWidth / 3.0), (float)(screenHeight / 3.0), (float)(screenWidth / 3.0), (float)(screenHeight / 12.0) };
						buton_back = { (float)(screenWidth / 4.0 + 10), (float)(screenHeight / 4.0 + 10), (float)(screenWidth / 20.), (float)(screenHeight / 30.) };
						buton_1080 = { (float)(screenWidth / 3.0), (float)(screenHeight / 3.0), (float)(screenWidth / 3.0), (float)(screenHeight / 12.0) };
						buton_720 = { (float)(screenWidth / 3.0), (float)(screenHeight / 2.0), (float)(screenWidth / 3.0), (float)(screenHeight / 12.0) };

						buton_backtomenu = { (float)(screenWidth / 3.), (float)(screenHeight * 3 / 4.), (float)(screenWidth / 3.), (float)(screenHeight / 8.) };
						buton_restart = { (float)(screenWidth / 3.), (float)(screenHeight * 4.5 / 8.), (float)(screenWidth / 3.), (float)(screenHeight / 8.) };

						ecran_curent = MENIU;
					}
				}
				break;
			}
			case PAUSE:
			{
				if (IsKeyPressed(KEY_P))
					ecran_curent = JOC;

				if (CheckCollisionPointRec(mouse_pos, buton_backtomenu))
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
						ecran_curent = MENIU;
				
				break;
			}

			case GAME_OVER:
			{
				if (score > high_score)
				{
					high_score = score;
					salveazaHighScore(high_score);
				}
				if (CheckCollisionPointRec(mouse_pos, buton_restart))
				{
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						inamici.clear();
						gloante.clear();
						pozitie = { (float)(screenWidth / 2.), (float)(screenHeight / 2.) };
						score = 0;
						ecran_curent = JOC;
					}
				}
				if (CheckCollisionPointRec(mouse_pos, buton_backtomenu))
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
					{
						score = 0;
						ecran_curent = MENIU;
					}
				break;
			}

		}



		BeginDrawing();
		ClearBackground(WHITE);

		switch (ecran_curent)
		{
			case JOC:
			{
				DrawCircle((int)pozitie.x, (int)pozitie.y, radius_player, RED);
				DrawLineEx(pozitie, { aimX, aimY }, 5.0f, BLUE);
				for (int i = 0; i < gloante.size(); i++)
				{
					if (gloante[i].activ)
						DrawCircle(gloante[i].pozitie.x, gloante[i].pozitie.y, radius_glont, BLACK);
				}
				for (int i = 0; i < inamici.size(); i++)
					if (inamici[i].activ)
						DrawCircle((int)inamici[i].pozitie.x, (int)inamici[i].pozitie.y, radius_enemy, YELLOW);
				
				DrawText(TextFormat("SCORE: %i", score), 20, (float)(screenHeight / 54.), (float)(screenWidth / 32.), BLACK);
				DrawText(TextFormat("HIGHSCORE: %i", high_score), 20, (float)(screenHeight / 10.8), (float)(screenWidth / 32.), BLACK);


				break;
			}

			case MENIU:
			{
				DrawTexturePro(meniu,
					{ 0.0f, 0.0f, 640, 853 },
					{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight },
					{ 0, 0 }, 0.0f, WHITE);
				DrawText("Esti in meniul jocului!", screenWidth / 10.0 + 8, screenHeight * 2 / 3.0 + 8, screenHeight / 10.8, BLACK);
				DrawText("Esti in meniul jocului!", screenWidth / 10.0, screenHeight * 2 / 3.0, screenHeight / 10.8, RED);

				if (CheckCollisionPointRec(mouse_pos, buton_start))
					DrawRectangleRec(buton_start, BLUE);
				else
					DrawRectangleRec(buton_start, RED);
				draw_text_centered("START", buton_start, screenWidth / 24., WHITE);

				if (CheckCollisionPointRec(mouse_pos, buton_optiuni))
					DrawRectangleRec(buton_optiuni, BLUE);
				else
					DrawRectangleRec(buton_optiuni, RED);
				draw_text_centered("OPTIONS", buton_optiuni, screenWidth / 24., WHITE);


				if (CheckCollisionPointRec(mouse_pos, buton_exit))
					DrawRectangleRec(buton_exit, BLUE);
				else
					DrawRectangleRec(buton_exit, RED);
				draw_text_centered("EXIT", buton_exit, screenWidth / 24., WHITE);

				break;
			}

			case OPTIUNI:
			{
				DrawTexturePro(meniu,
					{ 0.0f, 0.0f, 640, 853 },
					{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight},
					{ 0, 0 }, 0.0f, WHITE);
				DrawRectangle((float)(screenWidth / 4.0) - 5, (float)(screenHeight / 4.0) - 5, (float)(screenWidth / 2.0) + 10, (float)(screenHeight / 2.0) + 10, BLUE);
				DrawTexturePro(optiuni,
					{ 0.0f, 0.0f, 750, 1000 },
					{ (float)(screenWidth / 4.0), (float)(screenHeight / 4.0), (float)(screenWidth / 2.0), (float)(screenHeight / 2.0) },
					{ 0, 0 }, 0.0f, WHITE);

				if (CheckCollisionPointRec(mouse_pos, buton_resolution))
					DrawRectangleRec(buton_resolution, BLUE);
				else
					DrawRectangleRec(buton_resolution, RED);
				draw_text_centered("RESOLUTION", buton_resolution, screenHeight / 16., WHITE);

				if (CheckCollisionPointRec(mouse_pos, buton_back))
					DrawRectangleRec(buton_back, WHITE);
				else
					DrawRectangleRec(buton_back, GRAY);
				draw_text_centered("BACK", buton_back, 20, BLACK);

				break;
			}
			case RESOLUTIONS:
			{
				DrawTexturePro(meniu,
					{ 0.0f, 0.0f, 640, 853 },
					{ 0.0f, 0.0f, (float)screenWidth, (float)screenHeight},
					{ 0, 0 }, 0.0f, WHITE);
				DrawRectangle((float)(screenWidth / 4.0) - 5, (float)(screenHeight / 4.0) - 5, (float)(screenWidth / 2.0) + 10, (float)(screenHeight / 2.0) + 10, BLUE);
				DrawTexturePro(optiuni,
					{ 0.0f, 0.0f, 750, 1000 },
					{ (float)(screenWidth / 4.0), (float)(screenHeight / 4.0), (float)(screenWidth / 2.0), (float)(screenHeight / 2.0) },
					{ 0, 0 }, 0.0f, WHITE);
				if (CheckCollisionPointRec(mouse_pos, buton_back))
					DrawRectangleRec(buton_back, WHITE);
				else
					DrawRectangleRec(buton_back, GRAY);
				draw_text_centered("BACK", buton_back, 20, BLACK);

				if (CheckCollisionPointRec(mouse_pos, buton_1080))
					DrawRectangleRec(buton_1080, BLUE);
				else
					DrawRectangleRec(buton_1080, RED);
				draw_text_centered("1920x1080", buton_1080, 50, WHITE);

				if (CheckCollisionPointRec(mouse_pos, buton_720))
					DrawRectangleRec(buton_720, BLUE);
				else
					DrawRectangleRec(buton_720, RED);
				draw_text_centered("1280x720", buton_720, 50, WHITE);
				break;
			}
			case PAUSE:
			{
				DrawCircle((int)pozitie.x, (int)pozitie.y, radius_player, RED);
				DrawLineEx(pozitie, { aimX, aimY }, 5.0f, BLUE);
				for (int i = 0; i < gloante.size(); i++)
				{
					if (gloante[i].activ)
						DrawCircle(gloante[i].pozitie.x, gloante[i].pozitie.y, radius_glont, BLACK);
				}
				for (int i = 0; i < inamici.size(); i++)
					if (inamici[i].activ)
						DrawCircle((int)inamici[i].pozitie.x, (int)inamici[i].pozitie.y, radius_enemy, YELLOW);

				DrawRectangle(screenWidth / 10., screenHeight / 10., screenWidth * 8 / 10., screenHeight * 8 /10., Fade(BLACK, 0.6f));

				DrawRectangle(screenWidth / 3., screenHeight / 6., screenWidth / 3., screenHeight / 5., Fade(RED, 0.8f));
				draw_text_centered("GAME PAUSED", { (float)(screenWidth / 3.), (float)(screenHeight / 6.), (float)(screenWidth / 3.), (float)(screenHeight / 5.) }, (float)(screenWidth / 24.), WHITE);
				draw_text_centered("PENTRU A CONTINUA JOCUL APASATI [P]", { (float)(screenWidth / 3.), (float)(screenHeight / 2.), (float)(screenWidth / 3.), (float)(screenHeight / 5.) }, (float)(screenWidth / 38.4f), WHITE);

				if (CheckCollisionPointRec(mouse_pos, buton_backtomenu))
					DrawRectangleRec(buton_backtomenu, Fade(BLUE, 0.5f));
				else
					DrawRectangleRec(buton_backtomenu, Fade(RED, 0.5f));
				draw_text_centered("BACK TO MENU", buton_backtomenu, (float)(screenWidth / 48.f), WHITE);

				break;
			}
			case GAME_OVER:
			{
				DrawCircle((int)pozitie.x, (int)pozitie.y, radius_player, RED);
				DrawLineEx(pozitie, { aimX, aimY }, 5.0f, BLUE);
				for (int i = 0; i < gloante.size(); i++)
				{
					if (gloante[i].activ)
						DrawCircle(gloante[i].pozitie.x, gloante[i].pozitie.y, radius_glont, BLACK);
				}
				for (int i = 0; i < inamici.size(); i++)
					if (inamici[i].activ)
						DrawCircle((int)inamici[i].pozitie.x, (int)inamici[i].pozitie.y, radius_enemy, YELLOW);

				DrawRectangle(screenWidth / 10., screenHeight / 10., screenWidth * 8 / 10., screenHeight * 8 / 10., Fade(BLACK, 0.6f));
				DrawRectangle(screenWidth / 3., screenHeight / 6., screenWidth / 3., screenHeight / 5., Fade(RED, 0.8f));
				draw_text_centered("AI PIERDUT!", { (float)(screenWidth / 3.), (float)(screenHeight / 6.), (float)(screenWidth / 3.), (float)(screenHeight / 5.) }, (float)(screenWidth / 24.), WHITE);
		
				if (CheckCollisionPointRec(mouse_pos, buton_restart))
					DrawRectangleRec(buton_restart, Fade(BLUE, 0.5f));
				else
					DrawRectangleRec(buton_restart, Fade(RED, 0.5f));
				draw_text_centered("RESTART", buton_restart, (float)(screenWidth / 48.f), WHITE);



				if (CheckCollisionPointRec(mouse_pos, buton_backtomenu))
					DrawRectangleRec(buton_backtomenu, Fade(BLUE, 0.5f));
				else
					DrawRectangleRec(buton_backtomenu, Fade(RED, 0.5f));
				draw_text_centered("BACK TO MENU", buton_backtomenu, (float)(screenWidth / 48.f), WHITE);


				break;
			}

		}
		EndDrawing();
	}
	return 0;
}